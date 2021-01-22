#if !defined(SRC__NVMMGR__INC__NVMMGR_H)
#define      SRC__NVMMGR__INC__NVMMGR_H

//***********************************************************************************
/// \file
/// NVM Manager public API
///
/// \defgroup nvmmgr   NVM Manager
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
#include "SystemMgr.h"
#include "AssetMgr.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents an address in NVM
typedef uint32_t NvmMgrAddressT;

/// NVM Manager find context (opaque)
typedef struct
{
    uint32_t context[4];                ///< opaque context
} NvmMgrFindContextT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Handles entry to an operating phase
extern ErrorT NvmMgrOnPhaseEntry(SystemMgrPhaseT const phase);

// Finds the first asset of the specified type in NVM
extern ErrorT NvmMgrFindFirst(NvmMgrFindContextT* const context,
    AssetMgrAssetTypeT const assetType,
    NvmMgrAddressT* const assetAddress,
    uint32_t* const assetLengthLongWords);

// Finds the next asset of the specified type in NVM
extern ErrorT NvmMgrFindNext(NvmMgrFindContextT* const context,
    NvmMgrAddressT* const assetAddress,
    uint32_t* const assetLengthLongWords);

// Reads all or part of an asset from NVM
extern ErrorT NvmMgrReadAsset(NvmMgrAddressT const assetAddress,
    uint32_t* const buffer,
    uint32_t const offset,
    uint32_t const assetLengthLongWords);

// Writes a record containing an asset to NVM
extern ErrorT NvmMgrWriteAsset(AssetMgrAssetTypeT const assetType,
    uint32_t* const buffer,
    uint32_t const assetLengthLongWords);

// Get the start of the OTPM blank space
extern ErrorT NvmMgrGetFreeSpaceAddress(NvmMgrAddressT* const offset);

// @} endgroup nvmmgr
#endif  // !defined(SRC__NVMMGR__INC__NVMMGR_H)

