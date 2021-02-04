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

// Mocks
extern "C"
{
    MacDrvConfigT macDrvConfigCopy;
    MacDrvRoiConfigT macDrvRoiConfigCopy;

    // Specialized mock
    ErrorT MacDrvSetConfig(MacDrvConfigT* const config)
    {
        mock().actualCall("MacDrvSetConfig")
            .withParameter("config", config);

        // Copy the supplied configuration
        memcpy(&macDrvConfigCopy, config, sizeof(macDrvConfigCopy));

        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    // Specialized mock
    ErrorT MacDrvSetRoiConfig(MacDrvRoiConfigT* const config)
    {
        mock().actualCall("MacDrvSetRoiConfig")
            .withParameter("config", config);

        // Copy the supplied configuration
        memcpy(&macDrvRoiConfigCopy, config, sizeof(macDrvRoiConfigCopy));

        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
}

TEST_GROUP(SessionMgrSetVideoAuthRoiGroup)
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

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiReportsBadCommand)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_GET_STATUS;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_COMMAND)
        .withParameter("infoParam", command_code);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiReportsNullParam)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_PARAMS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfGetCurrentLcsFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfDecryptionBufferMallocFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)NULL);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_BADMSG, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfAeadDecryptionFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request; // encrypted, so unitialised
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    uint32_t controlChannelKeySizeBytes = 16U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];

    uint8_t controlChannelNonce[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    // Force the Session Mgr state
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters()
        .andReturnValue(ERROR_BADMSG);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_BADMSG, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfEnableClockFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request; // encrypted so not initialised
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    uint32_t controlChannelSizeBytes = 16U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t controlChannelNonce[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    // Force the Session Mgr state
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    // Setup the decrypted request
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", CRYPTO_MGR_SYMMETRIC_KEY_AES_128)
        .withOutputParameterReturning("sizeBytes", &controlChannelSizeBytes, sizeof(controlChannelSizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelSizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_BADMSG, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfReconfigInGmacMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request; // encrypted so don't initialise
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthkey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelkey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = true;
    // Control channel configuration
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelkey, sizeof(controlChannelkey));

    // Video channel configuration
    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthkey, sizeof(videoAuthkey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

     mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOEXEC, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfBadVideoAuthMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthkey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelkey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelkey, sizeof(controlChannelkey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthkey, sizeof(videoAuthkey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->videoAuthMode++; // BAD!
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfBadPixelPackingMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthkey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelkey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelkey, sizeof(controlChannelkey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthkey, sizeof(videoAuthkey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackMode++; // BAD!
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfSetSessionKeyFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 16U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", sizeof(videoAuthKey))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfSetGmacIvFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 16U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));


    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", sizeof(videoAuthKey));

    mock().expectOneCall("MacDrvSetGmacIv")
        .withParameter("iv", sessionMgrState.sessionState.sessionParams.videoAuthGmacIv)
        .withParameter("ivLenBytes", sizeof(videoAuthGmacIv))
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfSetConfigFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 16U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", sizeof(videoAuthKey));

    mock().expectOneCall("MacDrvSetGmacIv")
        .withParameter("iv", sessionMgrState.sessionState.sessionParams.videoAuthGmacIv)
        .withParameter("ivLenBytes", sizeof(videoAuthGmacIv));

    mock().expectOneCall("MacDrvSetConfig")
        .ignoreOtherParameters()
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfSetRoiConfigFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 32U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
        0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
        0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", 32U);

    mock().expectOneCall("MacDrvResetIv");

    mock().expectOneCall("MacDrvSetConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_ROI);

    mock().expectOneCall("MacDrvSetRoiConfig")
        .ignoreOtherParameters()
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiSucceedsInEncryptedGmacMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request; // encrypted so don't initialise
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint32_t videoAuthKeySizeBytes = 16U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthkey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelkey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;

    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelkey, sizeof(controlChannelkey));
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;

    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthkey, sizeof(videoAuthkey));
    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;

    // Setup the (decrypted) request
    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x1234U;
    decryptedConfig->frameCounter = 0xFEDCBA98U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", videoAuthKeySizeBytes);

    mock().expectOneCall("MacDrvSetGmacIv")
        .withParameter("iv", sessionMgrState.sessionState.sessionParams.videoAuthGmacIv)
        .withParameter("ivLenBytes", sizeof(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv));

    mock().expectOneCall("MacDrvSetConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("MacDrvStartAuthentication");

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(sessionMgrState.videoAuthState.isActive);

    // Check the MacDrv config was as expected
    CHECK_FALSE(macDrvConfigCopy.cmacNotGmac);
    LONGS_EQUAL(MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME, macDrvConfigCopy.videoAuthmode);
    LONGS_EQUAL(MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED, macDrvConfigCopy.pixelPackingMode);
    LONGS_EQUAL(0x1234U, macDrvConfigCopy.pixelPackingValue);
    LONGS_EQUAL(0xFEDCBA98, macDrvConfigCopy.frameCount);

    // Check the decryption buffer was cleared
    LONGS_EQUAL(0, decryptedConfig->pixelPackValue);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiConfigThenReconfigSucceedsInCmacMode)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 32U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t malloc_copy[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfigCopy = (SessionMgrVideoAuthConfigParamsT*)malloc_copy;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x5678U;

    decryptedConfig->firstRow = 0x123;
    decryptedConfig->lastRow = 0xabc;
    decryptedConfig->rowSkip = 8;

    decryptedConfig->firstColumn = 0x567;
    decryptedConfig->lastColumn = 0xcab;
    decryptedConfig->columnSkip = 2;

    decryptedConfig->frameCounter = 0x12345678;

    // decryptedConfig is set to zero by code to stop information leak, make a copy for for test purposeds
    memcpy(decryptedConfigCopy, decryptedConfig, sizeof(SessionMgrVideoAuthConfigParamsT));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", videoAuthKeySizeBytes);

    mock().expectOneCall("MacDrvResetIv");

    mock().expectOneCall("MacDrvSetConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_ROI);

    mock().expectOneCall("MacDrvSetRoiConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("MacDrvStartAuthentication")
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(sessionMgrState.videoAuthState.isActive);

    // Check the MacDrv config was as expected
    CHECK_TRUE(macDrvConfigCopy.cmacNotGmac);
    LONGS_EQUAL(MAC_DRV_VIDEO_AUTH_MODE_ROI, macDrvConfigCopy.videoAuthmode);
    LONGS_EQUAL(MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED, macDrvConfigCopy.pixelPackingMode);
    LONGS_EQUAL(0x5678U, macDrvConfigCopy.pixelPackingValue);
    LONGS_EQUAL(0x12345678UL, macDrvConfigCopy.frameCount);

    // Check the MacDrv ROI config was as expected
    LONGS_EQUAL(decryptedConfigCopy->firstRow, macDrvRoiConfigCopy.firstRow);
    LONGS_EQUAL(decryptedConfigCopy->lastRow, macDrvRoiConfigCopy.lastRow);
    LONGS_EQUAL(decryptedConfigCopy->rowSkip, macDrvRoiConfigCopy.rowSkip);
    LONGS_EQUAL(decryptedConfigCopy->firstColumn, macDrvRoiConfigCopy.firstCol);
    LONGS_EQUAL(decryptedConfigCopy->lastColumn, macDrvRoiConfigCopy.lastCol);
    LONGS_EQUAL(decryptedConfigCopy->columnSkip, macDrvRoiConfigCopy.colSkip);
    LONGS_EQUAL((uint32_t)decryptedConfigCopy->frameCounter, macDrvRoiConfigCopy.frameCount);

    //***********************************************************************************************
    // Re-config with a new ROI
    //***********************************************************************************************
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x5678U;

    decryptedConfig->firstRow = 0x321;
    decryptedConfig->lastRow = 0xcab;
    decryptedConfig->rowSkip = 4;

    decryptedConfig->firstColumn = 0x765;
    decryptedConfig->lastColumn = 0xbac;
    decryptedConfig->columnSkip = 16;

    decryptedConfig->frameCounter = 0x87654321;

    // decryptedConfig is set to zero by code to stop information leak, make a copy for for test purposeds
    memcpy(decryptedConfigCopy, decryptedConfig, sizeof(SessionMgrVideoAuthConfigParamsT));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_ROI);

    mock().expectOneCall("MacDrvSetRoiConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Check the MacDrv config was as expected
    CHECK_TRUE(macDrvConfigCopy.cmacNotGmac);
    LONGS_EQUAL(MAC_DRV_VIDEO_AUTH_MODE_ROI, macDrvConfigCopy.videoAuthmode);
    LONGS_EQUAL(MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED, macDrvConfigCopy.pixelPackingMode);
    LONGS_EQUAL(0x5678U, macDrvConfigCopy.pixelPackingValue);
    LONGS_EQUAL(0x12345678UL, macDrvConfigCopy.frameCount);

    // Check the MacDrv ROI config was as expected
    LONGS_EQUAL(decryptedConfigCopy->firstRow, macDrvRoiConfigCopy.firstRow);
    LONGS_EQUAL(decryptedConfigCopy->lastRow, macDrvRoiConfigCopy.lastRow);
    LONGS_EQUAL(decryptedConfigCopy->rowSkip, macDrvRoiConfigCopy.rowSkip);
    LONGS_EQUAL(decryptedConfigCopy->firstColumn, macDrvRoiConfigCopy.firstCol);
    LONGS_EQUAL(decryptedConfigCopy->lastColumn, macDrvRoiConfigCopy.lastCol);
    LONGS_EQUAL(decryptedConfigCopy->columnSkip, macDrvRoiConfigCopy.colSkip);
    LONGS_EQUAL((uint32_t)decryptedConfigCopy->frameCounter, macDrvRoiConfigCopy.frameCount);
}

TEST(SessionMgrSetVideoAuthRoiGroup, TestSetVideoAuthRoiFailsIfMacDrvStartAuthenticationFails)
{
    HostCommandCodeT command_code = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    SessionMgrSetVideoAuthRoiRequestT request;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    uint32_t videoAuthKeySizeBytes = 32U;
    uint32_t controlChannelKeySizeBytes = 24U;
    uint8_t malloc_buffer[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfig = (SessionMgrVideoAuthConfigParamsT*)malloc_buffer;

    uint8_t malloc_copy[sizeof(SessionMgrVideoAuthConfigParamsT)];
    SessionMgrVideoAuthConfigParamsT* decryptedConfigCopy = (SessionMgrVideoAuthConfigParamsT*)malloc_copy;

    uint8_t videoAuthGmacIv[] =
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x0B, 0x0C
    };

    uint8_t controlChannelNonce[] =
    {
        0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0x5B, 0x5C
    };

    uint8_t videoAuthKey[] =
    {
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
        0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20
    };

    uint8_t controlChannelKey[] =
    {
        0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x19, 0x2A, 0x23, 0x24, 0x25, 0x26, 0x27, 0x68
    };

    // Force the Session Mgr state
    sessionMgrState.videoAuthState.isActive = false;
    sessionMgrState.sessionState.sessionParams.controlChannelCipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    sessionMgrState.sessionState.sessionParams.controlChannelKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    memcpy(sessionMgrState.sessionState.sessionParams.controlChannelNonce, controlChannelNonce, sizeof(controlChannelNonce));
    memcpy(sessionMgrState.sessionState.sessionKeys.controlChannelKey, controlChannelKey, sizeof(controlChannelKey));

    sessionMgrState.sessionState.sessionParams.videoAuthCipherMode = CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC;
    sessionMgrState.sessionState.sessionParams.videoAuthKeyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    memcpy(sessionMgrState.sessionState.sessionParams.videoAuthGmacIv, videoAuthGmacIv, sizeof(videoAuthGmacIv));
    memcpy(sessionMgrState.sessionState.sessionKeys.videoAuthKey, videoAuthKey, sizeof(videoAuthKey));

    // Setup encrypted request object and its mac
    (void)memset(&request, 0, sizeof(request));
    request.mac[0] = 0xAA;

    (void)memset(decryptedConfig, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
    decryptedConfig->videoAuthMode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    decryptedConfig->pixelPackMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    decryptedConfig->pixelPackValue = 0x5678U;

    decryptedConfig->firstRow = 0x123;
    decryptedConfig->lastRow = 0xabc;
    decryptedConfig->rowSkip = 8;

    decryptedConfig->firstColumn = 0x567;
    decryptedConfig->lastColumn = 0xcab;
    decryptedConfig->columnSkip = 2;

    decryptedConfig->frameCounter = 0x12345678;

    // decryptedConfig is set to zero by code to stop information leak, make a copy for for test purposes
    memcpy(decryptedConfigCopy, decryptedConfig, sizeof(SessionMgrVideoAuthConfigParamsT));

    //***********************************************************************************************
    // Config ROI with a ERROR_ALREADY from MacDrvStartAuthentication() return;
    //***********************************************************************************************
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("size", sizeof(uint8_t))
        .andReturnValue((void*)malloc_buffer);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.controlChannelKeyType)
        .withOutputParameterReturning("sizeBytes", &controlChannelKeySizeBytes, sizeof(controlChannelKeySizeBytes));

    mock().expectOneCall("CryptoMgrAeadDecrypt")
        .withParameter("cipherMode", sessionMgrState.sessionState.sessionParams.controlChannelCipherMode)
        .withParameter("keySizeBytes", controlChannelKeySizeBytes)
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.controlChannelKey)
        .withParameter("ivNonce", sessionMgrState.sessionState.sessionParams.controlChannelNonce)
        .withParameter("associatedDataSizeBytes", sizeof(uint16_t))
        .withParameter("encryptedMsgSizeBytes", sizeof(SessionMgrVideoAuthConfigParamsT))
        .withParameter("encryptedMsgAndTag", &request)
        .withOutputParameterReturning("decryptedMsg", (void*)malloc_buffer, sizeof(malloc_buffer))
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", true);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    mock().expectOneCall("CryptoMgrGetSymmetricKeySizeBytes")
        .withParameter("type", sessionMgrState.sessionState.sessionParams.videoAuthKeyType)
        .withOutputParameterReturning("sizeBytes", &videoAuthKeySizeBytes, sizeof(videoAuthKeySizeBytes));

    mock().expectOneCall("MacDrvSetSessionKey")
        .withParameter("key", sessionMgrState.sessionState.sessionKeys.videoAuthKey)
        .withParameter("keyLenBytes", videoAuthKeySizeBytes);

    mock().expectOneCall("MacDrvResetIv");

    mock().expectOneCall("MacDrvSetConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_ROI);

    mock().expectOneCall("MacDrvSetRoiConfig")
        .ignoreOtherParameters();

    mock().expectOneCall("SystemMgrGetCurrentPhase")
          .andReturnValue(SYSTEM_MGR_PHASE_SESSION);

    mock().expectOneCall("MacDrvStartAuthentication")
        .andReturnValue(ERROR_ALREADY);

    // Disable clock when MacDrvStartAuthentication() returns an error.
    mock().expectOneCall("SystemDrvEnableClock")
        .withParameter("clock", SYSTEM_DRV_CSS_CLOCK_VCORE)
        .withParameter("enable", false);

	mock().expectOneCall("unittest_free")
        .withParameter("ptr", (void*)malloc_buffer);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    ErrorT status = SessionMgrOnSetVideoAuthROI(command_code, (CommandHandlerCommandParamsT)&request);
    LONGS_EQUAL(ERROR_ALREADY, status);
    CHECK_FALSE(sessionMgrState.videoAuthState.isActive);
}
