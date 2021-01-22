#if !defined(SRC__ASSETMGR__SRC__ASSETMGRINTERNAL_H)
#define      SRC__ASSETMGR__SRC__ASSETMGRINTERNAL_H

//***********************************************************************************
/// \file
/// Asset Manager private API
///
/// \addtogroup assetmgr   Asset Manager
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
#include "CommandHandler.h"
#include "NvmMgr.h"
#include "LifecycleMgr.h"
#include "CryptoMgr.h"
#include "Diag.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

// Certificate identifier field helpers - AssetMgrCertificatePurposeT
#define ASSET_MGR_CERT_FLAGS_PURPOSE__SIZE    2U  ///< Bit size for the certificate purpose inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET  0U  ///< Bit offset for the certificate purpose inside the certificateFlags
/// Mask for the certificate purpose inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_PURPOSE__MASK    \
    ((((uint32_t)1U << ASSET_MGR_CERT_FLAGS_PURPOSE__SIZE) - 1U) << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET)

// Certificate Authority identifier  field helpers - AssetMgrCertificateAuthIdT
#define ASSET_MGR_CERT_FLAGS_AUTH_ID__SIZE    2U  ///< Bit size for the certificate authority ID inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET  2U  ///< Bit offset for the certificate authority ID inside the certificateFlags
/// Mask for the certificate authority ID inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_AUTH_ID__MASK    \
    ((((uint32_t)1U << ASSET_MGR_CERT_FLAGS_AUTH_ID__SIZE) - 1U) << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET)

// Certificate version  field helpers- AssetMgrCertificateVersionT
#define ASSET_MGR_CERT_FLAGS_VERSION__SIZE    2U   ///< Bit size for the certificate version inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_VERSION__OFFSET  4U   ///< Bit offset for the certificate version inside the certificateFlags
/// Mask for the certificate version inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_VERSION__MASK    \
    ((((uint32_t)1U << ASSET_MGR_CERT_FLAGS_VERSION__SIZE) - 1U) << ASSET_MGR_CERT_FLAGS_VERSION__OFFSET)

// Certificate unique number  field helpers - AssetMgrCertificateNumberT
#define ASSET_MGR_CERT_FLAGS_NUMBER__SIZE     26U  ///< Bit size for the certificate number inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_NUMBER__OFFSET   6U   ///< Bit offset for the certificate number inside the certificateFlags
/// Mask for the certificate number inside the certificateFlags
#define ASSET_MGR_CERT_FLAGS_NUMBER__MASK     \
    ((((uint32_t)1U << ASSET_MGR_CERT_FLAGS_NUMBER__SIZE) - 1U) << ASSET_MGR_CERT_FLAGS_NUMBER__OFFSET)

#define ASSET_MGR_NONCE_SIZE_LONG_WORDS                    3U  ///< Size of the nonce(long words) in an encrypted Asset
#define ASSET_MGR_RESERVED_SIZE_LONG_WORDS                 2U  ///< Size of the reserved field(long words) in an encrypted Asset

// As per specification the Asset manager only allows to write certain number of assets in OTPM
// Each asset type has a maximum number of assets allowed:

