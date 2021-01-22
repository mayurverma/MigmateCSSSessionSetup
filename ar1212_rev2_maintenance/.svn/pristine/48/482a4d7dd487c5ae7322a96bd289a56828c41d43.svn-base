#if !defined(SRC__ASSETMGR__INC__ASSETMGR_H)
#define      SRC__ASSETMGR__INC__ASSETMGR_H

//***********************************************************************************
/// \file
/// Asset Manager public API
///
/// \defgroup assetmgr   Asset Manager
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
#include "CommandHandler.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#define ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES          256U        ///< 2048-bit key/modulus
#define ASSET_MGR_RSA_2048_SIGNATURE_SIZE_BYTES           256U        ///< 2048-bit signature
#define ASSET_MGR_RSA_2048_PRIVATE_EXPONENT_SIZE_BYTES    256U        ///< 2048-bit exponent

#define ASSET_MGR_RSA_3072_PUBLIC_KEY_SIZE_BYTES          384U        ///< 3072-bit key/modulus
#define ASSET_MGR_RSA_3072_SIGNATURE_SIZE_BYTES           384U        ///< 3072-bit signature
#define ASSET_MGR_RSA_3072_PRIVATE_EXPONENT_SIZE_BYTES    384U        ///< 3072-bit exponent

#define ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES          512U        ///< 4096-bit key/modulus
#define ASSET_MGR_RSA_4096_SIGNATURE_SIZE_BYTES           512U        ///< 4096-bit signature
#define ASSET_MGR_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES    512U        ///< 4096-bit exponent

#define ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES    16U         ///< 128-bit pre-shared master secret
#define ASSET_MGR_PSK_MASTER_SECRET_192_KEY_SIZE_BYTES    24U         ///< 192-bit pre-shared master secret
#define ASSET_MGR_PSK_MASTER_SECRET_256_KEY_SIZE_BYTES    32U         ///< 256-bit pre-shared master secret

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Asset type identifiers (note these are also record identifiers)
typedef enum
{
    ASSET_MGR_ASSET_TYPE_EMPTY = 0x00U,                   ///< Empty record (0x00000000)
    ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE = 0x01U,  ///< RSA public certificate
    ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY = 0x02U,         ///< RSA private key
    ASSET_MGR_ASSET_TYPE_PSK_MASTER_SECRET = 0x03U,       ///< Pre-shared master secret
    ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION = 0x04U,   ///< TRNG characterization data
    ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION = 0x05U,      ///< OTPM configuration data
    ASSET_MGR_ASSET_TYPE_PROVISIONING_ID = 0x06U,         ///< Provisioning Identifier
} AssetMgrAssetTypeT;

/// Certificate Type identfiers
typedef enum
{
    ASSET_MGR_CERTIFICATE_TYPE_2048_BIT = 0x4332,      ///< Type of the 2048 bits key certificate
    ASSET_MGR_CERTIFICATE_TYPE_3072_BIT = 0x4333,      ///< Type of the 3072 bits key certificate
    ASSET_MGR_CERTIFICATE_TYPE_4096_BIT = 0x4334,      ///< Type of the 4096 bits key certificate
    ASSET_MGR_CERTIFICATE_TYPE_RESERVED = 0x4335,      ///< Reserved for future use
} AssetMgrCertificateTypeT;

/// Certificate Authority identfiers
typedef enum
{
    ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A = 0x00U,   ///< Identifies certificates signed by the Vendor A certificate authority
    ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B = 0x01U,   ///< Identifies certificates signed by the (optional) Vendor B certificate authority
    ASSET_MGR_CERTIFICATE_AUTH_ID_RESERVED1 = 0x02U,  ///< Reserved for future use
    ASSET_MGR_CERTIFICATE_AUTH_ID_RESERVED2 = 0x03U,  ///< Reserved for future use
} AssetMgrCertificateAuthIdT;

/// Certificate purpose identifiers
typedef enum
{
    ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR = 0x00U,     ///< The Vendor certificate signed by the Vendor certificate authority
    ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR = 0x01U,     ///< The Sensor certificate signed by the Vendor certificate authority
    ASSET_MGR_CERTIFICATE_PURPOSE_RESERVED1 = 0x02U,  ///< Reserved for future use
    ASSET_MGR_CERTIFICATE_PURPOSE_RESERVED2 = 0x03U,  ///< Reserved for future use
} AssetMgrCertificatePurposeT;

/// Represents a certificate version
typedef uint8_t AssetMgrCertificateVersionT;

/// Represents a certificate number
typedef uint32_t AssetMgrCertificateNumberT;

