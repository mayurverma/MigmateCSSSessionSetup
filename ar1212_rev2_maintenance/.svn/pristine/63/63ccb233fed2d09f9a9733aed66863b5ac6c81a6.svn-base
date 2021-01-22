//***********************************************************************************
/// \file
///
/// Nvm Manager unit test
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
#include "Common.h"
#include "AR0820.h"
#include "Diag.h"
#include "OtpmDrv.h"

#include "NvmMgr.h"
#include "NvmMgrInternal.h"

}

// Mocks
extern "C"
{

}

TEST_GROUP(NvmMgrInit)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        (void)memset(&nvmMgr, 0, sizeof(nvmMgr));
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(NvmMgrApiGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT error;
        SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;
        NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

        mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
            .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

        error = NvmMgrOnPhaseEntry(phase);
        CHECK_EQUAL(ERROR_SUCCESS, error);
        CHECK_EQUAL(false, nvmMgr.isInitialized);

        phase = SYSTEM_MGR_PHASE_DEBUG;

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
        mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", 44U)
            .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
            .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

        error = NvmMgrOnPhaseEntry(phase);
        CHECK_EQUAL(ERROR_SUCCESS, error);
        CHECK_EQUAL(true, nvmMgr.isInitialized);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(NvmMgrSupport)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT error;
        SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;
        NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

        mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
            .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

        error = NvmMgrOnPhaseEntry(phase);
        CHECK_EQUAL(ERROR_SUCCESS, error);
        CHECK_EQUAL(false, nvmMgr.isInitialized);

        phase = SYSTEM_MGR_PHASE_DEBUG;

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
        mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", 44U)
            .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
            .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

        error = NvmMgrOnPhaseEntry(phase);
        CHECK_EQUAL(ERROR_SUCCESS, error);
        CHECK_EQUAL(true, nvmMgr.isInitialized);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(NvmMgrInit, TestOnPhaseEntryPhaseInvalid)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_NUM_PHASES;

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseUnsupported)
{
    ErrorT error;
    SystemMgrPhaseT phase[] = {SYSTEM_MGR_PHASE_BOOT, SYSTEM_MGR_PHASE_PATCH,
                                SYSTEM_MGR_PHASE_CONFIGURE, SYSTEM_MGR_PHASE_SESSION, SYSTEM_MGR_PHASE_SHUTDOWN,
                                SYSTEM_MGR_PHASE_BOOT};

    for (uint32_t i = 0U; i < sizeof(phase)/sizeof(phase[0]); i++)
    {
        error = NvmMgrOnPhaseEntry(phase[i]);
        CHECK_EQUAL(ERROR_SUCCESS, error);
        CHECK_EQUAL(false, nvmMgr.isInitialized);
    }
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseInitialize)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
        .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseInitializeDiagDumpFail)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;

    mock().expectNCalls(1, "OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump)
        .andReturnValue(ERROR_NOSPC);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_INIT_FAILED)
        .withParameter("infoParam", ERROR_NOSPC);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_NOSPC, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseInitializeClockNotifierFail)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;

    mock().expectNCalls(1, "OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
        .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange)
        .andReturnValue(ERROR_NOSPC);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_INIT_FAILED)
        .withParameter("infoParam", ERROR_NOSPC);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_NOSPC, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseDebugOtpmFull)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;
    NvmMgrRecordHeaderT  full  = {ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
        .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);

    phase = SYSTEM_MGR_PHASE_DEBUG;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    full.assetLengthLongWords = (uint16_t)(OtpmDrvGetArraySizeLongWords() -
        (44L + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t)));

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &full, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_SUCCESS);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseDebugOtpmInvalidRecordLength)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;
    NvmMgrRecordHeaderT  full  = {ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
        .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);

    phase = SYSTEM_MGR_PHASE_DEBUG;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    uint32_t otpmArrayLengthLongWords = OtpmDrvGetArraySizeLongWords();

    full.assetLengthLongWords = (uint16_t)(otpmArrayLengthLongWords -
        (44L + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t)) + 1L);          // Index past first empty location

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &full, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_RECORD_LENGTH)
        .withParameter("infoParam", otpmArrayLengthLongWords + 1U );

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrInit, TestOnPhaseEntryPhaseDebugOtpmInvalid)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;
    NvmMgrRecordHeaderT  invalidLength  = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))NvmMgrOnDiagFatalErrorDump);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
        .withFunctionPointerParameter("notifier", (void (*)(void))NvmMgrOnClockChange);

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);

    phase = SYSTEM_MGR_PHASE_DEBUG;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    invalidLength.assetLengthLongWords = uint16_t(OtpmDrvGetArraySizeLongWords() - 44L);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &invalidLength, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY)
        .withParameter("infoParam", invalidLength.assetLengthLongWords );

    error = NvmMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}


