#if !defined(SRC__NVMMGR__SRC__NVMMGRINTERNAL_H)
#define      SRC__NVMMGR__SRC__NVMMGRINTERNAL_H
//******************************************************************************
/// \file
///
/// Provides NvmMgr internal header file
///
/// \addtogroup nvmmgr
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "Common.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define NVM_MGR_MAGIC       0xCAFEBEEFU

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// NVM Manager control/status structure
typedef struct
{
    bool isInitialized;                         ///< Component initialized
    NvmMgrAddressT blankAddressLongWords;       ///< Start of blank area in OTPM
} NvmMgrT;

/// NVM Manager find context (internal) -
typedef struct
{
    uint32_t magic;                 ///< magic number to identify object
    NvmMgrAddressT currentAddress;  ///< current search address (record header)
    uint32_t assetLengthLongWords;  ///< Length of asset
    AssetMgrAssetTypeT assetType;   ///< asset type identifier
} NvmMgrFindContextConcreteT;

/// NVM Record header format
typedef struct
{
    uint16_t assetId;                   //< Identifies the asset
    uint16_t assetLengthLongWords;      //< Length of the asset in long words
} NvmMgrRecordHeaderT;
//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
/// Patch Manager internal control/state structure
extern NvmMgrT nvmMgr;

// Initializes internal control structure
extern ErrorT NvmMgrInit(void);

// Returns address of start of user space
extern NvmMgrAddressT NvmMgrGetUserSpaceAddress(void);

// Called by System Manager when there is a system clock frequency change
extern ErrorT NvmMgrOnClockChange(uint32_t const clockSpeedHz);

// Locates asset of specified type, search starting from supplied address
extern ErrorT NvmMgrFindAsset(AssetMgrAssetTypeT const assetType, NvmMgrAddressT* startAddressLongWords,
    NvmMgrRecordHeaderT* record);

// Dumps NVM Manager state in response to a fatal error
extern void NvmMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords);

// Find free space in NVM
extern ErrorT NvmMgrFindFreeSpace(NvmMgrAddressT* const startAddressLongWords);

/// @} endgroup nvmmgr
#endif  // !defined(SRC__NVMMGR__SRC__NVMMGRINTERNAL_H)

