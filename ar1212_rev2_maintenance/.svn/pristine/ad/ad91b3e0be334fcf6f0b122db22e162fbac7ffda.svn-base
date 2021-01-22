//***********************************************************************************
/// \file
///
/// Session Manager internal implementation - Session setup support
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
//*************************************************************************************

#include <stdlib.h>     // for calloc()/malloc()/free()
#include <string.h>     // for memset()/memcpy()

#include "Common.h"
#include "Diag.h"

#include "SessionMgrInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

/// Sets up a video authentication/control channel session
///
/// \param[in] masterSecret             The negotiated master secret
/// \param[in] masterSecretSizeBytes    Size of the master secret
/// \param[out] response                Buffer to contain response parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Session setup successfully
///
ErrorT SessionMgrSetupSession(uint8_t* const masterSecret,
    uint32_t const masterSecretSizeBytes,
    SessionMgrSetSessionKeysResponseT* const response)
{
    SessionMgrSessionStateT* sessionState = &sessionMgrState.sessionState;
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    status = CryptoMgrGenRandomNumber(sizeof(sessionState->sessionParams.sensorSalt),
            sessionState->sessionParams.sensorSalt);

    if (ERROR_SUCCESS == status)
    {
        status = CryptoMgrGenRandomNumber(sizeof(sessionState->sessionParams.videoAuthGmacIv),
                sessionState->sessionParams.videoAuthGmacIv);
    }

    if (ERROR_SUCCESS == status)
    {
        status = CryptoMgrGenRandomNumber(sizeof(sessionState->sessionParams.controlChannelNonce),
                sessionState->sessionParams.controlChannelNonce);
    }

    if (ERROR_SUCCESS == status)
    {
        uint32_t videoAuthKeySizeBytes;
        uint32_t controlChannelKeySizeBytes;

        // Get the video authentication key size (ignore the result because videoAuthKeyType has been validated)
        (void)CryptoMgrGetSymmetricKeySizeBytes(sessionState->sessionParams.videoAuthKeyType,
            &videoAuthKeySizeBytes);

        // Get the control channel key size (ignore the result because controlChannelKeyType has been validated)
        (void)CryptoMgrGetSymmetricKeySizeBytes(sessionState->sessionParams.controlChannelKeyType,
            &controlChannelKeySizeBytes);

        // Derive the session keys
        DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_DERIVE_KEYS);

        status = CryptoMgrDeriveKeys(masterSecretSizeBytes,
                masterSecret,
                sizeof(sessionState->sessionParams.hostSalt),
                sessionState->sessionParams.hostSalt,
                sizeof(sessionState->sessionParams.sensorSalt),
                sessionState->sessionParams.sensorSalt,
                videoAuthKeySizeBytes,
                sessionState->sessionKeys.videoAuthKey,
                controlChannelKeySizeBytes,
                sessionState->sessionKeys.controlChannelKey);
    }

    if (ERROR_SUCCESS == status)
    {
        // Write the response parameters
        (void)memcpy(response->sensorSalt,
            sessionState->sessionParams.sensorSalt,
            sizeof(sessionState->sessionParams.sensorSalt));

        (void)memcpy(response->videoAuthGmacIv,
            sessionState->sessionParams.videoAuthGmacIv,
            sizeof(sessionState->sessionParams.videoAuthGmacIv));

        (void)memcpy(response->controlChannelNonce,
            sessionState->sessionParams.controlChannelNonce,
            sizeof(sessionState->sessionParams.controlChannelNonce));

        // We can now disable the I2C write CRC reset capability
        SystemDrvEnableI2cWriteCrcReset(false);

        SystemMgrSetEvent(SYSTEM_MGR_EVENT_SESSION_START);
    }

    return status;
}

