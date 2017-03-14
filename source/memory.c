/*****************************************************************************
 * Paratool
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file memory.c
 *   Implementation of memory operations.
 *****************************************************************************/

#include "memory.h"

#include <spindle.h>
#include <stdint.h>
#include <string.h>


// -------- CONSTANTS ------------------------------------------------------ //

/// Minimum size of a memory operation, in bytes, before Paratool will parallelize it.
static const size_t kParatoolMinimumOperationSize = 32ull * 1024ull * 1024ull;


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Contains all information needed to define a memory operation.
/// For internal use only.
typedef struct SParatoolMemoryOperationSpec
{
    void* destination;                                                      ///< Base address of the destination memory buffer.
    const void* source;                                                     ///< Base address of the source memory buffer. Not all memory operations need this information.
    uint64_t value;                                                         ///< Arbitrary value argument to be used by individual memory operations. Not all memory operations need this information.
    size_t num64;                                                           ///< Number of 64-byte blocks (cache lines) to include in the memory operation.
} SParatoolMemoryOperationSpec;


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal control function for memory copy operations.
/// @param [in] arg Pointer to the #SParatoolMemoryOperationSpec structure that contains information about the overall memory copy operation to be parallelized.
static void paratoolMemoryCopyInternalThread(void* arg)
{
    SParatoolMemoryOperationSpec* memoryOpSpec = (SParatoolMemoryOperationSpec*)arg;

    if (((size_t)memoryOpSpec->destination & (size_t)31) || ((size_t)memoryOpSpec->source & (size_t)31))
    {
        // Either the source or destination address is not aligned on a 256-bit (32-byte) boundary, so the unaligned copy implementation must be used.
        paratoolMemoryCopyUnalignedThread(memoryOpSpec->destination, memoryOpSpec->source, memoryOpSpec->num64);
    }
    else
    {
        // Both source and destination addresses are aligned on a 256-bit (32-byte) boundary, so the aligned copy implementation can be used.
        // This is preferable, as it will result in higher performance.
        paratoolMemoryCopyAlignedThread(memoryOpSpec->destination, memoryOpSpec->source, memoryOpSpec->num64);
    }
}

/// Internal control function for memory initialization operations.
/// @param [in] arg Pointer to the #SParatoolMemoryOperationSpec structure that contains information about the overall memory initialization operation to be parallelized.
static void paratoolMemorySetInternalThread(void* arg)
{
    SParatoolMemoryOperationSpec* memoryOpSpec = (SParatoolMemoryOperationSpec*)arg;
    
    // Alignment is ensured by the function that spawns threads to initialize in parallel.
    // No need to have an unaligned implementation as a result.
    paratoolMemorySetAlignedThread(memoryOpSpec->destination, memoryOpSpec->value, memoryOpSpec->num64);
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "paratool.h" for documentation.

void* paratoolMemoryCopy(void* destination, const void* source, size_t num)
{
    if (num < (kParatoolMinimumOperationSize))
    {
        // For small enough buffers, it is not worth the overhead of setting up threads to parallelize.
        // If operating inside a Spindle parallelized region, it is also not possible to create another one.
        return memcpy(destination, source, num);
    }
    else
    {
        SParatoolMemoryOperationSpec memoryOpSpec;
        SSpindleTaskSpec taskSpec;
        size_t numUnalignedBytes;
        
        // Set up control information for Spindle.
        // TODO: Once Silo supports returning the NUMA node for a buffer, use that information for better NUMA awareness here.
        taskSpec.func = &paratoolMemoryCopyInternalThread;
        taskSpec.arg = (void*)&memoryOpSpec;
        taskSpec.numaNode = 0;
        taskSpec.numThreads = 0;
        taskSpec.smtPolicy = SpindleSMTPolicyPreferPhysical;
        
        // Try to steer the implementation towards 64-byte alignment.
        // The underlying memory copy implementation uses 256-bit (32-byte) AVX instructions in groups of 2, for an effective block size of 512 bits (64 bytes).
        // If source and destination pointers have similar cache-line alignment and are both off-alignment, correct for that here.
        numUnalignedBytes = 64 - (((size_t)destination) & 63);
        
        if ((0 != numUnalignedBytes) && (64 - ((((size_t)source) & 63)) == numUnalignedBytes))
        {
            for (size_t i = 0; i < numUnalignedBytes; ++i)
                ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];

            destination = (void*)((size_t)destination + numUnalignedBytes);
            source = (void*)((size_t)source + numUnalignedBytes);
            num -= numUnalignedBytes;
        }
        
        // Ensure the actual parallelized implementation is invoked with a multiple of 64 blocks, and perform any needed tail-end correction here.
        // Corrections are done at the tail end to ensure preservation of array base address alignment.
        numUnalignedBytes = (num & 63);

        for (size_t i = 0; i < numUnalignedBytes; ++i)
            ((uint8_t*)destination)[num - i - 1] = ((uint8_t*)source)[num - i - 1];

        // Set up control information for the memory copy operation.
        memoryOpSpec.destination = destination;
        memoryOpSpec.source = source;
        memoryOpSpec.value = 0ull;
        memoryOpSpec.num64 = num >> 6;

        // Dispatch the memory copy operation.
        spindleThreadsSpawn(&taskSpec, 1, false);

        return destination;
    }
}

