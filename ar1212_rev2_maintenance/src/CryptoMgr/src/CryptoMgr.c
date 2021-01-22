//***********************************************************************************
/// \file
///
/// Crypto Manager implementation
///
/// \addtogroup cryptomgr
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
#include "CommandHandler.h"
#include "AR0820.h"
#include "Diag.h"
#include "CryptoMgr.h"
#include "CryptoMgrInternal.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

// Fix a Klocwork issue : MISRA C 2012 Rule 12.1: The precedence of operators within expressions should be made explicit
#define CRYPTO_MGR_ASSET_ADATA_SIZE ((3 * CC_32BIT_WORD_SIZE) + ASSET_RESERVED_SIZE)
#if (CRYPTO_MGR_ASSET_ADATA_SIZE != ASSET_ADATA_SIZE)
#error "Review MBEDTLS release as this values should match"
#endif

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

/// Handles the GetSensorID host command
///
/// \param[in]  commandCode Command to handle (GET_SENSOR_ID)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns
/// \retval ERROR_SUCCESS   Command completed succesfully
/// \retval ERROR_IO        Command failed to compute the sensor ID
/// \retval ERROR_SYSERR    Command failed due to a system issue
///
ErrorT CryptoMgrOnGetSensorId(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params)
{
    CryptoMgrGetSensorIdResponseParamsT* responseParams = (CryptoMgrGetSensorIdResponseParamsT*)params;
    ErrorT status = ERROR_SUCCESS;

    if ((HOST_COMMAND_CODE_GET_SENSOR_ID != commandCode) ||
        (NULL == params))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_GET_SENSOR_ID_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }
    if (ERROR_SUCCESS == status)
    {
        status = CryptoMgrGetSensorId(responseParams->uniqueId);
    }
    if (ERROR_SUCCESS == status)
    {
        cryptoMgr.getSensorIdCalled = true;
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID, (uint16_t)status);

    return status;
}

/// Handles entry to an operating phase
///
/// \param[in] phase    Phase being entered
///
/// \returns
/// \retval ERROR_SUCCESS   Phase entry completed succesfully
/// \retval ERROR_SYSERR    Phase entry system error
/// \retval ERROR_NOSPC     No space in memory for the initialization
///
ErrorT CryptoMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT status = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            // Note that there is no DiagRegisterFatalErrorDumpHandler call because this component
            // doesn't dump anything on Fatal Error to avoid leaking any crypto information.
            (void)memset(&cryptoMgr, 0, sizeof(cryptoMgr));
            break;

        case SYSTEM_MGR_PHASE_CONFIGURE:
            status = CryptoMgrOnConfigEntry();
            if (ERROR_SUCCESS != status)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_ON_PHASE_ENTRY_CONFIGURATION,
                    (uint32_t)status);
            }
            break;

        case SYSTEM_MGR_PHASE_SHUTDOWN:
            status = CryptoMgrOnShutdownEntry();
            break;

        default:
            // nothing to do
            break;
    }

    return status;
}

/// Handles the SelfTest host command
///
/// \param[in]  commandCode Command to handle (SELF_TEST)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \retval ERROR_ACCESS    Command failed because it is not implemented
/// \retval ERROR_SYSERR    Command failed due to a system issue
///
ErrorT CryptoMgrOnSelfTest(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params)
{
    ErrorT status = ERROR_ACCESS;

    if ((HOST_COMMAND_CODE_SELF_TEST != commandCode) ||
        (NULL == params))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_SELF_TEST_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }

    // INFO: <SC>: Not implemented : The HW team did not implement the self-test feature
    // The decision is to support some self-test algorithm via patch, if requested.

    return status;
}

/// Called by CC312 runtime PAL CC_PalAbort()
///
/// Calls the System Manager to report a fatal error and not return
///
/// \returns Never
void CryptoMgrOnPalAbort(void)
{
    DiagSetCheckpoint(DIAG_CHECKPOINT_CRYPTO_MGR_ON_PAL_ABORT);

    // This should never return : More info AR0820FW-119 : Fatal Error from CC library
    SystemMgrReportCryptoFault(SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR, 0U, 0U);
}

