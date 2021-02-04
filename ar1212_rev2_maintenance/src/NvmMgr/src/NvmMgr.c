//***********************************************************************************
/// \file
///
/// NVM Manager implementation
///
/// \addtogroup nvmmgr
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
#include "OtpmDrv.h"

#include "NvmMgr.h"
#include "NvmMgrInternal.h"

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

/// Handles entry to an operating phase
/// \param[in] phase    Phase being entered
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failure
ErrorT NvmMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT startAddressLongWords;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            error = NvmMgrInit();
            if (ERROR_SUCCESS != error)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_INIT_FAILED,
                    (uint32_t)error);
            }
            break;

        case SYSTEM_MGR_PHASE_DEBUG:
            // The OTPM is first accessed during Crypto cell boot and errors detected and reported as part of Lifecycle
            // Manager processing in its debug phase. NvmMgr is notified after Lifecycle manager so it is at this point
            // NvmMgr can first access OTPM
            error = NvmMgrFindFreeSpace(&startAddressLongWords);
            if (ERROR_SUCCESS == error)
            {
                nvmMgr.blankAddressLongWords = startAddressLongWords;
                nvmMgr.isInitialized = true;
            }
            else if (ERROR_NOSPC == error)
            {
                // OTPM full but system configuration valid
                nvmMgr.isInitialized = true;
                error = ERROR_SUCCESS;
            }
            else
            {
                // Nothing to do just pass errors back
            }
            break;

        default:
            // Nothing to do for other phases
            break;
    }

    return error;
}

/// Finds the first asset of the specified type in NVM
///
/// Will search for asset and if found update locations pointed to by assetAddress with assetLengthLongWords
/// with new values. In all other situations these locations are not updated
///
/// \param[out]     context                 Pointer to opaque find context object
/// \param[in]      assetType               Identifies type of asset to find
/// \param[out]     assetAddress            Address of asset in NVM
/// \param[out]     assetLengthLongWords    Length of the asset in long words
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset located, assetAddress and assetLengthLongWords valid
/// \retval ERROR_SYSERR        Fatal error caused by input parameter check
/// \retval ERROR_NOENT         Failed to locate asset
///
ErrorT NvmMgrFindFirst(NvmMgrFindContextT* const context,
    AssetMgrAssetTypeT const assetType,
    NvmMgrAddressT* const assetAddress,
    uint32_t* const assetLengthLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrRecordHeaderT record;
    NvmMgrAddressT offsetLongWords;
    NvmMgrFindContextConcreteT* const contextConcrete = (NvmMgrFindContextConcreteT* const)context;

    if (true != nvmMgr.isInitialized)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_NOT_INITIALIZED, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == context)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_CONTEXT, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == assetAddress)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_ADDRESS, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == assetLengthLongWords)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_LENGTH, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_ENTRY);
        offsetLongWords = OtpmDrvGetUserSpaceAddress();
        error = NvmMgrFindAsset(assetType, &offsetLongWords, &record);
        if (ERROR_RANGE == error)
        {
            // Indicates read attempted beyond expected worse case i.e. implies length error in last record read
            error = ERROR_NOENT;
        }
        if (ERROR_SUCCESS == error)
        {
            DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_SUCCESS);
            // found asset
            contextConcrete->magic = NVM_MGR_MAGIC;
            contextConcrete->assetType = assetType;
            contextConcrete->currentAddress = offsetLongWords;
            *assetAddress = offsetLongWords;
            contextConcrete->assetLengthLongWords = (uint32_t)record.assetLengthLongWords;
            *assetLengthLongWords = (uint32_t)record.assetLengthLongWords;
        }
    }

    return error;
}

/// Finds the next asset of the specified type in NVM
///
/// Will search for asset and if found update locations pointed to by assetAddress with assetLengthLongWords
/// with new values. In all other situations these locations are not updated
///
/// \param[in,out]  context                 Pointer to opaque find context object
/// \param[out]     assetAddress            Address of asset in NVM
/// \param[out]     assetLengthLongWords    Length of the asset in long words
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset located, assetAddress and assetLengthLongWords valid
/// \retval ERROR_SYSERR        Fatal error caused by input parameter check
/// \retval ERROR_NOENT         Read outside OTPM area, no valid record or blank location found
///
ErrorT NvmMgrFindNext(NvmMgrFindContextT* const context,
    NvmMgrAddressT* const assetAddress,
    uint32_t* const assetLengthLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrFindContextConcreteT* const contextConcrete = (NvmMgrFindContextConcreteT* const)context;
    NvmMgrAddressT offsetLongWords;
    NvmMgrRecordHeaderT record;

    if (true != nvmMgr.isInitialized)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_NOT_INITIALIZED, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == context)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_CONTEXT, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == assetAddress)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_ADDRESS, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == assetLengthLongWords)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_LENGTH, 0U);
        error = ERROR_SYSERR;
    }
    else if (NVM_MGR_MAGIC != contextConcrete->magic)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_MAGIC,
            (uint16_t)(contextConcrete->magic & 0xFFFFU));
        error = ERROR_SYSERR;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_ENTRY);
        offsetLongWords = contextConcrete->currentAddress + (NvmMgrAddressT)contextConcrete->assetLengthLongWords;
        error = NvmMgrFindAsset(contextConcrete->assetType, &offsetLongWords, &record);
        if (ERROR_SUCCESS == error)
        {
            DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_SUCCESS);
            // found asset
            contextConcrete->currentAddress = offsetLongWords;
            *assetAddress = offsetLongWords;
            contextConcrete->assetLengthLongWords = (uint32_t)record.assetLengthLongWords;
            *assetLengthLongWords = (uint32_t)record.assetLengthLongWords;
        }
    }

    return error;
}

