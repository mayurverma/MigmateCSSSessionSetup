#if !defined(SRC__HEALTHMONITOR__INC__HEALTHMONITOR_H)
#define      SRC__HEALTHMONITOR__INC__HEALTHMONITOR_H

//***********************************************************************************
/// \file
/// Health Monitor public API
///
/// \defgroup healthmonitor   Health Monitor
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

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Watchdog timer interrupt handler
extern void Watchdog_IrqHandler(void);

// Handles entry to an operating phase
extern ErrorT HealthMonitorOnPhaseEntry(SystemMgrPhaseT const phase);

// Handles a watchdog event
extern ErrorT HealthMonitorOnWatchdogEvent(void);

// Handles the EnableMonitoring host command
extern ErrorT HealthMonitorOnEnableMonitoring(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the GetStatus host command
extern ErrorT HealthMonitorOnGetStatus(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the Clock Change event from System Manager
extern ErrorT HealthMonitorOnClockChange(uint32_t const clockSpeedHz);

// Optionally sets the Watchdog Timer interval. The default is 50ms
extern void HealthMonitorSetWdogTimerInterval(uint32_t const intervalMicroseconds);

// Optionally sets the safety gap (in Longwords) between Stack's HWM allowed maximum, and the MPU's no-go zone.
// Defaults to 16 longwords
extern void HealthMonitorSetStackCheckThreshold(uint16_t const thresholdLongWords);

/// @} endgroup healthmonitor
#endif  // !defined(SRC__HEALTHMONITOR__INC__HEALTHMONITOR_H)

