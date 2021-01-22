#if !defined(SRC__COMMANDDRV__INC__COMMANDDRV_H)
#define      SRC__COMMANDDRV__INC__COMMANDDRV_H
//******************************************************************************
/// \file
///
/// Command register hardware abstraction
///
/// \defgroup commanddrv Command Driver
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

// Initialize the Command Driver and clears the shared RAM
extern ErrorT CommandDrvInit(void);

// Retrieve the current command
extern uint16_t CommandDrvGetCommand(void);

// Returns a pointer to the command parameters in shared-memory
extern uint32_t * CommandDrvGetCommandParams(void);

// Set the command response. The relevant command handler should already have put its response in the shared memory
extern void CommandDrvSetResponse(uint16_t const response);

// Is there a command in progress?
extern bool CommandDrvIsCommandInProgress(void);

/// @} endgroup commanddrv
#endif  // !defined(SRC__COMMANDDRV__INC__COMMANDDRV_H)

