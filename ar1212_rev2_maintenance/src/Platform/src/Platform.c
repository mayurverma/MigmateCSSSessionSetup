//***********************************************************************************
/// \file
///
/// Platform support
///
/// \addtogroup platform
/// @{
//***********************************************************************************
// Copyright 2018 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//
//*************************************************************************************

#include <string.h>

#include "Common.h"

#include "Diag.h"
#include "SystemMgr.h"
#include "SystemDrv.h"
#include "WatchdogDrv.h"
#include "OtpmDrv.h"
#include "CommandDrv.h"
#include "MacDrv.h"
#include "Interrupts.h"

#include "PlatformInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

/// Represents a driver initialization function
typedef ErrorT (* PlatformDriverInitFunctionT)(void);

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

PlatformDriverInitFunctionT platformDriverInitFunctions[] =
{
    SystemDrvInit,
    CommandDrvInit,
    OtpmDrvInit,
    WatchdogDrvInit,
    MacDrvInit,

    // INFO: <PH>: add new drivers here
};

PlatformStateT platformState =
{
    .criticalSectionNestLevel = 0
};

/// Initialises the Driver components
/// \return Error status
///
ErrorT PlatformInitDrivers(void)
{
    uint16_t idx;
    ErrorT status = ERROR_SUCCESS;

    // Initialize all drivers
    // - note all drivers are initialized even if one (or more) drivers report an error
    for (idx = 0U; idx < (uint16_t)(sizeof(platformDriverInitFunctions) / sizeof((platformDriverInitFunctions)[0]));
         idx += 1U)
    {
        ErrorT driverStatus;

        DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DRIVER_INIT_START, idx);

        driverStatus = platformDriverInitFunctions[idx]();
        if (ERROR_SUCCESS != driverStatus)
        {
            if (ERROR_SUCCESS == status)
            {
                status = driverStatus;
            }
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_DRIVER_INIT_DONE);

    return status;
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the Platform components
/// - on exit we're in the critical section
/// \returns Error status
/// \retval ERROR_SUCCESS   Platform components initialized successfully
/// \retval ERROR_SYSERR    Critical Section nest level not zero
/// \retval OTHER           Driver component reported error
///
ErrorT PlatformInit(void)
{
    ErrorT interruptsStatus;
    ErrorT driverStatus;
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_PLATFORM_INIT_START);

    if (0U != platformState.criticalSectionNestLevel)
    {
        // Unexpected, report a system error
        status = ERROR_SYSERR;
    }
    else
    {
        PlatformEnterCritical();

        // Initialize interrupts and drivers
        interruptsStatus = InterruptsInit();
        driverStatus = PlatformInitDrivers();

        if (ERROR_SUCCESS != interruptsStatus)
        {
            status = interruptsStatus;
        }

        if (ERROR_SUCCESS != driverStatus)
        {
            status = driverStatus;
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_PLATFORM_INIT_DONE);

    return status;
}

/// Enter a (nested) critical section
/// - disables interrupts if they are enabled
/// \return void
///
void PlatformEnterCritical(void)
{
    InterruptsDisableAll();
    platformState.criticalSectionNestLevel += 1U;
}

/// Exit a (nested) critical section
/// - may enable interrupts on exit
/// \return void
///
void PlatformExitCritical(void)
{
    if (0U == platformState.criticalSectionNestLevel)
    {
        // INFO: <PH>: This function returns void so it cannot return an error. The pending fatal
        //             error will be taken when control returns to System Manager. The critical
        //             section is not exited.
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PLATFORM_CRITICAL_SECTION_NESTING_NOT_ZERO,
            platformState.criticalSectionNestLevel);
    }
    else
    {
        platformState.criticalSectionNestLevel -= 1U;
        if (0U == platformState.criticalSectionNestLevel)
        {
            InterruptsEnableAll();
        }
    }
}

// @} endgroup platform

