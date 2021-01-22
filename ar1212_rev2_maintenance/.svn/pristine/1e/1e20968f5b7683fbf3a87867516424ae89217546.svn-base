//***********************************************************************************
/// \file
///
/// Command driver
///
/// \addtogroup commanddrv
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

#include "AR0820.h"
#include "Common.h"
#include "crypto_icore.h"
#include "CommandDrv.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the Command Driver and clears the shared RAM
/// \returns Error status - always successful
/// \retval ERROR_SUCCESS   Command driver initialized successfully
/// \retval ERROR_IO        Failed to initialize shared RAM
///
ErrorT CommandDrvInit(void)
{
    ErrorT status = ERROR_SUCCESS;
    uint16_t idx;
    volatile uint32_t* sharedRamPtr = (uint32_t*)CRYPTO_ICORE_SHARED_RAM;

    // AR0820FW-153: FW gets stuck at COMMAND_HANDLER_DOORBELL_ISR checkpoint
    // Fix is to do an extra write-then-read at the start to clear uninitialised ECC errors
    *sharedRamPtr = 0U;
    if (0UL == *sharedRamPtr)
    {
        // initialize the shared-memory contents to zero to prevent ECC
        // hard faults should the host read an uninitialized location.
        for (idx = 0U; idx < (((uint32_t)CRYPTO_ICORE_SHARED_RAM_SIZE_BYTES) / sizeof(uint32_t)); idx += 1U)
        {
            *sharedRamPtr = 0U;
            sharedRamPtr++;
        }
    }
    else
    {
        status = ERROR_IO;
    }

    return status;
}

/// Retrieve the current command
///
/// \returns The current command register contents
///
uint16_t CommandDrvGetCommand(void)
{
    return (uint16_t)(CRYPTO_ICORE_REGS->command);
}

/// Returns a pointer to the command parameters in shared-memory
/// \returns Pointer to the command parameters
///
uint32_t * CommandDrvGetCommandParams(void)
{
    return (uint32_t*)CRYPTO_ICORE_SHARED_RAM;
}

/// Set the command response. The relevant command handler should already have put its response in the shared memory
/// \param[in] response     Command response code
///
void CommandDrvSetResponse(uint16_t const response)
{
    CRYPTO_ICORE_REGS->command = (uint32_t)response;
}

/// Is there a command in progress?
/// \returns bool  true if command is in progress
bool CommandDrvIsCommandInProgress(void)
{
    return ((CRYPTO_ICORE_REGS->command & CRYPTO_ICORE_COMMAND_DOORBELL__MASK) != 0U);
}

/// @} endgroup commanddrv

