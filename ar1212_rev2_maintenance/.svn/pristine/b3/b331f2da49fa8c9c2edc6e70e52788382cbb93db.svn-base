//***********************************************************************************
/// \file
///
/// System Manager implementation
///
/// \addtogroup systemmgr
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

#include "SystemMgrInternal.h"

#include "Platform.h"
#include "Init.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initializes the system - called by main()
///
/// Initializes the platform and drivers, interrupts and exceptions, and the application
/// components. If any failures occur during initialization the firmware terminates
/// immediately by calling SystemMgrFatalErrorStop().
///
/// \returns void
///
void SystemMgrInit(void)
{
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);
    (void)memset(&systemMgrState, 0, sizeof(systemMgrState));

    // Set default event and priority lookup tables (these can be overriden by a patch)
    systemMgrState.priorityByEvent = &systemMgrPriorityByEventDefaults[0];
    systemMgrState.eventByPriority = &systemMgrEventByPriorityDefaults[0];

    // Initialise the platform and drivers
    // - note on return we're in the critical section
    status = PlatformInit();
    if (ERROR_SUCCESS != status)
    {
        // Platform-layer component failed to initialize, terminate immediately
        SystemMgrFatalErrorStop(SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL,
            (uint32_t)SYSTEM_MGR_FATAL_ERROR_PLATFORM_INIT_FAILED,
            (uint32_t)status);
    }
    else
    {
        status = SystemMgrInitExceptionAndInterruptPriorities();
        if (ERROR_SUCCESS != status)
        {
            // Failed to initialize interrupts and exceptions, terminate immediately
            SystemMgrFatalErrorStop(SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL,
                (uint32_t)SYSTEM_MGR_FATAL_ERROR_EXCEPTION_INIT_FAILED,
                (uint32_t)status);
        }
        else
        {
            DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_INITIALIZE);
            status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_INITIALIZE);
            if (ERROR_SUCCESS != status)
            {
                // Failed to initialize components, terminate immediately
                SystemMgrFatalErrorStop(SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL,
                    (uint32_t)SYSTEM_MGR_FATAL_ERROR_COMPONENT_INIT_FAILED,
                    (uint32_t)status);
            }
            else
            {
                status = DiagRegisterFatalErrorDumpHandler(SystemMgrOnDiagFatalErrorDump);
                if (ERROR_SUCCESS != status)
                {
                    // Failed to register dump handler, terminate immediately
                    SystemMgrFatalErrorStop(SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL,
                        (uint32_t)SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_REGISTER_DUMP_HANDLER_FAILED,
                        (uint32_t)status);
                }
                else
                {
                    // Successfully initialized - we can enter the Debug phase
                    SystemMgrSetEvent(SYSTEM_MGR_EVENT_DEBUG_START);
                }
            }
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);
}

/// Main loop - called by main(), never returns in production systems
/// - MUST be called within a critical section
/// \returns void
///
void SystemMgrMainLoop(void)
{
    SystemMgrEventT currentEvent;
    ErrorT status = ERROR_SUCCESS;
    bool testAbort = false;

    do
    {
        do
        {
            currentEvent = SystemMgrGetNextEvent();
            if (SYSTEM_MGR_EVENT_NONE != currentEvent)
            {
                PlatformExitCritical();
                status = SystemMgrHandleEvent(currentEvent);
                PlatformEnterCritical();

                if ((ERROR_SYSERR == status) && systemMgrState.fatalErrorState.hasTerminated)
                {
                    // Event handler detected a fatal error and called SystemMgrReportFatalError.
                    // For testing purposes, we detect this case here and abort to

                    // allow control to return to the test runner.
                    testAbort = true;
                    break;
                }
            }
        }
        while (SYSTEM_MGR_EVENT_NONE != currentEvent);

        if (!testAbort)
        {
            DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT);
            SystemDrvWaitForInterrupt();

            // On return from WaitForInterrupt we're still in the critical section.
            // Leave and take the interrupt that woke us.
            PlatformExitCritical();
            PlatformEnterCritical();
        }
    }
    while (!testAbort);
}

