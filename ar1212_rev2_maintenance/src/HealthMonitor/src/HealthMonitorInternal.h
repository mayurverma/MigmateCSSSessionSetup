#if !defined(SRC__HEALTHMONITOR__SRC__HEALTHMONITORINTERNAL_H)
#define      SRC__HEALTHMONITOR__SRC__HEALTHMONITORINTERNAL_H

//***********************************************************************************
/// \file
/// Health Monitor private defines
///
/// \addtogroup healthmonitor   Health Monitor
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
//***********************************************************************************

#include "Common.h"
#include "CommandHandler.h"
#include "MacDrv.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
/// Stack is initialised with the following value
#define HEALTH_MONITOR_STACK_INIT_VALUE   0xFEEDFACEU

/// Response used to indicate an invalid stack calculation
#define HEALTH_MONITOR_STACK_USED_INVALID   0xFFFFU

// Return error status values for use with Vcore monitoring
#define     HEALTH_MONITOR_VCORE_ERROR_STATUS_GOOD              0U  ///< Vcore health status good
#define     HEALTH_MONITOR_VCORE_ERROR_STATUS_GMAC_STUCK        1U  ///< Vcore health status error GMAC not changing between frames
#define     HEALTH_MONITOR_VCORE_ERROR_STATUS_CMAC_STUCK        2U  ///< Vcore health status error CMAC not changing between frames
#define     HEALTH_MONITOR_VCORE_ERROR_STATUS_GMAC_MEMFULL      3U  ///< Vcore health status GMAC buffer memory overrun
#define     HEALTH_MONITOR_VCORE_ERROR_STATUS_CMAC_MEMFULL      4U  ///< Vcore health status CMAC buffer memory overrun

/// Response used to indicate an invalid life cycle value has been detected
#define HEALTH_MONITOR_LIFE_CYCLE_INVALID   0xFFFFU

/// The divisor used for Hz to MHz calculations
#define HEALTH_MONITOR_ONE_MILLION_DIVISOR  1000000U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// Represents Health Monitor state
typedef struct
{
    uint32_t extClkFreqHz;                      ///< The external clock frequency in Hz
    uint32_t pllFreqHz;                         ///< The PLL clock frequency in Hz
    uint32_t wdgTimerIntervalMicrosecs;         ///< The time period in microseconds of the watchdog periodic timer
    uint32_t wdgTimerReloadValueExt;            ///< The value that is loaded into the watchdog timer when running on EXT clock
    uint32_t wdgTimerReloadValuePll;            ///< The value that is loaded into the watchdog timer when running on PLL clock
    uint32_t wdgInterruptCtr;                   ///< The current count of uncleared Watchdog timer interrupts
    uint32_t wdgInterruptCtrMax;                ///< The maximum allowed count of uncleared Watchdog timer interrupts before system reset occurs
    uint32_t stackMaxAllowedSizeLongWords;      ///< Size of legitimate area that the FW stack may occupy
    bool isWatchdogActive;                      ///< True if the watchdog is active
    bool currentPllLockState;                   ///< True if the PLL is locked
    uint16_t stackHwmDangerZoneSizeLongWords;   ///< Size of zone that the stack HWM must not enter. Zone adjacent to HW MPU
} HealthMonitorStateT;

/// Represents the EnableMonitoring host command request parameters
typedef struct
{
    uint32_t extClkFreqHz;  ///< The external clock frequency
    uint32_t pllFreqHz;     ///< PLL clock frequency
} HealthMonitorSystemClockFreqT;

/// Represents the EnableMonitoring host command request parameters
typedef struct
{
    HealthMonitorSystemClockFreqT encryptedParams;  ///< The encrypted parameters
    uint16_t mac[8];                                ///< 128bit MAC of EnableMonitoring command for encrypted parameters
} HealthMonitorEnableMonitoringRequestParamsT;

/// Represents the GetStatus host command response parameters
typedef struct
{
    uint16_t currentPhase;        ///< The current operating phase
    uint16_t currentLcs;          ///< The current life cycle state
    uint16_t romVersion;          ///< The ROM version (major.minor)
    uint16_t patchVersion;        ///< The patch version
    uint16_t stackUsedLongWords;  ///< High-watermark of the stack
    // INFO:  add more here if required
} HealthMonitorGetStatusResponseParamsT;

/// The return type when getting the Vcore's status
typedef struct
{
    uint16_t errorStatus;       ///< Indicates error status
    uint16_t statusCode;        ///< Indicates general status
} HealthMonitorVcoreStatus;

//---------------------------------------------------------------------------------
//                                  Locals
//---------------------------------------------------------------------------------
extern HealthMonitorStateT healthMonitorState;
extern HealthMonitorSystemClockFreqT healthMonitorClockFreqs;

// Get the Vcore status
extern ErrorT HealthMonitorGetVCoreStatus(HealthMonitorVcoreStatus* const vCoreStatus);

// Start the watchdog using one of the supplied frequencies - which one depends upon current PLL lock state
extern ErrorT HealthMonitorStartWatchdog(HealthMonitorSystemClockFreqT* const decryptedParams);

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Searches stack to find high watermark i.e how much stack has been used
extern ErrorT HealthMonitorGetStackUsedSizeLongWords(uint16_t* const stackUsedSizeLongWords);

/// @} endgroup healthmonitor
#endif  // !defined(SRC__HEALTHMONITOR__SRC__HEALTHMONITORINTERNAL_H)

