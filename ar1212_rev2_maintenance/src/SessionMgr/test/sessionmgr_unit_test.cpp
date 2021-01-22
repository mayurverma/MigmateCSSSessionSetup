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
#include "HealthMonitorInternal.h"
}

// Mocks
extern "C"
{
    void* unittest_calloc(size_t nitems, size_t size)
    {
        mock().actualCall("unittest_calloc")
            .withParameter("nitems", nitems)
            .withParameter("size", size);
        return mock().returnPointerValueOrDefault(NULL);
    }

    void* unittest_malloc(size_t size)
    {
        mock().actualCall("unittest_malloc")
            .withParameter("size", size);
        return mock().returnPointerValueOrDefault(NULL);
    }

    void unittest_free(void* ptr)
    {
        mock().actualCall("unittest_free")
            .withPointerParameter("ptr", ptr);
    }
}

TEST_GROUP(SessionMgrInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(SessionMgrApiGroup)
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

TEST(SessionMgrInitGroup, TestInitSuccess)
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
    CHECK_FALSE(sessionMgrState.sessionState.sessionInitiated);
    CHECK_FALSE(sessionMgrState.videoAuthState.isActive);
}

TEST(SessionMgrInitGroup, TestInitFailure)
{
    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_DISABLE_VCORE_CLOCK_FAILED)
        .withParameter("infoParam", ERROR_INVAL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_INIT);

    ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrInitGroup, TestOnPhaseEntryWithBadPhaseDoesNotFail)
{
    ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_NUM_PHASES);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SessionMgrInitGroup, TestOnPhaseEntry)
{
    ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_PATCH);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SESSION);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SessionMgrInitGroup, TestShutdownPhaseNoVideoAuth)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ENTER_SHUTDOWN);

    ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SessionMgrInitGroup, TestShutdownPhaseWithVideoAuthActive)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ENTER_SHUTDOWN);

    mock().expectOneCall("MacDrvResetToIdle");
    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false);

    sessionMgrState.videoAuthState.isActive = true;

    ErrorT status = SessionMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SessionMgrInitGroup, TestFatalErrorDumpHandler)
{
    uint32_t dump_buffer[64];
    SessionMgrStateT* ptr_state = (SessionMgrStateT*)dump_buffer;

    sessionMgrState.sessionState.sessionInitiated = false;
    sessionMgrState.videoAuthState.isActive = true;

    SessionMgrOnDiagFatalErrorDump(dump_buffer, 64UL);
    CHECK_EQUAL(sessionMgrState.sessionState.sessionInitiated, ptr_state->sessionState.sessionInitiated);
    CHECK_EQUAL(sessionMgrState.videoAuthState.isActive, ptr_state->videoAuthState.isActive);

    // Call with NULL buffer
    ptr_state->sessionState.sessionInitiated = false;
    ptr_state->videoAuthState.isActive = false;
    SessionMgrOnDiagFatalErrorDump(NULL, 64UL);
    CHECK_FALSE(ptr_state->sessionState.sessionInitiated);
    CHECK_FALSE(ptr_state->videoAuthState.isActive);

    // Call with small buffer
    ptr_state->sessionState.sessionInitiated = false;
    ptr_state->videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionInitiated = true;
    sessionMgrState.videoAuthState.isActive = true;
    SessionMgrOnDiagFatalErrorDump(dump_buffer, 1UL);
    CHECK_FALSE(ptr_state->sessionState.sessionInitiated);
    CHECK_FALSE(ptr_state->videoAuthState.isActive);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsBadCommand)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_STATUS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_COMMAND)
        .withParameter("infoParam", command_code);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    ErrorT status = SessionMgrOnGetCertificates(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsNullParam)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_PARAMS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    ErrorT status = SessionMgrOnGetCertificates(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsBadRequestParam)
{
    SessionMgrGetCertificatesRequestT request;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_RESERVED1;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsLifecycleMgrGetCurrentLcsFailed)
{
    SessionMgrGetCertificatesRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsInvalidLcs)
{
    SessionMgrGetCertificatesRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsNoSpace)
{
    SessionMgrGetCertificatesRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOSPC, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesReportsNoEntryIfNoCertificateFound)
{
    uint8_t mallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    SessionMgrGetCertificatesRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_RMA;
    AssetMgrRsa4096PubCertT pubCert;
    uint32_t certSizeLongWords = 0;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(mallocBuffer);

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCert, sizeof(pubCert))
        .withOutputParameterReturning("certSizeLongWords", &certSizeLongWords, sizeof(certSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_NO_SENSOR_CERTIFICATE)
        .withParameter("info", ERROR_NOENT);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", mallocBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesRetrievesSensorCert)
{
    uint8_t sensorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t vendorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    SessionMgrGetCertificatesResponseT* response = (SessionMgrGetCertificatesResponseT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    AssetMgrRsa4096PubCertT pubCertBufferSensor;
    AssetMgrRsa4096PubCertT pubCertBufferVendor;
    uint8_t publicKeyBuffer[ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES];
    uint32_t publicKeySizeBytes;
    uint8_t* publicKeyAddr = pubCertBufferSensor.publicKey;

    uint32_t certSizeLongWords = 0;
    uint32_t sensorCertSizeLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(sensorCertMallocBuffer);

    pubCertBufferSensor.header.typeIdentifier = 0x4333;
    pubCertBufferSensor.publicKey[0] = 0xAA;
    pubCertBufferSensor.publicKey[1] = 0x55;
    pubCertBufferSensor.signature[0] = 0xBB;
    pubCertBufferSensor.signature[1] = 0xCC;

    publicKeySizeBytes = ASSET_MGR_RSA_3072_PUBLIC_KEY_SIZE_BYTES;

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferSensor, sizeof(pubCertBufferSensor))
        .withOutputParameterReturning("certSizeLongWords", &sensorCertSizeLongWords, sizeof(sensorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sensorCertMallocBuffer);

    mock().expectOneCall("AssetMgrGetPublicKeyReference")
        .withParameter("pubCert", response->sensorCertificate)
        .withOutputParameterReturning("pubKey", &publicKeyAddr, sizeof(publicKeyAddr))
        .withOutputParameterReturning("pubKeySizeBytes", &publicKeySizeBytes, sizeof(publicKeySizeBytes));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", publicKeySizeBytes)
        .andReturnValue(publicKeyBuffer);

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(vendorCertMallocBuffer);

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferVendor, sizeof(pubCertBufferVendor))
        .withOutputParameterReturning("certSizeLongWords", &certSizeLongWords, sizeof(certSizeLongWords))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_NO_VENDOR_CERTIFICATE)
        .withParameter("info", ERROR_NOENT);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", vendorCertMallocBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(response->sensorCertificateSizeWords, sensorCertSizeLongWords * sizeof(uint16_t));
    LONGS_EQUAL(response->sensorCertificate[0], 0x4333);

    response = (SessionMgrGetCertificatesResponseT*)&sharedMemory[(response->sensorCertificateSizeWords + 1U) * sizeof(uint16_t)];
    LONGS_EQUAL(response->sensorCertificateSizeWords, 0U);

    POINTERS_EQUAL(publicKeyBuffer, sessionMgrState.certificateState.publicKey);
    LONGS_EQUAL(publicKeySizeBytes, sessionMgrState.certificateState.publicKeySizeBytes);
    BYTES_EQUAL(pubCertBufferSensor.publicKey[0], sessionMgrState.certificateState.publicKey[0]);
    BYTES_EQUAL(pubCertBufferSensor.publicKey[1], sessionMgrState.certificateState.publicKey[1]);
}