/// Maximum number of PSK, TRNG characterization or OTPM config type assets in OTPM
#define ASSET_MGR_MAX_TRNG_OTPM_PSK_ASSETS_ALLOWED            2U
#define ASSET_MGR_MAX_PUB_CERT_4096_ASSETS_ALLOWED            4U  ///< Maximum number of 4K - bits Public Certificates in OTPM
#define ASSET_MGR_MAX_PUB_CERT_2048_3072_ASSETS_ALLOWED       6U  ///< Maximum number of 2K / 3K - bits Public Certificates in OTPM
#define ASSET_MGR_MAX_PRIVATE_RSA_4096_ASSETS_ALLOWED         2U  ///< Maximum number of 4K - bits RSA private keys in OTPM
#define ASSET_MGR_MAX_PRIVATE_RSA_2048_3072_ASSETS_ALLOWED    3U  ///< Maximum number of 2K / 3K - bits RSA private keys in OTPM

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// ProvisionAsset Command Request
typedef struct
{
    uint32_t assetId;                                       ///< Unique identifier for the asset
    uint32_t clockSpeedHz;                                  ///< Crypto subsystem clock speed in Hertz
    uint16_t rootOfTrust;                                   ///< Identifies the asset’s root-of-trust
    uint16_t pad;                                           ///< Padding
    // The following values match the CCBsvAssetProv_t
    uint32_t token;                                         ///< The token for the asset
    uint32_t version;                                       ///< The version of the asset format
    uint32_t assetSize;                                     ///< The asset size in bytes
    uint32_t reserved[ASSET_MGR_RESERVED_SIZE_LONG_WORDS];  ///< Reserved bytes
    uint32_t nonce[ASSET_MGR_NONCE_SIZE_LONG_WORDS];        ///< The nonce for the CCM encryption
    uint32_t encAsset[1];                                   ///< The asset data encoded (virtual) the tag is appended at the end
} AssetMgrProvisionAssetRequestParamsT;

//
// Asset support
//

/// Generic header for each asset
typedef struct
{
    uint16_t assetType;              ///< Identifies the asset (AssetMgrAssetTypeT)
    uint16_t assetLengthLongWords;   ///< Size of the asset in long words (32 bits)
} AssetMgrAssetHeaderT;

/// Represents an abstract 'asset'
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    uint32_t payload[1];          ///< The asset
} AssetMgrAssetT;

/// Represents an abstract RSA Public Certificate asset
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    AssetMgrRsaPubCertT cert;     ///< RSA public certificate
} AssetMgrRsaPubCertAssetT;

/// 2048-bit RSA Public Certificate asset
typedef struct
{
    AssetMgrAssetHeaderT header;   ///< Identifies the asset
    AssetMgrRsa2048PubCertT cert;  ///< 2048-bit RSA public certificate
} AssetMgrRsa2048PubCertAssetT;

/// 3072-bit RSA Public Certificate asset
typedef struct
{
    AssetMgrAssetHeaderT header;   ///< Identifies the asset
    AssetMgrRsa3072PubCertT cert;  ///< 3072-bit RSA public certificate
} AssetMgrRsa3072PubCertAssetT;

/// 4096-bit RSA Public Certificate asset
typedef struct
{
    AssetMgrAssetHeaderT header;   ///< Identifies the asset
    AssetMgrRsa4096PubCertT cert;  ///< 4096-bit RSA public certificate
} AssetMgrRsa4096PubCertAssetT;

/// Represents an abstract RSA private key asset
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    AssetMgrRsaPrivKeyT key;      ///< RSA private key
} AssetMgrRsaPrivKeyAssetT;

/// 2048-bit RSA Private Key asset
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    AssetMgrRsa2048PrivKeyT key;  ///< 2048-bit RSA private key
} AssetMgrRsa2048PrivKeyAssetT;

/// 3072-bit RSA Private Key asset
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    AssetMgrRsa3072PrivKeyT key;  ///< 3072-bit RSA private key
} AssetMgrRsa3072PrivKeyAssetT;

/// 4096-bit RSA Private Key asset
typedef struct
{
    AssetMgrAssetHeaderT header;  ///< Identifies the asset
    AssetMgrRsa4096PrivKeyT key;  ///< 4096-bit RSA private key
} AssetMgrRsa4096PrivKeyAssetT;

/// Represents an abstract PSK Master Secret asset
typedef struct
{
    AssetMgrAssetHeaderT header;            ///< Identifies the asset
    AssetMgrPskMasterSecretT masterSecret;  ///< PSK master secret
} AssetMgrPskMasterSecretAssetT;

