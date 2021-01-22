//***********************************************************************************
/// \file
///
/// SDPU Application unit test
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
#include "crypto_otpm.h"
#include "Diag.h"
#include "LifeCycleMgr.h"
#include "OtpmDrv.h"
#include "OtpmDrvInternal.h"
#include "SystemMgr.h"
#include "AssetMgr.h"
#include "cc_pal_types.h"
#include "cc_pal_trng.h"
#include "DepuAppInternal.h"
#include "ccProdOtpPatch.h"
#include "mcu_sysctrl.h"
#include "NvmMgrInternal.h"
#include "CommandHandler.h"
}

// Mocks
extern "C"
{
    // Make sure the TRNG are created
    CcPalTrngT ccPalTrng;
    ccProdOtpT ccProdOtp;
    MCU_SYSCTRL_REGS_T sysCtrlRegsUnitTest = {};
    uint16_t depuRomContentsUnitTest[0x7FFF] = {};

    NvmMgrT nvmMgr;

uint32_t CC_ProdOtpWriteVerifyWordBuff(uint32_t otpLongWordOffset, const uint32_t* longWordBuff,
    uint32_t const buffLongWordSize)
{
    printf("CC_ProdOtpWriteVerifyWordBuff address: %x otpLongWordOffset size: %d - nvm : %d\n",
        otpLongWordOffset, buffLongWordSize, nvmMgr.blankAddressLongWords);

    mock().actualCall("CC_ProdOtpWriteVerifyWordBuff")
        .withParameter("otpLongWordOffset", otpLongWordOffset)
        .withPointerParameter("longWordBuff", (void *)longWordBuff)
        .withParameter("buffLongWordSize", buffLongWordSize);

    uint32_t error = mock().returnUnsignedIntValueOrDefault(CC_OK);

    if (CC_OK != error)
    {
        ccProdOtp.lastError = ERROR_NODEV;
    }

    return error;
}

ErrorT PatchMgrSetupComparatorTable(uint32_t const startComparator,
    uint32_t const numComparators,
    PatchMgrComparatorFunctionAddressT const* const comparatorTable,
    uint32_t const sizeTable)
{
    mock().actualCall("PatchMgrSetupComparatorTable")
        .withParameter("startComparator", startComparator)
        .withParameter("numComparators", numComparators)
        .withPointerParameter("comparatorTable", (void *)comparatorTable)
        .withParameter("sizeTable", sizeTable);

    return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
}

void HardFault_PatchedHandler(void)
{
}

const PatchMgrComparatorFunctionAddressT depuAppComparatorTable[DEPU_APP_NUM_PATCHED_FUNCTIONS] =
{
    (PatchMgrComparatorFunctionAddressT)0,
};

ErrorT CC_ProdOtpGetLastError(void)
{
    mock().actualCall("CC_ProdOtpGetLastError");
    return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
}

}

TEST_GROUP(DepuAppGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ccPalTrng.isPalTrngParamSet = CC_FALSE;
        ccProdOtp.errorAddress = 0;
        ccProdOtp.numberSoftErrors = 0;
        ccProdOtp.lastError = ERROR_SUCCESS;
        nvmMgr.isInitialized = true;
        nvmMgr.blankAddressLongWords = 0x2C;
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(DepuAppGroup, TestDepuAppInit)
{
    ErrorT error;

    mock().expectOneCall("PatchMgrSetupComparatorTable")
            .ignoreOtherParameters();

    mock().expectOneCall("CommandHandlerAddCommand")
            .ignoreOtherParameters();

    mock().expectOneCall("InitRelocateVectorTable");

    mock().expectOneCall("InitInstallNewIrqHandler")
            .withParameter("irq", HardFault_IRQn)
            .ignoreOtherParameters();

    error = DepuAppLoader();
    CHECK_EQUAL(ERROR_SUCCESS, error);
}

TEST(DepuAppGroup, TestDepuAppInitError)
{
    ErrorT error;

    mock().expectOneCall("PatchMgrSetupComparatorTable")
            .andReturnValue(ERROR_SYSERR)
            .ignoreOtherParameters();

    error = DepuAppLoader();
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("PatchMgrSetupComparatorTable")
            .ignoreOtherParameters();

    mock().expectOneCall("CommandHandlerAddCommand")
            .ignoreOtherParameters()
            .andReturnValue(ERROR_SYSERR);

    error = DepuAppLoader();
    CHECK_EQUAL(ERROR_SYSERR, error);

    mock().expectOneCall("PatchMgrSetupComparatorTable")
            .ignoreOtherParameters();

    mock().expectOneCall("CommandHandlerAddCommand")
            .ignoreOtherParameters();

    mock().expectOneCall("InitRelocateVectorTable");

    mock().expectOneCall("InitInstallNewIrqHandler")
            .withParameter("irq", HardFault_IRQn)
            .andReturnValue(ERROR_SYSERR)
            .ignoreOtherParameters();

    error = DepuAppLoader();
    CHECK_EQUAL(ERROR_SYSERR, error);

}

TEST(DepuAppGroup, TestBasic)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    for(int i = 0; i < 2; i++)
    {
        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }

    uint32_t assetPakageBuffer[3][9] = {
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00410003U},
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00820001U},
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00820001U}
                                };

    for(int i = 0; i < 3; i++)
    {
        mock().expectOneCall("CryptoMgrAuthenticateAsset")
              .withOutputParameterReturning("assetPakageBuffer", assetPakageBuffer[i], sizeof(assetPakageBuffer[0]))
              .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }


    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestBasicErrors)
{
    uint32_t params[100];

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_DEPU_APP_BAD_COMMAND)
          .withParameter("infoParam", HOST_COMMAND_CODE_GET_STATUS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_GET_STATUS, params));

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_DEPU_APP_BAD_PARAM)
          .withParameter("infoParam", 0);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, NULL));

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NOEXEC);

    CHECK_EQUAL(ERROR_NOEXEC, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, params));

    currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, params));

}