TEST(NvmMgrApiGroup, TestNvmMgrFindFirstParameterErrors)
{
    ErrorT error;
    NvmMgrFindContextT context;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_CONTEXT)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindFirst(NULL, assetType, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_ADDRESS)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindFirst(&context, assetType, NULL, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_LENGTH)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);

}

TEST(NvmMgrApiGroup, TestNvmMgrFindFirstParameterNotInitialised)
{
    ErrorT error;
    NvmMgrFindContextT context;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;

    nvmMgr.isInitialized = false;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_NOT_INITIALIZED)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, nvmMgr.isInitialized);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindFirstEmptyOtpm)
{
    ErrorT error;
    NvmMgrFindContextT context;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data = {0U, 0U};

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_ENTRY);
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);

    CHECK_EQUAL(ERROR_NOENT, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindFirstOtpmFailure)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data0 = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 12U};
    NvmMgrRecordHeaderT  data1 = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, 6U};

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_ENTRY);
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &data0, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U + data0.assetLengthLongWords + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &data1, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_INVAL);

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);

    CHECK_EQUAL(ERROR_INVAL, error);
    CHECK_EQUAL(0U, context.context[0]);
    CHECK_EQUAL(0U, context.context[1]);
    CHECK_EQUAL(0U, context.context[2]);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindFirstOtpmFull)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 19U};
    NvmMgrAddressT expectedReadAddress;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_ENTRY);
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    expectedReadAddress = 44U;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", expectedReadAddress)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    expectedReadAddress += data.assetLengthLongWords + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", expectedReadAddress)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    expectedReadAddress += data.assetLengthLongWords + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", expectedReadAddress)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_RANGE);

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);

    CHECK_EQUAL(ERROR_NOENT, error);
    CHECK_EQUAL(0U, context.context[0]);
    CHECK_EQUAL(0U, context.context[1]);
    CHECK_EQUAL(0U, context.context[2]);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindFirstSuccess)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data0 = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 12U};
    NvmMgrRecordHeaderT  data1 = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, 6U};

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_ENTRY);
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &data0, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U + data0.assetLengthLongWords + sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &data1, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_FIRST_SUCCESS);

    error = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);

    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(NVM_MGR_MAGIC, context.context[0]);
    CHECK_EQUAL(44U + data0.assetLengthLongWords + 2*sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t), context.context[1]);
    CHECK_EQUAL(6, context.context[2]);
    CHECK_EQUAL(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, context.context[3]);
    CHECK_EQUAL(44U + data0.assetLengthLongWords + 2*sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t), assetAddress);
    CHECK_EQUAL(6, assetLengthLongWords);

}

TEST(NvmMgrApiGroup, TestNvmMgrFindNextParameterErrors)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;

    context.context[0] = NVM_MGR_MAGIC;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_CONTEXT)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindNext(NULL, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_ADDRESS)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindNext(&context, NULL, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_LENGTH)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindNext(&context, &assetAddress, NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_NOT_INITIALIZED)
        .withParameter("infoParam", 0U);

    nvmMgr.isInitialized = false;
    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_MAGIC)
        .withParameter("infoParam", 0U);

    nvmMgr.isInitialized = true;
    context.context[0] = 0U;                    // Clear Magic!!
    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

}

