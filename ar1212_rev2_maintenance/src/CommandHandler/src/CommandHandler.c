//***********************************************************************************
/// \file
///
/// Command Handler implementation
///
/// \addtogroup commandhandler
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
#include "AR0820.h"
#include "Common.h"
#include "SystemMgr.h"
#include "SystemMgrFatalError.h"
#include "Diag.h"
#include "HealthMonitor.h"
#include "LifecycleMgr.h"
#include "CryptoMgr.h"
#include "SessionMgr.h"
#include "AssetMgr.h"
#include "PatchMgr.h"
#include "Interrupts.h"
#include "CommandDrv.h"
#include "SystemDrv.h"

#include "CommandHandlerInternal.h"
#include "CommandHandler.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

// We support four entries in the RAM table to allow patches to add host
// commands, or override ROM host commands
/// The empty RAM host command table
CommandHandlerCommandTableEntryT commandHandlerRAMCommandTable[COMMAND_HANDLER_MAX_PATCH_COMMANDS];

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Handles a doorbell interrupt
///
/// Sets the doorbell event flag
///
/// \returns void
void Doorbell_IrqHandler(void)
{
    DiagSetCheckpoint(DIAG_CHECKPOINT_COMMAND_HANDLER_DOORBELL_ISR);
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_DOORBELL);
}

/// Handles a shared-Ram ECC DED interrupt
/// reports system crypto fault ECC failure
/// \returns void
void IcoreEccDed_IrqHandler(void)
{
    SystemMgrReportCryptoFault(SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE, 0, 0);
}

/// Handles entry to an operating phase
/// \param[in] phase    Phase being entered
/// \returns Error status of function
/// \retval ERROR_SUCCESS   Command completed correctly
/// \retval ERROR_SYSERR    Attempting to enter shutdown with a command still in progress
ErrorT CommandHandlerOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT returnVal = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            (void)memset(&commandHandlerRAMCommandTable, 0, sizeof(commandHandlerRAMCommandTable));

            // DV sims need pending interrupts to be set/cleared. It's quite safe to do so  because the
            // host cannot access the shared-RAM while the doorbell bit is set, and we know the firmware
            // hasn't read from the shared-RAM yet, therefore no ECC failures will have occurred
            // so it's safe to clear the interrupt before enabling it.
            returnVal = InterruptsClearInterruptPending(INTERRUPT_ICORE_ECC_DED_FAIL);
            if (ERROR_SUCCESS != returnVal)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_ICORE_ECC_DED_INTERRUPT_FAILED,
                    (uint32_t)INTERRUPT_ICORE_ECC_DED_FAIL);
            }
            else
            {
                // enable the shared-RAM's ECC DED error interrupt
                returnVal = InterruptsEnableInterrupt(INTERRUPT_ICORE_ECC_DED_FAIL, true);

                if (ERROR_SUCCESS != returnVal)
                {
                    SystemMgrReportFatalError(
                        SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_ICORE_ECC_DED_INTERRUPT_FAILED,
                        (uint32_t)INTERRUPT_ICORE_ECC_DED_FAIL);
                }
            }

            DiagSetCheckpoint(DIAG_CHECKPOINT_COMMAND_HANDLER_INIT);
            break;

        case SYSTEM_MGR_PHASE_DEBUG:
            // Enable the doorbell interrupt having first cleared the pending interrupt.
            // This pending interrupt is a power-up HW artifact to be cleared and ignored by FW.
            // If this is successful we can accept host commands
            returnVal = InterruptsClearInterruptPending(INTERRUPT_DOORBELL);
            if (ERROR_SUCCESS != returnVal)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_DOORBELL_INTERRUPT_FAILED,
                    (uint32_t)INTERRUPT_DOORBELL);
            }
            else
            {
                returnVal = InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true);
                if (ERROR_SUCCESS == returnVal)
                {
                    // Doorbell interrupt is enabled, so can now clear doorbell bit and await host commands
                    CommandDrvSetResponse((uint16_t)ERROR_SUCCESS);
                }
                else
                {
                    SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_DOORBELL_INTERRUPT_FAILED,
                        (uint32_t)INTERRUPT_DOORBELL);
                }
            }

            DiagSetCheckpoint(DIAG_CHECKPOINT_COMMAND_HANDLER_ENTER_DEBUG);
            break;

        default:
            // Should never happen
            break;
    }

    return returnVal;
}

