#if !defined(SRC__SYSTEMDRV__SRC__SYSTEMDRVINTERNAL_H)
#define      SRC__SYSTEMDRV__SRC__SYSTEMDRVINTERNAL_H
//******************************************************************************
/// \file
///
/// System driver internal functions and types
///
/// \addtogroup systemdrv System driver
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

#include <SystemDrv.h>

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

/// Terminate processor execution
///
/// Halts the processor
///
/// \returns Never
extern void SystemDrvTerminate(void);

/// @} endgroup systemdrv
#endif  // !defined(SRC__SYSTEMDRV__SRC__SYSTEMDRVINTERNAL_H)

