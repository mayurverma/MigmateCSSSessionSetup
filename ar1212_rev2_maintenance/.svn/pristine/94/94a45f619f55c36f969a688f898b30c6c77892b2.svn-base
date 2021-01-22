#if !defined(SRC__WATCHDOGDRV__INC__WATCHDOGDRV_H)
#define      SRC__WATCHDOGDRV__INC__WATCHDOGDRV_H
//******************************************************************************
/// \file
///
/// Watchdog timer hardware abstraction
///
/// \defgroup watchdogdrv Watchdog Driver
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "AR0820.h"
#include "Common.h"

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initialize the Watchdog Driver
extern ErrorT WatchdogDrvInit(void);

// Configure and start the watchdog
extern ErrorT WatchdogDrvStart(uint32_t const timeOutClocks);

// Clear the watchdog interrupt ('pat' the watchdog)
extern ErrorT WatchdogDrvClearInterrupt(void);

// Disable the watchdog
extern ErrorT WatchdogDrvDisable(void);

/// @} endgroup watchdogdrv
#endif  // !defined(SRC__WATCHDOGDRV__INC__WATCHDOGDRV_H)

