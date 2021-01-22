#if !defined(SRC__WATCHDOGDRV__SRC__WATCHDOGDRVINTERNAL_H)
#define      SRC__WATCHDOGDRV__SRC__WATCHDOGDRVINTERNAL_H
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
#include "WatchdogDrv.h"

#if defined(BUILD_TARGET_ARCH_x86)
extern uint32_t watchdogArray[769];
#undef WATCHDOG_REG_BASE
#undef WATCHDOG_REGS

#define WATCHDOG_REG_BASE          ((uint32_t*)            &watchdogArray[0])
#define WATCHDOG_REGS              ((WDOG_TypeDef*)        WATCHDOG_REG_BASE)
#endif

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

// Writing 0x1ACCE551 to the WATCHDOG lock register enables write access to all other registers.
// Writing any other value disables write access to all other registers.
#define WDOG_LOCK_ENABLE_KEY                               0x1ACCE551U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

/// @} endgroup watchdogdrv
#endif  // !defined(SRC__WATCHDOGDRV__SRC__WATCHDOGDRVINTERNAL_H)

