#if !defined(SRC__SESSIONMGR__SRC__SESSIONMGRINTERNAL_H)
#define      SRC__SESSIONMGR__SRC__SESSIONMGRINTERNAL_H

//***********************************************************************************
/// \file
/// Session Manager private types and functions
///
/// \addtogroup sessionmgr
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

#include "AssetMgr.h"
#include "CryptoMgr.h"
#include "LifecycleMgr.h"
#include "MacDrv.h"
#include "SessionMgr.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#define SESSION_MGR_KDF_SALT_SIZE_BYTES             16U     ///< Salt is 128-bits/16 bytes
#define SESSION_MGR_IV_SIZE_BYTES                   12U     ///< GMAC IV is 96-bits/12 bytes
#define SESSION_MGR_NONCE_SIZE_BYTES                12U     ///< GCM IV/CCM Nonce is 96-bits/12 bytes
#define SESSION_MGR_VIDEO_AUTH_KEY_SIZE_BYTES       32U     ///< Video authentication AES key is worse-case 32 bytes
#define SESSION_MGR_CONTROL_CHANNEL_KEY_SIZE_BYTES  32U     ///< Control channel AES key is worse-case 32 bytes

#define SESSION_MGR_VIDEO_AUTH_MAC_SIZE_BYTES       16U     ///< CMAC/GMAC tag is 16 bytes (128-bits)

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents the GetCertificates host command request parameters
typedef struct
{
    uint16_t certificateAuthId;  ///< Identifies the issuing Certificate Authority (as AssetMgrCertificateAuthIdT)
} SessionMgrGetCertificatesRequestT;

/// Represents the GetCertificates host command response parameters
typedef struct
{
    uint16_t sensorCertificateSizeWords;    ///< Size of the sensor certificate
    uint16_t sensorCertificate[1];          ///< First word of the certificate
} SessionMgrGetCertificatesResponseT;

/// Represents the SetEphemeralMasterSecret host command parameters
typedef struct
{
    uint16_t masterSecretKeyType;   ///< Identifies the symmetric key type (CryptoMgrSymmetricKeyTypeT)
    uint16_t pad;                   ///< Padding - ignored

    ///< 128/192/256-bit master secret
    uint8_t masterSecret[ASSET_MGR_PSK_MASTER_SECRET_256_KEY_SIZE_BYTES];
} SessionMgrSetEphemeralMasterSecretRequestT;

/// Represents the common session parameters for SetSessionKeys and SetPskSessionKeys host commands
typedef struct
{
    ///< Identifies which cipher and mode to use for video authentication (as CryptoMgrSymmetricAuthCipherModeT)
    uint16_t videoAuthCipherMode;

    ///< Identifies which cipher and key size to use for video authentication (as CryptoMgrSymmetricKeyTypeT)
    uint16_t videoAuthKeyType;

    ///< Identifies which cipher and mode to use for the secure control channel (as CryptoMgrSymmetricAECipherModeT)
    uint16_t controlChannelCipherMode;

    ///< Identifies which key type to use for video authentication (as CryptoMgrSymmetricKeyTypeT)
    uint16_t controlChannelKeyType;

    ///< 128-bit random salt to the HKDF key derivation function
    uint8_t hostSalt[SESSION_MGR_KDF_SALT_SIZE_BYTES];
} SessionMgrSessionSetupParamsT;

/// Represents the encrypted session parameters header
typedef struct
{
    uint16_t masterSecretKeyType;   ///< Identifies the symmetric key type (CryptoMgrSymmetricKeyTypeT)
    uint16_t pad;                   ///< Padding
} SessionMgrSetSessionKeyEncryptedSessionParamsHeaderT;

/// Represents the encrypted session parameters
typedef struct
{
    /// Session parameters header
    SessionMgrSetSessionKeyEncryptedSessionParamsHeaderT header;

    ///< 128-/192-/256-bit master secret for session key derivation
    uint8_t masterSecret[ASSET_MGR_PSK_MASTER_SECRET_256_KEY_SIZE_BYTES];
} SessionMgrSetSessionKeyEncryptedSessionParamsT;

/// Represents the SetSessionKeys host command request parameters
typedef struct
{
    SessionMgrSessionSetupParamsT common;   ///< Common session setup parameters
    uint16_t sessionParamsCipher;           ///< Identifies which cipher was used to encrypt the session parameters (as CryptoMgrAsymmetricCipherT)
    uint16_t encryptedParamsSizeWords;      ///< Size of the encrypted parameters in (16-bit) words

    ///< Encrypted session parameters
    SessionMgrSetSessionKeyEncryptedSessionParamsT encryptedParams;
} SessionMgrSetSessionKeysRequestT;

