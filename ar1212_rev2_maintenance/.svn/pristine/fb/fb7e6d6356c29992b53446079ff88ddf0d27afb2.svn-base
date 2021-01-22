#if !defined(SRC__SYSTICK__INC__SYSTICK_H)
#define      SRC__SYSTICK__INC__SYSTICK_H

//***********************************************************************************
/// \file
/// SysTick driver
///
/// \defgroup systickdrv   SysTick driver
///
/// This is ...
///
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

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Optional handler for the SysTick interrupt
typedef void (* SysTickDrvHandler)(uint32_t const numTicks);

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern ErrorT SysTickDrvInit(void);
extern ErrorT SysTickDrvStart(uint32_t const clocks);
extern ErrorT SysTickDrvStop(void);
extern uint32_t SysTickDrvGetCurrentTicks(void);
extern SysTickDrvHandler SysTickDrvSetHandler(SysTickDrvHandler handler);

// @} endgroup systickdrv
#endif  // !defined(SRC__SYSTICK__INC__SYSTICK_H)

