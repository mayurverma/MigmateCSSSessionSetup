//***********************************************************************************
/// \file
///
/// Health Monitor implementation
///
/// \addtogroup healthmonitor
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
#include "SessionMgr.h"
#include "LifecycleMgr.h"
#include "PatchMgr.h"
#include "WatchdogDrv.h"
#include "Interrupts.h"
#include "Init.h"
#include "MacDrv.h"

#include "HealthMonitor.h"
#include "HealthMonitorInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
/// For Stack HWM monitoring, the default gap between the FW HWM threshold and the MPU stack guard
#define HEALTH_MONITOR_HWM_DANGER_ZONE_SIZE_LONG_WORDS   16U

/// The watchdog default timer period in microseconds
#define HEATH_MONITOR_INITIAL_WDG_TIMER_INTERVAL_MICROSECS   50000U     // 50ms

/// The watchdog timeout period in microseconds
#define HEALTH_MONITOR_WATCHDOG_TIMEOUT_MICROSECS   1000000U            // one second

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
HealthMonitorStateT healthMonitorState;

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Watchdog timer interrupt handler
/// Called by watchdog timer time-out interrupt periodically.
/// \returns void
///
void Watchdog_IrqHandler(void)
{
    uint32_t newClockFreq;

    // Only pat the watchdog if it's active.
    // If this interrupt has occurred and the watchdog isn't active, it's because we're entering shutdown
    // and we must not pat it (must not restart the timer)
    if (true == healthMonitorState.isWatchdogActive)
    {
        // Increment the watchdog counter and check it hasn't reached the n-1 count
        healthMonitorState.wdgInterruptCtr++;

        if (healthMonitorState.wdgInterruptCtr < healthMonitorState.wdgInterruptCtrMax)
        {
            // normal operation
            // Pat the WDG. This reloads the count-down timer
            (void)WatchdogDrvClearInterrupt();  // always returns ERROR_SUCCESS

            // Inform the System Manager of the watchdog timer event so that a
            // foreground task can clear wdgInterruptCtr
            SystemMgrSetEvent(SYSTEM_MGR_EVENT_WATCHDOG);
        }
        else
        {
            // Something's wrong: The watchdog interrupt counter is not being cleared by the foreground
            // processing, the WDGRES line will be asserted after the next timer interval expires
        }

        // Need to periodically check if the PLL locked state has changed so we can reload WDG timer value
        // This will happen when the video streaming starts or stops.
        if (healthMonitorState.currentPllLockState != SystemDrvIsPllLocked())
        {
            // PLL status has changed since last interrupt
            healthMonitorState.currentPllLockState = !healthMonitorState.currentPllLockState;

            // System clock has changed, need to reload new WDG timer value and inform the System Manager
            // Remember, the two frequencies (EXT and PLL) were told to us by the host when it enabled health monitoring
            if (true == healthMonitorState.currentPllLockState)
            {
                // running from PLL
                newClockFreq = healthMonitorState.pllFreqHz;
            }
            else
            {
                // running from EXT clock
                newClockFreq = healthMonitorState.extClkFreqHz;
            }

            (void)SystemMgrSetClockSpeed(newClockFreq);     // cannot return a failure, we're in an ISR
        }
        else
        {
            // PLL lock status is unchanged from last time, nothing to do
        }
    }
}

