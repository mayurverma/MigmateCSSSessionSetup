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
#include "CommandHandler.h"
#include "AR0820.h"
#include "Diag.h"
#include "CryptoMgr.h"
#include "LifecycleMgr.h"
#include "CryptoMgrInternal.h"
#include "PatchMgr.h"
#include "cc_util_error.h"
#include "AssetMgr.h"
#include "CC312HalPal.h"

void CC_PalTrngParamSet(const AssetMgrCC312TrngCharT* pTrngParams)
{
    mock().actualCall("CC_PalTrngParamSet")
        .withParameter("pTrngParams", pTrngParams);
}

void* unittest_calloc(size_t nitems, size_t size)
{
    mock().actualCall("unittest_calloc")
        .withParameter("nitems", nitems)
        .withParameter("size", size);
    return mock().returnPointerValueOrDefault(NULL);
}

void unittest_free(void* ptr)
{
    mock().actualCall("unittest_free")
        .withPointerParameter("ptr", ptr);
}

}

TEST_GROUP(CryptoMgrGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        cryptoMgr.status = CRYPTO_MGR_UNINIT;
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(CryptoMgrGroup, TestGetSensorIdWrongHostCommand)
{
    CryptoMgrGetSensorIdResponseParamsT params = {0};

    // Wrong command code
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnGetSensorId(HOST_COMMAND_CODE_GET_CERTIFICATES, (CommandHandlerCommandParamsT)&params));
}

void TestGetSensorIdBasicLcs(LifecycleMgrLifecycleStateT lcs)
{
    CryptoMgrGetSensorIdResponseParamsT params = {0};

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcs, sizeof(LifecycleMgrLifecycleStateT));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID)
                .withParameter("info", ERROR_SUCCESS);

    cryptoMgr.getSensorIdCalled = false;
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnGetSensorId(HOST_COMMAND_CODE_GET_SENSOR_ID, (CommandHandlerCommandParamsT)&params));
    CHECK_EQUAL(true, cryptoMgr.getSensorIdCalled);

    for (uint32_t i = 0; i < sizeof(params.uniqueId)/sizeof(uint32_t); i+= 1)
    {
        //printf("Comparing data[%d] = %d\n", i, params.uniqueId[i]);
        CHECK_EQUAL(params.uniqueId[i], 0);
    }
}

TEST(CryptoMgrGroup, TestGetSensorIdBasicLcsCmRmaDm)
{
    TestGetSensorIdBasicLcs(LIFECYCLE_MGR_LIFECYCLE_STATE_CM);
    TestGetSensorIdBasicLcs(LIFECYCLE_MGR_LIFECYCLE_STATE_DM);
    TestGetSensorIdBasicLcs(LIFECYCLE_MGR_LIFECYCLE_STATE_RMA);
}

TEST(CryptoMgrGroup, TestGetSensorIdBasicLcsSecure)
{
    CryptoMgrGetSensorIdResponseParamsT params = {0};
    CryptoMgrGetSensorIdResponseParamsT response = {1,2,3,4,5,6,7,8};
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));

    mock().expectOneCall("CC_BsvSocIDCompute")
                .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                .withOutputParameterReturning("hashResult", response.uniqueId, sizeof(response.uniqueId));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnGetSensorId(HOST_COMMAND_CODE_GET_SENSOR_ID, (CommandHandlerCommandParamsT)&params));

    for (uint32_t i = 0; i < sizeof(params.uniqueId)/sizeof(uint32_t); i+= 1)
    {
        //printf("Comparing data[%d] = %d\n", i, params.uniqueId[i]);
        CHECK_EQUAL(params.uniqueId[i], response.uniqueId[i]);
    }
}

TEST(CryptoMgrGroup, TestGetSensorIdBsvErrorLcsSecure)
{
    CryptoMgrGetSensorIdResponseParamsT params = {0};
    CryptoMgrGetSensorIdResponseParamsT response = {1,2,3,4,5,6,7,8};
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));

    mock().expectOneCall("CC_BsvSocIDCompute")
                .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                .withOutputParameterReturning("hashResult", response.uniqueId, sizeof(response.uniqueId))
                .andReturnValue(0x12000078);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID_ERROR)
                .withParameter("info", 0x1278);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, CryptoMgrOnGetSensorId(HOST_COMMAND_CODE_GET_SENSOR_ID, (CommandHandlerCommandParamsT)&params));

    for (uint32_t i = 0; i < sizeof(params.uniqueId)/sizeof(uint32_t); i+= 1)
    {
        //printf("Comparing data[%d] = %d\n", i, params.uniqueId[i]);
        CHECK_EQUAL(params.uniqueId[i], response.uniqueId[i]);
    }
}

