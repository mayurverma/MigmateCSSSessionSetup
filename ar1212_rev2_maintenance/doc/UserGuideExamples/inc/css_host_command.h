#if !defined(CSS_HOST_COMMAND_H)
#define      CSS_HOST_COMMAND_H

//***********************************************************************************
/// \file
///
/// CSS Host Command types and function declarations
///
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
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "css_registers.h"
#include "css_shared_mem.h"

/// Command response codes
typedef enum
{
    ERROR_SUCCESS = 0U,       ///< 0x0000: Operation successful
    ERROR_NOENT = 1U,         ///< 0x0001: No entry found
    ERROR_SYSERR = 2U,        ///< 0x0002: System error occurred
    ERROR_IO = 3U,            ///< 0x0003: I/O error occurred
    ERROR_NODEV = 4U,         ///< 0x0004: No device or object found

    ERROR_AGAIN = 6U,         ///< 0x0006: Try again

    ERROR_ACCESS = 8U,        ///< 0x0008: Permission denied
    ERROR_BUSY = 9U,          ///< 0x0009: Busy

    ERROR_INVAL = 12U,        ///< 0x000C: Invalid parameter or argument
    ERROR_NOSPC = 13U,        ///< 0x000D: No memory or resource available
    ERROR_RANGE = 14U,        ///< 0x000E: Parameter or argument out-of-range

    ERROR_ALREADY = 16U,      ///< 0x0010: Operation already in-progress or done
    ERROR_CANCELLED = 17U,    ///< 0x0011: Operation cancelled
    ERROR_TIMEDOUT = 18U,     ///< 0x0012: Operation timed-out
    ERROR_BADMSG = 19U,       ///< 0x0013: Malformed message or parameter
    ERROR_NOMSG = 20U,        ///< 0x0014: No message found
    ERROR_NOEXEC = 21U,       ///< 0x0015: Cannot execute
} CssErrorT;

/// Host command codes
typedef enum
{
    /// Get CSS status (any phase)
    HOST_COMMAND_CODE_GET_STATUS = 0x8000U,

    /// Enable a debug feature (Debug phase)
    HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT = 0x8001U,

    /// Load a patch chunk (Debug/Patch phase)
    HOST_COMMAND_CODE_LOAD_PATCH_CHUNK = 0x8003U,

    /// Retrieve the unique sensor identifier (Configure phase)
    HOST_COMMAND_CODE_GET_SENSOR_ID = 0x8012U,

    /// Retrieve certs for sensor authentication (Configure phase)
    HOST_COMMAND_CODE_GET_CERTIFICATES = 0x8020U,

    /// Negotiate session keys using an ephemeral master secret (Configure phase)
    HOST_COMMAND_CODE_SET_SESSION_KEYS = 0x8021U,

    /// Set the video authentication region-of-interest (Session phase)
    HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI = 0x8030U,
} CssHostCommandCodeT;

/// System phase codes
typedef enum
{
    CSS_SYSTEM_PHASE_BOOT = 0x0,
    CSS_SYSTEM_PHASE_INITIALISE = 0x1,
    CSS_SYSTEM_PHASE_DEBUG = 0x2,
    CSS_SYSTEM_PHASE_PATCH = 0x3,
    CSS_SYSTEM_PHASE_CONFIGURE = 0x4,
    CSS_SYSTEM_PHASE_SESSION = 0x5,
    CSS_SYSTEM_PHASE_SHUTDOWN = 0x6,
} CssSystemPhaseT;

/// System lifecycle state codes
typedef enum
{
    CSS_SYSTEM_LCS_FP = 0,          ///< Factory Provisioning lifecycle state
    CSS_SYSTEM_LCS_CP = 1,          ///< Customer Provisioning lifecycle state
    CSS_SYSTEM_LCS_SE = 2,          ///< Secure lifecycle state
    CSS_SYSTEM_LCS_RESERVED = 3,    ///< Reserved
} CssSystemLcsT;

