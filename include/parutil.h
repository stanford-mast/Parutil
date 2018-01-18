/*****************************************************************************
 * Parutil
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file parutil.h
 *   Declaration of external API functions.
 *   Top-level header file for this library, to be included externally.
 *****************************************************************************/

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// -------- VERSION INFORMATION -------------------------------------------- //

/// 32-bit unsigned integer that represents the version of Parutil.
/// Incremented each time a change is made that affects the API.
/// - Version 1: Initial release.
/// - Version 2: Change semantics to allow invocation from within a Spindle parallelized region.
#define PARUTIL_LIBRARY_VERSION                 0x00000002


// -------- TYPE DEFINITIONS ----------------------------------------------- //

/// Communicates static schedule information to applications that use Parutil's scheduling assistance functionality.
typedef struct SParutilStaticSchedule
{
    uint64_t startUnit;                                                     ///< First unit of work assigned to the current thread.
    uint64_t endUnit;                                                       ///< One-past-last unit of work assigned to the current thread.
    uint64_t increment;                                                     ///< Number of units of work between units of work assigned to the current thread.
} SParutilStaticSchedule;

/// Enumerates the different types of static schedulers Parutil implements.
/// Used along with scheduling assistance functions to identify which type of static scheduler to use.
typedef enum EParutilStaticScheduler
{
    ParutilStaticSchedulerChunked,                                          ///< Chunked scheduler, which creates one continuous chunk of work per thread.
} EParutilStaticScheduler;


// -------- FUNCTIONS ------------------------------------------------------ //
#ifdef __cplusplus
extern "C" {
#endif

    /// Retrieves and returns the compiled Parutil library version.
    /// @return Parutil library version number.
    uint32_t parutilGetLibraryVersion(void);

    /// Copies `num` bytes of memory at `source` to memory at `destination`.
    /// Intended to be a drop-in replacement for the standard `memcpy()` function.
    /// It is the caller's responsibility to ensure that `source` and `destination` regions do not overlap.
    /// If called from within a Spindle parallelized region, every thread in the same task must invoke this function with the same arguments.
    /// Reverts to standard `memcpy()` if `num` is small enough.
    /// @param [in] destination Target memory buffer.
    /// @param [in] source Source memory buffer.
    /// @param [in] num Number of bytes to copy.
    /// @return `destination` is returned upon completion.
    void* parutilMemoryCopy(void* destination, const void* source, size_t num);
    
    /// Sets `num` bytes of memory at `buffer` to the value specified by `value`.
    /// Intended to be a drop-in replacement for the standard `memset()` function.
    /// If called from within a Spindle parallelized region, every thread in the same task must invoke this function with the same arguments.
    /// Reverts to standard `memset()` if `num` is small enough.
    /// @param [in] buffer Target memory buffer.
    /// @param [in] value Byte-sized value to write to the target memory buffer.
    /// @param [in] num Number of bytes to initialize.
    /// @return `buffer` is returned upon completion.
    void* parutilMemorySet(void* buffer, uint8_t value, size_t num);
    
    /// Uses a static scheduler of the specified type to provide the caller with information on assigned work.
    /// Intended to be called within a Spindle parallelized region and will fail otherwise.
    /// Each thread within a Spindle task should call this function.
    /// The information provided via the output #SParutilStaticSchedule can be used to determine which units of parallel work should be performed by each thread.
    /// For example, when parallelizing a `for` loop, the thread could start with `startUnit`, compare for less-than with `endUnit`, and increment by `increment`.
    /// @param [in] type Type of static scheduler to use.
    /// @param [in] units Total number of units of work that need to be scheduled.
    /// @param [out] schedule Scheduling information, provided as output.
    /// @return `true` if successful (i.e. in a parallel region, output parameter is not `NULL`, and so on), `false` otherwise.
    bool parutilSchedulerStatic(const EParutilStaticScheduler type, const uint64_t units, SParutilStaticSchedule* const schedule);
    
#ifdef __cplusplus
}
#endif