TEST(CryptoMgrGroup, TestGetSensorIdLcsError)
{
    CryptoMgrGetSensorIdResponseParamsT params = {0};
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent))
                .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnGetSensorId(HOST_COMMAND_CODE_GET_SENSOR_ID, (CommandHandlerCommandParamsT)&params));
}

TEST(CryptoMgrGroup, TestAuthenticatePatchBasic)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    uint8_t outputData[assetDataSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
    size_t AssetDataLen = assetDataSize;
    uint8_t outputPackage[assetDataSize];
    size_t outputPackageLen = assetDataSize;

    Patch.assetSize = assetDataSize;

    mock().expectOneCall("CC_BsvIcvAssetProvisioningOpen")
                    .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                    .withParameter("assetId", assetId)
                    .withParameter("pAssetPkgBuff", (uint32_t *)&Patch)
                    .withParameter("assetPackageLen", assetDataSize + ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE)
                    .withOutputParameterReturning("pOutAssetData", outputData, 16)
                    .withOutputParameterReturning("pAssetDataLen", &AssetDataLen, sizeof(uint32_t));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAuthenticatePatchChunk(assetId, (uint32_t *)&Patch, &outputPackageLen, outputPackage));

    MEMCMP_EQUAL(outputData, outputPackage, assetDataSize);
}

TEST(CryptoMgrGroup, TestAuthenticatePatchError)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    uint8_t outputPackage[assetDataSize];
    size_t outputPackageLen = assetDataSize;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAuthenticatePatchChunk(assetId, (uint32_t *)&Patch, &outputPackageLen, NULL));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAuthenticatePatchChunk(assetId, (uint32_t *)&Patch, NULL, outputPackage));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAuthenticatePatchChunk(assetId, NULL, &outputPackageLen, outputPackage));


}

TEST(CryptoMgrGroup, TestAuthenticatePatchBsvError)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    uint8_t outputData[assetDataSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
    size_t AssetDataLen = assetDataSize;
    uint8_t outputPackage[assetDataSize];
    size_t outputPackageLen = assetDataSize;

    Patch.assetSize = assetDataSize;

    mock().expectOneCall("CC_BsvIcvAssetProvisioningOpen")
                    .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                    .withParameter("assetId", assetId)
                    .withParameter("pAssetPkgBuff", (uint32_t *)&Patch)
                    .withParameter("assetPackageLen", assetDataSize + ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE)
                    .withOutputParameterReturning("pOutAssetData", outputData, 16)
                    .withOutputParameterReturning("pAssetDataLen", &AssetDataLen, sizeof(uint32_t))
                    .andReturnValue(0x12000078);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH_ERROR)
                .withParameter("info", 0x1278);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_PATCH)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrAuthenticatePatchChunk(assetId, (uint32_t *)&Patch, &outputPackageLen, outputPackage));

    for (uint32_t i = 0; i < assetDataSize; i+= 1)
    {
        //printf("Comparing data[%d] = %d\n", i, Patch.encAsset[i]);
        CHECK_EQUAL(0, Patch.encAsset[i]);
    }
}

TEST(CryptoMgrGroup, TestAuthenticateAssetBasic)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    uint8_t outputData[assetDataSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
    size_t AssetDataLen = assetDataSize;

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    for (CryptoMgrRootOfTrustT rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_CM;
            rootOfTrust <= CRYPTO_MGR_ROOT_OF_TRUST_DM;
            rootOfTrust = (CryptoMgrRootOfTrustT)(rootOfTrust + 1))
    {
        Patch.assetSize = assetDataSize;

        mock().expectOneCall("mbedtls_util_asset_pkg_unpack")
                        .withParameter("keyType", CRYPTO_MGR_ROOT_OF_TRUST_CM == rootOfTrust? ASSET_PROV_KEY_TYPE_KPICV : ASSET_PROV_KEY_TYPE_KCP)
                        .withParameter("assetId", assetId)
                        .withParameter("pAssetPackage", (uint32_t *)&Patch)
                        .withParameter("assetPackageLen", assetDataSize + ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE)
                        .withOutputParameterReturning("pAssetData", outputData, 16)
                        .withOutputParameterReturning("pAssetDataLen", &AssetDataLen, sizeof(uint32_t));

        mock().expectOneCall("DiagSetCheckpointWithInfo")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET)
                    .withParameter("info", ERROR_SUCCESS);

        CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAuthenticateAsset(assetId, rootOfTrust, (uint32_t *)&Patch));

        for (uint32_t i = 0; i < assetDataSize; i+= 1)
        {
            //printf("Comparing data[%d] = %d\n", i, Patch.encAsset[i]);
            CHECK_EQUAL(outputData[i], Patch.encAsset[i]);
        }
    }
}