TEST(NvmMgrApiGroup, TestNvmMgrFindNextEmptyOtpm)
{
    ErrorT error;
    NvmMgrFindContextT context;
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data = {0U, 0U};

    context.context[0] = NVM_MGR_MAGIC;                             // magic number to identify object
    context.context[1] = 48U;                                       // current search address (asset start)
    context.context[2] = 6U;                                        // Length of asset
    context.context[3] = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;   // asset type identifier

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 54U)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);

    CHECK_EQUAL(ERROR_NOENT, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindNextOtpmFailure)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data = {0U, 0U};

    context.context[0] = NVM_MGR_MAGIC;                             // magic number to identify object
    context.context[1] = 48U;                                       // current search address (asset start)
    context.context[2] = 6U;                                        // Length of asset
    context.context[3] = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;   // asset type identifier

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 54U)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_INVAL);

    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_INVAL, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindNextAccessBeyondArray)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    NvmMgrAddressT assetAddress = 0xAAAAU;
    uint32_t assetLengthLongWords = 0xBBBBU;

    // OTPM array is 2016 (2048 - 32) LongWords long
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    context.context[0] = NVM_MGR_MAGIC;                             // magic number to identify object
    context.context[1] = OtpmDrvGetArraySizeLongWords() - 6U;          // current search address (asset start)
    context.context[2] = 6U;                                        // Length of asset
    context.context[3] = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;   // asset type identifier

    printf("%d\n", context.context[1]);
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_NOENT, error);
    CHECK_EQUAL(NVM_MGR_MAGIC, context.context[0]);
    CHECK_EQUAL(2010U, context.context[1]);
    CHECK_EQUAL(6U, context.context[2]);
    CHECK_EQUAL(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, context.context[3]);
    CHECK_EQUAL(0xAAAAU, assetAddress);
    CHECK_EQUAL(0xBBBBU, assetLengthLongWords);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindNextSuccess)
{
    ErrorT error;
    NvmMgrFindContextT context = {0U};
    NvmMgrAddressT assetAddress;
    uint32_t assetLengthLongWords;
    NvmMgrRecordHeaderT  data = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, 2U};

    context.context[0] = NVM_MGR_MAGIC;                             // magic number to identify object
    context.context[1] = 48U;                                       // current search address (asset start)
    context.context[2] = 6U;                                        // Length of asset
    context.context[3] = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;   // asset type identifier

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 54U)
        .withOutputParameterReturning("buffer", &data, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_FIND_NEXT_SUCCESS);

    error = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(NVM_MGR_MAGIC, context.context[0]);
    CHECK_EQUAL(55U, context.context[1]);
    CHECK_EQUAL(2U, context.context[2]);
    CHECK_EQUAL(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, context.context[3]);
    CHECK_EQUAL(55U, assetAddress);
    CHECK_EQUAL(2U, assetLengthLongWords);

}

TEST(NvmMgrApiGroup, TestNvmMgrFindAssetParameterFailures)
{
    ErrorT error;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT startAddressLongWords = 0U;
    NvmMgrRecordHeaderT record = {0U, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_RECORD)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS)
        .withParameter("infoParam", 0U);

    error = NvmMgrFindAsset(assetType, NULL, &record);
    CHECK_EQUAL(ERROR_SYSERR, error);

    startAddressLongWords = 0xFFFF;
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS_VALUE)
        .withParameter("infoParam", startAddressLongWords);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, &record);
    CHECK_EQUAL(ERROR_SYSERR, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindAssetNotPresent)
{
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    ErrorT error;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT startAddressLongWords = OtpmDrvGetUserSpaceAddress();
    NvmMgrRecordHeaderT record = {0U, 0U};
    uint32_t arraySizeLongWords = OtpmDrvGetArraySizeLongWords();
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT otmpReadAddress;
    NvmMgrRecordHeaderT otpmValue = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 0U};
    NvmMgrRecordHeaderT otpmEmpty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

    // User array space is (arraySizeLongWords - startAddressLongWords) for the test simulate
    // array containing 4 long assets
    uint16_t headerPluAssetSizeLongWords = (uint16_t)((arraySizeLongWords - (uint32_t)startAddressLongWords)/4U);
    otpmValue.assetLengthLongWords = (uint16_t)((uint32_t)headerPluAssetSizeLongWords - recordHeaderLengthLongWords);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    otmpReadAddress = 44U;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmEmpty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, &record);
    CHECK_EQUAL(ERROR_NOENT, error);
    CHECK_EQUAL(otmpReadAddress, nvmMgr.blankAddressLongWords);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindAssetLengthNotZeroAssetIdEmpty)
{
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    ErrorT error;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT startAddressLongWords = OtpmDrvGetUserSpaceAddress();
    NvmMgrRecordHeaderT record = {0U, 0U};
    uint32_t arraySizeLongWords = OtpmDrvGetArraySizeLongWords();
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT otmpReadAddress;
    NvmMgrRecordHeaderT otpmValue = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 0U};
    NvmMgrRecordHeaderT otpmEmpty = {ASSET_MGR_ASSET_TYPE_EMPTY, 20U};

    // User array space is (arraySizeLongWords - startAddressLongWords) for the test simulate
    // array containing 4 long assets
    uint16_t headerPluAssetSizeLongWords = (uint16_t)((arraySizeLongWords - (uint32_t)startAddressLongWords)/4U);
    otpmValue.assetLengthLongWords = (uint16_t)((uint32_t)headerPluAssetSizeLongWords - recordHeaderLengthLongWords);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    otmpReadAddress = 44U;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmEmpty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_LENGTH_NOT_ZERO_IN_EMPTY)
        .withParameter("infoParam", 20U);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, &record);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(startAddressLongWords, nvmMgr.blankAddressLongWords);
}