/// Represents the GetStatus host command response parameters
typedef struct
{
    CssSystemPhaseT currentPhase;   ///< Current system phase
    CssSystemLcsT   currentLcs;     ///< Current lifecycle state
    uint16_t        romVersion;     ///< Version of the CCS firmware ROM
    uint16_t        patchVersion;   ///< Version of loaded patch (if loaded)
    uint16_t        reserved;       ///< Reserved
} CssGetStatusResponseT;

/// Certificate Authority identifiers
typedef enum
{
    CSS_CERTIFICATE_AUTH_ID_VENDOR_A = 0x00U,   ///< Vendor A certificate authority
    CSS_CERTIFICATE_AUTH_ID_VENDOR_B = 0x01U,   ///< Vendor B certificate authority
    CSS_CERTIFICATE_AUTH_ID_RESERVED1 = 0x02U,  ///< Reserved for future use
    CSS_CERTIFICATE_AUTH_ID_RESERVED2 = 0x03U,  ///< Reserved for future use
} CssCertificateAuthIdT;

/// Certificate purpose identifiers
typedef enum
{
    CSS_CERTIFICATE_PURPOSE_VENDOR = 0x00U,     ///< The Vendor certificate
    CSS_CERTIFICATE_PURPOSE_SENSOR = 0x01U,     ///< The Sensor certificate
    CSS_CERTIFICATE_PURPOSE_RESERVED1 = 0x02U,  ///< Reserved for future use
    CSS_CERTIFICATE_PURPOSE_RESERVED2 = 0x03U,  ///< Reserved for future use
} CssCertificatePurposeT;

/// Represents a certificate version
typedef uint8_t CssCertificateVersionT;

/// Represents a certificate number
typedef uint32_t CssCertificateNumberT;

#define CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES          256U    ///< 2048-bit key/modulus
#define CSS_RSA_2048_SIGNATURE_SIZE_BYTES           256U    ///< 2048-bit signature
#define CSS_RSA_2048_PRIVATE_EXPONENT_SIZE_BYTES    256U    ///< 2048-bit exponent

#define CSS_RSA_3072_PUBLIC_KEY_SIZE_BYTES          384U    ///< 3072-bit key/modulus
#define CSS_RSA_3072_SIGNATURE_SIZE_BYTES           384U    ///< 3072-bit signature
#define CSS_RSA_3072_PRIVATE_EXPONENT_SIZE_BYTES    384U    ///< 3072-bit exponent

#define CSS_RSA_4096_PUBLIC_KEY_SIZE_BYTES          512U    ///< 4096-bit key/modulus
#define CSS_RSA_4096_SIGNATURE_SIZE_BYTES           512U    ///< 4096-bit signature
#define CSS_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES    512U    ///< 4096-bit exponent

/// Digital certificate header
typedef struct
{
    uint16_t typeIdentifier;             ///< Known type identifier
    uint16_t signatureOffsetLongWords;   ///< Long-word offset to the signature section
    uint32_t certificateFlags;           ///< Identifiers and certificate number
} CssPubCertHeaderT;

/// Abstract RSA digital certificate
typedef struct
{
    CssPubCertHeaderT header;           ///< Certificate header
    uint8_t publicKey[1];               ///< The RSA public key
} CssRsaPubCertT;