/// Sets an event flag
///
/// Can be called by ISR or main loop
///
/// \param[in]  event   Event flag to set
/// \returns void
///
void SystemMgrSetEvent(SystemMgrEventT const event)
{
    if (SYSTEM_MGR_EVENT_MAX <= event)
    {
        // INFO: <PH>: This function returns void. The pending fatal error will
        //             be handled when control returns to System Manager.
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_SET_EVENT_INVALID,
            (uint32_t)event);
    }
    else
    {
        // Get the event's priority and set in current mask
        uint32_t priority = (uint32_t)systemMgrState.priorityByEvent[event];
        systemMgrState.currentPriorityMask |= (uint32_t)(1UL << priority);
    }
}

/// Indicates if the specified event is pending
///
/// \param[in]  event       Event to test
/// \param[out] isPending   Returned flag to indicate if event is pending
///
/// \returns Status
/// \retval ERROR_SUCCESS   isPending flag set successfully
/// \retval ERROR_INVALID   isPending is invalid
/// \retval ERROR_RANGE     event is out-of-range
///
ErrorT SystemMgrIsEventPending(SystemMgrEventT const event, bool* const isPending)
{
    ErrorT status = ERROR_SUCCESS;

    if (SYSTEM_MGR_EVENT_MAX > event)
    {
        if (NULL != isPending)
        {
            // Get the event's priority and test if set
            uint32_t priority = (uint32_t)systemMgrState.priorityByEvent[event];

            *isPending = (0U != (systemMgrState.currentPriorityMask & (1UL << priority)));
        }
        else
        {
            status = ERROR_INVAL;
        }
    }
    else
    {
        status = ERROR_RANGE;
    }

    return status;
}

/// Clears a pending event
///
/// \param[in]  event       Event to clear
///
/// \returns Status
/// \retval ERROR_SUCCESS   Pending event was cleared
/// \retval ERROR_ALREADY   Event was not pending
/// \retval ERROR_RANGE     event is out-of-range
///
ErrorT SystemMgrClearPendingEvent(SystemMgrEventT const event)
{
    ErrorT status = ERROR_SUCCESS;

    if (SYSTEM_MGR_EVENT_MAX > event)
    {
        // Get the event's priority and test if set
        uint32_t priority = (uint32_t)systemMgrState.priorityByEvent[event];
        bool isPending = (0U != (systemMgrState.currentPriorityMask & (1UL << priority)));

        if (isPending)
        {
            systemMgrState.currentPriorityMask &= ~(1U << priority);
        }
        else
        {
            status = ERROR_ALREADY;
        }
    }
    else
    {
        status = ERROR_RANGE;
    }

    return status;
}

/// Reports a fatal error
///
/// Reports a fatal error, which will result in shutdown of the cybersecurity subsystem.
///
/// \param[in]  context     context code (or zero)
/// \param[in]  infoParam   Optional information parameter for diagnostics (or zero)
/// \returns void
///
void SystemMgrReportFatalError(SystemMgrFatalErrorT const context,
    uint32_t const infoParam)
{
    SystemMgrReportCryptoFault(SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL, (uint32_t)context, infoParam);
}

/// Reports a crypto fault
///
/// Reports a crypto fault, which will result in shutdown of the cybersecurity subsystem.
///
/// \param[in]  fault       Crypto fault identifier
/// \param[in]  context     Optional context (or zero)
/// \param[in]  infoParam   Optional parameter for diagnostics (or zero)
/// \returns void
///
void SystemMgrReportCryptoFault(SystemMgrCssFaultT const fault,
    uint32_t const context,
    uint32_t const infoParam)
{
    bool isPending;

    PlatformEnterCritical();

    // Store the fatal error context and set the event if a fatal error is not already pending
    (void)SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &isPending);
    if (!isPending)
    {
        systemMgrState.fatalErrorState.fault = fault;
        systemMgrState.fatalErrorState.context = context;
        systemMgrState.fatalErrorState.infoParam = infoParam;

        SystemMgrSetEvent(SYSTEM_MGR_EVENT_FATAL_ERROR);

        // Request a Diagnostic dump
        // - this will only occur if a command handler is active
        DiagOnFatalError();
    }

    if (SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR == fault)
    {
        // Special case for fatal errors reported by CC312 library functions. The CC312 library
        // calls CC_PalAbort() in the PAL, which in turns calls CryptoMgrOnPalAbort(), which
        // calls this function.
        SystemMgrOnCc312Abort();
    }

    PlatformExitCritical();
}