TEST(SessionMgrApiGroup, TestGetCertificatesRetrievesSensorCertAndVendorCert)
{
    uint8_t sensorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t vendorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    SessionMgrGetCertificatesResponseT* response = (SessionMgrGetCertificatesResponseT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    AssetMgrRsa4096PubCertT pubCertBufferSensor;
    AssetMgrRsa4096PubCertT pubCertBufferVendor;
    uint32_t sensorCertSizeLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);
    uint32_t vendorCertSizeLongWords = sizeof(AssetMgrRsa4096PubCertT) / sizeof(uint32_t);
    uint8_t publicKeyBuffer[ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES];
    uint32_t publicKeySizeBytes;
    uint8_t* publicKeyAddr = pubCertBufferSensor.publicKey;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(sensorCertMallocBuffer);

    pubCertBufferSensor.header.typeIdentifier = 0x4333;
    pubCertBufferSensor.publicKey[0] = 0xAA;
    pubCertBufferSensor.publicKey[1] = 0xBB;
    pubCertBufferSensor.signature[0] = 0xCC;
    pubCertBufferSensor.signature[1] = 0xDD;

    publicKeySizeBytes = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;

    pubCertBufferVendor.header.typeIdentifier = 0x4334;
    pubCertBufferSensor.publicKey[0] = 0xCC;
    pubCertBufferSensor.signature[0] = 0xDD;

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferSensor, sizeof(pubCertBufferSensor))
        .withOutputParameterReturning("certSizeLongWords", &sensorCertSizeLongWords, sizeof(sensorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sensorCertMallocBuffer);

    mock().expectOneCall("AssetMgrGetPublicKeyReference")
        .withParameter("pubCert", response->sensorCertificate)
        .withOutputParameterReturning("pubKey", &publicKeyAddr, sizeof(publicKeyAddr))
        .withOutputParameterReturning("pubKeySizeBytes", &publicKeySizeBytes, sizeof(publicKeySizeBytes));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", publicKeySizeBytes)
        .andReturnValue(publicKeyBuffer);

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(vendorCertMallocBuffer);

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferVendor, sizeof(pubCertBufferVendor))
        .withOutputParameterReturning("certSizeLongWords", &vendorCertSizeLongWords, sizeof(vendorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", vendorCertMallocBuffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(sensorCertSizeLongWords * sizeof(uint16_t), response->sensorCertificateSizeWords);
    LONGS_EQUAL(0x4333, response->sensorCertificate[0]);

    response = (SessionMgrGetCertificatesResponseT*)&sharedMemory[(response->sensorCertificateSizeWords + 1U) * sizeof(uint16_t)];
    LONGS_EQUAL(vendorCertSizeLongWords * sizeof(uint16_t), response->sensorCertificateSizeWords);
    LONGS_EQUAL(0x4334, response->sensorCertificate[0]);

    POINTERS_EQUAL(publicKeyBuffer, sessionMgrState.certificateState.publicKey);
    LONGS_EQUAL(publicKeySizeBytes, sessionMgrState.certificateState.publicKeySizeBytes);
    BYTES_EQUAL(pubCertBufferSensor.publicKey[0], sessionMgrState.certificateState.publicKey[0]);
    BYTES_EQUAL(pubCertBufferSensor.publicKey[1], sessionMgrState.certificateState.publicKey[1]);
}

TEST(SessionMgrApiGroup, TestGetCertificatesFailsWhenAssetMgrGetPublicKeyFails)
{
    uint8_t sensorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    SessionMgrGetCertificatesResponseT* response = (SessionMgrGetCertificatesResponseT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    AssetMgrRsa4096PubCertT pubCertBufferSensor;
    uint32_t sensorCertSizeLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);
    uint32_t publicKeySizeBytes;
    uint8_t* publicKeyAddr = pubCertBufferSensor.publicKey;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(sensorCertMallocBuffer);

    pubCertBufferSensor.header.typeIdentifier = 0x4333;
    pubCertBufferSensor.publicKey[0] = 0xCC;
    pubCertBufferSensor.signature[0] = 0xDD;
    publicKeySizeBytes = ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES;

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferSensor, sizeof(pubCertBufferSensor))
        .withOutputParameterReturning("certSizeLongWords", &sensorCertSizeLongWords, sizeof(sensorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sensorCertMallocBuffer);

    mock().expectOneCall("AssetMgrGetPublicKeyReference")
        .withParameter("pubCert", response->sensorCertificate)
        .withOutputParameterReturning("pubKey", &publicKeyAddr, sizeof(publicKeyAddr))
        .withOutputParameterReturning("pubKeySizeBytes", &publicKeySizeBytes, sizeof(publicKeySizeBytes))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_NO_PUBLIC_KEY)
        .withParameter("info", ERROR_INVAL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesFailsWhenFailsToAllocPublicKeyBuffer)
{
    uint8_t sensorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    SessionMgrGetCertificatesResponseT* response = (SessionMgrGetCertificatesResponseT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    AssetMgrRsa4096PubCertT pubCertBufferSensor;
    uint32_t sensorCertSizeLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);
    uint32_t publicKeySizeBytes;
    uint8_t* publicKeyAddr = pubCertBufferSensor.publicKey;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(sensorCertMallocBuffer);

    pubCertBufferSensor.header.typeIdentifier = 0x4333;
    pubCertBufferSensor.publicKey[0] = 0xCC;
    pubCertBufferSensor.signature[0] = 0xDD;
    publicKeySizeBytes = ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES;

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferSensor, sizeof(pubCertBufferSensor))
        .withOutputParameterReturning("certSizeLongWords", &sensorCertSizeLongWords, sizeof(sensorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sensorCertMallocBuffer);

    mock().expectOneCall("AssetMgrGetPublicKeyReference")
        .withParameter("pubCert", response->sensorCertificate)
        .withOutputParameterReturning("pubKey", &publicKeyAddr, sizeof(publicKeyAddr))
        .withOutputParameterReturning("pubKeySizeBytes", &publicKeySizeBytes, sizeof(publicKeySizeBytes));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", publicKeySizeBytes)
        .andReturnValue((void*)NULL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_NOSPC, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesFailsWhenFailsToAllocSecondVendorCertBuffer)
{
    uint8_t sensorCertMallocBuffer[sizeof(AssetMgrRsa4096PubCertT)];
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    SessionMgrGetCertificatesResponseT* response = (SessionMgrGetCertificatesResponseT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    AssetMgrRsa4096PubCertT pubCertBufferSensor;
    uint32_t sensorCertSizeLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);
    uint8_t publicKeyBuffer[ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES];
    uint32_t publicKeySizeBytes;
    uint8_t* publicKeyAddr = pubCertBufferSensor.publicKey;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue(sensorCertMallocBuffer);

    pubCertBufferSensor.header.typeIdentifier = 0x4333;
    pubCertBufferSensor.publicKey[0] = 0xCC;
    pubCertBufferSensor.signature[0] = 0xDD;
    publicKeySizeBytes = ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES;

    mock().expectOneCall("AssetMgrRetrieveCertificate")
        .withParameter("certAuthId", ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B)
        .withParameter("certPurpose", ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR)
        .withOutputParameterReturning("pubCert", &pubCertBufferSensor, sizeof(pubCertBufferSensor))
        .withOutputParameterReturning("certSizeLongWords", &sensorCertSizeLongWords, sizeof(sensorCertSizeLongWords))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", sensorCertMallocBuffer);

    mock().expectOneCall("AssetMgrGetPublicKeyReference")
        .withParameter("pubCert", response->sensorCertificate)
        .withOutputParameterReturning("pubKey", &publicKeyAddr, sizeof(publicKeyAddr))
        .withOutputParameterReturning("pubKeySizeBytes", &publicKeySizeBytes, sizeof(publicKeySizeBytes));

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", publicKeySizeBytes)
        .andReturnValue(publicKeyBuffer);

    mock().expectOneCall("unittest_malloc")
        .withParameter("size", sizeof(AssetMgrRsa4096PubCertT))
        .andReturnValue((void*)NULL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_NOSPC, status);
}

TEST(SessionMgrApiGroup, TestGetCertificatesRejectsAttemptToRetrieveSecondCertificates)
{
    uint8_t sharedMemory[4096];
    SessionMgrGetCertificatesRequestT* request = (SessionMgrGetCertificatesRequestT*)sharedMemory;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    sessionMgrState.certificateState.certificatesIssued = true;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_CERTIFICATES;
    request->certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    ErrorT status = SessionMgrOnGetCertificates(command_code, (CommandHandlerCommandParamsT)request);
    LONGS_EQUAL(ERROR_ACCESS, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretReportsBadCommand)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_STATUS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_COMMAND)
        .withParameter("infoParam", command_code);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretReportsNullParam)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_PARAMS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretReportsLifecycleMgrGetCurrentLcsFailed)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    SessionMgrSetEphemeralMasterSecretRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretReportsInvalidLcs)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    SessionMgrSetEphemeralMasterSecretRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);

    // Repeat for RMA LCS
    currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_RMA;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ACCESS, status);

}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretRejectsIfMasterSecretPresent)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    SessionMgrSetEphemeralMasterSecretRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    sessionMgrState.ephemeralMasterSecret.keySizeBytes = 16U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ALREADY, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretRejectsIfKeyTypeInvalid)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    SessionMgrSetEphemeralMasterSecretRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    uint32_t sizeBytes = 0;

    request.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED)
        .withOutputParameterReturning("sizeBytes", &sizeBytes, sizeof(sizeBytes))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrApiGroup, TestSetEphemeralMasterSecretSucceeds)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    SessionMgrSetEphemeralMasterSecretRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    uint32_t sizeBytes = 16U;

    request.masterSecretKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memset(request.masterSecret, 0xAA, sizeBytes);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &sizeBytes, sizeof(sizeBytes))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    ErrorT status = SessionMgrOnSetEphemeralMasterSecret(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(sizeBytes, sessionMgrState.ephemeralMasterSecret.keySizeBytes);
    LONGS_EQUAL(request.masterSecretKeyType, sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType);
    LONGS_EQUAL(request.masterSecret[0], sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret[0]);
}

