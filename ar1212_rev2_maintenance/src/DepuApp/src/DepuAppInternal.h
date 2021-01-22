#if !defined(SRC__DEPUAPP__SRC__DEPUAPPINTERNAL_H)
#define      SRC__DEPUAPP__SRC__DEPUAPPINTERNAL_H

//***********************************************************************************
/// \file
/// DEPU Application header file, for internal use.
///
/// \addtogroup DepuApp
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
#include "DepuApp.h"
#include "AR0820.h"
#include "PatchMgr.h"
#include "CC312HalPal.h"
#include "cc_pal_types.h"
#include "cc_pal_trng.h"
#include "cc_cmpu.h"
#include "cc_dmpu.h"
#include "AssetMgr.h"
#include "AssetMgrInternal.h"
#include "cc_asset_prov.h"
#include "OtpmDrv.h"
#include "CC312HalPalPatch.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define DEPU_APP_NUM_PATCHED_FUNCTIONS 1U   ///< Number of functions to be patched (using HW comparators, the max is 32)

#define DEPU_APP_PROVISIONING_ID       DEPU_PATCH_VERSION  ///< This should added by the build system

/// Expected value for the first free address after NVM has been written (manually calculated)
#define DEPU_APP_EXPECTED_LAST_OTPM_ADDRESS_LW        379U

#define DEPU_APP_NONCE_SIZE_LONG_WORDS                    3U  ///< Size of the nonce(long words) in an encrypted Asset
#define DEPU_APP_RESERVED_SIZE_LONG_WORDS                 2U  ///< Size of the reserved field(long words) in an encrypted Asset

#define DEPU_APP_ASSET_HEADER_SIZE        (sizeof(DepuAppAssetT) - sizeof(uint32_t))
#define DEPU_APP_ASSET_MIN_BLOCK_SIZE     16U
#define DEPU_APP_ASSET_MAC_SIZE           16U

#define DEPU_APP_AES_KEY_SIZE             16U
#define DEPU_APP_RSA_KEY_SIZE             (2048U / 8U)
#define DEPU_APP_HBK_SIZE                 16U

// Payload sizes in bytes
#define DEPU_APP_PL_HEADER_SIZE           4U
#define DEPU_APP_PL_RSA_PUB_HEADER_SIZE   8U
#define DEPU_APP_PL_RSA_PRIV_HEADER_SIZE  4U

#define DEPU_APP_RSA_PUB_SIZE       (DEPU_APP_PL_HEADER_SIZE + DEPU_APP_PL_RSA_PUB_HEADER_SIZE + \
                                     DEPU_APP_RSA_KEY_SIZE + DEPU_APP_RSA_KEY_SIZE)

#define DEPU_APP_RSA_PRIV_SIZE      (DEPU_APP_PL_HEADER_SIZE + DEPU_APP_PL_RSA_PRIV_HEADER_SIZE + \
                                     DEPU_APP_RSA_KEY_SIZE)

#define DEPU_APP_RSA_2048_PUB_SIZE_BYTES    (((DEPU_APP_RSA_PUB_SIZE + DEPU_APP_ASSET_MIN_BLOCK_SIZE - 1U) / \
                                              DEPU_APP_ASSET_MIN_BLOCK_SIZE) * DEPU_APP_ASSET_MIN_BLOCK_SIZE)

#define DEPU_APP_RSA_2048_PUB_ASSET_SIZE_LW ((DEPU_APP_ASSET_HEADER_SIZE + \
                                              DEPU_APP_RSA_2048_PUB_SIZE_BYTES + DEPU_APP_ASSET_MAC_SIZE) / \
                                             sizeof(uint32_t))

#define DEPU_APP_RSA_2048_PRIV_SIZE_BYTES   (((DEPU_APP_RSA_PRIV_SIZE + DEPU_APP_ASSET_MIN_BLOCK_SIZE - 1U) / \
                                              DEPU_APP_ASSET_MIN_BLOCK_SIZE) * DEPU_APP_ASSET_MIN_BLOCK_SIZE)

#define DEPU_APP_RSA_2048_PRIV_ASSET_SIZE_LW ((DEPU_APP_ASSET_HEADER_SIZE + \
                                               DEPU_APP_RSA_2048_PRIV_SIZE_BYTES + DEPU_APP_ASSET_MAC_SIZE) / \
                                              sizeof(uint32_t))

#define DEPU_APP_AES_128_ASSET_SIZE_LW       ((DEPU_APP_ASSET_HEADER_SIZE + \
                                               DEPU_APP_AES_KEY_SIZE + DEPU_APP_ASSET_MAC_SIZE) / \
                                              sizeof(uint32_t))

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// ProvisionAsset Command Request
typedef struct
{
    uint32_t assetId;                                       ///< Unique identifier for the asset
    uint16_t rootOfTrust;                                   ///< Identifies the asset’s root-of-trust
    uint16_t pad;                                           ///< Padding
    // The following values match the CCBsvAssetProv_t
    uint32_t token;                                         ///< The token for the asset
    uint32_t version;                                       ///< The version of the asset format
    uint32_t assetSize;                                     ///< The asset size in bytes
    uint32_t reserved[DEPU_APP_RESERVED_SIZE_LONG_WORDS];   ///< Reserved bytes
    uint32_t nonce[DEPU_APP_NONCE_SIZE_LONG_WORDS];         ///< The nonce for the CCM encryption
    uint32_t encAsset[1];                                   ///< The asset data encoded (virtual) the tag is appended at the end
} DepuAppAssetT;