TEST(CryptoMgrGroup, TestAuthenticateAssetError)
{
    uint32_t assetId = 0x12345678;
    CryptoMgrRootOfTrustT rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_ASSET_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAuthenticateAsset(assetId, rootOfTrust, NULL));
}

TEST(CryptoMgrGroup, TestAuthenticateAssetKeyError)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    CryptoMgrRootOfTrustT rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    rootOfTrust = (CryptoMgrRootOfTrustT)(rootOfTrust + 1);
    Patch.assetSize = assetDataSize;

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrAuthenticateAsset(assetId, rootOfTrust, (uint32_t *)&Patch));

    for (uint32_t i = 0; i < assetDataSize; i+= 1)
    {
        //printf("Comparing data[%d] = %d\n", i, Patch.encAsset[i]);
        CHECK_EQUAL(0, Patch.encAsset[i]);
    }
}

TEST(CryptoMgrGroup, TestAuthenticateAssetBsvError)
{
    uint32_t assetId = 0x12345678;
    CCBsvAssetProv_t Patch = {};
    const uint32_t assetDataSize = 16;
    uint8_t outputData[assetDataSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
    size_t AssetDataLen = assetDataSize;

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    CryptoMgrRootOfTrustT rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_CM;
    Patch.assetSize = assetDataSize;

    mock().expectOneCall("mbedtls_util_asset_pkg_unpack")
                    .withParameter("keyType", ASSET_PROV_KEY_TYPE_KPICV)
                    .withParameter("assetId", assetId)
                    .withParameter("pAssetPackage", (uint32_t *)&Patch)
                    .withParameter("assetPackageLen", assetDataSize + ASSET_ADATA_SIZE + ASSET_NONCE_SIZE + ASSET_TAG_SIZE)
                    .withOutputParameterReturning("pAssetData", outputData, 16)
                    .withOutputParameterReturning("pAssetDataLen", &AssetDataLen, sizeof(uint32_t))
                    .andReturnValue((CCError_t)CC_UTIL_ILLEGAL_PARAMS_ERROR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET_ERROR)
                .withParameter("info", 0x8006U);


    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AUTHENTICATE_ASSET)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrAuthenticateAsset(assetId, rootOfTrust, (uint32_t *)&Patch));


//    for (uint32_t i = 0; i < assetDataSize; i+= 1)
//    {
//        //printf("Comparing data[%d] = %d\n", i, Patch.encAsset[i]);
//        CHECK_EQUAL(0, Patch.encAsset[i]);
//    }
}

TEST(CryptoMgrGroup, TestEntryPhaseBasic)
{
    CCRndWorkBuff_t mallocBuffer;

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_BOOT));
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE));
    CHECK_EQUAL(CRYPTO_MGR_UNINIT, cryptoMgr.status);
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_DEBUG));
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_PATCH));
    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_NOENT)
        .ignoreOtherParameters();
    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();
    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t))
        .andReturnValue(&mallocBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", &mallocBuffer);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CC_LibInit");
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));
    CHECK_EQUAL(CRYPTO_MGR_CONFIGURED, cryptoMgr.status);
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SESSION));
    mock().expectOneCall("CC_LibFini");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));
    CHECK_EQUAL(CRYPTO_MGR_UNINIT, cryptoMgr.status);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_NUM_PHASES));
}

TEST(CryptoMgrGroup, TestConfigurationPhaseBasic)
{
    CCRndWorkBuff_t mallocBuffer;

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .ignoreOtherParameters();

    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t))
        .andReturnValue(&mallocBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", &mallocBuffer);

    mock().expectOneCall("CC_LibInit");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));
}