TEST(SessionMgrApiGroup, TestDecryptCommandRequestParamsSucceeds)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_ENABLE_MONITORING;
    HealthMonitorEnableMonitoringRequestParamsT commandParams;
    HealthMonitorSystemClockFreqT  decryptedParams;

    sessionMgrState.sessionState.sessionInitiated = true;

    uint32_t sizeBytes = sizeof(HealthMonitorSystemClockFreqT);

    commandParams.encryptedParams.extClkFreqHz = 10000000;    //  10MHz
    commandParams.encryptedParams.pllFreqHz = 100000000;      // 100MHz

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);
    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
          .ignoreOtherParameters();
    mock().expectOneCall("CryptoMgrAeadDecrypt")
          .ignoreOtherParameters();

    ErrorT status = SessionMgrDecryptCommandRequestParams(
            command_code,
            (uint8_t*)&commandParams,
            (uint8_t*)&decryptedParams,
            sizeBytes);

    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SessionMgrApiGroup, TestDecryptCommandRequestParamsBadPhase)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_ENABLE_MONITORING;
    HealthMonitorEnableMonitoringRequestParamsT commandParams;
    HealthMonitorSystemClockFreqT  decryptedParams;

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_CONFIGURE);

    uint32_t sizeBytes = sizeof(HealthMonitorSystemClockFreqT);

    commandParams.encryptedParams.extClkFreqHz = 10000000;    //  10MHz
    commandParams.encryptedParams.pllFreqHz = 100000000;      // 100MHz

    ErrorT status = SessionMgrDecryptCommandRequestParams(
            command_code,
            (uint8_t*)&commandParams,
            (uint8_t*)&decryptedParams,
            sizeBytes);

    LONGS_EQUAL(ERROR_ACCESS, status);
}

TEST(SessionMgrApiGroup, TestDecryptCommandRequestWithCryptoMgrFailure)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_ENABLE_MONITORING;
    HealthMonitorEnableMonitoringRequestParamsT commandParams;
    HealthMonitorSystemClockFreqT  decryptedParams;

    sessionMgrState.sessionState.sessionInitiated = true;

    uint32_t sizeBytes = sizeof(HealthMonitorSystemClockFreqT);

    commandParams.encryptedParams.extClkFreqHz = 10000000;    //  10MHz
    commandParams.encryptedParams.pllFreqHz = 100000000;      // 100MHz

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);
    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_INVAL);

    ErrorT status = SessionMgrDecryptCommandRequestParams(
            command_code,
            (uint8_t*)&commandParams,
            (uint8_t*)&decryptedParams,
            sizeBytes);

    LONGS_EQUAL(ERROR_INVAL, status);
}