TEST(NvmMgrApiGroup, TestNvmMgrFindAssetOtpmFull)
{
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    ErrorT error;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT startAddressLongWords = OtpmDrvGetUserSpaceAddress();
    NvmMgrRecordHeaderT record = {0U, 0U};
    uint32_t arraySizeLongWords = OtpmDrvGetArraySizeLongWords();
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT otmpReadAddress;
    NvmMgrRecordHeaderT otpmValue = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 0U};

    // User array space is (arraySizeLongWords - startAddressLongWords) for the test simulate
    // array containing 4 long assets
    uint16_t headerPluAssetSizeLongWords = (uint16_t)((arraySizeLongWords - (uint32_t)startAddressLongWords)/4U);
    otpmValue.assetLengthLongWords = (uint16_t)((uint32_t)headerPluAssetSizeLongWords - recordHeaderLengthLongWords);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    otmpReadAddress = 44U;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, &record);
    CHECK_EQUAL(ERROR_NOENT, error);

}

TEST(NvmMgrApiGroup, TestNvmMgrFindAssetOtpmLengthCorruption)
{
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    ErrorT error;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;
    NvmMgrAddressT startAddressLongWords = OtpmDrvGetUserSpaceAddress();
    NvmMgrRecordHeaderT record = {0U, 0U};
    uint32_t arraySizeLongWords = OtpmDrvGetArraySizeLongWords();
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT otmpReadAddress;
    NvmMgrRecordHeaderT otpmValue = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 0U};
    NvmMgrRecordHeaderT otpmValueCorrupt = {ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, 0U};

    // User array space is (arraySizeLongWords - startAddressLongWords) for the test simulate
    // array containing 4 long assets
    uint16_t headerPluAssetSizeLongWords = (uint16_t)((arraySizeLongWords - (uint32_t)startAddressLongWords)/4U);
    otpmValue.assetLengthLongWords = (uint16_t)((uint32_t)headerPluAssetSizeLongWords - recordHeaderLengthLongWords);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    otmpReadAddress = 44U;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValue, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    // This will point to 2 location beyond end of array should never occur
    otpmValueCorrupt.assetLengthLongWords = headerPluAssetSizeLongWords;
    otmpReadAddress += headerPluAssetSizeLongWords;
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", otmpReadAddress)
        .withOutputParameterReturning("buffer", &otpmValueCorrupt, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_RECORD_LENGTH)
        .withParameter("infoParam", otmpReadAddress + recordHeaderLengthLongWords +
            otpmValueCorrupt.assetLengthLongWords);

    error = NvmMgrFindAsset(assetType, &startAddressLongWords, &record);
    CHECK_EQUAL(ERROR_SYSERR, error);

}

