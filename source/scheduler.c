/*****************************************************************************
 * Parutil
 *   Multi-platform library of parallelized utility functions.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file scheduler.c
 *   Implementation of interface to scheduling assistance functionality.
 *****************************************************************************/

#include "../parutil.h"

#include <spindle.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// -------- INTERNAL FUNCTIONS --------------------------------------------- //

/// Internal initializaton function for a static chunked scheduler.
/// @param [in] units Total number of units of work that need to be scheduled.
/// @param [out] schedule Scheduling information, provided as output.
void parutilSchedulerStaticChunkedInternal(const uint64_t units, SParutilStaticSchedule* const schedule);




// -------- FUNCTIONS ------------------------------------------------------ //
// See "parutil.h" for documentation.

bool parutilSchedulerStatic(const EParutilStaticScheduler type, const uint64_t units, SParutilStaticSchedule* const schedule)
{
    // Check pre-conditions for this function.
    if ((!spindleIsInParallelRegion()) || (NULL == schedule))
        return false;
    
    // Branch based on the scheduler type that was selected.
    switch (type)
    {
    case ParutilStaticSchedulerChunked:
        parutilSchedulerStaticChunkedInternal(units, schedule);
        break;
        
    default:
        return false;
    }
    
    return true;
}
