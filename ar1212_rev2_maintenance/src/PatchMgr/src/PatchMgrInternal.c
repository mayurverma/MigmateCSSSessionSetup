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
#include <stdlib.h>
#include <string.h>

#include "AR0820.h"
#include "Common.h"
#include "Diag.h"
#include "LifecycleMgr.h"
#include "CryptoMgr.h"

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
/// Patch Manager internal control/state structure
PatchMgrT patchMgr;

/// Dumps Patch Manager state in response to a fatal error
///
/// \param[in] dumpBuffer       Pointer to buffer to contain the dump
/// \param[in] sizeLongWords    Size of the dump buffer
/// \returns void
///
void PatchMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords)
{
    uint32_t sizeBytes = sizeLongWords * (sizeof(uint32_t));

    if (NULL != dumpBuffer)
    {
        uint32_t dumpSizeBytes = sizeof(patchMgr);

        if (dumpSizeBytes > sizeBytes)
        {
            dumpSizeBytes = sizeBytes;
        }

        (void)memcpy((void*)dumpBuffer, (void*)&patchMgr, dumpSizeBytes);
    }
}

/// Initialises Patch Manager component
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Initialized
/// \retval ERROR_NOSPC     Registration of fatal error handler failed.
///
ErrorT PatchMgrInit(void)
{
    ErrorT error = ERROR_SUCCESS;

    (void)memset(&patchMgr, 0, sizeof(patchMgr));
    DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_INIT);
    error = DiagRegisterFatalErrorDumpHandler(PatchMgrOnDiagFatalErrorDump);
    if (ERROR_SUCCESS == error)
    {
        patchMgr.status = PATCH_MGR_STATUS_INIT;
    }

    return error;
}

/// Processes a patch chunk, after de-cryption (if required)
///
/// \param[in]  chunk       Pointer to type PatchMgrChunkHeaderT
/// \returns Error status
/// \retval ERROR_SUCCESS   Chunk processed successfully
/// \retval ERROR_RANGE     Patch or ROM version incorrect
/// \retval ERROR_INVAL     Unexpected chunk being processed, wrong order or not from same patch version
/// \retval ERROR_NOENT     Last chunk does not contain a Loader address
///
ErrorT PatchMgrProcessChunk(PatchMgrChunkHeaderT* chunk)
{
    ErrorT error = ERROR_SUCCESS;
    PatchMgrChunkHeaderT* patchCode;

    DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    if (SystemMgrGetRomVersion() != chunk->romVersion)
    {
        error = ERROR_RANGE;
    }
    else if (0U == chunk->patchVersion)
    {
        error = ERROR_RANGE;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
        // The following code is to catch errors when processing a following chunk
        // apply test to first chunk which will pass by default
        if (patchMgr.lastChunk != chunk->lastChunk)
        {
            error = ERROR_INVAL;
        }
        else if (patchMgr.expectedChunk != chunk->thisChunk)
        {
            error = ERROR_INVAL;
        }
        else if (patchMgr.expectedPatchVersion != chunk->patchVersion)
        {
            error = ERROR_RANGE;
        }
        else
        {
            // All parameter checking passed so now copy data
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH,
                (uint16_t)(chunk->thisChunk));
            patchCode = chunk;
            // patchcode is of type PatchMgrChunkHeaderT, this is only the header. By incrementing the pointer we
            // have created a pointer to the payload i.e. patch data
            patchCode++;
            (void)memcpy((void*)(chunk->loadAddress), (const void*)patchCode,
                (sizeof(uint16_t) * (chunk->dataLengthWords)));

            // Check if this the last chunk so we can call loader
            if (patchMgr.lastChunk == chunk->thisChunk)
            {
                if (0U != chunk->loaderAddress)
                {
                    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_CALL_LOADER,
                        (uint16_t)(chunk->loaderAddress));
                    error = ((PatchMgrLoaderT)chunk->loaderAddress)();
                    if (ERROR_SUCCESS == error)
                    {
                        patchMgr.status = PATCH_MGR_STATUS_COMPLETE;
                        SystemMgrSetEvent(SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
                        patchMgr.patchVersion = patchMgr.expectedPatchVersion;
                    }
                }
                else
                {
                    error = ERROR_NOENT;
                }
            }
            patchMgr.expectedChunk++;
        }
    }

    if (ERROR_SUCCESS != error)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    }

    return error;
}