/// Authenticates and decrypts a patch chunk
///
/// Note assumes a patch chunk cannot be decrypted in-place, the assetPackageBuffer and decryptedPackage should
/// not overlap.
///
/// \param[in]  assetId                        Unique identifier for the patch chunk
/// \param[in]  assetPackageBuffer             Pointer to the Asset Package
/// \param[in,out] decryptedPackageSizeBytes   As input the maximum size of the output buffer. As output the len of the decrypted package
/// \param[out] decryptedPackage               Decrypted package
///
/// \returns
/// \retval ERROR_SUCCESS   Asset is authentic and has been decrypted
/// \retval ERROR_BADMSG    Asset is not authentic
/// \retval ERROR_SYSERR    Failure due to a system issue
///
ErrorT CryptoMgrAuthenticatePatchChunk(uint32_t const assetId,
    uint32_t* const assetPackageBuffer,
    uint32_t* const decryptedPackageSizeBytes,
    uint8_t* const decryptedPackage)
{
    CCError_t error;
    ErrorT status = ERROR_SUCCESS;
    CCBsvAssetProv_t* const assetPointer = (CCBsvAssetProv_t* const)assetPackageBuffer;

    if ((NULL == assetPackageBuffer) ||
        (NULL == decryptedPackageSizeBytes) ||
        (NULL == decryptedPackage))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }
    if (ERROR_SUCCESS == status)
    {
        // This is similar behaviour as mbedtls_util_asset_pkg_unpack, but this doesn't require mbedtls to be initialize.
        // This is handy because this function is executed before patching so it minimises the amount of dependencies.
        error = CC_BsvIcvAssetProvisioningOpen(
            (uint32_t)CRYPTOCELL_CORE_BASE,
                assetId,
                assetPackageBuffer,
                assetPointer->assetSize + (uint32_t)(CRYPTO_MGR_ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE),
                decryptedPackage,
                decryptedPackageSizeBytes);

        if ((CCError_t)CC_OK != error)
        {
            CryptoMgrReportBsvError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH_ERROR, error);
            status = ERROR_BADMSG;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH, (uint16_t)status);

    return status;
}

/// Authenticates and decrypts an asset
///
/// Note assumes assets can be decrypted in-place...
///
/// \param[in]  assetId              Unique identifier for the asset
/// \param[in]  rootOfTrust          Identifies which provisioning key to use
/// \param[in]  assetPackageBuffer   Pointer to the Asset Package
///
/// \returns
/// \retval ERROR_SUCCESS   Asset is authentic and has been decrypted
/// \retval ERROR_BADMSG    Asset is not authentic
/// \retval ERROR_RANGE     rootOfTrust parameter out of range
/// \retval ERROR_SYSERR    Failure due to a system issue
///
ErrorT CryptoMgrAuthenticateAsset(uint32_t const assetId,
    CryptoMgrRootOfTrustT const rootOfTrust,
    uint32_t* const assetPackageBuffer)
{
    ErrorT status = CryptoMgrCheckStatus();
    CCAssetProvKeyType_t key = ASSET_PROV_KEY_TYPE_KPICV;
    CCBsvAssetProv_t* const assetPointer = (CCBsvAssetProv_t* const)assetPackageBuffer;

    if (NULL == assetPackageBuffer)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_ASSET_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }

    if (ERROR_SUCCESS == status)
    {
        if (CRYPTO_MGR_ROOT_OF_TRUST_CM == rootOfTrust)
        {
            key = ASSET_PROV_KEY_TYPE_KPICV;
        }
        else if (CRYPTO_MGR_ROOT_OF_TRUST_DM == rootOfTrust)
        {
            key = ASSET_PROV_KEY_TYPE_KCP;
        }
        else
        {
            status = ERROR_BADMSG;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        CCError_t error;
        size_t bufferSizeBytes = assetPointer->assetSize;
        uint32_t* decryptedAssetContents = (uint32_t*)assetPointer->encAsset;

        error = mbedtls_util_asset_pkg_unpack(
                key,
                assetId,
                assetPackageBuffer,
            (assetPointer->assetSize) + (uint32_t)(CRYPTO_MGR_ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE),
                decryptedAssetContents,
                &bufferSizeBytes);

        if ((CCError_t)CC_OK != error)
        {
            CryptoMgrReportBsvError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET_ERROR, error);
            status = ERROR_BADMSG;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET, (uint16_t)status);

    return status;
}