/// Sets-up a PSK session
///
/// \param[out] response    Buffer to contain response parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Session setup successfully
/// \retval ERROR_NOSPC     Failed to allocate buffer for master secret
/// \retval ERROR_INVAL     Invalid master secret key type
/// \retval ERROR_IO        Error during NVM access
/// \retval ERROR_NOENT     No master secret is present
///
ErrorT SessionMgrSetupPskSession(SessionMgrSetSessionKeysResponseT* const response)
{
    AssetMgrPskMasterSecret256T* sessionMasterSecret = NULL;
    uint32_t sessionMasterSecretSizeBytes;
    uint32_t assetSizeLongWords = sizeof(AssetMgrPskMasterSecret256T) / sizeof(uint32_t);
    ErrorT status = ERROR_SUCCESS;

    // Allocate a buffer to contain the worse-case sized master secret
    // - use calloc to ensure the buffer is 'clean' before use
    sessionMasterSecret = (AssetMgrPskMasterSecret256T*)calloc(sizeof(AssetMgrPskMasterSecret256T), sizeof(uint8_t));
    if (NULL == sessionMasterSecret)
    {
        DiagPrintf("SessionMgrSetupPskSession: failed to alloc buffer for master secret\n");
        status = ERROR_NOSPC;
    }
    else
    {
        // Is there a master secret in the NVM?
        status = AssetMgrRetrievePskMasterSecret((AssetMgrPskMasterSecretT*)sessionMasterSecret,
                &assetSizeLongWords);

        if (ERROR_SUCCESS == status)
        {
            uint32_t expectedSizeLongWords;

            // We found a master secret, get its size
            status = CryptoMgrGetSymmetricKeySizeBytes(
                (CryptoMgrSymmetricKeyTypeT)sessionMasterSecret->header.masterSecretKeyType,
                    &sessionMasterSecretSizeBytes);

            if (ERROR_SUCCESS != status)
            {
                // Master secret in NVM is corrupt
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_KEY,
                    (uint32_t)sessionMasterSecret->header.masterSecretKeyType);
            }
            else
            {
                // Check the asset is the expected size
                expectedSizeLongWords =
                    (sizeof(AssetMgrPskMasterSecretHeaderT) + sessionMasterSecretSizeBytes) / sizeof(uint32_t);

                if (assetSizeLongWords != expectedSizeLongWords)
                {
                    SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_LENGTH,
                        assetSizeLongWords);
                    status = ERROR_IO;
                }
            }
        }
        else if (ERROR_NOENT == status)
        {
            // No master secret present in NVM, have we already received an ephemeral secret?
            if (0U != sessionMgrState.ephemeralMasterSecret.keySizeBytes)
            {
                // Use the ephemeral master secret
                (void)memcpy(sessionMasterSecret->masterSecret,
                    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret,
                    sizeof(sessionMasterSecret->masterSecret));

                sessionMasterSecretSizeBytes = sessionMgrState.ephemeralMasterSecret.keySizeBytes;
                status = ERROR_SUCCESS;
            }
            else
            {
                DiagPrintf("SessionMgrSetupPskSession: no master secret - rejecting\n");
                status = ERROR_NOENT;
            }
        }
        else
        {
            // Asset Manager reported an error, abort
        }
    }

    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrSetupSession(sessionMasterSecret->masterSecret,
                sessionMasterSecretSizeBytes,
                response);
    }

    // before freeing, erase the master secret that was used to setup the session to prevent
    // information leaks
    if (NULL != sessionMasterSecret)
    {
        (void)memset(sessionMasterSecret->masterSecret, 0, sizeof(sessionMasterSecret->masterSecret));
        free(sessionMasterSecret);
    }

    return status;
}