TEST(NvmMgrApiGroup, TestNvmMgrReadAssetParameterErrors)
{
    ErrorT error;
    NvmMgrAddressT const assetAddress = 88U;
    uint32_t buffer[8] = {0U};
    uint32_t offset = 2U;
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    nvmMgr.isInitialized = false;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_NOT_INITIALIZED)
        .withParameter("infoParam", 0U);

    error = NvmMgrReadAsset(assetAddress, buffer, offset, assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    nvmMgr.isInitialized = true;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_BAD_PARAM_BUFFER)
        .withParameter("infoParam", 0U);

    error = NvmMgrReadAsset(assetAddress, NULL, offset, assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrReadAssetSuccess)
{
    ErrorT error;
    NvmMgrAddressT const assetAddress = 88U;
    uint32_t otpm[8] = {1U,2U,3U,4U,5U,6U,7U, 8U};
    uint32_t buffer[8];
    uint32_t offset = 2U;
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_READ_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 90U)
        .withOutputParameterReturning("buffer", &otpm, sizeof(otpm))
        .withParameter("numLongWords", assetLengthLongWords);

    error = NvmMgrReadAsset(assetAddress, buffer, offset, assetLengthLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    MEMCMP_EQUAL(otpm, buffer, sizeof(buffer));
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetParameterErrors)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    nvmMgr.isInitialized = false;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_NOT_INITIALIZED)
        .withParameter("infoParam", 0U);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);

    nvmMgr.isInitialized = true;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_BAD_PARAM_BUFFER)
        .withParameter("infoParam", 0U);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, NULL, assetLengthLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetOtpmFull)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};

    // Available OTPM area is 2016 LongWords, first 44 are reserved thus we have 1972 for user space
    nvmMgr.blankAddressLongWords = 2016U;
    CHECK_EQUAL(2016U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, 4U);
    CHECK_EQUAL(2016U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_NOSPC, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetNoSpace)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, 4U*1024U);
    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_NOSPC, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetRecordHeaderWriteFail)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters()
        .andReturnValue(ERROR_RANGE);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetRecordHeaderReadBackFail)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters()
        .andReturnValue(ERROR_RANGE);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(53U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetRecordHeaderReadBackCompareFail)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", buffer, sizeof(NvmMgrRecordHeaderT));

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(53U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_IO, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetPayloadRecordHeaderReadBackFail)
{
    ErrorT error;
    uint32_t buffer[8] = {0U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);
    NvmMgrRecordHeaderT record = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)assetLengthLongWords};

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &record, sizeof(NvmMgrRecordHeaderT));

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 45U)
        .withParameter("buffer", buffer)
        .withParameter("numLongWords", assetLengthLongWords)
        .andReturnValue(ERROR_RANGE);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(53U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetPayloadReadBackCompareFail)
{
    ErrorT error;
    uint32_t buffer[8] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
    uint32_t corrupted = 0xFFU;
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);
    NvmMgrRecordHeaderT record = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)assetLengthLongWords};

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &record, sizeof(NvmMgrRecordHeaderT));

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 45U)
        .withParameter("buffer", buffer)
        .withParameter("numLongWords", assetLengthLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK);

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 45U)
        .withParameter("numLongWords", 1U)
        .withOutputParameterReturning("buffer", &buffer[0], sizeof(uint32_t));

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 46U)
        .withParameter("numLongWords", 1U)
        .withOutputParameterReturning("buffer", &buffer[1], sizeof(uint32_t));

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 47U)
        .withParameter("numLongWords", 1U)
        .withOutputParameterReturning("buffer", &corrupted, sizeof(uint32_t));

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(53U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_IO, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetPayloadReadBackSuccess)
{
    ErrorT error;
    uint32_t buffer[8] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);
    NvmMgrRecordHeaderT record = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)assetLengthLongWords};

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &record, sizeof(NvmMgrRecordHeaderT));

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 45U)
        .withParameter("buffer", buffer)
        .withParameter("numLongWords", assetLengthLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK);

    for (uint32_t i = 0; i < sizeof(buffer)/sizeof(buffer[0]); i++)
    {
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", (45U + i))
            .withParameter("numLongWords", 1U)
            .withOutputParameterReturning("buffer", &buffer[i], sizeof(uint32_t));
    }

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(53U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetPayloadReadBackSuccessNoneBlankNvm)
{
    ErrorT error;
    uint32_t buffer[8] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);
    NvmMgrRecordHeaderT record = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)assetLengthLongWords};

    nvmMgr.blankAddressLongWords = 80U;
    CHECK_EQUAL(80U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 80U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 80U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &record, sizeof(NvmMgrRecordHeaderT));

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 81U)
        .withParameter("buffer", buffer)
        .withParameter("numLongWords", assetLengthLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK);

    for (uint32_t i = 0; i < sizeof(buffer)/sizeof(buffer[0]); i++)
    {
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", (81U + i))
            .withParameter("numLongWords", 1U)
            .withOutputParameterReturning("buffer", &buffer[i], sizeof(uint32_t));
    }

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(89U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(NvmMgrApiGroup, TestNvmMgrWriteAssetPayloadReadBackDataCompareError)
{
    ErrorT error;
    uint32_t buffer[8] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U};
    uint32_t assetLengthLongWords = sizeof(buffer)/sizeof(buffer[0]);
    NvmMgrRecordHeaderT record = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)assetLengthLongWords};
    uint32_t corrupted = 0xFFFFFFFF;

    nvmMgr.blankAddressLongWords = 80U;
    CHECK_EQUAL(80U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);
    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);
    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 80U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK);
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 80U)
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .withOutputParameterReturning("buffer", &record, sizeof(NvmMgrRecordHeaderT));

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 81U)
        .withParameter("buffer", buffer)
        .withParameter("numLongWords", assetLengthLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK);

    for (uint32_t i = 0; i < sizeof(buffer)/sizeof(buffer[0]) - 2U; i++)
    {
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", (81U + i))
            .withParameter("numLongWords", 1U)
            .withOutputParameterReturning("buffer", &buffer[i], sizeof(uint32_t));
    }

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", (87U))
        .withParameter("numLongWords", 1U)
        .withOutputParameterReturning("buffer", &corrupted, sizeof(uint32_t))
        .andReturnValue(ERROR_INVAL);

    error = NvmMgrWriteAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, buffer, assetLengthLongWords);
    CHECK_EQUAL(89U, nvmMgr.blankAddressLongWords);
    CHECK_EQUAL(ERROR_IO, error);
}

