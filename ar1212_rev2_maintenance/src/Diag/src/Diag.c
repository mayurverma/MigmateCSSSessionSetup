//***********************************************************************************
/// \file
///
/// Diag component implementation
///
/// \addtogroup diag
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

#include "CommandHandler.h"
#include "CommandDrv.h"
#include "SystemDrv.h"
#include "SystemMgr.h"
#include "Platform.h"
#include "Diag.h"
#include "DiagInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
/// Diag control/status structure
// Default infoEnabled field to true so that diagnostic information is seen during system startup
DiagT diagControl = {true};

/// The registered fatal error dump handlers
DiagFatalErrorDumpHandlerT diagFatalErrorDumpHandlers[DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS];

/// Block until Pause bit is cleared
///
/// Function will set Pause bit in pause register and block until the Pause bit is cleared by external source
///
/// \returns void
///
void DiagOnWaitPaused(void)
{
    bool isPaused;

    SystemDrvSetPaused();
    do
    {
        isPaused = SystemDrvIsPaused();
    }
    while (isPaused);
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialise the Diagnostics platform-layer
/// \returns Error status
/// \retval ERROR_SUCCESS       Diagnostics initialized successfully
///
ErrorT DiagInit(void)
{
    int32_t idx;

    diagControl.infoEnabled = true;

    for (idx = 0; idx < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; idx++)
    {
        diagFatalErrorDumpHandlers[idx] = NULL;
    }

    return ERROR_SUCCESS;
}

/// Sets the Checkpoint register, and tests the Pausepoint.
///
/// Function will block if the Pausepoint register matches the checkpoint.
/// It will return when the host releases the pause-point.
///
/// \param[in] checkpoint   Checkpoint to set
/// \returns void
///
void DiagSetCheckpoint(DiagCheckpointT const checkpoint)
{
    uint16_t pausepoint;

    PlatformEnterCritical();

    SystemDrvSetCheckpoint((uint16_t)checkpoint);
    pausepoint = SystemDrvGetPausepoint();
    if (pausepoint == (uint16_t)checkpoint)
    {
        DiagOnWaitPaused();
    }

    PlatformExitCritical();
}

/// Enables access to info register
///
/// Function intended to be called from Life cycle manager to control access to info register when calling
/// DiagSetCheckpointWithInfo(
///
/// \param[in] enable   Enable/disable access
/// \returns void
///
void  DiagEnableInfo(bool const enable)
{
    diagControl.infoEnabled = enable;
}

/// Sets the Checkpoint and Info registers (in insecure state only), and tests the Pausepoint.
///
/// Function will block if the Pausepoint register matches the checkpoint.
/// Will write to Info register if not in secure (blocked) state
/// It will return when the host releases the pause-point.
///
/// \param[in] checkpoint   Checkpoint to set
/// \param[in] info         Info to set
/// \returns void
///
void DiagSetCheckpointWithInfo(DiagCheckpointT const checkpoint, uint16_t info)
{
    uint16_t pausepoint;

    PlatformEnterCritical();

    if (true == diagControl.infoEnabled)
    {
        SystemDrvSetInfo(info);
    }
    SystemDrvSetCheckpoint((uint16_t)checkpoint);
    pausepoint = SystemDrvGetPausepoint();
    if (pausepoint == (uint16_t)checkpoint)
    {
        DiagOnWaitPaused();
    }

    PlatformExitCritical();
}

/// Sets the Checkpoint and Info registers (unprotected), and tests the Pausepoint.
///
/// Will write to Info register irrespective of life cycle
/// It will return when the host releases the pause-point.
///
/// \param[in] checkpoint   Checkpoint to set
/// \param[in] info         Info to set
/// \returns void
///
void DiagSetCheckpointWithInfoUnprotected(DiagCheckpointT const checkpoint, uint16_t info)
{
    uint16_t pausepoint;

    PlatformEnterCritical();

    SystemDrvSetInfo(info);
    SystemDrvSetCheckpoint((uint16_t)checkpoint);
    pausepoint = SystemDrvGetPausepoint();
    if (pausepoint == (uint16_t)checkpoint)
    {
        DiagOnWaitPaused();
    }

    PlatformExitCritical();
}

/// Dumps diagnostics data from each component to shared-memory
///
/// Only dumps data if a host command is in progress (otherwise the shared-memory
/// is not accessible).
///
/// Calls each registered handler in turn
/// \returns void
///
void DiagOnFatalError(void)
{
    if (CommandHandlerIsHostCommandInProgress())
    {
        DiagFatalErrorDumpBufferT dumpBuffer = (DiagFatalErrorDumpBufferT)CommandDrvGetCommandParams();
        int32_t idx;

        for (idx = 0; idx < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; idx++)
        {
            if (NULL != diagFatalErrorDumpHandlers[idx])
            {
                diagFatalErrorDumpHandlers[idx](dumpBuffer, DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS);
                // dumpBuffer += DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS;
                // INFO: <RW>: Above line has been recoded to remove Klocwork issues
                dumpBuffer = &dumpBuffer[DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS];
            }
            else
            {
                // No more registered handlers
                break;
            }
        }
    }
}

/// Registers a fatal error dump handler
/// \param[in] handler handler to register
/// \returns Command success/failure
/// \retval ERROR_SUCCESS Handler registered successfully
/// \retval ERROR_NOSPC No space to register handler
///
ErrorT DiagRegisterFatalErrorDumpHandler(DiagFatalErrorDumpHandlerT handler)
{
    ErrorT error = ERROR_NOSPC;
    int32_t idx;

    for (idx = 0; idx < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; idx++)
    {
        if (NULL == diagFatalErrorDumpHandlers[idx])
        {
            diagFatalErrorDumpHandlers[idx] = handler;
            error = ERROR_SUCCESS;
            break;
        }
    }

    return error;
}

/// @} endgroup diag