/// Handles a doorbell event
///
/// Invokes the registered handler for the command. On return the command
/// response parameters (if any) have been written to the shared memory,
/// and the command status code written to complete the command.
///
/// \param[in] commandCode  Host command to handle
/// \returns Error status of command handler
///
ErrorT CommandHandlerOnDoorbellEvent(HostCommandCodeT const commandCode)
{
    ErrorT returnVal = ERROR_ACCESS;
    const CommandHandlerCommandTableEntryT* entry;
    CommandHandlerFunctionT handler = NULL;
    uint16_t phaseMask;
    uint8_t idx;
    SystemMgrPhaseT phase;
    bool continueLookingRamTable, commandIsInRamTable;

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT, (uint16_t)commandCode);

    phase = SystemMgrGetCurrentPhase();
    phaseMask = COMMAND_HANDLER_SET_PHASE_MASK(phase);

    // Search the RAM table first, any entries in here override their equivalents in the ROM table
    continueLookingRamTable = true;
    commandIsInRamTable = false;
    idx = 0U;
    while ((continueLookingRamTable) && (COMMAND_HANDLER_MAX_PATCH_COMMANDS > idx))
    {
        entry = &commandHandlerRAMCommandTable[idx];
        if (HOST_COMMAND_CODE_NULL != entry->commandCode)
        {
            if (commandCode == entry->commandCode)
            {
                // Found a match, this will supercede any equivalent in the ROM table, whether or not it's allowed in this phase.
                commandIsInRamTable = true;
                continueLookingRamTable = false;

                // is it supported for this phase?
                if (0U != (phaseMask & entry->phaseMask))
                {
                    handler = entry->handler;       // yes, assign the handler
                }
                else
                {
                    // Command found in RAM table but not supported for this phase. Abort and don't search in ROM
                }
            }
        }
        else
        {
            continueLookingRamTable = false;        // Found a NULL entry, indicates remainder of the table is empty
        }

        idx += 1U;
    }

    // if this command is not found in RAM table, look in the ROM table
    if (!commandIsInRamTable)
    {
        // Search the ROM command table to match the specified command
        // - note if we don't find an entry we'll return ERROR_ACCESS
        for (idx = 0U; COMMAND_HANDLER_NUM_COMMANDS_ROM_TABLE > idx; idx += 1U)
        {
            entry = &commandHandlerROMCommandTable[idx];
            if (commandCode == entry->commandCode)
            {
                // Found a match, is it supported for this phase?
                if (0U != (phaseMask & entry->phaseMask))
                {
                    handler = entry->handler;
                }
                else
                {
                    break;      // Command is not supported in this phase
                }
            }
        }
    }

    if (NULL != handler)
    {
        // Call the handler
        DiagSetCheckpoint(DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);
        returnVal = handler(commandCode, (CommandHandlerCommandParamsT)CommandDrvGetCommandParams());
    }

    // Complete the command.
    CommandDrvSetResponse((uint16_t)returnVal);

    DiagSetCheckpoint(DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

    return returnVal;
}

/// Performs the final step of the fatal error shutdown sequence
///
/// Completes the current host command with response ERROR_SYSERR (if a host
/// command is in progress). Then terminates execution.
///
/// Does not return in production systems. For unit test, returns to allow
/// test runner to regain control.
///
/// This function can be called at any time during the Initialisation phase,
/// _before_ Command Handler has been initialised. Therefore it must not depend
/// on any initialisation activities.
///
/// \returns never
///
void CommandHandlerFatalErrorStop(void)
{
    // Terminates the current command (if any) with ERROR_SYSERR
    if (CommandDrvIsCommandInProgress())
    {
        CommandDrvSetResponse((uint16_t)ERROR_SYSERR);
    }

    SystemDrvTerminate();
}

/// Is a host command in progress?
/// \returns true if a command is in progress, false otherwise
bool CommandHandlerIsHostCommandInProgress(void)
{
    return CommandDrvIsCommandInProgress();
}

/// API function to add a new command to the dynamic command table
///
/// \param[in] command     Pointer to the Command Table entry to be placed in the command table
/// \returns               Function error status
/// \retval ERROR_INVAL    Invalid parameters passed to function
/// \retval ERROR_NOSPC    Command table already full
/// \retval ERROR_SUCCESS  Command bound successfully
ErrorT CommandHandlerAddCommand(CommandHandlerCommandTableEntryT* const command)
{
    ErrorT err = ERROR_SUCCESS;
    bool spaceFound = false;

    if (NULL == command)
    {
        err = ERROR_INVAL;
    }
    else if (HOST_COMMAND_CODE_NULL == command->commandCode)
    {
        err = ERROR_INVAL;
    }
    else if (NULL == command->handler)
    {
        err = ERROR_INVAL;
    }
    else
    {
        for (uint32_t idx = 0; idx < COMMAND_HANDLER_MAX_PATCH_COMMANDS; idx++)
        {
            if (HOST_COMMAND_CODE_NULL == commandHandlerRAMCommandTable[idx].commandCode)
            {
                commandHandlerRAMCommandTable[idx] = *command;
                spaceFound = true;
                break;
            }
        }
        if (!spaceFound)
        {
            err = ERROR_NOSPC;
        }
    }

    return err;
}

/// @} endgroup commandhandler

