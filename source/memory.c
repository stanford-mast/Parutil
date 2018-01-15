/*****************************************************************************
 * Parutil
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

#include <silo.h>
#include <spindle.h>
#include <stdint.h>
#include <string.h>


// -------- CONSTANTS ------------------------------------------------------ //

/// Minimum size of a memory operation, in bytes, before Parutil will parallelize it.
static const size_t kParutilMinimumOperationSize = 4ull * 1024ull * 1024ull;


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Contains all information needed to define a memory operation.
/// For internal use only.
typedef struct SParutilMemoryOperationSpec
{
    void* destination;                                                      ///< Base address of the destination memory buffer.
    const void* source;                                                     ///< Base address of the source memory buffer. Not all memory operations need this information.
    uint64_t value;                                                         ///< Arbitrary value argument to be used by individual memory operations. Not all memory operations need this information.
    size_t num64;                                                           ///< Number of 64-byte blocks (cache lines) to include in the memory operation.
} SParutilMemoryOperationSpec;


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal control function for memory copy operations.
/// @param [in] arg Pointer to the #SParutilMemoryOperationSpec structure that contains information about the overall memory copy operation to be parallelized.
static void parutilMemoryCopyInternalThread(void* arg)
{
    SParutilMemoryOperationSpec* memoryOpSpec = (SParutilMemoryOperationSpec*)arg;

    if (((size_t)memoryOpSpec->destination & (size_t)31) || ((size_t)memoryOpSpec->source & (size_t)31))
    {
        // Either the source or destination address is not aligned on a 256-bit (32-byte) boundary, so the unaligned copy implementation must be used.
        parutilMemoryCopyUnalignedThread(memoryOpSpec->destination, memoryOpSpec->source, memoryOpSpec->num64);
    }
    else
    {
        // Both source and destination addresses are aligned on a 256-bit (32-byte) boundary, so the aligned copy implementation can be used.
        // This is preferable, as it will result in higher performance.
        parutilMemoryCopyAlignedThread(memoryOpSpec->destination, memoryOpSpec->source, memoryOpSpec->num64);
    }
}

/// Internal control function for memory initialization operations.
/// @param [in] arg Pointer to the #SParutilMemoryOperationSpec structure that contains information about the overall memory initialization operation to be parallelized.
static void parutilMemorySetInternalThread(void* arg)
{
    SParutilMemoryOperationSpec* memoryOpSpec = (SParutilMemoryOperationSpec*)arg;
    
    // Alignment is ensured by the calling function.
    parutilMemorySetAlignedThread(memoryOpSpec->destination, memoryOpSpec->value, memoryOpSpec->num64);
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "parutil.h" for documentation.

void* parutilMemoryCopy(void* destination, const void* source, size_t num)
{
    if (num < (kParutilMinimumOperationSize))
    {
        // For small enough buffers, it is not worth the overhead of setting up threads to parallelize.
        if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
            return memcpy(destination, source, num);
        else
            return destination;
    }
    else
    {
        SParutilMemoryOperationSpec memoryOpSpec;
        size_t numUnalignedBytes;
        
        // Try to steer the implementation towards 64-byte alignment.
        // The underlying memory copy implementation uses 256-bit (32-byte) AVX instructions in groups of 2, for an effective block size of 512 bits (64 bytes).
        // If source and destination pointers have similar cache-line alignment and are both off-alignment, correct for that here.
        numUnalignedBytes = 64 - (((size_t)destination) & 63);
        
        if ((0 != numUnalignedBytes) && (64 - ((((size_t)source) & 63)) == numUnalignedBytes))
        {
            if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
            {
                for (size_t i = 0; i < numUnalignedBytes; ++i)
                    ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];
            }

            destination = (void*)((size_t)destination + numUnalignedBytes);
            source = (void*)((size_t)source + numUnalignedBytes);
            num -= numUnalignedBytes;
        }
        
        // Ensure the actual parallelized implementation is invoked with a multiple of 64 blocks, and perform any needed tail-end correction here.
        // Corrections are done at the tail end to ensure preservation of array base address alignment.
        numUnalignedBytes = (num & 63);

        if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
        {
            for (size_t i = 0; i < numUnalignedBytes; ++i)
                ((uint8_t*)destination)[num - i - 1] = ((uint8_t*)source)[num - i - 1];
        }

        // Set up control information for the memory copy operation.
        memoryOpSpec.destination = destination;
        memoryOpSpec.source = source;
        memoryOpSpec.value = 0ull;
        memoryOpSpec.num64 = num >> 6;
        
        if (spindleIsInParallelRegion())
        {
            spindleBarrierLocal();
            parutilMemoryCopyInternalThread((void*)&memoryOpSpec);
        }
        else
        {
            SSpindleTaskSpec taskSpec;
            int32_t targetNUMANode = siloGetNUMANodeForVirtualAddress(destination);
            
            // Set up control information for Spindle.
            if (0 > targetNUMANode)
                targetNUMANode = 0;
            
            taskSpec.func = &parutilMemoryCopyInternalThread;
            taskSpec.arg = (void*)&memoryOpSpec;
            taskSpec.numaNode = targetNUMANode;
            taskSpec.numThreads = 0;
            taskSpec.smtPolicy = SpindleSMTPolicyPreferPhysical;
            
            // Dispatch the memory copy operation.
            if (0 != spindleThreadsSpawn(&taskSpec, 1, false))
                return NULL;
        }
        
        return destination;
    }
}

// --------

void* parutilMemorySet(void* buffer, uint8_t value, size_t num)
{
    if (num < (kParutilMinimumOperationSize))
    {
        // For small enough buffers, it is not worth the overhead of setting up threads to parallelize.
        if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
            return memset(buffer, (int)value, num);
        else
            return buffer;
    }
    else
    {
        SParutilMemoryOperationSpec memoryOpSpec;
        size_t numUnalignedBytes;
        
        // Steer the implementation towards 64-byte alignment.
        // The underlying memory copy implementation uses 256-bit (32-byte) AVX instructions in groups of 2, for an effective block size of 512 bits (64 bytes).
        // Correct for buffer mis-alignment here.
        numUnalignedBytes = 64 - (((size_t)buffer) & 63);
        
        if ((0 != numUnalignedBytes))
        {
            if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
            {
                for (size_t i = 0; i < numUnalignedBytes; ++i)
                    ((uint8_t*)buffer)[i] = value;
            }
            
            buffer = (void*)((size_t)buffer + numUnalignedBytes);
            num -= numUnalignedBytes;
        }
        
        // Ensure the actual parallelized implementation is invoked with a multiple of 64 blocks, and perform any needed tail-end correction here.
        // Corrections are done at the tail end to ensure preservation of array base address alignment.
        numUnalignedBytes = (num & 63);

        if ((!spindleIsInParallelRegion()) || (0 == spindleGetLocalThreadID()))
        {
            for (size_t i = 0; i < numUnalignedBytes; ++i)
                ((uint8_t*)buffer)[num - i - 1] = value;
        }

        // Set up control information for the memory set operation.
        memoryOpSpec.destination = buffer;
        memoryOpSpec.source = NULL;
        memoryOpSpec.value = (uint64_t)value;
        memoryOpSpec.value |= memoryOpSpec.value << 32ull;
        memoryOpSpec.value |= memoryOpSpec.value << 16ull;
        memoryOpSpec.value |= memoryOpSpec.value << 8ull;
        memoryOpSpec.num64 = num >> 6;

        if (spindleIsInParallelRegion())
        {
            spindleBarrierLocal();
            parutilMemorySetInternalThread((void*)&memoryOpSpec);
        }
        else
        {
            SSpindleTaskSpec taskSpec;
            int32_t targetNUMANode = siloGetNUMANodeForVirtualAddress(buffer);
            
            // Set up control information for Spindle.
            if (0 > targetNUMANode)
                targetNUMANode = 0;
            
            taskSpec.func = &parutilMemorySetInternalThread;
            taskSpec.arg = (void*)&memoryOpSpec;
            taskSpec.numaNode = targetNUMANode;
            taskSpec.numThreads = 0;
            taskSpec.smtPolicy = SpindleSMTPolicyPreferPhysical;
            
            // Dispatch the memory set operation.
            if (0 != spindleThreadsSpawn(&taskSpec, 1, false))
                return NULL;
        }

        return buffer;
    }
}
