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

#include <silo.h>
#include <spindle.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// -------- TYPE DEFINITIONS ----------------------------------------------- //

typedef struct SParutilDynamicSchedule
{
    uint64_t currentUnit;                                                   ///< Current unit of work to be assigned.
    uint64_t numUnits;                                                      ///< Total number of units of work.
} SParutilDynamicSchedule;


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

// --------

uint64_t parutilSchedulerDynamicInit(const uint64_t numUnits, void** schedule)
{
    if (spindleIsInParallelRegion())
    {
        SParutilDynamicSchedule* scheduleBuf = NULL;

        if (0 == spindleGetLocalThreadID())
        {
            // First thread allocates, initializes, and shares the dynamic scheduler object.
            scheduleBuf = (SParutilDynamicSchedule*)siloSimpleBufferAllocLocal(sizeof(SParutilDynamicSchedule));

            if (NULL != scheduleBuf)
            {
                scheduleBuf->currentUnit = (uint64_t)spindleGetLocalThreadCount();
                scheduleBuf->numUnits = numUnits;
            }

            spindleDataShareSendLocal((uint64_t)scheduleBuf);
        }
        else
        {
            // All other threads wait for the address of the dynamic scheduler object.
            scheduleBuf = (SParutilDynamicSchedule*)spindleDataShareReceiveLocal();
        }

        *schedule = (void*)scheduleBuf;
        
        if (NULL == scheduleBuf)
            return UINT64_MAX;

        // First unit of work is just the current thread's local identifier.
        const uint64_t firstWorkUnit = (uint64_t)spindleGetLocalThreadID();

        if (firstWorkUnit < scheduleBuf->numUnits)
            return firstWorkUnit;
        else
            return UINT64_MAX;
    }
    else
    {
        *schedule = NULL;
        return UINT64_MAX;
    }
}

// --------

uint64_t parutilSchedulerDynamicGetWork(void* schedule)
{
    // Check pre-conditions for this function.
    if ((!spindleIsInParallelRegion()) || (NULL == schedule))
        return UINT64_MAX;

    // Get the next unit of work for this thread.
    SParutilDynamicSchedule* const scheduleBuf = (SParutilDynamicSchedule*)schedule;

    const uint64_t nextWorkUnit = parutilAtomicExchangeAdd64(&scheduleBuf->currentUnit, 1ull);

    if (nextWorkUnit < scheduleBuf->numUnits)
        return nextWorkUnit;
    else
        return UINT64_MAX;
}

// --------

void parutilSchedulerDynamicExit(void* schedule)
{
    // Check pre-conditions for this function.
    if ((!spindleIsInParallelRegion()) || (NULL == schedule))
        return;

    spindleBarrierLocal();

    // First thread frees the previously-allocated dynamic scheduler object.
    if (0 == spindleGetLocalThreadID)
        siloFree(schedule);
}
