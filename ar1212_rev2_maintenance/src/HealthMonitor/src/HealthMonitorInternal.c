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
#include "LifecycleMgr.h"
#include "WatchdogDrv.h"
#include "Init.h"
#include "Interrupts.h"

#include "HealthMonitor.h"
#include "HealthMonitorInternal.h"
#include "crypto_vcore.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
/// Initial value for tracking variable used to determine stack usage
#define HEALTH_MONITOR_WATERMARK_NOT_FOUND   0xFFFFFFFFU

/// Size of stack water mark search windows
#define HEALTH_MONITOR_WATERWARK_SEARCH_WINDOW_LONG_WORDS   8U
//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Searches stack to find high watermark i.e how much stack has been used
///
/// Used binary search to find how much stack has been used. Stack is initialised with
/// HEALTH_MONITOR_STACK_INIT_VALUE. The base of the stack is protected by MPU so this
/// function does not access this region. The stack size is effectively Image$$MAIN_STACK$$ZI$$Length minus
/// the protected region
///
/// The search window HEALTH_MONITOR_WATERWARK_SEARCH_WINDOW_LONG_WORDS is centred around the  search point
/// i.e. we search +/-(HEALTH_MONITOR_WATERWARK_SEARCH_WINDOW_LONG_WORDS/2) locations. If the search finds
/// an instance of HEALTH_MONITOR_STACK_INIT_VALUE it will move the search window up in memory ((larger value)
/// for next search, if it does not find this value it will move window down in memory (assume more stack has been used)
/// Each new search reduces the stackRegionLongWords by 2 and applies the search point to the  mid point of this region
///
/// For a 1024 stack it we assume the stack has been exhausted the search will operate in the following manor
/// Search window applied to location 512 (nothing found search down)
/// Search window applied to location 256 (nothing found search down)
/// Search window applied to location 128 (nothing found search down)
/// Search window applied to location  64 (nothing found search down)
/// Search window applied to location  32 (nothing found search down)
/// Search window applied to location  16 (nothing found search down)
/// Search window applied to location   8 In this case as locations 0-7 are protected will only search 8-11
///
/// For a 1024 stack it we assume the stack has never been used the search will operate in the following manor
/// Search window applied to location  512 (nothing found up down)
/// Search window applied to location  768 (nothing found up down)
/// Search window applied to location  896 (nothing found up down)
/// Search window applied to location  960 (nothing found up down)
/// Search window applied to location  992 (nothing found up down)
/// Search window applied to location 1008 (nothing found up down)
/// Search window applied to location 1016 (nothing found up down)
/// Search window applied to location 1020 will search from 1016-1023
///
/// param[out]      Pointer to memory to populate with stackUsedSizeLongWords
/// \returns Command success/failure
/// \retval ERROR_SUCCESS       Successful completion
/// \retval ERROR_SYSERR        Internal fatal error
///
ErrorT HealthMonitorGetStackUsedSizeLongWords(uint16_t* const stackUsedSizeLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    InitMpuStackConfigType stackConfig;
    const uint32_t windowSizeLongWords = HEALTH_MONITOR_WATERWARK_SEARCH_WINDOW_LONG_WORDS;
    uint32_t stackRegionLongWords;
    uint32_t index = 0;
    uint32_t i;
    uint32_t foundInit = 0U;
    int32_t direction = 1;
    uint32_t locationLongWords = 0U;
    uint32_t lastFoundLocationLongWords = HEALTH_MONITOR_WATERMARK_NOT_FOUND;
    uint16_t usedSizeLongWords;

    if (NULL == stackUsedSizeLongWords)
    {
        error = ERROR_SYSERR;
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_USED_SIZE_NULL_PTR,
            (uint32_t)error);
    }
    else
    {
        error = InitMpuGetStackInformation(&stackConfig);
        if (ERROR_SUCCESS != error)
        {
            SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_INFORMATION_ERROR,
                (uint32_t)error);
        }
        else
        {
            stackRegionLongWords = (stackConfig.sizeLongWords / 2U);
            DiagPrintf("HealthMonitorGetStackUsedSizeLongWords: Stack size %d (protected %d)\n",
                (uint32_t)stackConfig.sizeLongWords,
                (uint32_t)stackConfig.stackGuardRegionSizeLongWords);
            // Search is started at mid-point of whole stack region (including protected region)
            do
            {
                if (0 < direction)
                {
                    index += stackRegionLongWords;
                }
                else
                {
                    index -= stackRegionLongWords;
                }
                foundInit = 0U;
                for (i = 0; i < windowSizeLongWords; i++)
                {
                    locationLongWords = index + i - (windowSizeLongWords / 2U);
                    if (stackConfig.stackGuardRegionSizeLongWords <= locationLongWords)
                    {
                        if (HEALTH_MONITOR_STACK_INIT_VALUE == stackConfig.base[locationLongWords])
                        {
                            foundInit += 1U;
                            lastFoundLocationLongWords = locationLongWords;
                        }
                    }
                }
                if (0U < foundInit)
                {
                    direction = 1;
                }
                else
                {
                    direction = -1;
                }

                stackRegionLongWords = stackRegionLongWords / 2U;
            }
            while ((windowSizeLongWords / 2U) <= stackRegionLongWords);

            DiagPrintf("HealthMonitorGetStackUsedSizeLongWords: index:%d, lastFoundLocationLongWords:%d\n",
                (uint32_t)index,
                (uint32_t)lastFoundLocationLongWords);

            if (HEALTH_MONITOR_WATERMARK_NOT_FOUND == lastFoundLocationLongWords)
            {
                usedSizeLongWords = (uint16_t)stackConfig.sizeLongWords;
            }
            else
            {
                // lastFoundLocationLongWords is an index i.e. 0 - (stackConfig.sizeLongWords - 1) thus
                // if the last found location was 1023 (stack not used) we need to adjust the result by -1
                // i.e.  1024 - 1023 -1 = 0
                usedSizeLongWords = (uint16_t)(stackConfig.sizeLongWords - lastFoundLocationLongWords - 1U);
            }
            *stackUsedSizeLongWords = usedSizeLongWords;
        }
    }

    return error;
}

