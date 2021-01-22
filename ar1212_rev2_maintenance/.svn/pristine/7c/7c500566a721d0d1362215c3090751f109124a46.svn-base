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

TEST_GROUP(SessionMgrSetSessionKeysGroup)
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

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeysReportsBadCommand)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_STATUS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_COMMAND)
        .withParameter("infoParam", command_code);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeysFailsIfNullParam)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_PARAMS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsIfGetCurrentLcsFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    SessionMgrSetSessionKeysRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsIfInvalidLcs)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    SessionMgrSetSessionKeysRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsIfGetSensorIdCalledFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = false;
    SessionMgrSetSessionKeysRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);

    // Repeat but in Secure LCS
    currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsIfGetSensorIdNotCalled)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = false;
    SessionMgrSetSessionKeysRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_AGAIN, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsIfCertificatesNotRetrieved)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = false;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_AGAIN, status);
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsInvalidAuthCipherMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    // All invalid
    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_RESERVED;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsInvalidAuthKeyType)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    // Invalid
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsInvalidAeCipherMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    // Invalid
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsInvalidControlChannelKeyType)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    // Invalid
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsInvalidSessionParamsCipher)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    // Invalid
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyRejectsSecondSessionAttempt)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT request;

    request.common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request.common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request.common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request.common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    // Invalid
    request.sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RESERVED;
    request.encryptedParamsSizeWords = 0U;
    request.encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    request.encryptedParams.masterSecret[0] = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenDecryptionBufferMallocFails)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)NULL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_NOSPC, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenPrivateKeyBufferMallocFails)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = (uint8_t*)0x1234;
    sessionMgrState.certificateState.publicKeySizeBytes = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;

    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;
    decryptedParams->masterSecret[0] = 0x12;
    decryptedParams->masterSecret[1] = 0x34;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)NULL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);
    BYTES_EQUAL(0, decryptedParams->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParams->masterSecret[1]);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenFailsToRetrievePrivateKey)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyLongWords = sizeof(privateKey) / sizeof(uint32_t);

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = (uint8_t*)0x1234;
    sessionMgrState.certificateState.publicKeySizeBytes = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;

    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;
    decryptedParams->masterSecret[0] = 0x12;
    decryptedParams->masterSecret[1] = 0x34;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withOutputParameterReturning("privKey", &privateKey, sizeof(privateKey))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeyLongWords, sizeof(privateKeyLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);
    BYTES_EQUAL(0, decryptedParams->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParams->masterSecret[1]);

}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenPrivateKeyIsCorrupt)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = (uint8_t*)0x1234;
    sessionMgrState.certificateState.publicKeySizeBytes = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;

    // Force a bad private key in NVM
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_RESERVED1;

    // Init the decrypted params buffer so we can check its been cleared
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;
    decryptedParams->masterSecret[0] = 0x12;
    decryptedParams->masterSecret[1] = 0x34;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withOutputParameterReturning("privKey", &privateKey, sizeof(privateKey))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords))
        .andReturnValue(ERROR_RANGE);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    BYTES_EQUAL(0, decryptedParams->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParams->masterSecret[1]);

}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenPrivateKeyLengthIsBad)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;

    // Force a bad private key size
    privateKeySizeLongWords -= 1;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = (uint8_t*)0x1234;
    sessionMgrState.certificateState.publicKeySizeBytes = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;

    // Force a bad private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    // Init the decrypted params buffer so we can check its been cleared
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;
    decryptedParams->masterSecret[0] = 0x12;
    decryptedParams->masterSecret[1] = 0x34;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withOutputParameterReturning("privKey", &privateKey, sizeof(privateKey))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    BYTES_EQUAL(0, decryptedParams->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParams->masterSecret[1]);

}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenDecryptionFails)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint32_t decryptedMsgSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT);
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PubCertT sensorCert;
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Init the private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    privateKey.header.authPrivKeyId = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    privateKey.exponent[0] = 0xEE;
    privateKey.exponent[1] = 0xFF;

     // Init the public key in the sensor certificate
    sensorCert.header.typeIdentifier = 0x4334;
    sensorCert.publicKey[0] = 0x88;
    sensorCert.publicKey[1] = 0x99;

    // Init the decrypted params buffer so we can check its been cleared
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParams = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;
    decryptedParams->masterSecret[0] = 0x12;
    decryptedParams->masterSecret[1] = 0x34;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = sensorCert.publicKey;
    sessionMgrState.certificateState.publicKeySizeBytes = sizeof(sensorCert.publicKey);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withOutputParameterReturning("privKey", &privateKey, sizeof(privateKey))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

    mock().expectOneCall("CryptoMgrRsaDecrypt")
        .withParameter("keySizeBytes", sizeof(privateKey.exponent))
        .withParameter("privateExponent", ((AssetMgrRsa4096PrivKeyT*)privateKeyBuffer)->exponent)
        .withParameter("modulus", sensorCert.publicKey)
        .withParameter("encryptedMsg", &request->encryptedParams)
        .withOutputParameterReturning("decryptedMsgSizeBytes", &decryptedMsgSizeBytes, sizeof(decryptedMsgSizeBytes))
        .withOutputParameterReturning("decryptedMsg", decryptedParamsBuffer, sizeof(decryptedParamsBuffer))
        .andReturnValue(ERROR_BADMSG);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    BYTES_EQUAL(0, decryptedParams->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParams->masterSecret[1]);

}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeySucceedsWhenDecryptionSucceeds)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    SessionMgrSetSessionKeysResponseT* response;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PubCertT sensorCert;
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;
    uint32_t masterSecretSizeBytes = ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;
    uint8_t randomNumber[3][SESSION_MGR_KDF_SALT_SIZE_BYTES] = {
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F },
        { 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0x0F },
        { 0xC0, 0xDE, 0xBA, 0xBE, 0xDE, 0xAD, 0xBE, 0xEF, 0x1A, 0xCE, 0xBA, 0xBE, 0xFA, 0xCE, 0xFE, 0xED }
    };
    uint32_t videoAuthKeySizeBytes = 16U;
    uint32_t controlChannelKeySizeBytes = 16U;
    SessionMgrSetSessionKeyEncryptedSessionParamsT decryptedParams;
    uint32_t decryptedParamsSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT) - ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Init the private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;
    privateKey.header.authPrivKeyId = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    privateKey.exponent[0] = 0xEE;
    privateKey.exponent[1] = 0xFF;

    // Init the decrypted parameters
    decryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    decryptedParams.masterSecret[0] = 0x12;
    decryptedParams.masterSecret[1] = 0x34;

    // Init the public key in the sensor certificate
    sensorCert.header.typeIdentifier = 0x4334;
    sensorCert.publicKey[0] = 0x88;
    sensorCert.publicKey[1] = 0x99;

    // Init the decryptedParamsBuffer
    decryptedParamsBuffer[0] = 0xfd;
    decryptedParamsBuffer[1] = 0xec;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = sensorCert.publicKey;
    sessionMgrState.certificateState.publicKeySizeBytes = sizeof(sensorCert.publicKey);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withOutputParameterReturning("privKey", privateKey.exponent, sizeof(privateKey.exponent))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

    mock().expectOneCall("CryptoMgrRsaDecrypt")
        .withParameter("keySizeBytes", sizeof(privateKey.exponent))
        .withParameter("privateExponent", ((AssetMgrRsa4096PrivKeyT*)privateKeyBuffer)->exponent)
        .withParameter("modulus", sensorCert.publicKey)
        .withParameter("encryptedMsg", &request->encryptedParams)
        .withOutputParameterReturning("decryptedMsgSizeBytes", &decryptedParamsSizeBytes, sizeof(decryptedParamsSizeBytes))
        .withOutputParameterReturning("decryptedMsg", &decryptedParams, sizeof(decryptedParams));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS)
        .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes));

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
        .withParameter("masterSecretSizeBytes", masterSecretSizeBytes)
        .withParameter("hostSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("sensorSaltSizeBytes", SESSION_MGR_KDF_SALT_SIZE_BYTES)
        .withParameter("authSessionKeySizeBytes", videoAuthKeySizeBytes)
        .withParameter("controlChannelKeySizeBytes", controlChannelKeySizeBytes)
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableI2cWriteCrcReset")
        .withParameter("enable", false);

    mock().expectOneCall("SystemMgrSetEvent")
        .withParameter("event", SYSTEM_MGR_EVENT_SESSION_START);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParamsBufferObject = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;;
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[1]);

    // Check the response params
    response = (SessionMgrSetSessionKeysResponseT*)sharedMemory;
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

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenDecryptionSucceedsWithMalformedParams)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PubCertT sensorCert;
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;
    uint32_t masterSecretSizeBytes = ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;
    SessionMgrSetSessionKeyEncryptedSessionParamsT decryptedParams;

    // Bad!
    uint32_t decryptedParamsSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT) - ASSET_MGR_PSK_MASTER_SECRET_192_KEY_SIZE_BYTES;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Init the private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;
    privateKey.header.authPrivKeyId = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    privateKey.exponent[0] = 0xEE;
    privateKey.exponent[1] = 0xFF;

    // Init the decrypted parameters
    decryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    decryptedParams.masterSecret[0] = 0x12;
    decryptedParams.masterSecret[1] = 0x34;

    // Init the public key in the sensor certificate
    sensorCert.header.typeIdentifier = 0x4334;
    sensorCert.publicKey[0] = 0x88;
    sensorCert.publicKey[1] = 0x99;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = sensorCert.publicKey;
    sessionMgrState.certificateState.publicKeySizeBytes = sizeof(sensorCert.publicKey);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withOutputParameterReturning("privKey", &privateKey, sizeof(privateKey))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

    mock().expectOneCall("CryptoMgrRsaDecrypt")
        .withParameter("keySizeBytes", sizeof(privateKey.exponent))
        .withParameter("privateExponent", ((AssetMgrRsa4096PrivKeyT*)privateKeyBuffer)->exponent)
        .withParameter("modulus", sensorCert.publicKey)
        .withParameter("encryptedMsg", &request->encryptedParams)
        .withOutputParameterReturning("decryptedMsgSizeBytes", &decryptedParamsSizeBytes, sizeof(decryptedParamsSizeBytes))
        .withOutputParameterReturning("decryptedMsg", &decryptedParams, sizeof(decryptedParams));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS)
        .withParameter("info", ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes));

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParamsBufferObject = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;;
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[1]);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenRsaSessionParamsMasterSecretKeyTypeIsBad)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PubCertT sensorCert;
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;
    uint32_t masterSecretSizeBytes = ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;
    SessionMgrSetSessionKeyEncryptedSessionParamsT decryptedParams;
    uint32_t decryptedParamsSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT) - ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Init the private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;
    privateKey.header.authPrivKeyId = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    privateKey.exponent[0] = 0xEE;
    privateKey.exponent[1] = 0xFF;

    // Init the decrypted parameters
    decryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    decryptedParams.masterSecret[0] = 0x12;
    decryptedParams.masterSecret[1] = 0x34;

    // Init the public key in the sensor certificate
    sensorCert.header.typeIdentifier = 0x4334;
    sensorCert.publicKey[0] = 0x88;
    sensorCert.publicKey[1] = 0x99;

    // Init the decryptedParamsBuffer
    decryptedParamsBuffer[0] = 0xfd;
    decryptedParamsBuffer[1] = 0xec;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = sensorCert.publicKey;
    sessionMgrState.certificateState.publicKeySizeBytes = sizeof(sensorCert.publicKey);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withOutputParameterReturning("privKey", privateKey.exponent, sizeof(privateKey.exponent))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

    mock().expectOneCall("CryptoMgrRsaDecrypt")
        .withParameter("keySizeBytes", sizeof(privateKey.exponent))
        .withParameter("privateExponent", ((AssetMgrRsa4096PrivKeyT*)privateKeyBuffer)->exponent)
        .withParameter("modulus", sensorCert.publicKey)
        .withParameter("encryptedMsg", &request->encryptedParams)
        .withOutputParameterReturning("decryptedMsgSizeBytes", &decryptedParamsSizeBytes, sizeof(decryptedParamsSizeBytes))
        .withOutputParameterReturning("decryptedMsg", &decryptedParams, sizeof(decryptedParams));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS)
        .withParameter("info", ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParamsBufferObject = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;;
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[1]);
}

