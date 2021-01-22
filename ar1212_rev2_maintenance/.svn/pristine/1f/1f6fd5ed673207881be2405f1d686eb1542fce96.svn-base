//***********************************************************************************
/// \file
///
/// NvmMgr default Mocks
//
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

#include "cpputest_support.h"

extern "C"
{
#include "Common.h"
#include "NvmMgr.h"
#include "AssetMgr.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_NVM_MGR_ON_PHASE_ENTRY)
    // Handles entry to an operating phase
    ErrorT NvmMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("NvmMgrOnPhaseEntry")
           .withParameter("phase", phase);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_NVM_MGR_FIND_FIRST)
    // Finds the first asset of the specified type in NVM
    ErrorT NvmMgrFindFirst(NvmMgrFindContextT* const context,
        AssetMgrAssetTypeT const assetType,
        NvmMgrAddressT* const assetAddress,
        uint32_t* const assetLengthLongWords)
    {
        mock().actualCall("NvmMgrFindFirst")
                   .withOutputParameter("context", context)
                   .withParameter("assetType", assetType)
                   .withOutputParameter("assetAddress", assetAddress)
                   .withOutputParameter("assetLengthLongWords", assetLengthLongWords);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_NVM_MGR_FIND_NEXT)
    // Finds the next asset of the specified type in NVM
    ErrorT NvmMgrFindNext(NvmMgrFindContextT* const context,
        NvmMgrAddressT* const assetAddress,
        uint32_t* const assetLengthLongWords)
    {
        mock().actualCall("NvmMgrFindNext")
                   .withOutputParameter("context", context)
                   .withOutputParameter("assetAddress", assetAddress)
                   .withOutputParameter("assetLengthLongWords", assetLengthLongWords);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_NVM_MGR_READ_ASSET)
    // Reads all or part of an asset from NVM
    ErrorT NvmMgrReadAsset(NvmMgrAddressT const assetAddress,
        uint32_t* const buffer,
        uint32_t const offset,
        uint32_t const assetLengthLongWords)
    {

        mock().actualCall("NvmMgrReadAsset")
                           .withParameter("assetAddress", assetAddress)
                           .withOutputParameter("buffer", buffer)
                           .withParameter("offset", offset)
                           .withParameter("assetLengthLongWords", assetLengthLongWords);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_NVM_MGR_WRITE_ASSET)
    // Writes a record containing an asset to NVM
    ErrorT NvmMgrWriteAsset(AssetMgrAssetTypeT const assetType,
        uint32_t* const buffer,
        uint32_t const assetLengthLongWords)
    {
        mock().actualCall("NvmMgrWriteAsset")
                           .withParameter("assetType", assetType)
                           .withPointerParameter("buffer", buffer)
                           .withParameter("assetLengthLongWords", assetLengthLongWords);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_NVM_MGR_GET_FREE_SPACE_ADDRESS)
    ErrorT NvmMgrGetFreeSpaceAddress(NvmMgrAddressT* const offset)
    {
        mock().actualCall("NvmMgrGetFreeSpaceAddress")
                .withOutputParameter("offset",offset);
        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

}