TEST(CryptoMgrGroup, TestConfigurationPhaseError)
{
    CCRndWorkBuff_t mallocBuffer;

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_IO)
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_IO);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_ON_PHASE_ENTRY_CONFIGURATION)
        .withParameter("infoParam", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_NOENT)
        .ignoreOtherParameters();
    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t))
        .andReturnValue(&mallocBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", &mallocBuffer);

    mock().expectOneCall("CC_LibInit")
        .andReturnValue(CC_LIB_RET_EINVAL_CTX_PTR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION_ERROR)
                .withParameter("info", CC_LIB_RET_EINVAL_CTX_PTR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_SYSERR);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_ON_PHASE_ENTRY_CONFIGURATION)
        .withParameter("infoParam", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_NOENT)
        .ignoreOtherParameters();
    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_NOSPC);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_ON_PHASE_ENTRY_CONFIGURATION)
        .withParameter("infoParam", ERROR_NOSPC);

    CHECK_EQUAL(ERROR_NOSPC, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

}

TEST(CryptoMgrGroup, TestShutdownPhaseBasic)
{
    CCRndWorkBuff_t mallocBuffer;

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_NOENT)
        .ignoreOtherParameters();
    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t))
        .andReturnValue(&mallocBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", &mallocBuffer);

    mock().expectOneCall("CC_LibInit");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));


    mock().expectOneCall("CC_LibFini");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));
}

TEST(CryptoMgrGroup, TestShutdownPhaseError)
{
    CCRndWorkBuff_t mallocBuffer;

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));

    mock().expectOneCall("AssetMgrRetrieveCC312TrngChar")
        .andReturnValue(ERROR_NOENT)
        .ignoreOtherParameters();
    mock().expectOneCall("CC_PalTrngParamSet")
        .ignoreOtherParameters();

    mock().expectOneCall("unittest_calloc")
        .withParameter("nitems", 1)
        .withParameter("size", sizeof(CCRndWorkBuff_t))
        .andReturnValue(&mallocBuffer);

    mock().expectOneCall("unittest_free")
        .withParameter("ptr", &mallocBuffer);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION)
                .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CC_LibInit");
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));


    mock().expectOneCall("CC_LibFini")
       .andReturnValue(CC_LIB_RET_EINVAL_CTX_PTR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN_ERROR)
                .withParameter("info", CC_LIB_RET_EINVAL_CTX_PTR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));
}

TEST(CryptoMgrGroup, TestRSADecryptBasic)
{
    size_t const keySizeBytes = 1024;
    uint8_t privateExponent[keySizeBytes] = {};
    uint8_t modulus[keySizeBytes] = {};
    uint8_t encryptedMsg[keySizeBytes] = {};
    size_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t decryptedMsg[keySizeBytes];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_rsa_init");
    mock().expectOneCall("mbedtls_rsa_import_raw")
        .withParameter("N_len", keySizeBytes)
        .withParameter("D_len", keySizeBytes);
    mock().expectOneCall("mbedtls_rsa_complete");
    mock().expectOneCall("mbedtls_rsa_pkcs1_decrypt")
        .withParameter("output_max_len", keySizeBytes)
        .withOutputParameterReturning("olen", &decryptedMsgSizeBytes, sizeof(size_t));
    mock().expectOneCall("mbedtls_rsa_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrRsaDecrypt(keySizeBytes,
                                                   privateExponent,
                                                   modulus,
                                                   encryptedMsg,
                                                   &decryptedMsgSizeBytes,
                                                   decryptedMsg));
}

TEST(CryptoMgrGroup, TestRSADecryptError)
{
    size_t const keySizeBytes = 1024;
    uint8_t privateExponent[keySizeBytes] = {};
    uint8_t modulus[keySizeBytes] = {};
    uint8_t encryptedMsg[keySizeBytes] = {};
    size_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t decryptedMsg[keySizeBytes];

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrRsaDecrypt(keySizeBytes, NULL, modulus, encryptedMsg,
                                                  &decryptedMsgSizeBytes, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, NULL, encryptedMsg,
                                                  &decryptedMsgSizeBytes, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, NULL,
                                                  &decryptedMsgSizeBytes, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, encryptedMsg,
                                                  NULL, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, encryptedMsg,
                                                  &decryptedMsgSizeBytes, NULL));
}