/// Authenticates and decrypts an RSA encrypted message
///
/// The encrypted message can be generated with openssl 1.0.2f-fips:
/// openssl pkeyutl -in <plaintext.bin> -out <ciphertext.bin> -inkey <pubkey.pem> -pubin -encrypt
///        -pkeyopt rsa_padding_mode:oaep -pkeyopt rsa_oaep_md:sha256
///
/// Note: Byte 0 in <ciphertext.bin> would be in encryptedMsg[0]
///
/// \param[in] keySizeBytes                 Size of the RSA key in bytes
/// \param[in] privateExponent              RSA private key exponent to decrypt the message (big endian format)
/// \param[in] modulus                      RSA key modulus (big endian format)
/// \param[in] encryptedMsg                 Message to decrypt
/// \param[in,out] decryptedMsgSizeBytes    As input the maximum size of the output buffer. As output the len of the decrypted message
/// \param[out] decryptedMsg                Decrypted message
///
/// \returns Error status
/// \retval ERROR_SUCCESS                   Message authenticated and decrypted
/// \retval ERROR_BADMSG                    Message is not authentic
/// \retval ERROR_SYSERR                    Failure due to a system issue
///
ErrorT CryptoMgrRsaDecrypt(uint32_t const keySizeBytes,
    uint8_t* const privateExponent,
    uint8_t* const modulus,
    uint8_t* const encryptedMsg,
    uint32_t* const decryptedMsgSizeBytes,
    uint8_t* const decryptedMsg)
{
    ErrorT status = CryptoMgrCheckStatus();
    int32_t mbedtls_error;
    uint8_t const publicExponent[] = {0x01U, 0x00U, 0x01U};  // This represents the typical 65537 (0x0010001) exponent
    size_t const publicExponentSizeBytes = sizeof(publicExponent);

    if ((NULL == privateExponent) ||
        (NULL == modulus) ||
        (NULL == encryptedMsg) ||
        (NULL == decryptedMsgSizeBytes) ||
        (NULL == decryptedMsg))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }

    if (ERROR_SUCCESS == status)
    {
        // This is kept in the stack. The context size it's about 200 bytes
        mbedtls_rsa_context rsaCtx;

        mbedtls_rsa_init(&rsaCtx, (int32_t)MBEDTLS_RSA_PKCS_V21, (int32_t)MBEDTLS_MD_SHA256);

        // The FW only runs CryptoMgrRsaDecrypt once, so it is ok to have one call to
        // mbedtls_rsa_import_raw and mbedtls_rsa_complete in this function instead of
        // during configuration.
        mbedtls_error = mbedtls_rsa_import_raw(&rsaCtx,
                modulus, keySizeBytes,                      // N
                NULL, 0U,                                   // P
                NULL, 0U,                                   // Q
                privateExponent, keySizeBytes,              // D
                publicExponent, publicExponentSizeBytes     // E
            );

        if (0 == mbedtls_error)
        {
            mbedtls_error = mbedtls_rsa_complete(&rsaCtx);
        }
        if (0 == mbedtls_error)
        {
            mbedtls_error = mbedtls_rsa_pkcs1_decrypt(&rsaCtx,
                    mbedtls_ctr_drbg_random,
                    &cryptoMgr.rndState,
                    MBEDTLS_RSA_PRIVATE,
                    decryptedMsgSizeBytes,
                    encryptedMsg,
                    decryptedMsg,
                    *decryptedMsgSizeBytes);
        }

        // Free heap space
        mbedtls_rsa_free(&rsaCtx);

        if (0 != mbedtls_error)
        {
            CryptoMgrReportMbedTlsError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT_ERROR, mbedtls_error);
            status = ERROR_BADMSG;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT, (uint16_t)status);

    return status;
}

