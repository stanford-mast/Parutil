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

/// Minimum size of a memory copy operation, in bytes, before Paratool will parallelize it.
static const size_t kParatoolMinimumCopySize = 1024ull * 1024ull * 1024ull;


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Contains all information needed to define an internal memory copy operation.
typedef struct SParatoolMemoryCopySpec
{
    void* destination;                                                      ///< Base address of the destination memory buffer.
    const void* source;                                                     ///< Base address of the source memory buffer.
    size_t num64;                                                           ///< Number of 64-byte blocks (cache lines) to copy.
} SParatoolMemoryCopySpec;


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal control function for memory copy operations.
/// @param [in] arg Pointer to the #SParatoolMemoryCopySpec structure that contains information about the overall memory copy operation to be parallelized.
static void paratoolMemoryCopyInternalThread(void* arg)
{
    SParatoolMemoryCopySpec* memoryCopySpec = (SParatoolMemoryCopySpec*)arg;

    if (((size_t)memoryCopySpec->destination & (size_t)31) || ((size_t)memoryCopySpec->source & (size_t)31))
    {
        // Either the source or destination address is not aligned on a 256-bit (32-byte) boundary, so the unaligned copy implementation must be used.
        paratoolMemoryCopyUnalignedThread(memoryCopySpec->destination, memoryCopySpec->source, memoryCopySpec->num64);
    }
    else
    {
        // Both source and destination addresses are aligned on a 256-bit (32-byte) boundary, so the aligned copy implementation can be used.
        // This is preferable, as it will result in higher performance.
        paratoolMemoryCopyAlignedThread(memoryCopySpec->destination, memoryCopySpec->source, memoryCopySpec->num64);
    }
}


// -------- FUNCTIONS ------------------------------------------------------ //
// See "paratool.h" for documentation.

void* paratoolMemoryCopy(void* destination, const void* source, size_t num)
{
    if (num < (kParatoolMinimumCopySize))
    {
        // For small enough buffers, it is not worth the overhead of setting up threads to parallelize.
        // If operating inside a Spindle parallelized region, it is also not possible to create another one.
        return memcpy(destination, source, num);
    }
    else
    {
        SParatoolMemoryCopySpec memoryCopySpec;
        SSpindleTaskSpec taskSpec;
        size_t numUnalignedBytes;
        
        // Set up control information for Spindle.
        // Once Silo supports returning the NUMA node for a buffer, use that information for better NUMA awareness here.
        taskSpec.func = &paratoolMemoryCopyInternalThread;
        taskSpec.arg = (void*)&memoryCopySpec;
        taskSpec.numaNode = 0;
        taskSpec.numThreads = 0;
        taskSpec.smtPolicy = SpindleSMTPolicyPreferPhysical;
        
        // Try to steer the implementation towards 64-byte alignment.
        // The underlying memory copy implementation uses 256-bit (32-byte) AVX instructions in groups of 2, for an effective block size of 512 bits (64 bytes).
        // If source and destination pointers have similar cache-line alignment and are both off-alignment, correct for that here.
        numUnalignedBytes = (((size_t)destination) & 63);
        
        if ((0 != numUnalignedBytes) && ((((size_t)source) & 63) == numUnalignedBytes))
        {
            for (size_t i = 0; i < numUnalignedBytes; ++i)
                ((uint8_t*)destination)[i] = ((uint8_t*)source)[i];

            destination = (void*)((size_t)destination + numUnalignedBytes);
            source = (void*)((size_t)source + numUnalignedBytes);
            num -= numUnalignedBytes;
        }
        
        // Ensure the actual parallelized implementation is invoked with a multiple of 64 blocks, and perform any needed tail-end correction here.
        // Corrections are done at the tail end to ensure preservation of array base address alignment.
        numUnalignedBytes = (num & (size_t)63);

        for (size_t i = 0; i < numUnalignedBytes; ++i)
            ((uint8_t*)destination)[num - i - 1] = ((uint8_t*)source)[num - i - 1];

        // Set up control information for the memory copy operation.
        memoryCopySpec.destination = destination;
        memoryCopySpec.source = source;
        memoryCopySpec.num64 = num >> (size_t)6;

        // Dispatch the memory copy operation.
        spindleThreadsSpawn(&taskSpec, 1, true);

        return destination;
    }
}
