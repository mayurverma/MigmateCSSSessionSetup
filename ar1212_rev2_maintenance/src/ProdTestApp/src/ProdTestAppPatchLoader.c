//***********************************************************************************
/// \file
///
/// Patch loader implementation
///
/// \addtogroup prodtestapp
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

#include "Common.h"
#include "AR0820.h"
#include "Diag.h"
#include "Patch.h"
#include "Init.h"
#include "CommandHandler.h"
#include "CommandHandlerInternal.h"
#include "ProdTestApp.h"
#include "ProdTestAppInternal.h"

extern void HardFault_PatchedHandler(void);

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// After all patch chunks are downloaded, this function is called to load/hook the patched functions
/// and patched-in new commands.
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Patch loaded successfully.
/// \retval ERROR_NOSPC                 Command table full, could not add commands
ErrorT ProdTestAppLoader(void)
{
    ErrorT error = ERROR_SUCCESS;
    CommandHandlerCommandTableEntryT command;

    error = PatchMgrSetupComparatorTable(0U, PROD_TEST_APP_NUM_PATCHED_FUNCTIONS, prodTestAppComparatorTable,
            PROD_TEST_APP_NUM_PATCHED_FUNCTIONS);

    if (ERROR_SUCCESS == error)
    {
        // add a new ProdTestApp command and handler function
        command.commandCode = HOST_COMMAND_CODE_OTPM_BLANK_CHECK;
        command.phaseMask = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
        command.handler = ProdTestAppOnOtpmBlankCheck;
        error = CommandHandlerAddCommand(&command);
    }
    if (ERROR_SUCCESS == error)
    {
        command.commandCode = HOST_COMMAND_CODE_OTPM_VERIFY;
        command.phaseMask = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
        command.handler = ProdTestAppOnOtpmVerify;
        error = CommandHandlerAddCommand(&command);
    }
    if (ERROR_SUCCESS == error)
    {
        // Need to bind new interrupt handler into interrupt table
        InitRelocateVectorTable();
        error = InitInstallNewIrqHandler(HardFault_IRQn, (InitVectorTableHandlerT)HardFault_PatchedHandler);
    }

    return error;
}

/// @} endgroup prodtestapp