TEST(NvmMgrApiGroup,TestNvmMgrPatchGetFreeSpaceAddressUninitialised)
{
    nvmMgr.isInitialized = false;
    ErrorT err = ERROR_SUCCESS;
    NvmMgrAddressT offset = 0;
    NvmMgrAddressT* blankSpaceAddress = &offset;
    err = NvmMgrGetFreeSpaceAddress(blankSpaceAddress);
    CHECK_EQUAL(ERROR_ACCESS,err);
}

TEST(NvmMgrApiGroup,TestNvmMgrPatchGetFreeSpaceAddressNullPointer)
{
    nvmMgr.isInitialized = false;
    ErrorT err = ERROR_SUCCESS;
    NvmMgrAddressT* blankSpaceAddress = NULL;
    err = NvmMgrGetFreeSpaceAddress(blankSpaceAddress);
    CHECK_EQUAL(ERROR_INVAL,err);
}

TEST(NvmMgrApiGroup,TestNvmMgrPatchGetFreeSpaceAddress)
{
    ErrorT err = ERROR_SUCCESS;
    NvmMgrAddressT offset = 0;
    NvmMgrAddressT* blankSpaceAddress = &offset;
    err = NvmMgrGetFreeSpaceAddress(blankSpaceAddress);
    CHECK_EQUAL(ERROR_SUCCESS,err);
    CHECK_EQUAL(nvmMgr.blankAddressLongWords,*blankSpaceAddress);
}

TEST(NvmMgrSupport, TestOnDiagFatalErrorDumpParamError)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);

    nvmMgr.isInitialized = true;
    nvmMgr.blankAddressLongWords = 0xCAFEBABEU;

    NvmMgrOnDiagFatalErrorDump(NULL, bufferSizeLongWords);
    CHECK_EQUAL(0U, buffer[0]);
    CHECK_EQUAL(0U, buffer[1]);

    NvmMgrOnDiagFatalErrorDump(buffer, 1U);
    CHECK_EQUAL(1U, buffer[0]);
    CHECK_EQUAL(0U, buffer[1]);
}

