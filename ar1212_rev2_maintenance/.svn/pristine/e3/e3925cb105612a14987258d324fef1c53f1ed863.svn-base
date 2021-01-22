//***********************************************************************************
/// \file
///
/// Patch Loader functionality for DEPU Application
///
/// \addtogroup DepuApp
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
//*************************************************************************************
#include <string.h>

#include "DepuAppInternal.h"
#include "CommandHandler.h"
#include "CommandHandlerInternal.h"
#include "Init.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

const uint32_t depuAppIcvDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE] =
{
    0xffffffe0U, 0xffffffffU, 0xffffffffU, 0xffffffffU
};

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initializes Application (will be called by loader)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Executed correctly
/// \retval ERROR_NOSPC     Command table already full
/// \retval ERROR_SYSERR    Internal fault or error during execution
//
ErrorT DepuAppInit(void)
{
    ErrorT error = ERROR_SUCCESS;
    CommandHandlerCommandTableEntryT command;

    // copy const data to the structure
    (void)memcpy(depuApp.icvDcuDefaultLock, depuAppIcvDcuDefaultLock, sizeof(depuAppIcvDcuDefaultLock));

    // add 1 new DepuApp commands
    command.commandCode = HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM;
    command.phaseMask = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
    command.handler = DepuAppOnProgramOtpm;
    error = CommandHandlerAddCommand(&command);

    if (ERROR_SUCCESS == error)
    {
        // Need to bind new interrupt handler into interrupt table
        InitRelocateVectorTable();
        error = InitInstallNewIrqHandler(HardFault_IRQn, (InitVectorTableHandlerT)HardFault_PatchedHandler);
    }

    return error;
}

/// After all patch chunks are downloaded, this function is called to load/hook the patched functions
/// and patched-in new commands.
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Patch loaded successfully.
/// \retval ERROR_SYSERR                Internal fault or error during execution
/// \retval ERROR_NOSPC                 Command table already full
ErrorT DepuAppLoader(void)
{
    ErrorT error;

    error = PatchMgrSetupComparatorTable(0U, DEPU_APP_NUM_PATCHED_FUNCTIONS, depuAppComparatorTable,
            DEPU_APP_NUM_PATCHED_FUNCTIONS);

    if (ERROR_SUCCESS == error)
    {
        error = DepuAppInit();
    }

    return error;
}

/// @} endgroup DepuApp

