/*****************************************************************************
 * Paratool
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file version.c
 *   Implementation of library version API functions.
 *****************************************************************************/

#include "../paratool.h"

#include <stdint.h>


// -------- FUNCTIONS ------------------------------------------------------ //
// See "paratool.h" for documentation.

uint32_t paratoolGetLibraryVersion(void)
{
    return PARATOOL_LIBRARY_VERSION;
}