/// Decrypts an (RSA-encrypted) master secret
///
/// \param[in] request                              The SetSessionKeys request
/// \param[out] decryptedSessionParams              Buffer to contain the decrypted parameters
/// \param[in,out] decryptedSessionParamsSizeBytes  On input the size of the decryption buffer, on output the size of the decrypted parameters
///
/// \return Error status
/// \retval ERROR_SUCCESS   Parameters successfully decrypted
/// \retval ERROR_NOSPC     Failed to allocate a buffer to contain the private key
/// \retval ERROR_NOENT     Failed to locate private key in NVM
/// \retval ERROR_IO        Retrieved private key is corrupt/invalid
/// \retval ERROR_BADMSG    Fail to decrypt the parameters
///
ErrorT SessionMgrDecryptMasterSecret(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeyEncryptedSessionParamsT* const decryptedSessionParams,
    uint32_t* const decryptedSessionParamsSizeBytes)
{
    ErrorT status = ERROR_SUCCESS;

    AssetMgrRsa4096PrivKeyT* privateKey = NULL;
    uint32_t const privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(AssetMgrRsa4096PrivKeyT) / sizeof(uint32_t)) -
                                       privateKeyHeaderSizeLongWords;

    // Allocate a buffer to contain the worse-case private key
    // - use calloc to ensure the buffer is 'clean' before use
    privateKey = (AssetMgrRsa4096PrivKeyT*)calloc(sizeof(AssetMgrRsa4096PrivKeyT), sizeof(uint8_t));
    if (NULL == privateKey)
    {
        DiagPrintf("SessionMgrDecryptMasterSecret: failed to alloc buffer for private key\n");
        status = ERROR_NOSPC;
    }
    else
    {
        // Retrieve the appropriate private key from NVM
        status = AssetMgrRetrievePrivateKey(sessionMgrState.certificateState.certificateAuthId,
                privateKey->exponent, &privateKeySizeLongWords);

        if (ERROR_SUCCESS != status)
        {
            DiagPrintf("SessionMgrDecryptMasterSecret: failed to retrieve private key from NVM\n");
        }
        else
        {
            // Validate the retrieved key
            uint32_t privateKeySizeBytes = privateKeySizeLongWords * sizeof(uint32_t);

            if (sessionMgrState.certificateState.publicKeySizeBytes != privateKeySizeBytes)
            {
                DiagPrintf("SessionMgrDecryptMasterSecret: private/public key length mismatch (%d/%d)\n",
                    sessionMgrState.certificateState.publicKeySizeBytes, privateKeySizeBytes);
                status = ERROR_IO;
            }
        }
    }

    if (ERROR_SUCCESS == status)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

        status = CryptoMgrRsaDecrypt(sessionMgrState.certificateState.publicKeySizeBytes,
            (uint8_t*)privateKey->exponent,
                sessionMgrState.certificateState.publicKey,
            (uint8_t*)&request->encryptedParams,
                decryptedSessionParamsSizeBytes,
            (uint8_t* const)decryptedSessionParams);
    }

    // Clean-up
    if (NULL != privateKey)
    {
        (void)memset(privateKey, 0, sizeof(AssetMgrRsa4096PrivKeyT));
        free(privateKey);
    }

    // We free the cached public key because we won't need it again
    // whether or not the decryption was successful.
    free(sessionMgrState.certificateState.publicKey);
    sessionMgrState.certificateState.publicKey = NULL;

    return status;
}

/// Validates the RSA session request parameters.
///
/// Retrieves the size of the session master secret
///
/// \param[in] request                          RSA session request
/// \param[in] decryptedParams                  Decrypted session parameters
/// \param[in] decryptedParamsSizeBytes         Size of the decrypted parameters
/// \param[out] sessionMasterSecretSizeBytes    Command request parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Parameters are valid
/// \retval ERROR_INVAL     Parameters are invalid
///
ErrorT SessionMgrValidateRsaSessionParams(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeyEncryptedSessionParamsT* const decryptedParams,
    uint32_t const decryptedParamsSizeBytes,
    uint32_t* const sessionMasterSecretSizeBytes)
{
    // Get the size of the master secret
    ErrorT status = CryptoMgrGetSymmetricKeySizeBytes(
        (CryptoMgrSymmetricKeyTypeT)decryptedParams->header.masterSecretKeyType,
            sessionMasterSecretSizeBytes);

    if (ERROR_SUCCESS == status)
    {
        // Check we received the expected master secret length
        uint32_t expectedParamsSizeWords =
            (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsHeaderT) + *sessionMasterSecretSizeBytes) /
            sizeof(uint16_t);

        if (expectedParamsSizeWords != request->encryptedParamsSizeWords)
        {
            DiagPrintf("SessionMgrSetupRsaSession: master secret incorrect size (%d/%d) - rejecting\n",
                expectedParamsSizeWords, request->encryptedParamsSizeWords);
            status = ERROR_INVAL;
        }
        else if ((expectedParamsSizeWords * sizeof(uint16_t)) != decryptedParamsSizeBytes)
        {
            DiagPrintf("SessionMgrSetupRsaSession: encrypted params incorrect size (%d/%d) - rejecting\n",
                expectedParamsSizeWords * sizeof(uint16_t), decryptedParamsSizeBytes);
            status = ERROR_INVAL;
        }
        else
        {
            // We're good
        }
    }
    else
    {
        DiagPrintf("SessionMgrSetupRsaSession: master secret key size invalid (%d) - rejecting\n",
            decryptedParams->header.masterSecretKeyType);
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS,
        (uint16_t)status);

    return status;
}

