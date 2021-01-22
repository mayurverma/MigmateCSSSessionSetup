//***********************************************************************************
/// \file
///
/// Patch Manager
///
/// \addtogroup patchmgr
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
#include "Diag.h"
#include "SystemMgr.h"

#include "PatchMgr.h"
#include "PatchMgrInternal.h"

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

/// Handles the LoadPatchChunk host command
///
/// \param[in]  commandCode Command to handle (LOAD_PATCH_CHUNK)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Error status
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_ACCESS    Access denied system locked out
/// \retval ERROR_SYSERR    Internal parameters are incorrect
///
ErrorT PatchMgrOnLoadPatchChunk(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT error = ERROR_SUCCESS;
    PatchMgrLoadPatchChunkCommandHeaderT* commandHeader;

    DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    if (PATCH_MGR_STATUS_UNINIT == patchMgr.status)
    {
        error = ERROR_ACCESS;
    }
    else if (PATCH_MGR_STATUS_ERROR == patchMgr.status)
    {
        error = ERROR_ACCESS;
    }
    else if (PATCH_MGR_STATUS_COMPLETE == patchMgr.status)
    {
        error = ERROR_ACCESS;
    }
    else if (HOST_COMMAND_CODE_LOAD_PATCH_CHUNK != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_ON_LOAD_PATCH_CHUNK_BAD_COMMAND,
            (uint32_t)commandCode);
        error = ERROR_SYSERR;
    }
    else
    {
        commandHeader = (PatchMgrLoadPatchChunkCommandHeaderT*)params;
        error = PatchMgrHandleCommand(commandHeader);
    }

    if (ERROR_SUCCESS != error)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);
    }

    return error;
}

/// Handles entry to an operating phase
/// \param[in] phase    Phase being entered
/// \returns Error status
/// \retval ERROR_SUCCESS   Patch loaded and version returned
/// \retval ERROR_SYSERR    Error during initialization
ErrorT PatchMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT status = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            status = PatchMgrInit();
            if (ERROR_SUCCESS != status)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_INIT_FAILED,
                    (uint32_t)status);
            }
            break;

        default:
            // Do nothing for all other phases i.e.
            break;
    }

    return status;
}

/// Retrieves the current patch version (if loaded)
///
/// \returns Patch version (if loaded)
/// \retval 0U              No patch loaded
/// \retval >0U             Version of loaded patch
///
PatchMgrPatchVersionT PatchMgrGetPatchVersion(void)
{
    return patchMgr.patchVersion;
}
/// @} endgroup patchmgr