TEST(CryptoMgrGroup, TestRSADecryptMbedtlsError1)
{
    size_t const keySizeBytes = 1024;
    uint8_t privateExponent[keySizeBytes] = {};
    uint8_t modulus[keySizeBytes] = {};
    uint8_t encryptedMsg[keySizeBytes] = {};
    size_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t decryptedMsg[keySizeBytes];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_rsa_init");
    mock().expectOneCall("mbedtls_rsa_import_raw")
        .andReturnValue(MBEDTLS_ERR_RSA_BAD_INPUT_DATA)
        .withParameter("N_len", keySizeBytes)
        .withParameter("D_len", keySizeBytes);
    mock().expectOneCall("mbedtls_rsa_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT_ERROR)
                .withParameter("info", -(MBEDTLS_ERR_RSA_BAD_INPUT_DATA));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, encryptedMsg,
                                                  &decryptedMsgSizeBytes, decryptedMsg));
}

TEST(CryptoMgrGroup, TestRSADecryptMbedtlsError2)
{
    size_t const keySizeBytes = 1024;
    uint8_t privateExponent[keySizeBytes] = {};
    uint8_t modulus[keySizeBytes] = {};
    uint8_t encryptedMsg[keySizeBytes] = {};
    size_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t decryptedMsg[keySizeBytes];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_rsa_init");
    mock().expectOneCall("mbedtls_rsa_import_raw")
        .withParameter("N_len", keySizeBytes)
        .withParameter("D_len", keySizeBytes);
    mock().expectOneCall("mbedtls_rsa_complete")
        .andReturnValue(MBEDTLS_ERR_RSA_BAD_INPUT_DATA);
    mock().expectOneCall("mbedtls_rsa_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT_ERROR)
                .withParameter("info", -(MBEDTLS_ERR_RSA_BAD_INPUT_DATA));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, encryptedMsg,
                                                  &decryptedMsgSizeBytes, decryptedMsg));
}

TEST(CryptoMgrGroup, TestRSADecryptMbedtlsError3)
{
    size_t const keySizeBytes = 1024;
    uint8_t privateExponent[keySizeBytes] = {};
    uint8_t modulus[keySizeBytes] = {};
    uint8_t encryptedMsg[keySizeBytes] = {};
    size_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t decryptedMsg[keySizeBytes];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_rsa_init");
    mock().expectOneCall("mbedtls_rsa_import_raw")
        .withParameter("N_len", keySizeBytes)
        .withParameter("D_len", keySizeBytes);
    mock().expectOneCall("mbedtls_rsa_complete");
    mock().expectOneCall("mbedtls_rsa_pkcs1_decrypt")
        .withParameter("output_max_len", keySizeBytes)
        .withOutputParameterReturning("olen", &decryptedMsgSizeBytes, sizeof(size_t))
        .andReturnValue(MBEDTLS_ERR_RSA_BAD_INPUT_DATA);
    mock().expectOneCall("mbedtls_rsa_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT_ERROR)
                .withParameter("info", -(MBEDTLS_ERR_RSA_BAD_INPUT_DATA));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RSA_DECRYPT)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrRsaDecrypt(keySizeBytes, privateExponent, modulus, encryptedMsg,
                                                  &decryptedMsgSizeBytes, decryptedMsg));
}

TEST(CryptoMgrGroup, TestAeadDecryptBasic)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    uint32_t keySizeBytes = 16;
    uint8_t key[16] = {};
    uint8_t ivNonce[12] = {};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32];
    uint8_t decryptedMsg[16];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_cipher_init");
    mock().expectOneCall("mbedtls_cipher_info_from_values")
        .withParameter("key_bitlen", keySizeBytes * 8U)
        .withParameter("mode", CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM == cipherMode?  MBEDTLS_MODE_CCM : MBEDTLS_MODE_GCM);

    mock().expectOneCall("mbedtls_cipher_setup");
    mock().expectOneCall("mbedtls_cipher_setkey")
        .withParameter("key_bitlen", keySizeBytes * 8);

    mock().expectOneCall("mbedtls_cipher_auth_decrypt")
        .withParameter("ad_len", associatedDataSizeBytes)
        .withParameter("ilen", encryptedMsgSizeBytes)
        .withOutputParameterReturning("olen", &encryptedMsgSizeBytes, sizeof(size_t));

    mock().expectOneCall("mbedtls_cipher_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAeadDecrypt(
        cipherMode,
        keySizeBytes,
        key,
        ivNonce,
        associatedDataSizeBytes,
        associatedData,
        encryptedMsgSizeBytes,
        encryptedMsgAndTag,
        decryptedMsg));
}