/// Get the status of the vCore
///
/// param[out]      Pointer to memory to populate VCore Status
/// \returns Command success / failure
/// \retval ERROR_SUCCESS       Successful completion
/// \retval ERROR_SYSERR        Function called with NULL pointer
///
ErrorT HealthMonitorGetVCoreStatus(HealthMonitorVcoreStatus* const vCoreStatus)
{
    ErrorT retVal = ERROR_SUCCESS;
    uint32_t aesCmacStatus;

    if (NULL == vCoreStatus)
    {
        retVal = ERROR_SYSERR;
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_VCORE_STATUS_NULL_PTR,
            (uint32_t)retVal);
    }
    else
    {
        aesCmacStatus = MacDrvGetCmacStatus();
        // Check the AESCMAC MEMFULL bit is clear
        if (0U != (aesCmacStatus & CRYPTO_VCORE_AESCMAC_STATUS_AESCMAC_MEMFULL__MASK))
        {
            vCoreStatus->errorStatus = HEALTH_MONITOR_VCORE_ERROR_STATUS_CMAC_MEMFULL;
            vCoreStatus->statusCode = 0U;       // for future use
        }
        else
        {
            // Add more VCore testing here as the HW monitoring develops
            vCoreStatus->errorStatus = HEALTH_MONITOR_VCORE_ERROR_STATUS_GOOD;
            vCoreStatus->statusCode = 0U;       // for future use
        }
    }

    return retVal;
}

