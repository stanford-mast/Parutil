/*****************************************************************************
 * Parutil
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file version.c
 *   Implementation of library version API functions.
 *****************************************************************************/

#include "../parutil.h"

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "parutil.h" for documentation.

uint32_t parutilGetLibraryVersion(void)
{
    return PARUTIL_LIBRARY_VERSION;
}
