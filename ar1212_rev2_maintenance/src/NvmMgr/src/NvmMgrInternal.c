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
#include "SystemMgr.h"
#include "OtpmDrv.h"
#include "AssetMgr.h"

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

/// Called by System Manager when there is a system clock frequency change
///
/// Will re-configure OtpmDrv based using new clock frequency. First searches OPTM for OTPM configuration
/// if not found will pass in NULL configuration and OtpmDrv will use default values
///
/// \param[in]  clockSpeedHz     System clock speed in Hz
/// \returns Command success/failure
/// \retval ERROR_SUCCESS      Command completed successfully
/// \retval ERROR_IO           OTPM access error from AssetMgrRetrieveOtpmConfig()
///
ErrorT NvmMgrOnClockChange(uint32_t const clockSpeedHz)
{
    ErrorT error;
    AssetMgrOtpmConfigT* config = NULL;
    AssetMgrOtpmConfigT otpmConfiguration;

    // Search for OTPM configuration
    DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);
    error = AssetMgrRetrieveOtpmConfig((AssetMgrOtpmConfigT*)&otpmConfiguration);

    if (ERROR_SUCCESS == error)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_FOUND_ASSET);
        config = &otpmConfiguration;
    }
    else if (ERROR_NOENT == error)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_NO_ASSET_FOUND);
        error = ERROR_SUCCESS;
    }
    else
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_ON_CLOCK_CHANGE_ASSET_RETRIEVAL_FAILED,
            (uint32_t)error);
    }

    if (ERROR_SUCCESS == error)
    {
        error = OtpmDrvConfigure(config, clockSpeedHz);
        if (ERROR_SUCCESS != error)
        {
            SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_OTPM_DRV_CONFIGURE_FAILED, (uint32_t)error);
        }
    }

    return error;
}
//
//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
NvmMgrT nvmMgr;

/// Dumps NVM Manager state in response to a fatal error
///
/// \param[in] dumpBuffer       Pointer to buffer to contain the dump
/// \param[in] sizeLongWords    Size of the dump buffer
/// \returns void
///
void NvmMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords)
{
    uint32_t sizeBytes = sizeLongWords * (sizeof(uint32_t));

    if (NULL != dumpBuffer)
    {
        uint32_t dumpSizeBytes = sizeof(nvmMgr);

        if (dumpSizeBytes > sizeBytes)
        {
            dumpSizeBytes = sizeBytes;
        }

        (void)memcpy((void*)dumpBuffer, (void*)&nvmMgr, dumpSizeBytes);
    }
}

/// Initializes internal control structure
///
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failure
ErrorT NvmMgrInit(void)
{
    ErrorT error = ERROR_SUCCESS;

    (void)memset(&nvmMgr, 0, sizeof(nvmMgr));
    // blankAddressLongWords tracks next blank location. Initialise it to beyond the end of the array to start with. It is
    // evaluated later
    nvmMgr.blankAddressLongWords = OtpmDrvGetArraySizeLongWords();

    error = DiagRegisterFatalErrorDumpHandler(NvmMgrOnDiagFatalErrorDump);
    if (ERROR_SUCCESS == error)
    {
        error = SystemMgrRegisterClockSpeedChangeNotifier(NvmMgrOnClockChange);
    }

    return error;
}

