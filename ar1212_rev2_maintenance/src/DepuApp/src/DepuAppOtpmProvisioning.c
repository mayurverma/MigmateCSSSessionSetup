//***********************************************************************************
/// \file
///
/// DEPU App otpm provisioing internal functions
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
#include "DepuAppInternal.h"
#include "Diag.h"
#include "NvmMgr.h"
#include "NvmMgrInternal.h"
#include "ccProdOtpPatch.h"

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

/// OTPM write and verify a number of long words.
///
/// \param[in] address      Address in OTPM to write (relative to OTPM base)
/// \param[in] buffer      Buffer to write
/// \param[in] numLongWords Number of long-words to write
/// \returns Command success/failure
/// \retval ERROR_SUCCESS               No errors in read/write OTPM.
/// \retval ERROR_RANGE                 Parameter address (offset) is outside array size
/// \retval ERROR_ACCESS                OTPM has not been configured so writes are not allowed
/// \retval ERROR_NOMSG                 OTPM write has not been successful as the read value is not the expected
/// \retval ERROR_IO                    OTPM location not blank.
///
ErrorT DepuAppOtpmDrvWrite(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t rc;

    rc = CC_ProdOtpWriteVerifyWordBuff(address, buffer, numLongWords);
    if ((uint32_t)CC_OK != rc)
    {
        error = ccProdOtp.lastError;
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
ErrorT DepuAppWriteAsset(uint16_t const assetType,
    uint32_t* const buffer,
    uint32_t const assetLengthLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT address;
    NvmMgrAddressT recordStartAddress;
    NvmMgrRecordHeaderT record;
    uint32_t freeSpaceLongWords;
    uint32_t recordHeaderSizeLongWords = (sizeof(NvmMgrRecordHeaderT) / sizeof(uint32_t));

    if (true != nvmMgr.isInitialized)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_NOT_INITIALIZED, 0U);
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
            record.assetId = assetType;
            record.assetLengthLongWords = (uint16_t)assetLengthLongWords;
            // Write header
            error = DepuAppOtpmDrvWrite(recordStartAddress, (uint32_t*)&record, recordHeaderSizeLongWords);
            if (ERROR_SUCCESS == error)
            {
                nvmMgr.blankAddressLongWords = recordStartAddress + recordHeaderSizeLongWords +
                                               assetLengthLongWords;
                // Write asset
                address = recordStartAddress + recordHeaderSizeLongWords;
                error = DepuAppOtpmDrvWrite(address, buffer, assetLengthLongWords);
            }
        }
    }

    return error;
}

/// @} endgroup DepuApp