/// Authenticates and decrypts an AES-CCM or AES-GCM encrypted message
///
/// Note it autoincrements the Nonce by 1 after a succesful decryption, it only does it on success because the
/// host should retry the same data if it has failed (due to a bit flip for example). It might be dangerous that
/// the host sends the same message again with another nonce.
///
/// \param[in] cipherMode                   AES authenticated encryption mode
/// \param[in] keySizeBytes                 Size of the AES key in bytes
/// \param[in] key                          Pointer to AES key to decrypt the message
/// \param[inout] ivNonce                   Pointer to GCM IV or CCM nonce (12 bytes). Autoincremented by 1 (little endian)
/// \param[in] associatedDataSizeBytes      Length of the associated data in bytes
/// \param[in] associatedData               Pointer of the associated data
/// \param[in] encryptedMsgSizeBytes        Length of the encrypted message in bytes (not including the tag)
/// \param[in] encryptedMsgAndTag           Pointer to encrypted message followed by the tag (16 bytes)
/// \param[out] decryptedMsg                Pointer to buffer to contain the decrypted message (must be length of encryptedMsgSizeBytes)
///
/// \returns Error status
/// \retval ERROR_SUCCESS                   Message authenticated and decrypted
/// \retval ERROR_BADMSG                    Message is not authentic
/// \retval ERROR_RANGE                     cipherMode is not supported
/// \retval ERROR_SYSERR                    Failure due to a system issue
///
ErrorT CryptoMgrAeadDecrypt(CryptoMgrSymmetricAECipherModeT const cipherMode,
    uint32_t const keySizeBytes,
    uint8_t* const key,
    uint8_t* const ivNonce,
    uint32_t const associatedDataSizeBytes,
    uint8_t* const associatedData,
    uint32_t const encryptedMsgSizeBytes,
    uint8_t* const encryptedMsgAndTag,
    uint8_t* const decryptedMsg)
{
    ErrorT status = CryptoMgrCheckStatus();
    mbedtls_cipher_mode_t mode = MBEDTLS_MODE_NONE;

    if ((NULL == key) ||
        (NULL == ivNonce) ||
        (NULL == associatedData) ||
        (NULL == encryptedMsgAndTag) ||
        (NULL == decryptedMsg))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }

    if (ERROR_SUCCESS == status)
    {
        switch (cipherMode)
        {
            case CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM:
                mode = MBEDTLS_MODE_CCM;
                break;

            case CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM:
                mode = MBEDTLS_MODE_GCM;
                break;

            default:
                status = ERROR_RANGE;
                break;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        int32_t keySizeBits = (int32_t)keySizeBytes * 8;
        int32_t mbedtls_error = 0;
        mbedtls_cipher_context_t ctx;
        const mbedtls_cipher_info_t* cipherInfo = NULL;
        uint32_t decryptedMsgSizeBytes = encryptedMsgSizeBytes;

        mbedtls_cipher_init(&ctx);

        cipherInfo = mbedtls_cipher_info_from_values(MBEDTLS_CIPHER_ID_AES, keySizeBits, mode);
        if (NULL == cipherInfo)
        {
            mbedtls_error = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
        }
        if (0 == mbedtls_error)
        {
            mbedtls_error = mbedtls_cipher_setup(&ctx, cipherInfo);
        }
        if (0 == mbedtls_error)
        {
            mbedtls_error = mbedtls_cipher_setkey(&ctx, key, keySizeBits, MBEDTLS_DECRYPT);
        }
        if (0 == mbedtls_error)
        {
            uint8_t* tagBuffer = &encryptedMsgAndTag[encryptedMsgSizeBytes];
            mbedtls_error = mbedtls_cipher_auth_decrypt(&ctx,
                    ivNonce, CRYPTO_MGR_AEAD_IV_NONCE_SIZE_BYTES,
                    associatedData, associatedDataSizeBytes,
                    encryptedMsgAndTag, encryptedMsgSizeBytes,
                    decryptedMsg, &decryptedMsgSizeBytes,
                    tagBuffer, CRYPTO_MGR_AEAD_TAG_SIZE_BYTES);
        }
        // Free heap space
        mbedtls_cipher_free(&ctx);

        if (0 == mbedtls_error)
        {
            status = CryptoMgrIncrementNumber(CRYPTO_MGR_AEAD_IV_NONCE_SIZE_BYTES, ivNonce);
        }
        else
        {
            CryptoMgrReportMbedTlsError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT_ERROR, mbedtls_error);
            status = ERROR_BADMSG;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT, (uint16_t)status);

    return status;
}