/// 128-bit Master Secret asset
typedef struct
{
    AssetMgrAssetHeaderT header;               ///< Identifies the asset
    AssetMgrPskMasterSecret128T masterSecret;  ///< Pre-shared master secret
} AssetMgrPskMasterSecret128AssetT;

/// 192-bit Master Secret asset
typedef struct
{
    AssetMgrAssetHeaderT header;               ///< Identifies the asset
    AssetMgrPskMasterSecret192T masterSecret;  ///< Pre-shared master secret
} AssetMgrPskMasterSecret192AssetT;

/// 256-bit Master Secret asset
typedef struct
{
    AssetMgrAssetHeaderT header;               ///< Identifies the asset
    AssetMgrPskMasterSecret256T masterSecret;  ///< Pre-shared master secret
} AssetMgrPskMasterSecret256AssetT;

/// CC312 TRNG characterization asset
typedef struct
{
    AssetMgrAssetHeaderT header;      ///< Identifies the asset
    AssetMgrCC312TrngCharT TRNGChar;  ///< CC312 TRNG characterization parameters
} AssetMgrCC312TrngCharAssetT;

/// OTPM configuration asset
typedef struct
{
    AssetMgrAssetHeaderT header;     ///< Identifies the asset
    AssetMgrOtpmConfigT OTPMConfig;  ///< OTPM configuration parameters
} AssetMgrOtpmConfigAssetT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Retrieves the certificate purpose identifier (from the given certificate)
extern AssetMgrCertificatePurposeT AssetMgrGetCertificatePurpose(uint32_t certificateFlags);

// Retrieves the certificate authority identifier (from the given certificate)
extern AssetMgrCertificateAuthIdT AssetMgrGetCertificateAuthId(uint32_t certificateFlags);

// Retrieves the certificate version (from the given certificate)
extern AssetMgrCertificateVersionT AssetMgrGetCertificateVersion(uint32_t certificateFlags);

// Retrieves the certificate number (from the given certificate)
extern AssetMgrCertificateNumberT AssetMgrGetCertificateNumber(uint32_t certificateFlags);

// Get the flags fields from a Certificate in NVM
extern ErrorT AssetMgrGetCertFlagsFromPubCert(NvmMgrAddressT const assetAddress,
    AssetMgrCertificateAuthIdT* certAuthId,
    AssetMgrCertificatePurposeT* certPurpose,
    AssetMgrCertificateVersionT* certVersion,
    AssetMgrCertificateNumberT* certNumber);
// Get the Certificate Authority ID from a OTPM private key
extern ErrorT AssetMgrGetCertAuthIdFromPrivateKey(NvmMgrAddressT const assetAddress,
    AssetMgrCertificateAuthIdT* certAuthId);
// Retrieves the 'latest' Asset from NVM
extern ErrorT AssetMgrRetrieveLatestAsset(AssetMgrAssetTypeT const assetType,
    uint32_t* const buffer,
    uint32_t* const bufferSizeLongWords);
// Compare Public Certificate versions in OTPM
extern ErrorT AssetMgrCompareVersionWithOtpm(AssetMgrRsaPubCertT* const pubCert, uint32_t maxAllowedCount);
// Compare Asset with OTPM contents
extern ErrorT AssetMgrCompareAssetWithOtpm(AssetMgrAssetT* const asset, uint32_t maxAllowedCount);
// Get public key size in bytes from public certificate
extern ErrorT AssetMgrGetPublicKeySize(AssetMgrRsaPubCertT* const pubCert, uint32_t* keySize);
// Get private key size in bytes from asset
extern ErrorT AssetMgrGetPrivateKeySize(AssetMgrRsaPrivKeyT* const privateKeyAsset, uint32_t* keySize);
// Checks if the current LCS supports provisioning
extern ErrorT AssetMgrCheckCurrentLcsForProvisioning(CryptoMgrRootOfTrustT const rootOfTrust);

/// @} endgroup assetmgr

#endif  // !defined(SRC__ASSETMGR__SRC__ASSETMGRINTERNAL_H)

