#if !defined(SRC__CRYPTOMGR__INC__CRYPTOMGR_H)
#define      SRC__CRYPTOMGR__INC__CRYPTOMGR_H

//***********************************************************************************
/// \file
/// Crypto Manager public API
///
/// \defgroup cryptomgr   Crypto Manager
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

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Indicates a root-of-trust
typedef enum
{
    CRYPTO_MGR_ROOT_OF_TRUST_CM = 0U,   ///< Chip Manufacturer root-of-trust
    CRYPTO_MGR_ROOT_OF_TRUST_DM = 1U,   ///< Device Manufacturer root-of-trust
} CryptoMgrRootOfTrustT;

/// Represents an asymmetric cipher
typedef enum
{
    CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP = 0U,  ///< RSAES-OAEP conformant to PKCS #1 v2.2 (RFC8017)
    CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED = 1U,    ///< Reserved for future use
} CryptoMgrAsymmetricCipherT;

/// Represents a symmetric authenticated encryption cipher mode
typedef enum
{
    CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM = 0U,   ///< AES-CCM authenticated encryption conformant to NIST SP800-38C
    CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM = 1U,   ///< AES-GCM authenticated encryption conformant to NIST SP800-38D
    CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED = 2U,  ///< Reserved for future use
} CryptoMgrSymmetricAECipherModeT;

/// Represents a symmetric authentication cipher mode
typedef enum
{
    CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC = 0U,  ///< AES-CMAC authentication conformant to NIST SP800-38B
    CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC = 1U,  ///< AES-GMAC authentication conformant to NIST SP800-38D
    CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_RESERVED = 2U,  ///< Reserved for future use
} CryptoMgrSymmetricAuthCipherModeT;

/// Represents a symmetric cipher key
typedef enum
{
    CRYPTO_MGR_SYMMETRIC_KEY_AES_128 = 0U,       ///< 128-bit AES key
    CRYPTO_MGR_SYMMETRIC_KEY_AES_192 = 1U,       ///< 192-bit AES key
    CRYPTO_MGR_SYMMETRIC_KEY_AES_256 = 2U,       ///< 256-bit AES key
    CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED = 3U,  ///< Reserved for future use
} CryptoMgrSymmetricKeyTypeT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Handles the GetSensorID host command
extern ErrorT CryptoMgrOnGetSensorId(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);
// Handles entry to an operating phase
extern ErrorT CryptoMgrOnPhaseEntry(SystemMgrPhaseT const phase);
// Handles the SelfTest host command
extern ErrorT CryptoMgrOnSelfTest(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);
// Authenticates (and decrypts) a patch chunk
extern ErrorT CryptoMgrAuthenticatePatchChunk(uint32_t const assetId,
    uint32_t* const assetPackageBuffer,
    uint32_t* const decryptedPackageSizeBytes,
    uint8_t* const decryptedPackage);
// Authenticates (and decrypts) an asset
extern ErrorT CryptoMgrAuthenticateAsset(uint32_t const assetId,
    CryptoMgrRootOfTrustT const rootOfTrust,
    uint32_t* const assetPakageBuffer);
// Authenticates and decrypts an RSA encrypted message
extern ErrorT CryptoMgrRsaDecrypt(uint32_t const keySizeBytes,
    uint8_t* const privateExponent,
    uint8_t* const modulus,
    uint8_t* const encryptedMsg,
    uint32_t* const decryptedMsgSizeBytes,
    uint8_t* const decryptedMsg);
// Authenticates and decryptes an AES-CCM or AES-GCM encrypted message
extern ErrorT CryptoMgrAeadDecrypt(CryptoMgrSymmetricAECipherModeT const cipherMode,
    uint32_t const keySizeBytes,
    uint8_t* const key,
    uint8_t* const ivNonce,
    uint32_t const associatedDataSizeBytes,
    uint8_t* const associatedData,
    uint32_t const encryptedMsgSizeBytes,
    uint8_t* const encryptedMsg,
    uint8_t* const decryptedMsg);
// Derives authentication session and control channel keys from the supplied Master Secret
extern ErrorT CryptoMgrDeriveKeys(uint32_t const masterSecretSizeBytes,
    uint8_t* const masterSecret,
    uint32_t const hostSaltSizeBytes,
    uint8_t* const hostSalt,
    uint32_t const sensorSaltSizeBytes,
    uint8_t* const sensorSalt,
    uint32_t const authSessionKeySizeBytes,
    uint8_t* const authSessionKey,
    uint32_t const controlChannelKeySizeBytes,
    uint8_t* const controlChannelKey);
// Generates a random number using the deterministic random bit generator
extern ErrorT CryptoMgrGenRandomNumber(uint32_t const randomNumberSizeBytes,
    uint8_t* const randomNumber);
// Called by CC312 runtime PAL CC_PalAbort()
extern void CryptoMgrOnPalAbort(void);
// Returns the size in bytes of a symmetric key
extern ErrorT CryptoMgrGetSymmetricKeySizeBytes(CryptoMgrSymmetricKeyTypeT const type,
    uint32_t* const sizeBytes);
// Indicates if the GetSensorId host command has been called successfully
extern ErrorT CryptoMgrGetSensorIdCalled(bool* const isCalled);

/// @} endgroup cryptomgr
#endif  // !defined(SRC__CRYPTOMGR__INC__CRYPTOMGR_H)