/// Reads all or part of an asset from NVM
///
/// \param[in]      assetAddress            Address of asset in NVM
/// \param[out]     buffer                  Buffer to contain the payload
/// \param[in]      offset                  Offset within payload to start
/// \param[in]      assetLengthLongWords    Number of long words to read
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset read successfully into buffer
/// \retval ERROR_SYSERR        Fatal error caused by input parameter check
///
ErrorT NvmMgrReadAsset(NvmMgrAddressT const assetAddress,
    uint32_t* const buffer,
    uint32_t const offset,
    uint32_t const assetLengthLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT address;

    if (true != nvmMgr.isInitialized)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_NOT_INITIALIZED, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == buffer)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_BAD_PARAM_BUFFER, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_READ_ASSET_ENTRY);
        address = assetAddress + (NvmMgrAddressT)offset;
        error = OtpmDrvRead(address, buffer, assetLengthLongWords);
    }

    return error;
}

/// Writes a record containing an asset to NVM
///
/// \param[in]      assetType               Identifies type of asset to write
/// \param[in]      buffer                  Buffer containing the asset
/// \param[in]      assetLengthLongWords    Length of the asset in long words
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset written successfully to NVM
/// \retval ERROR_IO            IO error accessing NVM
/// \retval ERROR_NOSPC         Insufficient NVM to contain asset
///
ErrorT NvmMgrWriteAsset(AssetMgrAssetTypeT const assetType,
    uint32_t* const buffer,
    uint32_t const assetLengthLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT address;
    NvmMgrAddressT recordStartAddress;
    NvmMgrRecordHeaderT record;
    NvmMgrRecordHeaderT recordReadBack;
    uint32_t index;
    uint32_t dataReadBack;
    uint32_t freeSpaceLongWords;
    uint32_t recordHeaderSizeLongWords = (sizeof(NvmMgrRecordHeaderT) / sizeof(uint32_t));

    if (true != nvmMgr.isInitialized)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_NOT_INITIALIZED, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == buffer)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_BAD_PARAM_BUFFER, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
        address = nvmMgr.blankAddressLongWords;
        recordStartAddress = address;
        // Check we have enough space for both the record header and payload (asset)
        freeSpaceLongWords = OtpmDrvGetArraySizeLongWords() - recordStartAddress;
        if (freeSpaceLongWords < (assetLengthLongWords + recordHeaderSizeLongWords))
        {
            error = ERROR_NOSPC;
        }
        else
        {
            // Write split into two, first we write the record header, if successful we then write the payload (asset)
            DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
            record.assetId = (uint16_t)assetType;
            record.assetLengthLongWords = (uint16_t)assetLengthLongWords;
            // Write header
            error = OtpmDrvWrite(recordStartAddress, (uint32_t*)&record, recordHeaderSizeLongWords);
            if (ERROR_SUCCESS == error)
            {
                nvmMgr.blankAddressLongWords = recordStartAddress + recordHeaderSizeLongWords +
                                               assetLengthLongWords;
                error = OtpmDrvRead(recordStartAddress, (uint32_t*)&recordReadBack, recordHeaderSizeLongWords);
                if (ERROR_SUCCESS == error)
                {
                    DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
                    if (0 != memcmp(&record, &recordReadBack, sizeof(record)))
                    {
                        error = ERROR_IO;
                    }
                    else
                    {
                        // Write asset
                        address = recordStartAddress + recordHeaderSizeLongWords;
                        error = OtpmDrvWrite(address, buffer, assetLengthLongWords);
                        if (ERROR_SUCCESS == error)
                        {
                            DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK);
                            for (index = 0U; index < assetLengthLongWords; index++)
                            {
                                error = OtpmDrvRead((address + index), &dataReadBack,
                                        sizeof(dataReadBack) / sizeof(uint32_t));
                                if (ERROR_SUCCESS != error)
                                {
                                    error = ERROR_IO;
                                }
                                if (dataReadBack != buffer[index])
                                {
                                    error = ERROR_IO;
                                }
                                if (ERROR_IO == error)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return error;
}

/// Gets the start of the OTPM blank space
///
/// \param[out] offset            Memory location to store the free space address in
///
/// \returns Error status
/// \retval ERROR_SUCCESS         Address fetch completed successfully
/// \retval ERROR_ACCESS          NvmMgr not initialised, blank space address could not be determined
/// \retval ERROR_INVAL           Pointer passed to function was null
///
ErrorT NvmMgrGetFreeSpaceAddress(NvmMgrAddressT* const offset)
{
    ErrorT err = ERROR_SUCCESS;

    if (NULL == offset)
    {
        err = ERROR_INVAL;
    }
    else
    {
        if (nvmMgr.isInitialized)
        {
            *offset = nvmMgr.blankAddressLongWords;
        }
        else
        {
            err = ERROR_ACCESS;
        }
    }

    return err;
}

/// @} endgroup nvmmgr