/// RSA certificate header
typedef struct
{
    uint16_t typeIdentifier;             ///< Known type identifier (0x43 0x32|0x33|0x34). Identifies the type of the certificate (its format).
    uint16_t signatureOffsetLongWords;   ///< Long-word offset to the signature section
    uint32_t certificateFlags;           ///< Identifiers and certificate number
} AssetMgrRsaPubCertHeaderT;

/// Abstract RSA public certificate
typedef struct
{
    AssetMgrRsaPubCertHeaderT header;                     ///< Certificate header
    uint8_t publicKey[1];                                 ///< The publicKey (virtual)
} AssetMgrRsaPubCertT;

/// 2048-bit RSA public certificate
typedef struct
{
    AssetMgrRsaPubCertHeaderT header;                             ///< Certificate header
    uint8_t publicKey[ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[ASSET_MGR_RSA_2048_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} AssetMgrRsa2048PubCertT;

/// 3072-bit RSA public certificate
typedef struct
{
    AssetMgrRsaPubCertHeaderT header;                             ///< Certificate header
    uint8_t publicKey[ASSET_MGR_RSA_3072_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[ASSET_MGR_RSA_3072_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} AssetMgrRsa3072PubCertT;

/// 4096-bit RSA public certificate
typedef struct
{
    AssetMgrRsaPubCertHeaderT header;                             ///< Certificate header
    uint8_t publicKey[ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[ASSET_MGR_RSA_4096_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} AssetMgrRsa4096PubCertT;

/// Represents an asymmetric authentication key
typedef enum
{
    ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED0 = 0,        ///< Reserved
    ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048 = 1U,  ///< 2048-bit RSA private key in basic format
    ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_3072 = 2U,  ///< 3072-bit RSA private key in basic format
    ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_4096 = 3U,  ///< 4096-bit RSA private key in basic format
    ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED4 = 4U,       ///< Reserved for future use
} AssetMgrSensorAuthPrivKeyIdT;

/// RSA private key header
typedef struct
{
    uint16_t certificateAuthId;  ///< Identifies the certificate authority issuing the private key (AssetMgrCertificateAuthIdT)
    uint16_t authPrivKeyId;      ///< Identifies the private key type (AssetMgrSensorAuthPrivKeyIdT)
} AssetMgrRsaPrivKeyHeaderT;

/// Abstract RSA private key
typedef struct
{
    AssetMgrRsaPrivKeyHeaderT header;  ///< Private key header
    uint8_t exponent[1];               ///< RSA private exponent (virtual)
} AssetMgrRsaPrivKeyT;

/// 2048-bit RSA private Key
typedef struct
{
    AssetMgrRsaPrivKeyHeaderT header;                                  ///< Private key header
    uint8_t exponent[ASSET_MGR_RSA_2048_PRIVATE_EXPONENT_SIZE_BYTES];  ///< RSA private exponent
} AssetMgrRsa2048PrivKeyT;

/// 3072-bit RSA private Key
typedef struct
{
    AssetMgrRsaPrivKeyHeaderT header;                                  ///< Private key header
    uint8_t exponent[ASSET_MGR_RSA_3072_PRIVATE_EXPONENT_SIZE_BYTES];  ///< RSA private exponent
} AssetMgrRsa3072PrivKeyT;

/// 4096-bit RSA private Key
typedef struct
{
    AssetMgrRsaPrivKeyHeaderT header;                                  ///< Private key header
    uint8_t exponent[ASSET_MGR_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES];  ///< RSA private exponent
} AssetMgrRsa4096PrivKeyT;

/// PSK Master Secret header
typedef struct
{
    uint16_t masterSecretKeyType;   ///< Identifies the symmetric key type (CryptoMgrSymmetricKeyTypeT)
    uint16_t pad;                   ///< Padding
} AssetMgrPskMasterSecretHeaderT;

/// Abstract PSK Master Secret
typedef struct
{
    AssetMgrPskMasterSecretHeaderT header;  ///< PSK master secret header
    uint8_t masterSecret[1];                ///< pre-shared master secret (virtual)
} AssetMgrPskMasterSecretT;

/// 128-bit PSK Master Secret
typedef struct
{
    AssetMgrPskMasterSecretHeaderT header;                                 ///< PSK master secret header
    uint8_t masterSecret[ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES];  ///< 128-bit pre-shared master secret
} AssetMgrPskMasterSecret128T;

/// 192-bit PSK Master Secret
typedef struct
{
    AssetMgrPskMasterSecretHeaderT header;                                 ///< PSK master secret header
    uint8_t masterSecret[ASSET_MGR_PSK_MASTER_SECRET_192_KEY_SIZE_BYTES];  ///< 192-bit pre-shared master secret
} AssetMgrPskMasterSecret192T;

/// 256-bit PSK Master Secret
typedef struct
{
    AssetMgrPskMasterSecretHeaderT header;                                 ///< PSK master secret header
    uint8_t masterSecret[ASSET_MGR_PSK_MASTER_SECRET_256_KEY_SIZE_BYTES];  ///< 256-bit pre-shared master secret
} AssetMgrPskMasterSecret256T;

/// Represents CC312 TRNG characterization data
typedef struct
{
    uint32_t R0;  ///< R0 parameter
    uint32_t R1;  ///< R1 parameter
    uint32_t R2;  ///< R2 parameter
    uint32_t R3;  ///< R3 parameter
} AssetMgrCC312TrngCharT;

/// Represents OTPM configuration data
typedef struct
{
    uint32_t tvpps_ns;          ///< Charge Pump Warm-up time in nSec
    uint32_t tpas_ns;           ///< Program Address Setup time in nSec
    uint32_t tpp_ns;            ///< Program pulse width in nSec
    uint32_t tpr_ns;            ///< Program recovery time in nSec
    uint32_t tspp_ns;           ///< Soak pulse width in nSec
    uint32_t tpsr_ns;           ///< Power Supply Recovery time in nSec
    uint16_t tds_ns;            ///< Data setup time in nSec
    uint16_t twr_ns;            ///< Write Recovery time in nSec
    uint16_t twp_ns;            ///< Write Pulse width in nSec
    uint16_t tras_ns;           ///< Read address setup time in nSec
    uint16_t trr_ns;            ///< Read recovery time in nSec
    uint16_t soak_count_max;    ///< Maximum soak counter (retry)
    uint16_t mr_pgm;            ///< MR value for OTPM programming
    uint16_t mr_pgm_soak;       ///< MR value for OTPM program soak
    uint16_t mr_read;           ///< MR value for OTPM read
    uint16_t mra_pgm;           ///< MRA value for OTPM programming
    uint16_t mra_prg_soak;      ///< MRA value for OTPM program soak
    uint16_t mra_read;          ///< MRA value for OTPM read
    uint16_t mrb_pgm;           ///< MRB value for OTPM programming
    uint16_t mrb_prg_soak;      ///< MRB value for OTPM program soak
    uint16_t mrb_read;          ///< MRB value for OTPM read
    uint16_t pad;               ///< Padding so that structure is a multiple of 4-bytes
} AssetMgrOtpmConfigT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Handles entry to an operating phase
extern ErrorT AssetMgrOnPhaseEntry(SystemMgrPhaseT const phase);
// Handles the ProvisionAsset host command
extern ErrorT AssetMgrOnProvisionAsset(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);
// Get pointer to the public key (modulus) within the specified certificate, and its size
extern ErrorT AssetMgrGetPublicKeyReference(AssetMgrRsaPubCertT* const pubCert,
    uint8_t** pubKey,
    uint32_t* const pubKeySizeBytes);
// Retrieves the 'latest' public certificate issued by the specified authority from NVM
extern ErrorT AssetMgrRetrieveCertificate(AssetMgrCertificateAuthIdT const certAuthId,
    AssetMgrCertificatePurposeT const certPurpose,
    AssetMgrRsaPubCertT* const pubCert,
    uint32_t* const certSizeLongWords);
// Retrieves the 'latest' private key issued by the specified authority from NVM
extern ErrorT AssetMgrRetrievePrivateKey(AssetMgrCertificateAuthIdT const certAuthId,
    uint8_t* const privKey,
    uint32_t* const privKeySizeLongWords);
// Retrieves the 'latest' PSK Master Secret from NVM
extern ErrorT AssetMgrRetrievePskMasterSecret(AssetMgrPskMasterSecretT* masterSecret,
    uint32_t* const masterSecretSizeLongWords);
// Retrieves the 'latest' CC312 TRNG characterization data from NVM
extern ErrorT AssetMgrRetrieveCC312TrngChar(AssetMgrCC312TrngCharT* CC312TrngChar);
// Retrieves the 'latest' OTPM configuration data from NVM
extern ErrorT AssetMgrRetrieveOtpmConfig(AssetMgrOtpmConfigT* OTPMConfig);

/// @} endgroup assetmgr

#endif  // !defined(SRC__ASSETMGR__INC__ASSETMGR_H)