/// 2048-bit RSA digital certificate
typedef struct
{
    CssPubCertHeaderT header;                               ///< Certificate header
    uint8_t publicKey[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[CSS_RSA_2048_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} CssRsa2048PubCertT;

/// 3072-bit RSA digital certificate
typedef struct
{
    CssPubCertHeaderT header;                               ///< Certificate header
    uint8_t publicKey[CSS_RSA_3072_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[CSS_RSA_3072_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} CssRsa3072PubCertT;

/// 4096-bit RSA digital certificate
typedef struct
{
    CssPubCertHeaderT header;                               ///< Certificate header
    uint8_t publicKey[CSS_RSA_4096_PUBLIC_KEY_SIZE_BYTES];  ///< The RSA public key
    uint8_t signature[CSS_RSA_4096_SIGNATURE_SIZE_BYTES];   ///< The RSA signature
} CssRsa4096PubCertT;

/// Size of the sensor's unique identifier returned by GetSensorId
#define CSS_SENSOR_ID_SIZE_BYTES        32U

/// Represents the sensor's unique identifier
typedef uint8_t CssSensorIdT[CSS_SENSOR_ID_SIZE_BYTES];

/// Size of the host/sensor salt in bytes
#define CSS_SALT_SIZE_BYTES             16U

/// Size of the GMAC IV in bytes
#define CSS_IV_SIZE_BYTES               12U

/// Size of the GCM/CCM nonce in bytes
#define CSS_NONCE_SIZE_BYTES            12U

/// Size of the GCM/CCM MAC in bytes
#define CSS_MAC_SIZE_BYTES              16U

/// Size of 128-bit AES key
#define CSS_AES_128_KEY_SIZE_BYTES      16U

/// Size of AES block
#define CSS_AES_BLOCK_SIZE_BYTES        16U

/// Size of the SHA-256 digest
#define CSS_SHA256_DIGEST_SIZE_BYTES    32U

/// Represents an asymmetric cipher
typedef enum
{
    CSS_ASYMMETRIC_CIPHER_RSAES_OAEP = 0U,  ///< RSAES-OAEP conformant to RFC8017
    CSS_ASYMMETRIC_CIPHER_RESERVED = 1U,    ///< Reserved for future use
} CssAsymmetricCipherT;

/// Represents a symmetric authenticated encryption cipher mode
typedef enum
{
    CSS_SYMMETRIC_AE_CIPHER_MODE_AES_CCM = 0U,   ///< AES-CCM conformant to NIST SP800-38C
    CSS_SYMMETRIC_AE_CIPHER_MODE_AES_GCM = 1U,   ///< AES-GCM conformant to NIST SP800-38D
    CSS_SYMMETRIC_AE_CIPHER_MODE_RESERVED = 2U,  ///< Reserved for future use
} CssSymmetricAECipherModeT;

/// Represents a symmetric authentication cipher mode
typedef enum
{
    CSS_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC = 0U,   ///< AES-CMAC conformant to NIST SP800-38B
    CSS_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC = 1U,   ///< AES-GMAC conformant to NIST SP800-38D
    CSS_SYMMETRIC_AUTH_CIPHER_MODE_RESERVED = 2U,   ///< Reserved for future use
} CssSymmetricAuthCipherModeT;

/// Represents a symmetric cipher key
typedef enum
{
    CSS_SYMMETRIC_KEY_AES_128 = 0U,         ///< 128-bit AES key
    CSS_SYMMETRIC_KEY_AES_192 = 1U,         ///< 192-bit AES key
    CSS_SYMMETRIC_KEY_AES_256 = 2U,         ///< 256-bit AES key
    CSS_SYMMETRIC_KEY_AES_RESERVED = 3U,    ///< Reserved for future use
} CssSymmetricKeyTypeT;

/// Represents session parameters encoding a 128-bit master secret
typedef struct
{
    /// Key type of the master secret (as CssSymmetricKeyTypeT)
    uint16_t masterSecretKeyType;

    /// Padding
    uint16_t pad;

    /// Master secret for session key derivation
    uint8_t  masterSecret[CSS_AES_128_KEY_SIZE_BYTES];
} CssSessionParametersT;

/// Represents a video authentication mode
typedef enum
{
    /// Every pixel and every row in the full frame will be authenticated.
    CSS_VIDEO_AUTH_MODE_FULL_FRAME = 0x00U,

    /// Every pixel and every row in the specified ROI will be authenticated.
    CSS_VIDEO_AUTH_MODE_ROI = 0x01U,
} CssVideoAuthModeT;

/// Represents a pixel packing mode
typedef enum
{
    /// Pixels will be tightly-packed with minimal padding
    CSS_PIXEL_PACKING_MODE_OPTIMIZED = 0x00U,

    /// Pixels will be loosely-packed
    CSS_PIXEL_PACKING_MODE_NON_OPTIMIZED = 0x01U,
} CssPixelPackingModeT;

/// Represents a region-of-interest
typedef struct
{
    uint16_t firstRow;      ///< First row of the ROI (relative to sensor window)
    uint16_t lastRow;       ///< Last row (inclusive) of the ROI
    uint16_t rowSkip;       ///< Number of rows to skip
    uint16_t firstColumn;   ///< First column of the ROI (relative to sensor window)
    uint16_t lastColumn;    ///< Last column (inclusive) of the ROI
    uint16_t columnSkip;    ///< Number of columns to skip
} CssRegionOfInterestT;

/// Represents the VideoAuthRoi configuration parameters
typedef struct
{
    uint16_t videoAuthMode;     ///< Video authentication mode (as CssVideoAuthModeT)
    uint16_t pixelPackMode;     ///< Pixel packing mode (as CssPixelPackingModeT)
    uint16_t pixelPackValue;    ///< Pixel packing value
    CssRegionOfInterestT roi;   ///< Region-of-interest
    uint16_t padding[3];        ///< Padding
    uint64_t frameCounter;      ///< Frame on which new ROI should take effect
} CssVideoAuthConfigParamsT;

// Sets the timeout (poll loop count) for host commands
extern void CssSetHostCommandTimeout(uint32_t const pollCount);

// Issues the specified host command and waits for a response
extern CssErrorT CssIssueHostCommand(CssHostCommandCodeT const commandCode);

// Issues the GetStatus host command and returns the response parameters
extern CssErrorT CssGetStatus(CssGetStatusResponseT* const response);

// Issues the GetSensorId host command and returns the sensor's unique identifier
extern CssErrorT CssGetSensorId(CssSensorIdT sensorId);

// Issues the ApplyDebugEntitlement host command
extern CssErrorT CssApplyDebugEntitlement(  uint32_t const clockSpeedHz,
                                            uint8_t* const entitlementBlob,
                                            uint32_t const entitlementBlobSizeWords);

// Issues the LoadPatchChunk host command
extern CssErrorT CssLoadPatchChunk(uint8_t* const patchChunk, uint32_t const patchChunkSizeWords);

// Issues the GetCertificates host command and returns the retrieved certificates
extern CssErrorT CssGetCertificates(CssCertificateAuthIdT const CertificateAuthId,
                                    uint8_t* const sensorCert,
                                    uint32_t* const sensorCertSizeWords,
                                    uint8_t* const vendorCert,
                                    uint32_t* const vendorCertSizeWords);

// Issues the SetSessionKeys host command and returns the response parameters
extern CssErrorT CssSetSessionKeys( CssSymmetricAuthCipherModeT const videoAuthCipherMode,
                                    CssSymmetricKeyTypeT const videoAuthKeyType,
                                    CssSymmetricAECipherModeT const controlChannelCipherMode,
                                    CssSymmetricKeyTypeT const controlChannelKeyType,
                                    uint8_t const hostSalt[CSS_SALT_SIZE_BYTES],
                                    CssAsymmetricCipherT const sessionParamsCipher,
                                    uint32_t const encryptedParamsSizeWords,
                                    uint8_t* const encryptedParams,
                                    uint8_t sensorSalt[CSS_SALT_SIZE_BYTES],
                                    uint8_t videoAuthGmacIv[CSS_IV_SIZE_BYTES],
                                    uint8_t controlChannelNonce[CSS_NONCE_SIZE_BYTES]);

// Issues the SetVideoAuthRoi host command
extern CssErrorT CssSetVideoAuthRoi(uint8_t* const encryptedParams, uint8_t* const mac);

#endif // !defined(CSS_HOST_COMMAND_H)
