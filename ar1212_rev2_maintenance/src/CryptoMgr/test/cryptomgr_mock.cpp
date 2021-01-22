//***********************************************************************************
/// \file
///
/// Crypto Manager unit test
//
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

#include "cpputest_support.h"


extern "C"
{
    #include "CryptoMgr.h"
}


// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_CRYPTO_MGR_ON_GET_SENSOR_ID)
    ErrorT CryptoMgrOnGetSensorId(  HostCommandCodeT const commandCode,
                                    CommandHandlerCommandParamsT params)
    {
        mock().actualCall("CryptoMgrOnGetSensorId")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_ON_PHASE_ENTRY)
    ErrorT CryptoMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("CryptoMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_ON_SELF_TEST)
    ErrorT CryptoMgrOnSelfTest( HostCommandCodeT const commandCode,
                                CommandHandlerCommandParamsT params)
    {
        mock().actualCall("CryptoMgrOnSelfTest")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK)
    ErrorT CryptoMgrAuthenticatePatchChunk(uint32_t const assetId,
        uint32_t* const assetPackageBuffer,
        uint32_t* const decryptedPackageSizeBytes,
        uint8_t* const decryptedPackage)
    {
        mock().actualCall("CryptoMgrAuthenticatePatchChunk")
            .withParameter("assetId", assetId)
            .withParameter("assetPackageBuffer", assetPackageBuffer)
            .withOutputParameter("decryptedPackageSizeBytes", decryptedPackageSizeBytes)
            .withOutputParameter("decryptedPackage", decryptedPackage);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_AUTHENTICATE_ASSET)
    ErrorT CryptoMgrAuthenticateAsset(uint32_t const assetId,
        CryptoMgrRootOfTrustT const rootOfTrust,
        uint32_t* const assetPakageBuffer)
    {
        mock().actualCall("CryptoMgrAuthenticateAsset")
            .withParameter("assetId", assetId)
            .withParameter("rootOfTrust", rootOfTrust)
            .withOutputParameter("assetPakageBuffer", (void *)assetPakageBuffer);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_RSA_DECRYPT)
    ErrorT CryptoMgrRsaDecrypt(uint32_t const keySizeBytes,
        uint8_t* const privateExponent,
        uint8_t* const modulus,
        uint8_t* const encryptedMsg,
        uint32_t* const decryptedMsgSizeBytes,
        uint8_t* const decryptedMsg)
    {
        mock().actualCall("CryptoMgrRsaDecrypt")
            .withParameter("keySizeBytes", keySizeBytes)
            .withParameter("privateExponent", privateExponent)
            .withParameter("modulus", modulus)
            .withParameter("encryptedMsg", encryptedMsg)
            .withOutputParameter("decryptedMsgSizeBytes", decryptedMsgSizeBytes)
            .withOutputParameter("decryptedMsg", decryptedMsg);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_AEAD_DECRYPT)
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
        mock().actualCall("CryptoMgrAeadDecrypt")
            .withParameter("cipherMode", cipherMode)
            .withParameter("keySizeBytes", keySizeBytes)
            .withParameter("key", key)
            .withParameter("ivNonce", ivNonce)
            .withParameter("associatedDataSizeBytes", associatedDataSizeBytes)
            .withParameter("associatedData", associatedData)
            .withParameter("encryptedMsgSizeBytes", encryptedMsgSizeBytes)
            .withParameter("encryptedMsgAndTag", encryptedMsgAndTag)
            .withOutputParameter("decryptedMsg", decryptedMsg);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_DERIVE_KEYS)
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
        mock().actualCall("CryptoMgrDeriveKeys")
            .withParameter("masterSecretSizeBytes", masterSecretSizeBytes)
            .withParameter("masterSecret", masterSecret)
            .withParameter("hostSaltSizeBytes", hostSaltSizeBytes)
            .withParameter("hostSalt", hostSalt)
            .withParameter("sensorSaltSizeBytes", sensorSaltSizeBytes)
            .withParameter("sensorSalt", sensorSalt)
            .withParameter("authSessionKeySizeBytes", authSessionKeySizeBytes)
            .withOutputParameter("authSessionKey", authSessionKey)
            .withParameter("controlChannelKeySizeBytes", controlChannelKeySizeBytes)
            .withOutputParameter("controlChannelKey", controlChannelKey);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_GEN_RANDOM_NUMBER)
    ErrorT CryptoMgrGenRandomNumber(uint32_t const randomNumberSizeBytes,
        uint8_t* const randomNumber)
    {
        mock().actualCall("CryptoMgrGenRandomNumber")
            .withParameter("randomNumberSizeBytes", randomNumberSizeBytes)
            .withOutputParameter("randomNumber", randomNumber);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_ON_PAL_ABORT)
    void CryptoMgrOnPalAbort(void)
    {
        mock().actualCall("CryptoMgrOnPalAbort");
    }
#endif

#if !defined(MOCK_DISABLE_CRYPTO_MGR_GET_SYMMETRIC_KEY_SIZE_BYTES)
    ErrorT CryptoMgrGetSymmetricKeySizeBytes(CryptoMgrSymmetricKeyTypeT const type,
        uint32_t* const sizeBytes)
    {
        mock().actualCall("CryptoMgrGetSymmetricKeySizeBytes")
            .withParameter("type", type)
            .withOutputParameter("sizeBytes", sizeBytes);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_CRYPTO_MGR_GET_SENSOR_ID_CALLED)
    ErrorT CryptoMgrGetSensorIdCalled(bool* const isCalled)
    {
        mock().actualCall("CryptoMgrGetSensorIdCalled")
            .withOutputParameter("isCalled", isCalled);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
}