TEST(CryptoMgrGroup, TestAeadDecryptError)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    uint32_t keySizeBytes = 16;
    uint8_t key[16] = {};
    uint8_t ivNonce[12] = {};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32];
    uint8_t decryptedMsg[16];

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAeadDecrypt( cipherMode, keySizeBytes, NULL, ivNonce, associatedDataSizeBytes, associatedData,
        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAeadDecrypt( cipherMode, keySizeBytes, key, NULL, associatedDataSizeBytes, associatedData,
        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAeadDecrypt( cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, NULL,
        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAeadDecrypt( cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, associatedData,
        encryptedMsgSizeBytes, NULL, decryptedMsg));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrAeadDecrypt( cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, associatedData,
        encryptedMsgSizeBytes, encryptedMsgAndTag, NULL));

}

TEST(CryptoMgrGroup, TestAeadDecryptMbedtlsError1)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    uint32_t keySizeBytes = 16;
    uint8_t key[16] = {};
    uint8_t ivNonce[12] = {};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32];
    uint8_t decryptedMsg[16];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_cipher_init");
    mock().expectOneCall("mbedtls_cipher_info_from_values")
        .withParameter("key_bitlen", keySizeBytes * 8U)
        .withParameter("mode", CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM == cipherMode?  MBEDTLS_MODE_CCM : MBEDTLS_MODE_GCM)
        .andReturnValue((const void *)NULL);

    mock().expectOneCall("mbedtls_cipher_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT_ERROR)
                .withParameter("info", -(MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrAeadDecrypt(cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, associatedData,
                        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));
}

TEST(CryptoMgrGroup, TestAeadDecryptMbedtlsError2)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    uint32_t keySizeBytes = 16;
    uint8_t key[16] = {};
    uint8_t ivNonce[12] = {};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32];
    uint8_t decryptedMsg[16];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;


    mock().expectOneCall("mbedtls_cipher_init");
    mock().expectOneCall("mbedtls_cipher_info_from_values")
        .withParameter("key_bitlen", keySizeBytes * 8U)
        .withParameter("mode", CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM == cipherMode?  MBEDTLS_MODE_CCM : MBEDTLS_MODE_GCM);

    mock().expectOneCall("mbedtls_cipher_setup");
    mock().expectOneCall("mbedtls_cipher_setkey")
        .withParameter("key_bitlen", keySizeBytes * 8);

    mock().expectOneCall("mbedtls_cipher_auth_decrypt")
        .withParameter("ad_len", associatedDataSizeBytes)
        .withParameter("ilen", encryptedMsgSizeBytes)
        .withOutputParameterReturning("olen", &encryptedMsgSizeBytes, sizeof(size_t))
        .andReturnValue(MBEDTLS_ERR_CIPHER_AUTH_FAILED);

    mock().expectOneCall("mbedtls_cipher_free");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT_ERROR)
                .withParameter("info", -(MBEDTLS_ERR_CIPHER_AUTH_FAILED));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, CryptoMgrAeadDecrypt(cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, associatedData,
                        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));

}

TEST(CryptoMgrGroup, TestAeadDecryptMbedtlsWrongCipherMode)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_RESERVED;
    uint32_t keySizeBytes = 16;
    uint8_t key[16] = {};
    uint8_t ivNonce[12] = {};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32];
    uint8_t decryptedMsg[16];

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_AEAD_DECRYPT)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, CryptoMgrAeadDecrypt(cipherMode, keySizeBytes, key, ivNonce, associatedDataSizeBytes, associatedData,
                        encryptedMsgSizeBytes, encryptedMsgAndTag, decryptedMsg));

}