/// Derives authentication session and control channel keys from the supplied Master Secret
///
/// Uses HKDF (RFC5869)
///
/// Assumes Crypto Manager can get uniqueId, ROM version and Patch version itself
///
/// \param[in] masterSecretSizeBytes        Master Secret size in bytes
/// \param[in] masterSecret                 The Master Secret
/// \param[in] hostSaltSizeBytes            Host salt size in bytes
/// \param[in] hostSalt                     The host salt
/// \param[in] sensorSaltSizeBytes          Sensor salt size in bytes
/// \param[in] sensorSalt                   The sensor salt
/// \param[in] authSessionKeySizeBytes      Required size of the authentication session key in bytes
/// \param[out] authSessionKey              Returned authentication session key
/// \param[in] controlChannelKeySizeBytes   Required size of the control channel key in bytes
/// \param[out] controlChannelKey           Returned control channel key
///
/// \returns Error status
/// \retval ERROR_SUCCESS                   Keys generated successfully
/// \retval ERROR_SYSERR                    Error encounter in the arguments
/// \retval ERROR_INVAL                     Error encounter during operation
///
ErrorT CryptoMgrDeriveKeys(uint32_t const masterSecretSizeBytes,
    uint8_t* const masterSecret,
    uint32_t const hostSaltSizeBytes,
    uint8_t* const hostSalt,
    uint32_t const sensorSaltSizeBytes,
    uint8_t* const sensorSalt,
    uint32_t const authSessionKeySizeBytes,
    uint8_t* const authSessionKey,
    uint32_t const controlChannelKeySizeBytes,
    uint8_t* const controlChannelKey)
{
    ErrorT status = CryptoMgrCheckStatus();
    CCError_t error;
    CryptoMgrHkdfSaltT salt;
    CryptoMgrHkdfInfoT info =
    {
        // info = "AR0820R2" || ROM Version || Patch Version || 256-bit ID
        .sensorName = CRYPTO_MGR_HKDF_INFO_AR0820R2_VALUE
    };
    uint8_t sessionkeys[CRYPTO_MGR_HKDF_DERIVED_KEY_MAX_SIZE_BYTES * 2U] = {0U};

    if ((hostSaltSizeBytes > CRYPTO_MGR_HKDF_HOST_SALT_SIZE_BYTES) ||
        (sensorSaltSizeBytes > CRYPTO_MGR_HKDF_SENSOR_SALT_SIZE_BYTES) ||
        (authSessionKeySizeBytes > CRYPTO_MGR_HKDF_DERIVED_KEY_MAX_SIZE_BYTES) ||
        (controlChannelKeySizeBytes > CRYPTO_MGR_HKDF_DERIVED_KEY_MAX_SIZE_BYTES) ||
        (masterSecretSizeBytes > CRYPTO_MGR_HKDF_DERIVED_KEY_MAX_SIZE_BYTES) ||
        (NULL == hostSalt) ||
        (NULL == sensorSalt) ||
        (NULL == authSessionKey) ||
        (NULL == controlChannelKey) ||
        (NULL == masterSecret))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }

    if (ERROR_SUCCESS == status)
    {
        (void)memset(&salt, 0, sizeof(CryptoMgrHkdfSaltT));
        (void)memcpy(salt.hostSalt, hostSalt, hostSaltSizeBytes);
        (void)memcpy(salt.sensorSalt, sensorSalt, sensorSaltSizeBytes);
        info.romVersion = SystemMgrGetRomVersion();
        info.patchVersion = PatchMgrGetPatchVersion();
        status = CryptoMgrGetSensorId(info.sensorId);
    }
    if (ERROR_SUCCESS == status)
    {
        error = mbedtls_hkdf_key_derivation(CC_HKDF_HASH_SHA256_mode,
            (uint8_t*)&salt,
            (size_t)sizeof(salt),
                masterSecret,
                masterSecretSizeBytes,
            (uint8_t*)&info,
            (size_t)sizeof(info),
                sessionkeys,
                authSessionKeySizeBytes + controlChannelKeySizeBytes,
                CC_FALSE);  // Always false because the hkdf needs to perform the extraction phase

        if ((CCError_t)CC_OK != error)
        {
            CryptoMgrReportBsvError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION_ERROR, error);
            status = ERROR_INVAL;
        }
        else
        {
            (void)memcpy(authSessionKey, sessionkeys, authSessionKeySizeBytes);
            (void)memcpy(controlChannelKey, &sessionkeys[authSessionKeySizeBytes], controlChannelKeySizeBytes);
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION, (uint16_t)status);

    return status;
}

