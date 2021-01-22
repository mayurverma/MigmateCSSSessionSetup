//***********************************************************************************
/// \file
///
/// Session Manager unit test
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
#include "Diag.h"
#include "CommandHandler.h"
#include "AssetMgr.h"
#include "LifecycleMgr.h"
#include "CryptoMgr.h"
#include "SessionMgrInternal.h"
}

TEST_GROUP(SessionMgrSetPskSessionKeysGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        mock().expectOneCall("SystemDrvEnableClock")
            .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
            .withParameter("enable", false);

        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpoint")
            .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_INIT);

        ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysReportsBadCommand)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_STATUS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_COMMAND)
        .withParameter("infoParam", command_code);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysReportsNullParam)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_PARAMS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysReportsGetSensorIdNotCalled)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = false;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_AGAIN, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);

    // Repeat but with CryptoMgrGetSensorIdCalled failing
    bool* badBool = NULL;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &badBool, sizeof(badBool))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsInvalidVideoAuthCipherMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    // All invalid
    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_RESERVED;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsInvalidVideoAuthKeyType)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    // Invalid
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsInvalidControlChannelCipherMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    // Invalid
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsInvalidControlChannelKey)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    // Invalid
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsSecondInitiationAttempt)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_RESERVED;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsIfNoMasterSecretBufferMallocFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)NULL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOSPC, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysRejectsIfNoMasterSecretPresent)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecretT* masterSecret = NULL;
    uint32_t masterSecretSizeLongWords = 0U;
    SessionMgrSetPskSessionKeysRequestT request;
    bool getSensorIdCalled = true;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &masterSecret, sizeof(masterSecret))
        .withOutputParameterReturning("masterSecretSizeLongWords", &masterSecretSizeLongWords, sizeof(masterSecretSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOENT, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysWithEphemeralMasterSecretSucceeds)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecretT* masterSecret = NULL;
    uint32_t masterSecretSizeLongWords = 0U;
    SessionMgrSetPskSessionKeysRequestT* request;
    SessionMgrSetSessionKeysResponseT* response;
    bool getSensorIdCalled = true;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };
    uint32_t videoAuthKeySizeBytes = 16U;
    uint32_t controlChannelKeySizeBytes = 16U;

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Force the ephemeral master secret
    sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    sessionMgrState.ephemeralMasterSecret.keySizeBytes = 16U;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[0] = 0xAA;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[1] = 0xBB;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &masterSecret, sizeof(masterSecret))
        .withOutputParameterReturning("masterSecretSizeLongWords", &masterSecretSizeLongWords, sizeof(masterSecretSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[0], sizeof(randomNumber[0]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[1], sizeof(randomNumber[1]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[2], sizeof(randomNumber[2]));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", request->common.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", request->common.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_DERIVE_KEYS);

    mock().expectOneCall("CryptoMgrDeriveKeys")
        .withParameter("masterSecretSizeBytes", sessionMgrState.ephemeralMasterSecret.keySizeBytes)
        .withParameter("hostSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("sensorSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("authSessionKeySizeBytes", videoAuthKeySizeBytes)
        .withParameter("controlChannelKeySizeBytes", controlChannelKeySizeBytes)
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("SystemDrvEnableI2cWriteCrcReset")
        .withParameter("enable", false);

    mock().expectOneCall("SystemMgrSetEvent")
        .withParameter("event", SYSTEM_MGR_EVENT_SESSION_START);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);

    // Check the response params
    response = (SessionMgrSetSessionKeysResponseT*)shared_memory;
    for (size_t i = 0; i < sizeof(response->sensorSalt); i++)
    {
        LONGS_EQUAL(randomNumber[0][i], response->sensorSalt[i]);
    }
    for (size_t i = 0; i < sizeof(response->videoAuthGmacIv); i++)
    {
        LONGS_EQUAL(randomNumber[1][i], response->videoAuthGmacIv[i]);
    }
    for (size_t i = 0; i < sizeof(response->controlChannelNonce); i++)
    {
        LONGS_EQUAL(randomNumber[2][i], response->controlChannelNonce[i]);
    }
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysWithEphemeralMasterSecretButGenRandomSaltFail)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecretT* masterSecret = NULL;
    uint32_t masterSecretSizeLongWords = 0U;
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Force the ephemeral master secret
    sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    sessionMgrState.ephemeralMasterSecret.keySizeBytes = 32U;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[0] = 0xAA;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[1] = 0xBB;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &masterSecret, sizeof(masterSecret))
        .withOutputParameterReturning("masterSecretSizeLongWords", &masterSecretSizeLongWords, sizeof(masterSecretSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[0], sizeof(randomNumber[0]))
        .andReturnValue(ERROR_IO);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_IO, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysWithEphemeralMasterSecretButGenRandomIvFail)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecretT* masterSecret = NULL;
    uint32_t masterSecretSizeLongWords = 0U;
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Force the ephemeral master secret
    sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    sessionMgrState.ephemeralMasterSecret.keySizeBytes = 32U;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[0] = 0xAA;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[1] = 0xBB;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &masterSecret, sizeof(masterSecret))
        .withOutputParameterReturning("masterSecretSizeLongWords", &masterSecretSizeLongWords, sizeof(masterSecretSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[0], sizeof(randomNumber[0]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[1], sizeof(randomNumber[1]))
        .andReturnValue(ERROR_IO);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_IO, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysWithEphemeralMasterSecretButGenRandomNonceFail)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecretT* masterSecret = NULL;
    uint32_t masterSecretSizeLongWords = 0U;
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Force the ephemeral master secret
    sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    sessionMgrState.ephemeralMasterSecret.keySizeBytes = 32U;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[0] = 0xAA;
    sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[1] = 0xBB;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &masterSecret, sizeof(masterSecret))
        .withOutputParameterReturning("masterSecretSizeLongWords", &masterSecretSizeLongWords, sizeof(masterSecretSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[0], sizeof(randomNumber[0]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[1], sizeof(randomNumber[1]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_NONCE_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[2], sizeof(randomNumber[2]))
        .andReturnValue(ERROR_IO);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_IO, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysWithNVMMasterSecretSucceeds)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecret256T asset;
    uint32_t assetSizeLongWords = sizeof(asset) / sizeof(uint32_t);
    SessionMgrSetPskSessionKeysRequestT* request;
    SessionMgrSetSessionKeysResponseT* response;
    bool getSensorIdCalled = true;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };
    uint32_t masterSecretSizeBytes = 32U;
    uint32_t videoAuthKeySizeBytes = 24U;
    uint32_t controlChannelKeySizeBytes = 32U;

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Setup the NVM master secret
    asset.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    asset.masterSecret[0] = 0xAA;
    asset.masterSecret[1] = 0xFF;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &asset, sizeof(asset))
        .withOutputParameterReturning("masterSecretSizeLongWords", &assetSizeLongWords, sizeof(assetSizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_GENERATE_RANDOMS);

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[0], sizeof(randomNumber[0]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[1], sizeof(randomNumber[1]));

    mock().expectOneCall("CryptoMgrGenRandomNumber")
        .withParameter("randomNumberSizeBytes", SESSION_MGR_IV_SIZE_BYTES)
        .withOutputParameterReturning("randomNumber", &randomNumber[2], sizeof(randomNumber[2]));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", asset.header.masterSecretKeyType)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", request->common.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", request->common.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_DERIVE_KEYS);

    mock().expectOneCall("CryptoMgrDeriveKeys")
        .withParameter("masterSecretSizeBytes", masterSecretSizeBytes)
        .withParameter("hostSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("sensorSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("authSessionKeySizeBytes", videoAuthKeySizeBytes)
        .withParameter("controlChannelKeySizeBytes", controlChannelKeySizeBytes)
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("SystemDrvEnableI2cWriteCrcReset")
        .withParameter("enable", false);

    mock().expectOneCall("SystemMgrSetEvent")
        .withParameter("event", SYSTEM_MGR_EVENT_SESSION_START);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);

    // Check the response params
    response = (SessionMgrSetSessionKeysResponseT*)shared_memory;
    for (size_t i = 0; i < sizeof(response->sensorSalt); i++)
    {
        LONGS_EQUAL(randomNumber[0][i], response->sensorSalt[i]);
    }
    for (size_t i = 0; i < sizeof(response->videoAuthGmacIv); i++)
    {
        LONGS_EQUAL(randomNumber[1][i], response->videoAuthGmacIv[i]);
    }
    for (size_t i = 0; i < sizeof(response->controlChannelNonce); i++)
    {
        LONGS_EQUAL(randomNumber[2][i], response->controlChannelNonce[i]);
    }
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysFailsWithBadNVMMasterSecret)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecret256T asset;
    uint32_t assetSizeLongWords = sizeof(asset) / sizeof(uint32_t);
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;
    uint32_t masterSecretSizeBytes = 32U;

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Setup the NVM master secret
    asset.header.masterSecretKeyType = 0xFF; // Bad!

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &asset, sizeof(asset))
        .withOutputParameterReturning("masterSecretSizeLongWords", &assetSizeLongWords, sizeof(assetSizeLongWords));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", asset.header.masterSecretKeyType)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_KEY)
        .withParameter("infoParam", asset.header.masterSecretKeyType);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_INVAL, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysFailsWithInvalidLengthNVMMasterSecret)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecret256T asset;
    uint32_t assetSizeLongWords = sizeof(asset) / sizeof(uint32_t); // Bad!
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;
    uint32_t masterSecretSizeBytes = 16U;

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Setup the NVM master secret
    asset.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    asset.masterSecret[0] = 0xAB;
    asset.masterSecret[1] = 0xCD;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &asset, sizeof(asset))
        .withOutputParameterReturning("masterSecretSizeLongWords", &assetSizeLongWords, sizeof(assetSizeLongWords));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", asset.header.masterSecretKeyType)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_LENGTH)
        .withParameter("infoParam", assetSizeLongWords);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_IO, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetPskSessionKeysGroup, TestSetPskSessionKeysFailsWithCorruptNVMMasterSecret)
{
    uint8_t malloc_buffer[sizeof(AssetMgrPskMasterSecret256T)];
    uint8_t shared_memory[4096];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS;
    AssetMgrPskMasterSecret256T asset;
    uint32_t assetSizeLongWords = sizeof(asset) / sizeof(uint32_t); // Bad!
    SessionMgrSetPskSessionKeysRequestT* request;
    bool getSensorIdCalled = true;

    // Setup the request params
    request = (SessionMgrSetPskSessionKeysRequestT*)shared_memory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.hostSalt[0] = 0x11;
    request->common.hostSalt[1] = 0x22;

    // Setup the NVM master secret
    asset.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    asset.masterSecret[0] = 0xAB;
    asset.masterSecret[1] = 0xCD;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &getSensorIdCalled, sizeof(getSensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrPskMasterSecret256T))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue(malloc_buffer);

    mock().expectOneCall("AssetMgrRetrievePskMasterSecret")
        .withOutputParameterReturning("masterSecret", &asset, sizeof(asset))
        .withOutputParameterReturning("masterSecretSizeLongWords", &assetSizeLongWords, sizeof(assetSizeLongWords))
        .andReturnValue(ERROR_RANGE);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetPskSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