/// Handles entry to an operating phase
///
/// \param[in] phase    Phase being entered. INITIALIZE or SHUTDOWN are valid, others do nothing
/// \returns Error status
/// \retval ERROR_SUCCESS   Successful execution
/// \retval ERROR_NOSPC   System failed to register clock-change notifier
///
ErrorT HealthMonitorOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT status = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            DiagSetCheckpoint(DIAG_CHECKPOINT_HEALTH_MONITOR_INIT);
            (void)memset(&healthMonitorState, 0, sizeof(healthMonitorState));
            healthMonitorState.wdgTimerIntervalMicrosecs = HEATH_MONITOR_INITIAL_WDG_TIMER_INTERVAL_MICROSECS;
            healthMonitorState.stackHwmDangerZoneSizeLongWords = HEALTH_MONITOR_HWM_DANGER_ZONE_SIZE_LONG_WORDS;
            healthMonitorState.wdgInterruptCtrMax =
                (HEALTH_MONITOR_WATCHDOG_TIMEOUT_MICROSECS / healthMonitorState.wdgTimerIntervalMicrosecs) -
                1U;         // -1 because the WDOGRES occurs one timer period after

            // Register for clock-change notifications
            status = SystemMgrRegisterClockSpeedChangeNotifier(HealthMonitorOnClockChange);
            if (ERROR_SUCCESS != status)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_REGISTER_CLOCK_CHANGE_NOTIFIER_FAILED,
                    (uint32_t)status);
            }
            break;

        case SYSTEM_MGR_PHASE_SHUTDOWN:
            DiagSetCheckpoint(DIAG_CHECKPOINT_HEALTH_MONITOR_ENTER_SHUTDOWN);

            // want to ignore any watchdog timer interrupts from now on
            healthMonitorState.isWatchdogActive = false;

            // disable watchdog interrupt and counter
            (void)WatchdogDrvDisable();                                 // driver only ever returns ERROR_SUCCESS

            // Clear any pending interrupts at the NVIC
            (void)InterruptsClearInterruptPending(INTERRUPT_WATCHDOG);  // driver only ever returns ERROR_SUCCESS

            break;

        default:
            // Do nothing for any unhandled phases
            break;
    }

    return status;
}

/// Handles a periodic watchdog timer event
///
/// Called by System Manager in response to a watchdog event.
/// Will perform stack HWM and Video Auth SS check and clear the WDG timer counter
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Successful execution
/// \retval ERROR_SYSERR    At least one of the health checks failed
///
ErrorT HealthMonitorOnWatchdogEvent(void)
{
    uint16_t stackUsedSizeLongWords;
    HealthMonitorVcoreStatus vCoreStatus;
    ErrorT retVal;

    // Perform the stack HWM check first - need to FATAL ERROR within 100ms if stack HWM has reached the MPU stack guard zone.
    // This gives us time to fatal error gracefully rather than incurring nearly instant death by the MPU.
    retVal = HealthMonitorGetStackUsedSizeLongWords(&stackUsedSizeLongWords);
    if (ERROR_SUCCESS == retVal)
    {
        // We know the stack details, so test usage
        if (stackUsedSizeLongWords > healthMonitorState.stackMaxAllowedSizeLongWords)
        {
            SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_STACK_OVERRUN,
                (uint32_t)stackUsedSizeLongWords);
            retVal = ERROR_SYSERR;
        }
        else
        {
            // Stack OK, now check the Video Auth Subsys
            (void)HealthMonitorGetVCoreStatus(&vCoreStatus);    // function only tests for NULL pointer so cannot fail here
            if (HEALTH_MONITOR_VCORE_ERROR_STATUS_GOOD != vCoreStatus.errorStatus)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_VCORE_ERROR,
                    (uint32_t)vCoreStatus.errorStatus);
                retVal = ERROR_SYSERR;
            }
            else
            {
                // Everything in good health, reset the WDG interrupt counter
                healthMonitorState.wdgInterruptCtr = 0;
            }
        }
    }
    else
    {
        // HealthMonitorGetStackUsedSizeLongWords() would already have raised a FATAL_ERROR
    }

    return retVal;
}