TEST(CryptoMgrGroup, TestDeriveKeysBasic)
{
    uint8_t  masterSecret[16] = {};
    uint8_t  hostSalt[16] = {};
    uint8_t  sensorSalt[16] = {};
    uint8_t  authSessionKey[16];
    uint8_t  controlChannelKey[16];
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CCHashResult_t sensorId = {};

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcs, sizeof(LifecycleMgrLifecycleStateT));

    mock().expectOneCall("CC_BsvSocIDCompute")
                .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                .withOutputParameterReturning("hashResult", sensorId, sizeof(sensorId));

    mock().expectOneCall("mbedtls_hkdf_key_derivation")
        .withParameter("SaltLen", sizeof(hostSalt) + sizeof(sensorSalt))
        .withParameter("IkmLen", sizeof(masterSecret))
        .withParameter("InfoLen", 8+4+32)
        .withParameter("OkmLen", sizeof(authSessionKey) + sizeof(controlChannelKey));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrDeriveKeys(
        sizeof(masterSecret),
        masterSecret,
        sizeof(hostSalt),
        hostSalt,
        sizeof(sensorSalt),
        sensorSalt,
        sizeof(authSessionKey),
        authSessionKey,
        sizeof(controlChannelKey),
        controlChannelKey));
}

TEST(CryptoMgrGroup, TestDeriveKeysError)
{
    uint8_t  masterSecret[16] = {};
    uint8_t  hostSalt[16] = {};
    uint8_t  sensorSalt[16] = {};
    uint8_t  authSessionKey[16];
    uint8_t  controlChannelKey[16];

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  NULL, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), NULL, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), NULL,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), NULL, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), NULL));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys(33,  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, 17, hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, 17, sensorSalt,
        sizeof(authSessionKey), authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        33, authSessionKey, sizeof(controlChannelKey), controlChannelKey));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrDeriveKeys( sizeof(masterSecret),  masterSecret, sizeof(hostSalt), hostSalt, sizeof(sensorSalt), sensorSalt,
        sizeof(authSessionKey), authSessionKey, 33, controlChannelKey));

}

TEST(CryptoMgrGroup, TestDeriveKeysMbedtlsError1)
{
    uint8_t  masterSecret[16] = {};
    uint8_t  hostSalt[16] = {};
    uint8_t  sensorSalt[16] = {};
    uint8_t  authSessionKey[16];
    uint8_t  controlChannelKey[16];
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CCHashResult_t sensorId = {};

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcs, sizeof(LifecycleMgrLifecycleStateT));

    mock().expectOneCall("CC_BsvSocIDCompute")
                .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
                .withOutputParameterReturning("hashResult", sensorId, sizeof(sensorId));

    mock().expectOneCall("mbedtls_hkdf_key_derivation")
        .andReturnValue(0xAA000055)
        .withParameter("SaltLen", sizeof(hostSalt) + sizeof(sensorSalt))
        .withParameter("IkmLen", sizeof(masterSecret))
        .withParameter("InfoLen", 8+4+32)
        .withParameter("OkmLen", sizeof(authSessionKey) + sizeof(controlChannelKey));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION_ERROR)
                .withParameter("info", 0xAA55);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, CryptoMgrDeriveKeys(
        sizeof(masterSecret),
        masterSecret,
        sizeof(hostSalt),
        hostSalt,
        sizeof(sensorSalt),
        sensorSalt,
        sizeof(authSessionKey),
        authSessionKey,
        sizeof(controlChannelKey),
        controlChannelKey));
}

TEST(CryptoMgrGroup, TestDeriveKeysMbedtlsError2)
{
    uint8_t  masterSecret[16] = {};
    uint8_t  hostSalt[16] = {};
    uint8_t  sensorSalt[16] = {};
    uint8_t  authSessionKey[16];
    uint8_t  controlChannelKey[16];
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcs, sizeof(LifecycleMgrLifecycleStateT))
                .andReturnValue(ERROR_ACCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_KEY_DERIVATION)
                .withParameter("info", ERROR_ACCESS);

    CHECK_EQUAL(ERROR_ACCESS, CryptoMgrDeriveKeys(
        sizeof(masterSecret),
        masterSecret,
        sizeof(hostSalt),
        hostSalt,
        sizeof(sensorSalt),
        sensorSalt,
        sizeof(authSessionKey),
        authSessionKey,
        sizeof(controlChannelKey),
        controlChannelKey));
}

TEST(CryptoMgrGroup, TestGenRandomNumberBasic)
{
    uint32_t const randomNumberSizeBytes = 16;
    uint8_t randomNumber[randomNumberSizeBytes] = {};

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_ctr_drbg_random")
        .withParameter("output_len", randomNumberSizeBytes);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));
}