/// Represents the SetPskSessionKeys host command request parameters
typedef struct
{
    SessionMgrSessionSetupParamsT common;   ///< Common session setup parameters
} SessionMgrSetPskSessionKeysRequestT;

/// Represents the SetPskSessionKeys and SetSessionKeys host command response parameters
typedef struct
{
    uint8_t sensorSalt[SESSION_MGR_KDF_SALT_SIZE_BYTES];           ///< 128-bit random salt to the HKDF key derivation function
    uint8_t videoAuthGmacIv[SESSION_MGR_IV_SIZE_BYTES];            ///< 96-bit random initialization vector (for GMAC)
    uint8_t controlChannelNonce[SESSION_MGR_NONCE_SIZE_BYTES];     ///< 96-bit random nonce for CCM/iv for GCM
} SessionMgrSetSessionKeysResponseT;

/// Represents the SetVideoAuthRoi video authentication configuration parameters
typedef struct
{
    uint16_t videoAuthMode;                 ///< Video authentication mode (as MacDrvVideoAuthModeT)
    uint16_t pixelPackMode;                 ///< Pixel packing mode (as MacDrvPixelPackModeT)
    uint16_t pixelPackValue;                ///< Pixel packing value
    uint16_t firstRow;                      ///< First row of the ROI (relative to sensor window)
    uint16_t lastRow;                       ///< Last row (inclusive) of the ROI (relative to sensor window)
    uint16_t rowSkip;                       ///< Number of rows to skip
    uint16_t firstColumn;                   ///< First column of the ROI (relative to sensor window)
    uint16_t lastColumn;                    ///< Last column (inclusive) of the ROI (relative to sensor window)
    uint16_t columnSkip;                    ///< Number of columns to skip
    uint16_t pad[3];                        ///< Padding for 64-bit alignment
    uint64_t frameCounter;                  ///< Frame counter to start authentication/apply new ROI
} SessionMgrVideoAuthConfigParamsT;

/// Represents the SetVideoAuthRoi command parameters
typedef struct
{
    SessionMgrVideoAuthConfigParamsT configParams;          ///< Configuration parameters
    uint8_t mac[SESSION_MGR_VIDEO_AUTH_MAC_SIZE_BYTES];     ///< Authentication tag
} SessionMgrSetVideoAuthRoiRequestT;

/// Represents the status of the GetCertificates command
typedef struct
{
    ///< Flags if certificates have been requested and issued
    bool certificatesIssued;

    ///< Identifies which certificate authority issued the retrieved certificate(s)
    AssetMgrCertificateAuthIdT certificateAuthId;

    ///< Pointer to the public key (retrieved from NVM)
    uint8_t* publicKey;

    ///< Size of the public key (retrieved from NVM)
    uint32_t publicKeySizeBytes;
} SessionMgrCertificateStateT;

/// Represents a (worse-case) ephemeral master secret
typedef struct
{
    uint32_t keySizeBytes;                     ///< Size of the key (0 if not present)
    AssetMgrPskMasterSecret256T masterSecret;  ///< Stores the (worse-case size) master secret
} SessionMgrEphemeralMasterSecretT;

/// Represents the current session keys
typedef struct
{
    uint8_t videoAuthKey[SESSION_MGR_VIDEO_AUTH_KEY_SIZE_BYTES];            ///< The negotiated video authentication key
    uint8_t controlChannelKey[SESSION_MGR_CONTROL_CHANNEL_KEY_SIZE_BYTES];  ///< The negotiated control channel key
} SessionMgrSessionKeysT;

/// Session host- and sensor-defined parameters
typedef struct
{
    CryptoMgrSymmetricAuthCipherModeT videoAuthCipherMode;     ///< video authentication cipher
    CryptoMgrSymmetricKeyTypeT videoAuthKeyType;               ///< video authentication key type
    CryptoMgrSymmetricAECipherModeT controlChannelCipherMode;  ///< control channel cipher
    CryptoMgrSymmetricKeyTypeT controlChannelKeyType;          ///< control channel key type
    uint8_t hostSalt[SESSION_MGR_KDF_SALT_SIZE_BYTES];         ///< host-specific salt
    uint8_t sensorSalt[SESSION_MGR_KDF_SALT_SIZE_BYTES];       ///< 128-bit random salt to the HKDF key derivation function
    uint8_t videoAuthGmacIv[SESSION_MGR_IV_SIZE_BYTES];        ///< 96-bit random initialization vector (for GMAC)

    ///< 96-bit random nonce for CCM/iv for GCM
    uint8_t controlChannelNonce[SESSION_MGR_NONCE_SIZE_BYTES];
} SessionMgrSessionParamsT;