TEST(NvmMgrSupport, TestOnDiagFatalErrorDump)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);

    nvmMgr.isInitialized = true;
    nvmMgr.blankAddressLongWords = 0xCAFEBABEU;

    NvmMgrOnDiagFatalErrorDump(buffer, bufferSizeLongWords);
    CHECK_EQUAL(1U, buffer[0]);
    CHECK_EQUAL(0xCAFEBABEU, buffer[1]);
}

TEST(NvmMgrSupport, TestNvmMgrOnClockChangeWithConfigAssetReadFail)
{
    ErrorT error;
    AssetMgrOtpmConfigT otpmConfig;
    AssetMgrOtpmConfigT* otpmConfigPtr = &otpmConfig;

    memset(&otpmConfig, 0U, sizeof(AssetMgrOtpmConfigT));
    otpmConfig.tvpps_ns = 0xFFFFFFFF;
    otpmConfig.mrb_read = 0xAAAA;

    nvmMgr.isInitialized = true;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);

    mock().expectOneCall("AssetMgrRetrieveOtpmConfig")
        .withOutputParameterReturning("OTPMConfig", &otpmConfigPtr, sizeof(AssetMgrOtpmConfigT*))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_NO_ASSET_FOUND);

    mock().expectOneCall("OtpmDrvConfigure")
        .withPointerParameter("config", NULL)
        .withParameter("clockRateHz", 1U);

    error = NvmMgrOnClockChange(1U);
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(NvmMgrSupport, TestNvmMgrOnClockChangeWithConfig)
{
    ErrorT error;
    AssetMgrOtpmConfigT otpmConfig;

    memset(&otpmConfig, 0U, sizeof(AssetMgrOtpmConfigT));
    otpmConfig.tvpps_ns = 0xFFFFFFFF;
    otpmConfig.mrb_read = 0xAAAA;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);

    mock().expectOneCall("AssetMgrRetrieveOtpmConfig")
        .withOutputParameterReturning("OTPMConfig", &otpmConfig, sizeof(AssetMgrOtpmConfigT));

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_FOUND_ASSET);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", 1U)
        .ignoreOtherParameters();

    error = NvmMgrOnClockChange(1U);
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(NvmMgrSupport, TestNvmMgrOnClockChangeOptmError)
{
    ErrorT error;
    AssetMgrOtpmConfigT otpmConfig;

    memset(&otpmConfig, 0U, sizeof(AssetMgrOtpmConfigT));

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);

    mock().expectOneCall("AssetMgrRetrieveOtpmConfig")
        .withOutputParameterReturning("OTPMConfig", &otpmConfig, sizeof(AssetMgrOtpmConfigT))
        .andReturnValue(ERROR_IO);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_ON_CLOCK_CHANGE_ASSET_RETRIEVAL_FAILED)
        .withParameter("infoParam", ERROR_IO);

    error = NvmMgrOnClockChange(1U);
    CHECK_EQUAL(ERROR_IO, error);
}

TEST(NvmMgrSupport, TestNvmMgrOnClockChangeNoConfig)
{
    ErrorT error;
    AssetMgrOtpmConfigT otpmConfig;

    memset(&otpmConfig, 0U, sizeof(AssetMgrOtpmConfigT));

    CHECK_EQUAL(44U, nvmMgr.blankAddressLongWords);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);

    mock().expectOneCall("AssetMgrRetrieveOtpmConfig")
        .withOutputParameterReturning("OTPMConfig", &otpmConfig, sizeof(AssetMgrOtpmConfigT))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_NO_ASSET_FOUND);

    mock().expectOneCall("OtpmDrvConfigure")
        .withPointerParameter("config", NULL)
        .withParameter("clockRateHz", 1U);

    error = NvmMgrOnClockChange(1U);
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(NvmMgrSupport, TestNvmMgrOnClockChangeInvalidClock)
{
    ErrorT error;
    AssetMgrOtpmConfigT otpmConfig;

    memset(&otpmConfig, 0U, sizeof(AssetMgrOtpmConfigT));
    otpmConfig.tvpps_ns = 0xFFFFFFFF;
    otpmConfig.mrb_read = 0xAAAA;

    nvmMgr.isInitialized = true;
    nvmMgr.blankAddressLongWords = 0U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_ENTRY);

    mock().expectOneCall("AssetMgrRetrieveOtpmConfig")
        .withOutputParameterReturning("OTPMConfig", &otpmConfig, sizeof(AssetMgrOtpmConfigT))
        .andReturnValue(ERROR_NOENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_ON_CLOCK_CHANGE_NO_ASSET_FOUND);

    mock().expectOneCall("OtpmDrvConfigure")
        .withPointerParameter("config", NULL)
        .withParameter("clockRateHz", 1U)
        .andReturnValue(ERROR_RANGE);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_OTPM_DRV_CONFIGURE_FAILED)
        .withParameter("infoParam", ERROR_RANGE);

    error = NvmMgrOnClockChange(1U);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(NvmMgrSupport, TestNvmMgrFindFreeSpaceError)
{
    ErrorT error;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_PARAM_START_ADDRESS)
        .withParameter("infoParam", 0U);

    error =  NvmMgrFindFreeSpace(NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);
}

