/*****************************************************************************
 * Paratool
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file paratool.h
 *   Declaration of external API functions.
 *   Top-level header file for this library, to be included externally.
 *****************************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>


// -------- VERSION INFORMATION -------------------------------------------- //

/// 32-bit unsigned integer that represents the version of Paratool.
/// Incremented each time a change is made that affects the API.
/// - Version 1: Initial release.
#define PARATOOL_LIBRARY_VERSION                 0x00000001


// -------- FUNCTIONS ------------------------------------------------------ //
#ifdef __cplusplus
extern "C" {
#endif

    /// Retrieves and returns the compiled Paratool library version.
    /// @return Paratool library version number.
    uint32_t paratoolGetLibraryVersion(void);

    /// Copies `num` bytes of memory at `source` to memory at `destination`.
    /// Intended to be a drop-in replacement for the standard `memcpy()` function.
    /// Reverts to standard `memcpy()` if `num` is small enough or if called from within a Spindle parallelized region.
    /// @param [in] destination Target memory buffer.
    /// @param [in] source Source memory buffer.
    /// @param [in] num Number of bytes to copy.
    /// @return `destination` is returned upon completion.
    void* paratoolMemoryCopy(void* destination, const void* source, size_t num);

#ifdef __cplusplus
}
#endif
