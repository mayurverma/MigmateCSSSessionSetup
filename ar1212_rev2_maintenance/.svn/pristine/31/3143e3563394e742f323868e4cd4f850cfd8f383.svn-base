#if !defined(SRC__PATCHMGR__SRC__PATCHMGRINTERNAL_H)
#define      SRC__PATCHMGR__SRC__PATCHMGRINTERNAL_H

//***********************************************************************************
/// \file
/// PatchMgrInternal.h
///
/// \addtogroup patchmgr
///
/// Contains type and variable definitions internal to the PatcMgr component
///
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
#include "Diag.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// Status of Patch Manager
typedef enum
{
    PATCH_MGR_STATUS_UNINIT = 0U,       ///< Patch Manager not initialized or not usable.
    PATCH_MGR_STATUS_INIT = 1U,         ///< Patch Manager initialized
    PATCH_MGR_STATUS_ACTIVE = 2U,       ///< Patch Manager processing patch chunks
    PATCH_MGR_STATUS_COMPLETE = 3U,     ///< Patch Manager patch download sequence completed successfully
    PATCH_MGR_STATUS_ERROR = 4U,        ///< Patch Manager patch download sequence completed with error
} PatchMgrStatusT;

/// Represent internal control and state date use by Patch Manager
typedef struct
{
    PatchMgrStatusT status;                         ///< Patch Manager Status
    PatchMgrPatchVersionT patchVersion;             ///< Patch version
    uint16_t lastChunk;                             ///< Id of last 'chunk' in sequence, Total chunks = lastChunk + 1
    uint16_t expectedChunk;                         ///< Id of current chunk
    PatchMgrPatchVersionT expectedPatchVersion;     ///< Used to track version of patch as its being loaded
} PatchMgrT;

/// Represents the 'Asset' header associated with 'LoadPatchChunk' command
typedef struct
{
    uint16_t romVersion;                ///< Version of ROM code
    uint16_t patchVersion;              ///< Patch Version
    uint16_t lastChunk;                 ///< Id of last 'chunk' in sequence, Total chunks = lastChunk + 1
    uint16_t thisChunk;                 ///< Id of current expected chunk
    uint32_t loadAddress;               ///< Address in memory to copy payload data too
    uint32_t loaderAddress;             ///< If last chunk and value not zero, call function at this address
    uint32_t dataLengthWords;           ///< Number of words (16-bit) in this 'chunk'
} PatchMgrChunkHeaderT;

/// Represent the header of 'LoadPatchChunk' command
typedef struct
{
    uint32_t assetId;            ///< Unique identifier for the patch chunk
    uint32_t token;              ///< Asset provisioning token
    uint32_t version;            ///< Asset provisioning version
    uint32_t chunkLengthBytes;   ///< Length of the chunk in bytes
    uint8_t reserved[8];         ///< Reserved
    uint8_t nonce[12];           ///< Provisioning nonce (a number used only once)
    PatchMgrChunkHeaderT chunk;  ///< Header of patch chunk
} PatchMgrLoadPatchChunkCommandHeaderT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
extern PatchMgrT patchMgr;

// Initialises Patch Manager component
extern ErrorT PatchMgrInit(void);
// Processes a patch chunk, after de-cryption (if required)
extern ErrorT PatchMgrProcessChunk(PatchMgrChunkHeaderT* chunk);
// Dumps Patch Manager state in response to a fatal error
extern void PatchMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords);

extern ErrorT PatchMgrHandleCommand(PatchMgrLoadPatchChunkCommandHeaderT* commandHeader);

// @} endgroup patchmgr
#endif  // !defined(SRC__PATCHMGR__SRC__PATCHMGRINTERNAL_H)