/// Generates a random number using the deterministic random bit generator
///
/// Uses TRNG-seeded DRBG
///
/// \param[in] randomNumberSizeBytes        Size of the random number in bytes
/// \param[out] randomNumber                Returned random number
///
/// \returns Error status
/// \retval ERROR_SUCCESS                   Random number generated successfully
/// \retval ERROR_SYSERR                    Error encounter during operation
///
ErrorT CryptoMgrGenRandomNumber(uint32_t const randomNumberSizeBytes,
    uint8_t* const randomNumber)
{
    ErrorT status = CryptoMgrCheckStatus();
    int32_t mbedtls_error;

    if (NULL == randomNumber)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RANDOM_GENERATOR_BAD_ARGS, 0U);
        status = ERROR_SYSERR;
    }
    if (ERROR_SUCCESS == status)
    {
        mbedtls_error = mbedtls_ctr_drbg_random(&cryptoMgr.rndState, randomNumber, randomNumberSizeBytes);
        if (0 != mbedtls_error)
        {
            CryptoMgrReportMbedTlsError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR_ERROR, mbedtls_error);
            status = ERROR_SYSERR;
        }
    }
    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR, (uint16_t)status);

    return status;
}

/// Returns the size in bytes of a symmetric key
///
/// \param[in] type         Type of the symmetric key
/// \param[out] sizeBytes   Returned size of the key
/// \returns Error status
/// \retval ERROR_SUCCESS   Size returned in sizeBytes
/// \retval ERROR_INVAL     Invalid key type
/// \retval ERROR_SYSERR    Unexpected invalid pointer
///
ErrorT CryptoMgrGetSymmetricKeySizeBytes(CryptoMgrSymmetricKeyTypeT const type,
    uint32_t* const sizeBytes)
{
    ErrorT status = ERROR_SUCCESS;

    if (NULL != sizeBytes)
    {
        switch (type)
        {
            case CRYPTO_MGR_SYMMETRIC_KEY_AES_128:
                *sizeBytes = 16U;
                break;

            case CRYPTO_MGR_SYMMETRIC_KEY_AES_192:
                *sizeBytes = 24U;
                break;

            case CRYPTO_MGR_SYMMETRIC_KEY_AES_256:
                *sizeBytes = 32U;
                break;

            default:
                status = ERROR_INVAL;
                break;
        }
    }
    else
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_KEY_SIZE_PARAM, 0U);
        status = ERROR_SYSERR;
    }

    return status;
}

/// Indicates if the GetSensorId host command has been called successfully
///
/// \param[out] called  Pointer to returned flag
/// \returns Error status
/// \retval ERROR_SUCCESS   called flag set
/// \retval ERROR_SYSERR    invalid parameter
///
ErrorT CryptoMgrGetSensorIdCalled(bool* const called)
{
    ErrorT status = ERROR_SUCCESS;

    if (NULL != called)
    {
        *called = cryptoMgr.getSensorIdCalled;
    }
    else
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_IS_CALLED_PARAM, 0U);
        status = ERROR_SYSERR;
    }

    return status;
}

/// @} endgroup cryptomgr