TEST(NvmMgrSupport, TestNvmMgrFindFreeSpaceReadError)
{
    ErrorT error;
    NvmMgrAddressT addressLongWords;
    NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t))
        .andReturnValue(ERROR_RANGE);
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_OTPM_READ_UNEXPECTED_ERROR)
        .withParameter("infoParam", ERROR_RANGE);

    error =  NvmMgrFindFreeSpace(&addressLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);
}


TEST(NvmMgrSupport, TestNvmMgrFindFreeSpaceAllEmpty)
{
    ErrorT error;
    NvmMgrAddressT addressLongWords;
    NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));

    error =  NvmMgrFindFreeSpace(&addressLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(44U, addressLongWords);
}

TEST(NvmMgrSupport, TestNvmMgrFindFreeSpaceAssetIdEmptyLengthNot)
{
    ErrorT error;
    NvmMgrAddressT addressLongWords = 0U;
    NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 20U};

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");
    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", 44U)
        .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t));
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY)
        .withParameter("infoParam", 20U);

    error =  NvmMgrFindFreeSpace(&addressLongWords);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(0U, addressLongWords);
}

TEST(NvmMgrSupport, TestNvmMgrFindFreeSpacePartiallyEmpty)
{
    ErrorT error;
    NvmMgrAddressT addressLongWords;
    NvmMgrRecordHeaderT  empty = {ASSET_MGR_ASSET_TYPE_EMPTY, 0U};
    NvmMgrRecordHeaderT  config = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)100U};
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT address = 44U;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    for (uint32_t i = 0; i < 10; i++)
    {
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", address)
            .withOutputParameterReturning("buffer", &config, sizeof(NvmMgrRecordHeaderT))
            .withParameter("numLongWords", recordHeaderLengthLongWords);
        address += config.assetLengthLongWords + recordHeaderLengthLongWords;
    }

    mock().expectOneCall("OtpmDrvRead")
        .withParameter("address", address)
        .withOutputParameterReturning("buffer", &empty, sizeof(NvmMgrRecordHeaderT))
        .withParameter("numLongWords", recordHeaderLengthLongWords);

    error =  NvmMgrFindFreeSpace(&addressLongWords);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(address, addressLongWords);
}

TEST(NvmMgrSupport, TestNvmMgrFindFreeSpaceFull)
{
    ErrorT error;
    NvmMgrAddressT addressLongWords;
    NvmMgrRecordHeaderT  config = {ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint16_t)((2016U - 44U)/4U) - 1U};
    uint32_t recordHeaderLengthLongWords = sizeof(NvmMgrRecordHeaderT)/sizeof(uint32_t);
    NvmMgrAddressT address = 44U;

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");
    mock().expectOneCall("OtpmDrvGetUserSpaceAddress");

    for (uint32_t i = 0; i < 4; i++)
    {
        mock().expectOneCall("OtpmDrvRead")
            .withParameter("address", address)
            .withOutputParameterReturning("buffer", &config, sizeof(NvmMgrRecordHeaderT))
            .withParameter("numLongWords", recordHeaderLengthLongWords);
        address += config.assetLengthLongWords + recordHeaderLengthLongWords;
    }

    error =  NvmMgrFindFreeSpace(&addressLongWords);
    CHECK_EQUAL(ERROR_NOSPC, error);
    CHECK_EQUAL(2016U, addressLongWords);
}
