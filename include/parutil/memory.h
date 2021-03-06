/*****************************************************************************
 * Parutil
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file memory.h
 *   Declaration of internal memory operation functionality.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //

/// Copies `num64` properly-aligned 64-byte blocks of memory from `destination` to `source`.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] destination Target memory buffer.
/// @param [in] source Source memory buffer.
/// @param [in] num64 Number of 64-byte blocks to copy.
void parutilMemoryCopyAlignedThread(void* destination, const void* source, size_t num64);

/// Copies `num64` 64-byte blocks of memory from `destination` to `source`.
/// No assumptions are made as to their alignment.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] destination Target memory buffer.
/// @param [in] source Source memory buffer.
/// @param [in] num64 Number of 64-byte blocks to copy.
void parutilMemoryCopyUnalignedThread(void* destination, const void* source, size_t num64);

/// Filters `num64` properly-aligned 64-byte blocks of memory by performing bitwise-and with `value`.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] buffer Target memory buffer.
/// @param [in] value Value to filter with each 64-bit element of the target memory buffer.
/// @param [in] num64 Number of 64-byte blocks to initialize.
void parutilMemoryFilterAlignedThread(void* buffer, uint64_t value, size_t num64);

/// Sets `num64` properly-aligned 64-byte blocks of memory to `value`.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] buffer Target memory buffer.
/// @param [in] value Value to write to each 64-bit element of the target memory buffer.
/// @param [in] num64 Number of 64-byte blocks to initialize.
void parutilMemorySetAlignedThread(void* buffer, uint64_t value, size_t num64);
