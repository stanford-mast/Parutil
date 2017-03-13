/*****************************************************************************
 * Paratool
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file memorycopy.h
 *   Declaration of internal memory copying functions.
 *   Not intended for external use.
 *****************************************************************************/

#pragma once


// -------- FUNCTIONS ------------------------------------------------------ //

/// Copies `num64` properly-aligned 64-byte blocks of memory from `destination` to `source`.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] destination Target memory buffer.
/// @param [in] source Source memory buffer.
/// @param [in] num64 Number of 64-byte blocks to copy.
void partoolMemoryCopyAlignedThread(void* destination, const void* source, size_t num64);

/// Copies `num64` 64-byte blocks of memory from `destination` to `source`.
/// No assumptions are made as to their alignment.
/// Intended to be called from within the context of a Spindle parallelized region.
/// Work is statically scheduled and distributed across all active threads.
/// @param [in] destination Target memory buffer.
/// @param [in] source Source memory buffer.
/// @param [in] num64 Number of 64-byte blocks to copy.
void partoolMemoryCopyUnalignedThread(void* destination, const void* source, size_t num64);