// --------

void* paratoolMemorySet(void* buffer, uint8_t value, size_t num)
{
    if (num < (kParatoolMinimumOperationSize))
    {
        // For small enough buffers, it is not worth the overhead of setting up threads to parallelize.
        // If operating inside a Spindle parallelized region, it is also not possible to create another one.
        return memset(buffer, (int)value, num);
    }
    else
    {
        SParatoolMemoryOperationSpec memoryOpSpec;
        SSpindleTaskSpec taskSpec;
        size_t numUnalignedBytes;
        
        // Set up control information for Spindle.
        // TODO: Once Silo supports returning the NUMA node for a buffer, use that information for better NUMA awareness here.
        taskSpec.func = &paratoolMemorySetInternalThread;
        taskSpec.arg = (void*)&memoryOpSpec;
        taskSpec.numaNode = 0;
        taskSpec.numThreads = 0;
        taskSpec.smtPolicy = SpindleSMTPolicyPreferPhysical;
        
        // Steer the implementation towards 64-byte alignment.
        // The underlying memory copy implementation uses 256-bit (32-byte) AVX instructions in groups of 2, for an effective block size of 512 bits (64 bytes).
        // Correct for buffer mis-alignment here.
        numUnalignedBytes = 64 - (((size_t)buffer) & 63);
        
        if ((0 != numUnalignedBytes))
        {
            for (size_t i = 0; i < numUnalignedBytes; ++i)
                ((uint8_t*)buffer)[i] = value;

            buffer = (void*)((size_t)buffer + numUnalignedBytes);
            num -= numUnalignedBytes;
        }
        
        // Ensure the actual parallelized implementation is invoked with a multiple of 64 blocks, and perform any needed tail-end correction here.
        // Corrections are done at the tail end to ensure preservation of array base address alignment.
        numUnalignedBytes = (num & 63);

        for (size_t i = 0; i < numUnalignedBytes; ++i)
            ((uint8_t*)buffer)[num - i - 1] = value;

        // Set up control information for the memory copy operation.
        memoryOpSpec.destination = buffer;
        memoryOpSpec.source = NULL;
        memoryOpSpec.value = (uint64_t)value;
        memoryOpSpec.value |= memoryOpSpec.value << 32ull;
        memoryOpSpec.value |= memoryOpSpec.value << 16ull;
        memoryOpSpec.value |= memoryOpSpec.value << 8ull;
        memoryOpSpec.num64 = num >> 6;

        // Dispatch the memory copy operation.
        spindleThreadsSpawn(&taskSpec, 1, false);

        return buffer;
    }
}