TEST(DepuAppGroup, TestIntegrityErrors)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    // wrong checksum

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum ^ 1);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

    // wrong size
    params->blockLengthBytes = blockLengthBytes + 1;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}


TEST(DepuAppGroup, TestSysClockError)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters()
        .andReturnValue(ERROR_RANGE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);
}

TEST(DepuAppGroup, TestCmpuError)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters()
          .andReturnValue(1);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_ERROR)
          .withParameter("info", 1);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);
}

TEST(DepuAppGroup, TestCmpuError2)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    ccProdOtp.lastError = ERROR_NOEXEC;

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters()
          .andReturnValue(CC_PROD_XXPU_MODULE_ERROR_BASE + (int)0xCAFE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_ERROR)
          .withParameter("info", 0xCAFE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_NOEXEC);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NOEXEC);

    CHECK_EQUAL(ERROR_NOEXEC, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

    TEST(DepuAppGroup, TestLcsErrorAfterCmpu)
    {
        uint32_t shared_ram[2000] = {};
        DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
        DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
        const uint32_t clockSpeedHz = 27000000;
        const uint32_t checksum = 0xFFFF;
        const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
        params->clockSpeedHz = clockSpeedHz;
        params->checksum = checksum;
        params->blockLengthBytes = blockLengthBytes;

        LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

        mock().expectOneCall("LifecycleMgrGetCurrentLcs")
              .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
              .andReturnValue(ERROR_SUCCESS);

        mock().expectOneCall("ToolboxCalcChecksum")
            .ignoreOtherParameters()
            .andReturnValue(checksum);

        mock().expectOneCall("OtpmDrvConfigure")
            .withParameter("clockRateHz", clockSpeedHz)
            .ignoreOtherParameters();

        mock().expectOneCall("CC_ProdOtpGetLastError");

        mock().expectOneCall("CCProd_Cmpu")
              .withParameter("ccHwRegBaseAddr", 0x40000000UL)
              .withParameter("workspaceSize", 4096)
              .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

        mock().expectOneCall("LifecycleMgrCc312Init")
              .andReturnValue(ERROR_SUCCESS);

        currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

        mock().expectOneCall("LifecycleMgrGetCurrentLcs")
              .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
              .andReturnValue(ERROR_SUCCESS);

        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
              .withParameter("info", ERROR_NODEV);

        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
              .withParameter("info", ERROR_NODEV);

        CHECK_EQUAL(ERROR_NODEV, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

        CHECK_EQUAL(0, response->errorAddress);
        CHECK_EQUAL(0, response->numberSoftErrors);

    }

TEST(DepuAppGroup, TestLcsError2AfterCmpu)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestCryptoMgrAuthenticateAssetErrorOnDmpu)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters()
          .andReturnValue(ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);
}