/// Encrypted assets and Public Hashes
typedef struct
{
    /// kpicv
    uint32_t kpicv[(uint32_t)PROD_ASSET_PKG_SIZE / sizeof(uint32_t)];
    /// kcp
    uint32_t kcp[DEPU_APP_AES_128_ASSET_SIZE_LW];
    /// HBK0
    uint8_t hbk0[DEPU_APP_HBK_SIZE];
    /// HBK1
    uint8_t hbk1[DEPU_APP_HBK_SIZE];
    /// Sensor certificate
    uint32_t rsaSensorCertificate[DEPU_APP_RSA_2048_PUB_ASSET_SIZE_LW];
    /// Vendor certificate
    uint32_t rsaVendorCertificate[DEPU_APP_RSA_2048_PUB_ASSET_SIZE_LW];
    /// Sensor Private Key
    uint32_t rsaPrivateKey[DEPU_APP_RSA_2048_PRIV_ASSET_SIZE_LW];
} DepuAppDataBaseT;

/// Depu Application control/state structure
typedef struct
{
    /// Default values for the DCU for CMPU
    uint32_t icvDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE];
    /// Default values for the DCU for DMPU
    uint32_t oemDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE];
    /// Allocate CMPU and DMPU and AssetMgrOnProvisionAsset work buffer. CMPU/AssetMgrOnProvisionAsset buffer is biggest
    uint32_t workSpaceBuffer[(uint32_t)CMPU_WORKSPACE_MINIMUM_SIZE / sizeof(uint32_t)];
    /// Temporary database containing all Depu assets
    DepuAppDataBaseT depuAppAssets;
} DepuAppT;

/// Represents the DEPU_PROGRAM_OTPM host command parameters
typedef struct
{
    uint32_t clockSpeedHz;                 ///< Crypto subsystem clock speed in Hertz
    uint16_t blockLengthBytes;             ///< Length of data block
    uint16_t checksum;                     ///< Checksum of data block, add all data together and invert
    DepuAppDataBaseT depuAppAssets;        ///< Database containing all Depu assets
} DepuAppProgramOtpmCommandParamsT;

/// Represents the DEPU_PROGRAM_OTPM host command's response parameters
typedef struct
{
    uint16_t errorAddress;              ///< The error address.
    uint16_t numberSoftErrors;          ///< The number of soft errors.
} DepuAppProgramOtpmResponseParamsT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
extern DepuAppT depuApp;
extern const PatchMgrComparatorFunctionAddressT depuAppComparatorTable[DEPU_APP_NUM_PATCHED_FUNCTIONS];

// After all patch chunks are downloaded, this function is called to load/hook the patched functions
extern ErrorT DepuAppLoader(void);

// Initialize the CC312 Secure Boot library
// We cannot #include "LifecycleMgrInternal.h" because it broke unit tests as CRYPTOCELL_CORE_BASE
// was defined to cryptoMemory[].
extern ErrorT LifecycleMgrCc312Init(void);

// DEPU App Decrypt and asset in a buffer
extern ErrorT DepuAppDecryptAsset(DepuAppAssetT* const asset, DepuAppAssetT* const assetDecrypted,
    uint32_t const assetSizeBytes);

// Insert a short delay between CC312 reset end polling NVM Idle register.
extern void DepuAppCc312ReadRegisterDelay(int32_t cycles);

// Reset CC312 hardware block.
extern void DepuAppCc312HardReset(void);

// Reworked HardFault_Handler. Ignore trap of DED errors.
extern void HardFault_PatchedHandler(void);

// Initialize DEPU App, to be called by PatchLoader.
extern ErrorT DepuAppInit(void);

// DEPU App CMPU stage processing.
extern ErrorT DepuAppCmpuProcess(void);

// DEPU App DMPU stage processing.
extern ErrorT DepuAppDmpuProcess(void);

// OnSemi Asset provisioning stage processing.
extern ErrorT DepuAppAssetProvisioning(void);

// DEPU App function provisions AR0820 with cyber-security
extern ErrorT DepuAppProgramOtpm(DepuAppProgramOtpmCommandParamsT* const params);

// Handles the Provision of an encrypted Asset
extern ErrorT DepuAppOnProvisionAsset(DepuAppAssetT* const asset, const uint32_t assetSizeBytes);

// OTPM write and verify a number of long words.
extern ErrorT DepuAppOtpmDrvWrite(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords);

// Writes a record containing an asset to NVM
extern ErrorT DepuAppWriteAsset(uint16_t const assetType,
    uint32_t* const buffer,
    uint32_t const assetLengthLongWords);

/// @} endgroup DepuApp
#endif  // !defined(SRC__DEPUAPP__SRC__DEPUAPPINTERNAL_H)

