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

#include <stddef.h>
#include <stdint.h>


// -------- VERSION INFORMATION -------------------------------------------- //

/// 32-bit unsigned integer that represents the version of Parutil.
/// Incremented each time a change is made that affects the API.
/// - Version 1: Initial release.
/// - Version 2: Change semantics to allow invocation from within a Spindle parallelized region.
#define PARUTIL_LIBRARY_VERSION                 0x00000002


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

#ifdef __cplusplus
}
#endif