/// Represents the session state
typedef struct
{
    bool sessionInitiated;                      ///< Flags if an RSA or PSK session has been initiated
    SessionMgrSessionParamsT sessionParams;     ///< Setup parameters supplied by the host
    SessionMgrSessionKeysT sessionKeys;         ///< The derived session keys
} SessionMgrSessionStateT;

/// Represents the video authentication state
typedef struct
{
    bool isActive;                                          ///< Flags if video authentication is active
    SessionMgrVideoAuthConfigParamsT currentConfig;         ///< The current configuration (if active)
} SessionMgrVideoAuthStateT;

/// Represents Session Manager's internal state
typedef struct
{
    SessionMgrCertificateStateT certificateState;            ///< State of the GetCertificates command
    SessionMgrEphemeralMasterSecretT ephemeralMasterSecret;  ///< The ephemeral master secret (if set)
    SessionMgrSessionStateT sessionState;                    ///< The current state of the authentication session
    SessionMgrVideoAuthStateT videoAuthState;                ///< The current state of video authentication
} SessionMgrStateT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern SessionMgrStateT sessionMgrState;

// Dumps Session Mgr state in response to a fatal error
extern void SessionMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer,
    uint32_t const sizeLongWords);

// Checks if the specified cipher mode is valid
extern ErrorT SessionMgrValidateSymmetricAuthCipherMode(
    CryptoMgrSymmetricAuthCipherModeT const symmetricAuthCipherMode);

// Checks if the specified key type is valid
extern ErrorT SessionMgrValidateSymmetricKeyType(
    CryptoMgrSymmetricKeyTypeT const symmetricKeyType);

// Checks if the specified AE cipher mode is valid
extern ErrorT SessionMgrValidateSymmetricAECipherMode(
    CryptoMgrSymmetricAECipherModeT const symmetricAECipherMode);

// Validates the host-supplied session setup parameters
extern ErrorT SessionMgrValidateSessionParams(
    SessionMgrSessionParamsT* const params);

/// Retrieves the Sensor certificate from NVM
extern ErrorT SessionMgrRetrieveSensorCertificate(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint32_t const worseCaseCertSizeLongWords,
    uint32_t* const sensorCertSizeWords,
    uint16_t* const responseBuffer);

/// Retrieves the Vendor certificate from NVM
extern ErrorT SessionMgrRetrieveVendorCertificate(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint32_t const worseCaseCertSizeLongWords,
    uint32_t const sensorCertSizeWords,
    uint16_t* const responseBuffer);

// Retrieves the Sensor and (optional) Vendor certificate from Asset Manager
extern ErrorT SessionMgrRetrieveCertificates(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint16_t* const responseBuffer);

// Initialises the (common) session setup parameters
extern void SessionMgrInitSessionParams(SessionMgrSessionSetupParamsT* const params);

// Sets up a video authentication session
extern ErrorT SessionMgrSetupSession(uint8_t* const masterSecret,
    uint32_t const masterSecretSizeBytes,
    SessionMgrSetSessionKeysResponseT* const response);

// Sets up a session using a pre-shared key
extern ErrorT SessionMgrSetupPskSession(SessionMgrSetSessionKeysResponseT* const response);

// Initiates a PSK session
extern ErrorT SessionMgrInitiatePskSession(SessionMgrSetPskSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response);

// Decrypts an (RSA-encrypted) master secret
extern ErrorT SessionMgrDecryptMasterSecret(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeyEncryptedSessionParamsT* const decryptedSessionParams,
    uint32_t* const decryptedSessionParamsSizeBytes);

// Validates the RSA session request parameters.
extern ErrorT SessionMgrValidateRsaSessionParams(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeyEncryptedSessionParamsT* const decryptedParams,
    uint32_t const decryptedParamsSizeBytes,
    uint32_t* const sessionMasterSecretSizeBytes);

// Sets up a session using a key transported with RSA encryption
extern ErrorT SessionMgrSetupRsaSession(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response);

// Initiates a session using a key transported with RSA encryption
extern ErrorT SessionMgrInitiateRsaSession(LifecycleMgrLifecycleStateT const currentLcs,
    SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response);

// Configures the MAC driver
extern ErrorT SessionMgrConfigMacDriver(SessionMgrVideoAuthConfigParamsT* const config,
    SessionMgrSessionStateT* const sessionState);

// Configures the video authentication ROI
extern ErrorT SessionMgrConfigRoi(SessionMgrVideoAuthConfigParamsT* const config);

// Configures the video authentication subsystem (VCore)
extern ErrorT SessionMgrConfigVideoAuth(SessionMgrVideoAuthConfigParamsT* const config);

/// @} endgroup sessionmgr
#endif  // !defined(SRC__SESSIONMGR__SRC__SESSIONMGRINTERNAL_H)

