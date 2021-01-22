#if !defined(SRC__DEBUGUARTDRV__INC__DEBUGUARTDRV_H)
#define      SRC__DEBUGUARTDRV__INC__DEBUGUARTDRV_H
//******************************************************************************
/// \file
///
/// Debug Uart hardware abstraction
///
/// \defgroup debuguartdrv Debug Uart Driver
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

#include "Common.h"

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

// Initialize the debug UART to 156Mbps, 8, n, 1 (debug non-asic builds)
extern void DebugUartDrvInit(void);

// Transmit a single char from the debug UART
// Function will block if it has to wait for space in Tx Buffer
extern void DebugUartDrvPutc(char txChar);

/// @} endgroup debuguartdrv
#endif  // !defined(SRC__DEBUGUARTDRV__INC__DEBUGUARTDRV_H)