TEST(SessionMgrSetSessionKeysGroup, TestSetSessionKeyFailsWhenRsaSessionParamsMasterSecretSizeIsBad)
{
    uint8_t sharedMemory[4096U];
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    bool sensorIdCalled = true;
    SessionMgrSetSessionKeysRequestT* request;
    uint8_t decryptedParamsBuffer[sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)];
    uint8_t privateKeyBuffer[sizeof(AssetMgrRsa4096PrivKeyT)];
    AssetMgrRsa4096PubCertT sensorCert;
    AssetMgrRsa4096PrivKeyT privateKey;
    uint32_t privateKeyHeaderSizeLongWords = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);
    uint32_t privateKeySizeLongWords = (sizeof(privateKey) / sizeof(uint32_t)) - privateKeyHeaderSizeLongWords;
    uint32_t masterSecretSizeBytes = ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES + 1;
    SessionMgrSetSessionKeyEncryptedSessionParamsT decryptedParams;
    uint32_t decryptedParamsSizeBytes = sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT) - ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;

    request = (SessionMgrSetSessionKeysRequestT*)sharedMemory;

    request->common.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    request->common.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->common.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    request->common.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->sessionParamsCipher = CRYPTO_MGR_ASYMMETRIC_CIPHER_RSAES_OAEP;
    request->encryptedParamsSizeWords = (sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT)- ASSET_MGR_PSK_MASTER_SECRET_192_KEY_SIZE_BYTES) / sizeof(uint16_t);
    request->encryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    request->encryptedParams.masterSecret[0] = 0xAA;
    request->encryptedParams.masterSecret[1] = 0xCC;

    // Init the private key
    privateKey.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;
    privateKey.header.authPrivKeyId = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    privateKey.exponent[0] = 0xEE;
    privateKey.exponent[1] = 0xFF;

    // Init the decrypted parameters
    decryptedParams.header.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    decryptedParams.masterSecret[0] = 0x12;
    decryptedParams.masterSecret[1] = 0x34;

    // Init the public key in the sensor certificate
    sensorCert.header.typeIdentifier = 0x4334;
    sensorCert.publicKey[0] = 0x88;
    sensorCert.publicKey[1] = 0x99;

    // Init the decryptedParamsBuffer
    decryptedParamsBuffer[0] = 0xfd;
    decryptedParamsBuffer[1] = 0xec;

    // Force the cached session mgr state public key
    sessionMgrState.certificateState.publicKey = sensorCert.publicKey;
    sessionMgrState.certificateState.publicKeySizeBytes = sizeof(sensorCert.publicKey);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrGetSensorIdCalled")
        .withOutputParameterReturning("isCalled", &sensorIdCalled, sizeof(sensorIdCalled));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrSetSessionKeyEncryptedSessionParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)decryptedParamsBuffer);

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(AssetMgrRsa4096PrivKeyT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)privateKeyBuffer);

    mock().expectOneCall("AssetMgrRetrievePrivateKey")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withOutputParameterReturning("privKey", privateKey.exponent, sizeof(privateKey.exponent))
        .withOutputParameterReturning("privKeySizeLongWords", &privateKeySizeLongWords, sizeof(privateKeySizeLongWords));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_RSA_DECRYPT);

    mock().expectOneCall("CryptoMgrRsaDecrypt")
        .withParameter("keySizeBytes", sizeof(privateKey.exponent))
        .withParameter("privateExponent", ((AssetMgrRsa4096PrivKeyT*)privateKeyBuffer)->exponent)
        .withParameter("modulus", sensorCert.publicKey)
        .withParameter("encryptedMsg", &request->encryptedParams)
        .withOutputParameterReturning("decryptedMsgSizeBytes", &decryptedParamsSizeBytes, sizeof(decryptedParamsSizeBytes))
        .withOutputParameterReturning("decryptedMsg", &decryptedParams, sizeof(decryptedParams));

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &masterSecretSizeBytes, sizeof(masterSecretSizeBytes));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS)
        .withParameter("info", ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)privateKeyBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sessionMgrState.certificateState.publicKey);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)decryptedParamsBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    sessionMgrState.certificateState.certificatesIssued = true;
    sessionMgrState.certificateState.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnSetSessionKeys(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK_TRUE(sessionMgrState.sessionState.sessionInitiated);
    LONGS_EQUAL((void*)NULL, sessionMgrState.certificateState.publicKey);

    // Check the master secret has been cleared from the malloced buffer
    SessionMgrSetSessionKeyEncryptedSessionParamsT* decryptedParamsBufferObject = (SessionMgrSetSessionKeyEncryptedSessionParamsT*)decryptedParamsBuffer;;
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[0]);
    BYTES_EQUAL(0, decryptedParamsBufferObject->masterSecret[1]);
}