/// Finds asset of the specified type in NVM
///
/// \param[in]      assetType               Identifies type of asset to find
/// \param[in,out]  startAddressLongWords   Starting point for search, updated with new address if asset found
/// \param[in,out]  record                  Pointer to record header structure
///
/// \returns Function success/failure
/// \retval ERROR_SUCCESS       Asset located, startAddressLongWords and record valid
/// \retval ERROR_NOENT         Failed to locate asset
/// \retval ERROR_SYSERR        OTPM read request contained invalid parameters or input parameters invalid
///
ErrorT NvmMgrFindAsset(AssetMgrAssetTypeT const assetType,
    NvmMgrAddressT* startAddressLongWords,
    NvmMgrRecordHeaderT* record)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT addressLongWords;
    const uint32_t recordLengthLongWords = sizeof(NvmMgrRecordHeaderT) / sizeof(uint32_t);
    const uint32_t nvmLengthLongWords = OtpmDrvGetArraySizeLongWords();
    bool terminateSearch = false;

    if (NULL == record)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_RECORD, 0U);
        error = ERROR_SYSERR;
    }
    else if (NULL == startAddressLongWords)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS, 0U);
        error = ERROR_SYSERR;
    }
    else if (nvmLengthLongWords < *startAddressLongWords)
    {
        // Pointing beyond the first location beyond array, invalid request some sort of error
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS_VALUE,
            *startAddressLongWords);
        error = ERROR_SYSERR;
    }
    else if (nvmLengthLongWords == *startAddressLongWords)
    {
        // Pointing to first location beyond array, no point searching OTPM full
        error = ERROR_NOENT;
    }
    else
    {
        addressLongWords = *startAddressLongWords;
        do
        {
            error = OtpmDrvRead(addressLongWords, (uint32_t*)record, recordLengthLongWords);
            if (ERROR_SUCCESS == error)
            {
                // Valid OTPM entry ready
                if ((uint16_t)ASSET_MGR_ASSET_TYPE_EMPTY == record->assetId)
                {
                    if (0U != record->assetLengthLongWords)
                    {
                        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_LENGTH_NOT_ZERO_IN_EMPTY,
                            record->assetLengthLongWords);
                        error = ERROR_SYSERR;
                    }
                    else
                    {
                        // Blank location found
                        error = ERROR_NOENT;
                        nvmMgr.blankAddressLongWords = addressLongWords;
                    }
                    terminateSearch = true;
                }
                else if ((uint16_t)assetType == record->assetId)
                {
                    // Asset found
                    *startAddressLongWords = addressLongWords + recordLengthLongWords;
                    terminateSearch = true;
                }
                else
                {
                    // Increment pointers and look again
                    addressLongWords += (NvmMgrAddressT)(record->assetLengthLongWords + recordLengthLongWords);
                    if (nvmLengthLongWords == addressLongWords)
                    {
                        // Points to first location beyond the array i.e. OTPM full
                        error = ERROR_NOENT;
                        nvmMgr.blankAddressLongWords = nvmLengthLongWords;
                        terminateSearch = true;
                    }
                    if (nvmLengthLongWords < addressLongWords)
                    {
                        // Index now points beyond the first location past the end of the array
                        // this is an invalid scenario indicating a corruption in the last record length field
                        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_RECORD_LENGTH,
                            addressLongWords);
                        error = ERROR_SYSERR;
                        nvmMgr.blankAddressLongWords = nvmLengthLongWords;
                        terminateSearch = true;
                    }
                }
            }
            else
            {
                // All error scenarios terminate loop
                terminateSearch = true;
            }
        }
        while (false == terminateSearch);
    }

    return error;
}

/// Find free space in NVM
///
/// This function searches the OTPM for first unprogrammed location and returns the address via pointer startAddressLongWords
/// If the function fails to find any free space it will return address of first location beyond the end of NVM via startAddressLongWords.
///
/// \param[out]  startAddressLongWords      If successful point to start of NVM free space, otherwise end of NVM
///
/// \returns Function success/failure
/// \retval ERROR_SUCCESS       Free space located, address returned via startAddressLongWords
/// \retval ERROR_NOSPC         OTPM Full
/// \retval ERROR_SYSERR        Input pointer is NULL, length field corrupt resulting in indexing beyond first
///                             location at and of array or OtpmDrvRead() returned unexpected error
///
ErrorT NvmMgrFindFreeSpace(NvmMgrAddressT* const startAddressLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT addressLongWords;
    const uint32_t recordLengthLongWords = sizeof(NvmMgrRecordHeaderT) / sizeof(uint32_t);
    const uint32_t nvmLengthLongWords = OtpmDrvGetArraySizeLongWords();
    const NvmMgrAddressT nvmUserSpaceAddress = OtpmDrvGetUserSpaceAddress();
    NvmMgrRecordHeaderT record;
    bool terminateSearch = false;

    if (NULL == startAddressLongWords)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_PARAM_START_ADDRESS, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        for (addressLongWords = nvmUserSpaceAddress; addressLongWords < nvmLengthLongWords; )
        {
            error = OtpmDrvRead(addressLongWords, (uint32_t*)&record, recordLengthLongWords);
            if (ERROR_SUCCESS == error)
            {
                // Valid OTPM entry ready
                if ((uint16_t)ASSET_MGR_ASSET_TYPE_EMPTY == record.assetId)
                {
                    if (0U != record.assetLengthLongWords)
                    {
                        SystemMgrReportFatalError(
                            SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY,
                            record.assetLengthLongWords);
                        error = ERROR_SYSERR;
                    }
                    else
                    {
                        *startAddressLongWords = addressLongWords;
                    }
                    terminateSearch = true;
                }
                else
                {
                    // Increment pointers and look again
                    addressLongWords += (NvmMgrAddressT)(record.assetLengthLongWords + recordLengthLongWords);
                }
            }
            else
            {
                // INFO: <RW>: This in fact should be unreachable based on arguments that are passed to OtpmDrvRead()
                // however this else has been added to defend against any future (patched) changes to function
                // All error scenarios terminate loop
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_OTPM_READ_UNEXPECTED_ERROR,
                    (uint32_t const)error);
                error = ERROR_SYSERR;
                terminateSearch = true;
            }
            if (true == terminateSearch)
            {
                break;
            }
        }
        if (false == terminateSearch)
        {
            if (nvmLengthLongWords < addressLongWords)
            {
                // The last read record length has indexed beyond the first location past the end of the array
                // this is an invalid scenario indicating a corruption in that record
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_RECORD_LENGTH,
                    addressLongWords);
                error = ERROR_SYSERR;
            }
            else
            {
                // No space found in array
                *startAddressLongWords = nvmLengthLongWords;
                error = ERROR_NOSPC;
            }
        }
    }

    return error;
}
/// @} endgroup nvmmgr