/// Handles the EnableMonitoring host command
///
/// \param[in]  commandCode Command to handle (ENABLE_MONITORING)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Error status
/// \retval ERROR_SUCCESS   Successful execution
/// \retval ERROR_RANGE     Invalid freq params or unsupported cipher mode
/// \retval ERROR_SYSERR    Called with wrong command code or CryptoMgr failed initialisation
/// \retval ERROR_ACCESS    Session hasn't been initiated
/// \retval ERROR_INVAL     Invalid key type
/// \retval ERROR_BADMSG    Message is not authentic
/// \retval ERROR_ALREADY   Health Monitor is already running
///
ErrorT HealthMonitorOnEnableMonitoring(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT status;
    InitMpuStackConfigType stackConfig;
    HealthMonitorSystemClockFreqT decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent;

    HealthMonitorEnableMonitoringRequestParamsT* requestParams = (HealthMonitorEnableMonitoringRequestParamsT*)params;

    DiagSetCheckpoint(DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    if (HOST_COMMAND_CODE_ENABLE_MONITORING != commandCode)
    {
        // incorrect command
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_ENABLE_MONITORING_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else
    {
        // Only allowed to enable Health Monitoring once
        if (true == healthMonitorState.isWatchdogActive)
        {
            status = ERROR_ALREADY;
        }
        else
        {
            // Authenticate and decrypt command parameters
            status = SessionMgrDecryptCommandRequestParams(commandCode, (uint8_t*)requestParams,
                (uint8_t*)&decryptedParams,
                    sizeof(HealthMonitorSystemClockFreqT));
            if (ERROR_SUCCESS == status)
            {
                // - validate parameters
                if ((SYSTEM_MGR_MAX_EXTCLOCK_RATE_HZ >= decryptedParams.extClkFreqHz) &&
                    (SYSTEM_MGR_MIN_EXTCLOCK_RATE_HZ <= decryptedParams.extClkFreqHz) &&
                    (SYSTEM_MGR_MAX_CLOCK_RATE_HZ >= decryptedParams.pllFreqHz) &&
                    (SYSTEM_MGR_MIN_CLOCK_RATE_HZ <= decryptedParams.pllFreqHz))
                {
                    // Good params, so calculate the Stack HWM fatal error threshold
                    (void)InitMpuGetStackInformation(&stackConfig);     // get the stack information. Can only return ERROR_SUCCESS
                    healthMonitorState.stackMaxAllowedSizeLongWords =
                        stackConfig.sizeLongWords -
                        stackConfig.stackGuardRegionSizeLongWords -
                        healthMonitorState.stackHwmDangerZoneSizeLongWords;

                    // And start the watchdog
                    status = HealthMonitorStartWatchdog(&decryptedParams);
                }
                else
                {
                    // Host command freq parameters are invalid, don't do anything, and don't update HealthMon internal status
                    (void)LifecycleMgrGetCurrentLcs(&lcsCurrent);               // already in error condition so ignore return value
                    if (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == lcsCurrent)
                    {
                        // only allowed to return EBADMSG in secure mode
                        status = ERROR_BADMSG;
                    }
                    else
                    {
                        status = ERROR_RANGE;
                    }
                }
            }
        }
    }

    return status;
}

/// Handles the GetStatus host command
///
/// \param[in]  commandCode Command to handle (GET_STATUS)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Error status
/// \retval ERROR_SUCCESS   Command executed successfully
/// \retval ERROR_SYSERR    Called with wrong command code.
///
ErrorT HealthMonitorOnGetStatus(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT status;

    HealthMonitorGetStatusResponseParamsT* responseParams = (HealthMonitorGetStatusResponseParamsT*)params;
    LifecycleMgrLifecycleStateT lcsCurrent;
    uint16_t stackUsedSizeLongWords;

    DiagSetCheckpoint(DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);

    if (HOST_COMMAND_CODE_GET_STATUS != commandCode)
    {
        // incorrect command
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STATUS_BAD_COMMAND, (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else
    {
        responseParams->romVersion = (uint16_t)SystemMgrGetRomVersion();
        responseParams->patchVersion = (uint16_t)PatchMgrGetPatchVersion();
        responseParams->currentPhase = (uint16_t)SystemMgrGetCurrentPhase();
        status = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if (ERROR_SUCCESS == status)
        {
            responseParams->currentLcs = (uint16_t)lcsCurrent;
            status = HealthMonitorGetStackUsedSizeLongWords(&stackUsedSizeLongWords);
            // NOTE: HealthMonitorGetStackUsedSizeLongWords() will call a fatal error if there is any internal failure
            if (ERROR_SUCCESS == status)
            {
                responseParams->stackUsedLongWords = stackUsedSizeLongWords;
            }
            else
            {
                responseParams->stackUsedLongWords = HEALTH_MONITOR_STACK_USED_INVALID;
            }
        }
        else
        {
            responseParams->currentLcs = HEALTH_MONITOR_LIFE_CYCLE_INVALID;
        }
    }

    return status;
}

/// Handles the Clock Change event from System Manager. Recalculates and reloads watchdog timer value if it's running
///
/// \param[in] clockSpeedHz
/// \returns Error status
/// \retval ERROR_SUCCESS   Command executed successfully
/// \retval ERROR_RANGE    Out of range clock frequency
///
ErrorT HealthMonitorOnClockChange(uint32_t const clockSpeedHz)
{
    uint32_t wdgTimerLoadValue;
    ErrorT retVal = ERROR_SUCCESS;

    // Recalculate and reload the watchdog timer value if the watchdog is running
    if (true == healthMonitorState.isWatchdogActive)
    {
        // Is the requested frequency in range?
        if ((SYSTEM_MGR_MAX_CLOCK_RATE_HZ >= clockSpeedHz) && (SYSTEM_MGR_MIN_CLOCK_RATE_HZ <= clockSpeedHz))
        {
            // Stop the watchdog
            (void)WatchdogDrvDisable();     // can only return ERROR_SUCCESS

            // reload and restart watchdog timer with recalculated value
            wdgTimerLoadValue =
                (uint32_t)(((uint64_t)clockSpeedHz * healthMonitorState.wdgTimerIntervalMicrosecs) /
                           HEALTH_MONITOR_ONE_MILLION_DIVISOR);

            (void)WatchdogDrvStart(wdgTimerLoadValue);       // can only return ERROR_SUCCESS
        }
        else
        {
            // invalid requested freq
            retVal = ERROR_RANGE;
        }
    }
    else
    {
        // watchdog not running, nothing to do
    }

    return retVal;
}

//----------------------------------------------------------------------------------
//   These functions aren't currently used. They're available for use by patch code
//----------------------------------------------------------------------------------

/// Optional function to change the watchdog timer interval. Otherwise it defaults to 50ms.
/// Note: The watchdog timeout is unaffected by changing this - it is always one second
///
/// @param[in] intervalMicroseconds  The new watchdog timer interval (period) in us
/// \returns void
///
void HealthMonitorSetWdogTimerInterval(uint32_t const intervalMicroseconds)
{
    healthMonitorState.wdgTimerIntervalMicrosecs = intervalMicroseconds;

    // The watchdog timer period has changed. Need to re-calculate how many of the
    // new timer-periods to count to maintain the one-second watchdog timeout
    healthMonitorState.wdgInterruptCtrMax =
        (HEALTH_MONITOR_WATCHDOG_TIMEOUT_MICROSECS / healthMonitorState.wdgTimerIntervalMicrosecs) -
        1U;           // -1 because the WDOGRES occurs one timer period after
}

/// Optional function to change the safety zone size for the stack HWM check. Otherwise it defaults to 16 longwords.
/// If the stack HWM gets in this safety zone the FW will generate a FATAL Error.
/// The HW MPU will kick-in if the stack traverses this safety zone.
///
/// @param[in] thresholdLongWords The number of longwords between maximum allowed stack HWM and MPU no-go zone
/// \returns void
///
void HealthMonitorSetStackCheckThreshold(uint16_t const thresholdLongWords)
{
    InitMpuStackConfigType stackConfig;

    healthMonitorState.stackHwmDangerZoneSizeLongWords = thresholdLongWords;

    // re-calculate the maximum stack HWM fatal error threshold
    (void)InitMpuGetStackInformation(&stackConfig);     // get the stack information
    healthMonitorState.stackMaxAllowedSizeLongWords =
        stackConfig.sizeLongWords -
        stackConfig.stackGuardRegionSizeLongWords -
        healthMonitorState.stackHwmDangerZoneSizeLongWords;
}

/// @} endgroup healthmonitor