TEST(DepuAppGroup, TestDmpuError)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters()
          .andReturnValue(1);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_ERROR)
          .withParameter("info", 1);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestDmpuError2)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    ccProdOtp.lastError = ERROR_NOEXEC;

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters()
          .andReturnValue(CC_PROD_XXPU_MODULE_ERROR_BASE + (int)0xCAFE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_ERROR)
          .withParameter("info", 0xCAFE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_NOEXEC);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NOEXEC);

    CHECK_EQUAL(ERROR_NOEXEC, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestLcsErrorAfterDmpu)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_NODEV);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NODEV);

    CHECK_EQUAL(ERROR_NODEV, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestLcsError2AfterDmpu)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_IO);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestAssetProvisionError)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

    mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
          .ignoreOtherParameters()
          .andReturnValue(CC_PROD_XXPU_WRITE_ERROR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_NODEV);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NODEV);

    CHECK_EQUAL(ERROR_NODEV, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestNvmNotInitialized)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    nvmMgr.isInitialized = false;

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_NOT_INITIALIZED)
          .withParameter("infoParam", 0);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestNvmWrongRange)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    for(int i = 0; i < 2; i++)
    {
        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }

    uint32_t assetPakageBuffer[3][9] = {
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00410003U},
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00820001U},
                                { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                  0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00820001U}
                                };

    for(int i = 0; i < 3; i++)
    {
        mock().expectOneCall("CryptoMgrAuthenticateAsset")
              .withOutputParameterReturning("assetPakageBuffer", assetPakageBuffer[i], sizeof(assetPakageBuffer[0]))
              .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    nvmMgr.blankAddressLongWords = 0x2D;

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestNoSpaceinOtpmforBigAsset)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    for(int i = 0; i < 2; i++)
    {
        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }

    uint32_t assetPakageBuffer[] = { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                     0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x7F410003U };

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .withOutputParameterReturning("assetPakageBuffer", assetPakageBuffer, sizeof(assetPakageBuffer))
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

    mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_NOSPC);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_NOSPC);

    CHECK_EQUAL(ERROR_NOSPC, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}

TEST(DepuAppGroup, TestFailureinAuthenticateUserAssets)
{
    uint32_t shared_ram[2000] = {};
    DepuAppProgramOtpmCommandParamsT* params = (DepuAppProgramOtpmCommandParamsT*)shared_ram;
    DepuAppProgramOtpmResponseParamsT* response = (DepuAppProgramOtpmResponseParamsT*)shared_ram;
    const uint32_t clockSpeedHz = 27000000;
    const uint32_t checksum = 0xFFFF;
    const uint32_t blockLengthBytes = sizeof(depuApp.depuAppAssets);
    params->clockSpeedHz = clockSpeedHz;
    params->checksum = checksum;
    params->blockLengthBytes = blockLengthBytes;

    LifecycleMgrLifecycleStateT currentLcs1 = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcs1, sizeof(currentLcs1))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("ToolboxCalcChecksum")
        .ignoreOtherParameters()
        .andReturnValue(checksum);

    mock().expectOneCall("OtpmDrvConfigure")
        .withParameter("clockRateHz", clockSpeedHz)
        .ignoreOtherParameters();

    mock().expectOneCall("CC_ProdOtpGetLastError");

    mock().expectOneCall("CCProd_Cmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 4096)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsCmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsCmpu, sizeof(currentLcsCmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .ignoreOtherParameters();

    mock().expectOneCall("CCProd_Dmpu")
          .withParameter("ccHwRegBaseAddr", 0x40000000UL)
          .withParameter("workspaceSize", 1536)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);

    mock().expectOneCall("LifecycleMgrCc312Init")
          .andReturnValue(ERROR_SUCCESS);

    LifecycleMgrLifecycleStateT currentLcsDmpu = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &currentLcsDmpu, sizeof(currentLcsDmpu))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING)
          .withParameter("info", ERROR_SUCCESS);

    for(int i = 0; i < 2; i++)
    {
        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET_ENTRY);

        mock().expectOneCall("OtpmDrvGetArraySizeLongWords");

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_NVM_MGR_WRITE_ASSET);

        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
        mock().expectOneCall("CC_ProdOtpWriteVerifyWordBuff")
              .ignoreOtherParameters();
    }

    uint32_t assetPakageBuffer[] = { 0x41736574U, 0x00010000U, 0x00000110U, 0x00000000U, 0x00000000U,
                                     0x6cea4006U, 0x66735002U, 0xcfb4c8f4U, 0x00410003U };

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
          .withOutputParameterReturning("assetPakageBuffer", assetPakageBuffer, sizeof(assetPakageBuffer))
          .andReturnValue(ERROR_BADMSG)
          .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING)
          .withParameter("info", ERROR_BADMSG);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
          .withParameter("checkpoint", DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM)
          .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, DepuAppOnProgramOtpm(HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM, shared_ram));

    CHECK_EQUAL(0, response->errorAddress);
    CHECK_EQUAL(0, response->numberSoftErrors);

}



