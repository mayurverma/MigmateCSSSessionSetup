#if !defined(SRC__DIAG__SRC__DIAGINTERNAL_H)
#define      SRC__DIAG__SRC__DIAGINTERNAL_H
//******************************************************************************
/// \file
///
/// Provides Diagnostic internal header file
///
/// \addtogroup diag
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
/// Maximum number of registered fatal error dump handlers
#define DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS  16

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// Diag control/status structure
typedef struct
{
    bool infoEnabled;             ///< Controls access to Info register (blocked in secure by default)
} DiagT;
//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Diag control/status structure
extern DiagT diagControl;

extern DiagFatalErrorDumpHandlerT diagFatalErrorDumpHandlers[DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS];

// Block until Pause bit is cleared
extern void DiagOnWaitPaused(void);

/// @} endgroup diag
#endif  // !defined(SRC__DIAG__SRC__DIAGINTERNAL_H)