/// Requests shutdown of the cybersecurity subsystem
///
/// \param[in]  context     Optional context code (or zero)
/// \param[in]  infoParam   Optional information parameter for diagnostics (or zero)
/// \returns void
///
void SystemMgrRequestShutdown(uint32_t const context,
    uint32_t const infoParam)
{
    bool isPending;

    // Store the context and infoParam and set the event if a shutdown is not already pending
    (void)SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &isPending);
    if (!isPending)
    {
        systemMgrState.shutdownState.context = context;
        systemMgrState.shutdownState.infoParam = infoParam;

        SystemMgrSetEvent(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING);
    }
}

/// Returns the current phase
/// \retval The current operational phase
///
SystemMgrPhaseT SystemMgrGetCurrentPhase(void)
{
    return systemMgrState.currentPhase;
}

/// Returns the ROM version
/// \returns The ROM version code
uint16_t SystemMgrGetRomVersion(void)
{
    return SYSTEM_MGR_ROM_VERSION;
}

/// Enables read/write and code access to the patch region
///
/// \param[in]  enable      Flag to enable or disable access
/// \returns void
///
void SystemMgrEnablePatchRegion(bool const enable)
{
    PlatformEnterCritical();
    InitMpuEnablePatchRegion(enable);
    PlatformExitCritical();
}

/// Tells System Manager the current cryptosubsystem clock speed
///
/// Notifies all registered clock speed change notifier functions
/// if the clock speed has changed.
///
/// \param[in] clockSpeedHz The current clock speed in Hertz
/// \returns Error status
/// \retval ERROR_SUCCESS   Clock speed change accepted
/// \retval ERROR_RANGE     Clock speed out of range
///
ErrorT SystemMgrSetClockSpeed(uint32_t clockSpeedHz)
{
    ErrorT status = ERROR_SUCCESS;
    int32_t idx;

    if (systemMgrState.clockSpeedHz != clockSpeedHz)
    {
        // Check the new clock speed is in range before publishing to the notifiers
        if ((SYSTEM_MGR_MIN_CLOCK_RATE_HZ > clockSpeedHz) ||
            (SYSTEM_MGR_MAX_CLOCK_RATE_HZ < clockSpeedHz))
        {
            status = ERROR_RANGE;
        }
        else
        {
            systemMgrState.clockSpeedHz = clockSpeedHz;

            for (idx = 0; idx < SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS; idx++)
            {
                if (NULL != systemMgrState.clockSpeedChangeNotifier[idx])
                {
                    status = systemMgrState.clockSpeedChangeNotifier[idx](clockSpeedHz);
                    if (ERROR_SUCCESS != status)
                    {
                        // Component rejected the clock speed - it will have reported
                        // a fatal error, so we simply abort here.
                        break;
                    }
                }
            }
        }
    }

    return status;
}

/// Retrieves the current cryptosubsystem clock speed (if known)
/// \param[in] clockSpeedHz Pointer to returned clock speed
/// \return Error status
/// \retval ERROR_SUCCESS clock speed known and returned
/// \retval ERROR_NOMSG clock speed is unknown
///
ErrorT SystemMgrGetClockSpeed(uint32_t* clockSpeedHz)
{
    ErrorT status = ERROR_NOMSG;

    if (0U != systemMgrState.clockSpeedHz)
    {
        if (NULL != clockSpeedHz)
        {
            *clockSpeedHz = systemMgrState.clockSpeedHz;
        }

        status = ERROR_SUCCESS;
    }

    return status;
}

/// Registers a notification handler for clock speed changes
///
/// \param[in] notifier pointer to notifier function
/// \returns Error status
/// retval ERROR_SUCCESS    Notifier registered successfully
/// retval ERROR_NOSPC      Notifier table full, no space
///
ErrorT SystemMgrRegisterClockSpeedChangeNotifier(SystemMgrClockSpeedChangeNotifierT notifier)
{
    ErrorT status = ERROR_NOSPC;
    int32_t idx;

    for (idx = 0; idx < SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS; idx++)
    {
        if (NULL == systemMgrState.clockSpeedChangeNotifier[idx])
        {
            systemMgrState.clockSpeedChangeNotifier[idx] = notifier;
            status = ERROR_SUCCESS;
            break;
        }
    }

    return status;
}

/// Indicates whether a CC312 abort (fatal error) has occurred
///
/// \return True if an abort has occurred
///
bool SystemMgrHasCC312AbortOccurred(void)
{
    return systemMgrState.fatalErrorState.isCC312Abort;
}

/// @} endgroup systemmgr

