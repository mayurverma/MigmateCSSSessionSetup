#if !defined(SRC__COMMANDHANDLER__INC__COMMANDHANDLER_H)
#define      SRC__COMMANDHANDLER__INC__COMMANDHANDLER_H

//***********************************************************************************
/// \file
/// Command Handler public API
///
/// \defgroup commandhandler    Command Handler
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
#include "SystemMgr.h"

// Include the auto-generated host command codes
#include "CommandHandlerHostCommandCode.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define COMMAND_HANDLER_MAX_PATCH_COMMANDS          4U
#define COMMAND_HANDLER_NUM_COMMANDS_ROM_TABLE      13U

//---------------------------------------------------------------------------------
//                           LOCAL Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents a pointer to the command parameters
typedef uint32_t* CommandHandlerCommandParamsT;

/// Represents a pointer to a command handler function
typedef ErrorT (* CommandHandlerFunctionT)(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params);

/// The structure of an entry in the host command table. Applies to ROM table and RAM table
typedef struct
{
    HostCommandCodeT commandCode;     ///< The command code
    uint16_t phaseMask;               ///< Bit-mask indicating in which phases the command is supported
    CommandHandlerFunctionT handler;  ///< The handler function
} CommandHandlerCommandTableEntryT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
extern const CommandHandlerCommandTableEntryT commandHandlerROMCommandTable[COMMAND_HANDLER_NUM_COMMANDS_ROM_TABLE];

// Handles entry to an operating phase
extern ErrorT CommandHandlerOnPhaseEntry(SystemMgrPhaseT const phase);

// Handles a doorbell interrupt
extern void Doorbell_IrqHandler(void);

// Handles a shared-Ram ECC DED interrupt
extern void IcoreEccDed_IrqHandler(void);

// Handles a doorbell event
extern ErrorT CommandHandlerOnDoorbellEvent(HostCommandCodeT const commandCode);

// Performs the final step of the fatal error shutdown sequence
extern void CommandHandlerFatalErrorStop(void);

// Returns true if a host command is in progress
extern bool CommandHandlerIsHostCommandInProgress(void);

// API function to add a new command to the dynamic command table
extern ErrorT CommandHandlerAddCommand(CommandHandlerCommandTableEntryT* command);

/// @} endgroup commandhandler
#endif  // !defined(SRC__COMMANDHANDLER__INC__COMMANDHANDLER_H)