TEST(CryptoMgrGroup, TestGenRandomNumberError)
{
    uint32_t const randomNumberSizeBytes = 16;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RANDOM_GENERATOR_BAD_ARGS)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrGenRandomNumber(randomNumberSizeBytes, NULL));
}

TEST(CryptoMgrGroup, TestGenRandomNumberMbedtlsError)
{
    uint32_t const randomNumberSizeBytes = 16;
    uint8_t randomNumber[randomNumberSizeBytes] = {};

    cryptoMgr.status = CRYPTO_MGR_CONFIGURED;

    mock().expectOneCall("mbedtls_ctr_drbg_random")
        .withParameter("output_len", randomNumberSizeBytes)
        .andReturnValue(-0xaa55);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR_ERROR)
                .withParameter("info", 0xaa55);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_RANDOM_GENERATOR)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));
}

TEST(CryptoMgrGroup, TestIncrementNumberBasic)
{
    const uint32_t dataLen = 4;
    uint8_t data[dataLen] = {0,0,0,0};
    uint8_t expected[dataLen] = {0,0,0,1};  // number is big-endian, least-significant byte incremented

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);

    expected[3] = 2;
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);

    memset(data, 0xFF, dataLen);
    memset(expected, 0, dataLen);
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);
}

TEST(CryptoMgrGroup, TestIncrementNumberError)
{
    const uint32_t dataLen = 4;

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrIncrementNumber(dataLen, NULL));
}

TEST(CryptoMgrGroup, TestGetSymmetricKeySizeBytes)
{
    CryptoMgrSymmetricKeyTypeT keyType;
    uint32_t sizeBytes;

    // Check invalid key type is rejected
    keyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
    ErrorT status = CryptoMgrGetSymmetricKeySizeBytes(keyType, &sizeBytes);
    LONGS_EQUAL(ERROR_INVAL, status);

    // Check invalid ptr raises fatal error
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_KEY_SIZE_PARAM)
        .withParameter("infoParam", 0U);

    keyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    status = CryptoMgrGetSymmetricKeySizeBytes(keyType, NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);

    // Check all valid key types
    keyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
    status = CryptoMgrGetSymmetricKeySizeBytes(keyType, &sizeBytes);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(16U, sizeBytes);

    keyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_192;
    status = CryptoMgrGetSymmetricKeySizeBytes(keyType, &sizeBytes);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(24U, sizeBytes);

    keyType = CRYPTO_MGR_SYMMETRIC_KEY_AES_256;
    status = CryptoMgrGetSymmetricKeySizeBytes(keyType, &sizeBytes);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(32U, sizeBytes);
}

TEST(CryptoMgrGroup, TestGetSensorIdCalled)
{
    bool isCalled;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_IS_CALLED_PARAM)
        .withParameter("infoParam", 0U);

    ErrorT status = CryptoMgrGetSensorIdCalled(NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);

    status = CryptoMgrGetSensorIdCalled(&isCalled);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_FALSE(isCalled);

    cryptoMgr.getSensorIdCalled = true;
    status = CryptoMgrGetSensorIdCalled(&isCalled);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(isCalled);
}

TEST(CryptoMgrGroup, TestOnSelfTestError)
{
    uint32_t params[10];

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_SELF_TEST_BAD_ARGS)
        .withParameter("infoParam", 0U);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnSelfTest(HOST_COMMAND_CODE_GET_SENSOR_ID, NULL));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_SELF_TEST_BAD_ARGS)
        .withParameter("infoParam", 0U);

    CHECK_EQUAL(ERROR_SYSERR, CryptoMgrOnSelfTest(HOST_COMMAND_CODE_GET_SENSOR_ID, params));

    CHECK_EQUAL(ERROR_ACCESS, CryptoMgrOnSelfTest(HOST_COMMAND_CODE_SELF_TEST, params));
}

TEST(CryptoMgrGroup, TestOnPalAbortBasic)
{
    mock().expectOneCall("DiagSetCheckpoint")
                .withParameter("checkpoint", DIAG_CHECKPOINT_CRYPTO_MGR_ON_PAL_ABORT);

    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR)
        .withParameter("context", 0)
        .withParameter("infoParam", 0);

    CryptoMgrOnPalAbort();
}