/// Internal function to start the watchdog.
/// In addition it saves the two new clock frequencies (EXT & PLL) and calculated watchdog divide ratios.
/// This internal function can only fail if SystemMgrSetClockSpeed() fails
///
/// param[in] frequencyParams pointer to Ext and PLL frequencies
/// \returns Command success / failure
/// \retval ERROR_SUCCESS       Successful completion
/// \retval ERROR_RANGE         Frequency parameter out-of-range
///
ErrorT HealthMonitorStartWatchdog(HealthMonitorSystemClockFreqT* const frequencyParams)
{
    uint32_t newClockFrequency, wdgTimerLoadValue;
    ErrorT retVal;

    // The command has told us the two possible clock frequencies, ext and pll.
    // The EnableMonitoring command is the only way the Health Monitor can know the clock frequencies.
    // Need to determine if we're currently using the EXT clock or the PLL
    healthMonitorState.currentPllLockState = SystemDrvIsPllLocked();
    if (true == healthMonitorState.currentPllLockState)
    {
        newClockFrequency = frequencyParams->pllFreqHz;
    }
    else
    {
        newClockFrequency = frequencyParams->extClkFreqHz;
    }

    // Let the SystemManager know the new clock frequency. This will cause HealthMonitor's clock-change
    // notifier to be called in the same thread - HealthMonitorOnClockChange().
    // This is OK (and will be ignored) since the watchdog is not yet running.
    retVal = SystemMgrSetClockSpeed(newClockFrequency);

    if (ERROR_SUCCESS == retVal)
    {
        // Save both frequencies (in case PLL lock retVal changes due to Video Streaming stopping / starting)
        // and calculate the watchdog timer load values for those two frequencies

        healthMonitorState.extClkFreqHz = frequencyParams->extClkFreqHz;
        healthMonitorState.pllFreqHz = frequencyParams->pllFreqHz;

        // Calculate and store the watchdog timer divider values to give the desired timer period, for both EXT and PLL freqs.
        // These will get used every time the IrqHandler detects that the PLL lock state (and hence the frequency) has changed.
        // divider value = ClockFreqHz / TimerFreqHz
        //               = ClockFreqHz x TimerPeriod in Secs
        //               = ClockFreqHz x TimerPeriodInMicrosecs / 1000000
        // Arithmetic done at 64bit because 156MHz (in Hz) x 50ms (in us) = 0x718143AB000, ie  will take 43 bits
        healthMonitorState.wdgTimerReloadValuePll =
            (uint32_t)(((uint64_t)frequencyParams->pllFreqHz *
                        healthMonitorState.wdgTimerIntervalMicrosecs) /
                       HEALTH_MONITOR_ONE_MILLION_DIVISOR);

        healthMonitorState.wdgTimerReloadValueExt =
            (uint32_t)(((uint64_t)frequencyParams->extClkFreqHz *
                        healthMonitorState.wdgTimerIntervalMicrosecs) /
                       HEALTH_MONITOR_ONE_MILLION_DIVISOR);

        // the watchdog timer has a new load value
        if (true == healthMonitorState.currentPllLockState)
        {
            wdgTimerLoadValue = healthMonitorState.wdgTimerReloadValuePll;
        }
        else
        {
            wdgTimerLoadValue = healthMonitorState.wdgTimerReloadValueExt;
        }

        // start the watchdog, clear any pending WDG timer interrupts, enable WDG timer interrupts
        (void)WatchdogDrvInit();                                    // Currently doesn't do anything, but here in case we patch it
        (void)WatchdogDrvDisable();                                 // Prevent anything else from disabling the watchdog
        (void)WatchdogDrvStart(wdgTimerLoadValue);                  // load counter, enable WDOGINT
        (void)InterruptsClearInterruptPending(INTERRUPT_WATCHDOG);  // clear any spurious interrupt at NVIC

        // Configure CPU for 'wait-on-reset' mode to prevent firmware restarting
        // when the watchdog reset occurs
        SystemDrvEnableCpuWait(true);

        (void)InterruptsEnableInterrupt(INTERRUPT_WATCHDOG, true);  // enable interrupt at NVIC

        // WDG is now running
        healthMonitorState.isWatchdogActive = true;
    }

    return retVal;
}

/// @} endgroup healthmonitor