/// Sets up a session using a key transported with RSA encryption
///
/// \param[in] request      Command request parameters
/// \param[out] response    Buffer to contain response parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Session setup successfully
/// \retval ERROR_IO        Error during NVM access
///
ErrorT SessionMgrSetupRsaSession(SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response)
{
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = NULL;
    uint32_t decryptedParamsSizeBytes;
    ErrorT status = ERROR_SUCCESS;

    decryptedParamsSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT);

    // Allocate a buffer to contain the decrypted params
    // - use calloc to ensure the buffer is 'clean' before use
    decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)calloc(decryptedParamsSizeBytes,
            sizeof(uint8_t));
    if (NULL == decryptedParams)
    {
        DiagPrintf("SessionMgrSetupPskSession: failed to alloc buffer for decrypted params\n");
        status = ERROR_NOSPC;
    }
    else
    {
        // Master secret is encrypted - decrypt it
        status = SessionMgrDecryptMasterSecret(request,
                decryptedParams, &decryptedParamsSizeBytes);
    }

    if (ERROR_SUCCESS == status)
    {
        uint32_t sessionMasterSecretSizeBytes;

        status = SessionMgrValidateRsaSessionParams(request,
                decryptedParams,
                decryptedParamsSizeBytes,
                &sessionMasterSecretSizeBytes);

        if (ERROR_SUCCESS == status)
        {
            status = SessionMgrSetupSession(decryptedParams->masterSecret,
                    sessionMasterSecretSizeBytes,
                    response);
        }
    }

    if (NULL != decryptedParams)
    {
        // erase the master secret that was used to setup the session
        // - this is to prevent potential information leaks
        (void)memset(decryptedParams->masterSecret, 0, sizeof(decryptedParams->masterSecret));
        free(decryptedParams);
    }

    return status;
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initiates a PSK session
///
/// \param[in] request      Command request parameters
/// \param[out] response    Buffer to contain response parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Session setup successfully
/// \retval ERROR_NOSPC     Failed to allocate buffer for master secret
/// \retval ERROR_IO        Error during NVM access
/// \retval ERROR_NOENT     No master secret is present
///
ErrorT SessionMgrInitiatePskSession(SessionMgrSetPskSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response)
{
    ErrorT status;

    // Flag that a session has been initiated
    sessionMgrState.sessionState.sessionInitiated = true;

    // Initialise and validate the request params
    SessionMgrInitSessionParams(&request->common);

    status = SessionMgrValidateSessionParams(&sessionMgrState.sessionState.sessionParams);
    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrSetupPskSession(response);
    }
    else
    {
        DiagPrintf("SessionMgrOnSetPskSessionKeys: invalid params - rejecting\n");
    }

    return status;
}

/// Initiates a session using a key transported with RSA encryption
///
/// \param[in] currentLcs   Current lifecycle state
/// \param[in] request      Command request parameters
/// \param[out] response    Buffer to contain response parameters
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Session setup successfully
/// \retval ERROR_IO        Error during NVM access
/// \retval ERROR_INVAL     Invalid RSA cipher
///
ErrorT SessionMgrInitiateRsaSession(LifecycleMgrLifecycleStateT const currentLcs,
    SessionMgrSetSessionKeysRequestT* const request,
    SessionMgrSetSessionKeysResponseT* const response)
{
    ErrorT status;

    // Flag that a session has been initiated
    sessionMgrState.sessionState.sessionInitiated = true;

    // Initialise and validate the (common) session parameters
    SessionMgrInitSessionParams(&request->common);

    status = SessionMgrValidateSessionParams(&sessionMgrState.sessionState.sessionParams);
    if (ERROR_SUCCESS == status)
    {
        // Validate the remaining RSA-specific parameters
        if (CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP == (CryptoMgrAsymmetricCipherT)request->sessionParamsCipher)
        {
            // We're good to go...
            status = SessionMgrSetupRsaSession(request, response);

            if ((LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == currentLcs) &&
                (ERROR_SUCCESS != status))
            {
                // Force all errors to BADMSG to prevent information leakage
                status = ERROR_BADMSG;
            }
        }
        else
        {
            DiagPrintf("SessionMgrOnSetPskSessionKeys: invalid cipher - rejecting\n");
            status = ERROR_INVAL;
        }
    }
    else
    {
        DiagPrintf("SessionMgrOnSetPskSessionKeys: invalid params - rejecting\n");
    }

    return status;
}

/// @} endgroup sessionmgr