/// Initial command handling i.e. decrypt payload if required.
///
/// If not in CM lifecycle patch will be encrypted so decrypt as first step
/// prio to passing data onto PatchMgrProcessChunk()
///
/// \param[in]  commandHeader    Pointer to command (including payload)
/// \returns Error status
/// \retval ERROR_SUCCESS   Chunk processed successfully
/// \retval ERROR_NOSPC     Could no allocate memory to perform deccryption
/// \retval ERROR_INVAL     First chunk received is not configured as chunk Zero
///
ErrorT PatchMgrHandleCommand(PatchMgrLoadPatchChunkCommandHeaderT* commandHeader)
{
    ErrorT error = ERROR_SUCCESS;
    PatchMgrChunkHeaderT* chunk;
    uint32_t workspaceLengthBytes;
    PatchMgrChunkHeaderT* workspace = NULL;
    LifecycleMgrLifecycleStateT lcsCurrent;

    if (NULL == commandHeader)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_HANDLE_COMMAND_BAD_PARAM, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
        chunk = &commandHeader->chunk;
        // Check state to determine if asset needs to be decrypted
        error = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if ((LIFECYCLE_MGR_LIFECYCLE_STATE_CM != lcsCurrent) && (ERROR_SUCCESS == error))
        {
            workspaceLengthBytes = commandHeader->chunkLengthBytes;
            workspace = (PatchMgrChunkHeaderT*)calloc(workspaceLengthBytes, sizeof(uint8_t));
            if (NULL != workspace)
            {
                DiagSetCheckpoint(DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING);
                // The CryptoMgrAuthenticatePatchChunk() API was simplified to minimise the number of parameters passed
                // with this call. This function has its own internal structure which defines the package and includes token,
                // version, token, nonce and asset and maps onto  PatchMgrLoadPatchChunkCommandHeaderT from token field
                error = CryptoMgrAuthenticatePatchChunk(
                        commandHeader->assetId,
                    (uint32_t*)(&commandHeader->token),
                        &workspaceLengthBytes,
                    (uint8_t*)workspace);
                chunk = workspace;
            }
            else
            {
                error = ERROR_NOSPC;
            }
        }
        if (ERROR_SUCCESS == error)
        {
            if (PATCH_MGR_STATUS_INIT == patchMgr.status)                           // Process first chunk
            {
                patchMgr.status = PATCH_MGR_STATUS_ACTIVE;
                if (0U != chunk->thisChunk)
                {
                    error = ERROR_INVAL;
                }
                else
                {
                    patchMgr.expectedPatchVersion = chunk->patchVersion;
                    patchMgr.lastChunk = chunk->lastChunk;
                    patchMgr.expectedChunk = 0U;
                    SystemMgrEnablePatchRegion(true);
                }
            }

            if (ERROR_SUCCESS == error)
            {
                error = PatchMgrProcessChunk(chunk);
            }
        }
        if (NULL != workspace)
        {
            // AR0820FW-345 : PatchMgr should clear decryption buffer after chunk is processed
            (void)memset((void*)workspace, 0, workspaceLengthBytes);
            free(workspace);
        }
    }

    if (ERROR_SUCCESS != error)
    {
        patchMgr.status = PATCH_MGR_STATUS_ERROR;
        SystemMgrEnablePatchRegion(false);
        SystemMgrSetEvent(SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    }

    return error;
}

/// @} endgroup patchmgr

