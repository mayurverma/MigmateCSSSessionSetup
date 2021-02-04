#if !defined(SRC__DIAG__INC__DIAG_H)
#define      SRC__DIAG__INC__DIAG_H
//******************************************************************************
/// \file
///
/// Provides Diagnostic platform-layer functions
///
/// \defgroup diag Diagnostics
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
#include "DiagCheckpoint.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

/// Maximum size of fatal error dump region in shared-memory
#define DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS    64U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Fatal error dump buffer
typedef uint32_t* DiagFatalErrorDumpBufferT;

/// Fatal error dump handler type
typedef void (* DiagFatalErrorDumpHandlerT)(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords);

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

#if defined(BUILD_TYPE_debug)
#include <stdio.h>
#define DiagPrintf(...)     printf(__VA_ARGS__)
#else
#define DiagPrintf(...)
#endif

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Initialise the Diagnostics platform-layer
extern ErrorT DiagInit(void);
// Sets the Checkpoint register, and tests the Pausepoint.
extern void DiagSetCheckpoint(DiagCheckpointT const checkpoint);
// Sets the Checkpoint and Info registers, and tests the Pausepoint.
extern void DiagSetCheckpointWithInfo(DiagCheckpointT const checkpoint, uint16_t info);
// Sets the Checkpoint and Info registers (unprotected), and tests the Pausepoint.
extern void DiagSetCheckpointWithInfoUnprotected(DiagCheckpointT const checkpoint, uint16_t info);
// Dumps diagnostics data from each registered component to shared-memory
extern void DiagOnFatalError(void);
// Registers a fatal error dump handler
extern ErrorT DiagRegisterFatalErrorDumpHandler(DiagFatalErrorDumpHandlerT handler);
// Enables access to info register
extern void  DiagEnableInfo(bool const enable);

/// @} endgroup diag
#endif  // !defined(SRC__DIAG__INC__DIAG_H)

