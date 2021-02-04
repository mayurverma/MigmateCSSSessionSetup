//***********************************************************************************
/// \file
///
/// Diag unit test
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
#include "AR0820.h"
#include "Diag.h"
#include "DiagCheckpoint.h"
#include "bsv_api.h"
#include "secdebug_api.h"
#include "dx_crys_kernel.h"
#include "bootimagesverifier_def.h"
#include "OtpmDrv.h"
#include "LifecycleMgr.h"
#include "LifecycleMgrInternal.h"
#include "bsv_error.h"

#if defined(BUILD_TARGET_ARCH_x86)
extern uint8_t cryptoMemory[];
#endif
}

// Mocks
extern "C"
{
    CCError_t CC_BsvInit( unsigned long hwBaseAddress)
    {
        mock().actualCall("CC_BsvInit")
            .withParameter("hwBaseAddress", hwBaseAddress);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvLcsGet( unsigned long hwBaseAddress, uint32_t *pLcs)
    {
        mock().actualCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withOutputParameter("pLcs", pLcs);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvLcsGetAndInit( unsigned long hwBaseAddress, uint32_t *pLcs)
    {
        mock().actualCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withOutputParameter("pLcs", pLcs);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvFatalErrorSet( unsigned long hwBaseAddress)
    {
        mock().actualCall("CC_BsvFatalErrorSet")
            .withParameter("hwBaseAddress", hwBaseAddress);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvSecureDebugSet( unsigned long   hwBaseAddress,
        uint32_t   *pDebugCertPkg,
        uint32_t   certPkgSize,
        uint32_t   *pEnableRmaMode,
        uint32_t   *pWorkspace,
        uint32_t   workspaceSize)
    {
        if (NULL != pEnableRmaMode)
        {
            mock().actualCall("CC_BsvSecureDebugSet")
                .withParameter("hwBaseAddress", hwBaseAddress)
                .withPointerParameter("pDebugCertPkg", pDebugCertPkg)
                .withParameter("certPkgSize", certPkgSize)
                .withOutputParameter("pEnableRmaMode", pEnableRmaMode)
                .withPointerParameter("pWorkspace", pWorkspace)
                .withParameter("workspaceSize", workspaceSize);
        }
        else
        {
            mock().actualCall("CC_BsvSecureDebugSet")
                .withParameter("hwBaseAddress", hwBaseAddress)
                .withPointerParameter("pDebugCertPkg", pDebugCertPkg)
                .withParameter("certPkgSize", certPkgSize)
                .withPointerParameter("pEnableRmaMode", NULL)
                .withPointerParameter("pWorkspace", pWorkspace)
                .withParameter("workspaceSize", workspaceSize);
        }
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvCoreClkGatingEnable( unsigned long hwBaseAddress)
    {
        mock().actualCall("CC_BsvCoreClkGatingEnable")
            .withParameter("hwBaseAddress", hwBaseAddress);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvSecModeSet( unsigned long hwBaseAddress,
            CCBool_t isSecAccessMode,
            CCBool_t isSecModeLock)
    {
        mock().actualCall("CC_BsvSecModeSet")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withParameter("isSecAccessMode", isSecAccessMode)
            .withParameter("isSecModeLock", isSecModeLock);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvPrivModeSet( unsigned long hwBaseAddress,
            CCBool_t isPrivAccessMode,
            CCBool_t isPrivModeLock)
    {
        mock().actualCall("CC_BsvPrivModeSet")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withParameter("isPrivAccessMode", isPrivAccessMode)
            .withParameter("isPrivModeLock", isPrivModeLock);
        return (CCError_t)mock().returnUnsignedLongIntValueOrDefault(CC_OK);
    }

    void* MockCalloc(size_t nItems, size_t size)
    {
        mock().actualCall("MockCalloc")
            .withParameter("nItems", nItems)
            .withParameter("size", size);
        return mock().returnPointerValueOrDefault(NULL);
    }

    void MockFree(void* ptr)
    {
        mock().actualCall("MockFree")
            .withParameter("ptr", ptr);
    }

}

TEST_GROUP(LifecycleMgrApiGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT error;

        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))LifecycleMgrOnDiagFatalErrorDump);
        error = LifecycleMgrInit();
        CHECK_EQUAL(ERROR_SUCCESS, error);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(LifecycleMgrGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT error;

        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))LifecycleMgrOnDiagFatalErrorDump);
        error = LifecycleMgrInit();
        CHECK_EQUAL(ERROR_SUCCESS, error);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(LifecycleMgrGroup, TestInit)
{
    ErrorT error;

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))LifecycleMgrOnDiagFatalErrorDump);
    error = LifecycleMgrInit();
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitFail)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;

    errorOtpm.isCorrectable = true;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_FAIL);
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);

    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitOtpmBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;


    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_OTPM_FAULT)
        .withParameter("infoParam", 0U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitLcsBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_FAIL);
    mock().expectOneCall("CC_BsvFatalErrorSet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT)
        .withParameter("infoParam", 0U);
    mock().expectOneCall("DiagEnableInfo")
        .withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitGatingsBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_FAIL);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_CRYPTO_CLOCK_GATING_FAULT)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitSecureGatingsBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent = CC_BSV_SECURE_LCS;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagEnableInfo")
        .withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_FAIL);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_CRYPTO_CLOCK_GATING_FAULT)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}


TEST(LifecycleMgrGroup, TestCc312InitSecFilterBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
    mock().expectOneCall("CC_BsvSecModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isSecAccessMode", CC_FALSE)
        .withParameter("isSecModeLock", CC_FALSE)
        .andReturnValue(CC_FAIL);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_SEC_FILTER_FAULT)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitPrivFilterBad)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
    mock().expectOneCall("CC_BsvSecModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isSecAccessMode", CC_FALSE)
        .withParameter("isSecModeLock", CC_FALSE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS);
    mock().expectOneCall("CC_BsvPrivModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isPrivAccessMode", CC_FALSE)
        .withParameter("isPrivModeLock", CC_FALSE)
        .andReturnValue(CC_FAIL);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_PRIV_FILTER_FAULT)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitSuccess)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
    mock().expectOneCall("CC_BsvSecModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isSecAccessMode", CC_FALSE)
        .withParameter("isSecModeLock", CC_FALSE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS);
    mock().expectOneCall("CC_BsvPrivModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isPrivAccessMode", CC_FALSE)
        .withParameter("isPrivModeLock", CC_FALSE)
        .andReturnValue(CC_OK);

    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestCc312InitSuccessOtpmCorrectable)
{
    ErrorT error;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = true;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
    mock().expectOneCall("CC_BsvSecModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isSecAccessMode", CC_FALSE)
        .withParameter("isSecModeLock", CC_FALSE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS);
    mock().expectOneCall("CC_BsvPrivModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isPrivAccessMode", CC_FALSE)
        .withParameter("isPrivModeLock", CC_FALSE)
        .andReturnValue(CC_OK);

    error = LifecycleMgrCc312Init();
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestLifecycleMgrGetInfoEnable)
{
    bool isEnabled;

    cryptoMemory[DX_HOST_DCU_EN0_REG_OFFSET] = 0xFF;

    isEnabled = LifecycleMgrGetInfoEnable();
    CHECK_EQUAL(true, isEnabled);

    cryptoMemory[DX_HOST_DCU_EN0_REG_OFFSET] = 0x00;

    isEnabled = LifecycleMgrGetInfoEnable();
    CHECK_EQUAL(false, isEnabled);
}

TEST(LifecycleMgrGroup, TestOnDiagFatalErrorDumpParamError)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);
    LifecycleMgrT*  dump = ( LifecycleMgrT*)buffer;

    lifecycleMgr.debugEntitlementApplied = true;

    LifecycleMgrOnDiagFatalErrorDump(NULL, bufferSizeLongWords);
    CHECK_EQUAL(0U, dump->debugEntitlementApplied);

    LifecycleMgrOnDiagFatalErrorDump(buffer, 0U);
    CHECK_EQUAL(false, dump->debugEntitlementApplied);
}

TEST(LifecycleMgrGroup, TestOnDiagFatalErrorDumpParam)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);
    LifecycleMgrT*  dump = ( LifecycleMgrT*)buffer;

    lifecycleMgr.debugEntitlementApplied = true;
    LifecycleMgrOnDiagFatalErrorDump(buffer, bufferSizeLongWords);
    CHECK_EQUAL(true, dump->debugEntitlementApplied);

    lifecycleMgr.debugEntitlementApplied = false;
    LifecycleMgrOnDiagFatalErrorDump(buffer, bufferSizeLongWords);
    CHECK_EQUAL(false, dump->debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsBsvLcsGetFailed)
{
    ErrorT error;
    uint32_t lcsCurrent;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_FAIL);
    mock().expectOneCall("CC_BsvFatalErrorSet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT)
        .withParameter("infoParam", 0U);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_CM, lcs);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsCM)
{
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_CHIP_MANUFACTURE_LCS;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_CM, lcs);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsDM)
{
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_DM, lcs);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsSecure)
{
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_SECURE_LCS;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE, lcs);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsRma)
{
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_RMA_LCS;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_RMA, lcs);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsNull)
{
    ErrorT error;

    error = LifecycleMgrGetCurrentLcs(NULL);
    CHECK_EQUAL(ERROR_INVAL, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetCurrentLcsInvalid)
{
    ErrorT error;
    uint32_t lcsCurrent = 0xff;
    LifecycleMgrLifecycleStateT lcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("CC_BsvFatalErrorSet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT)
        .withParameter("infoParam", 0U);

    error = LifecycleMgrGetCurrentLcs(&lcs);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestGetOnAuthorizeRMA)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_NULL;
    uint32_t params[2];
    ErrorT error;

    error = LifecycleMgrOnAuthorizeRMA(commandCode, (CommandHandlerCommandParamsT)params);
    CHECK_EQUAL(ERROR_ACCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementInvalidCommand)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_NULL;
    uint32_t params[2];
    ErrorT error;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_COMMAND)
        .withParameter("infoParam", commandCode);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)params);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementInvalidParams)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    ErrorT error;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_PARAM)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementInvalidLcs)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    uint32_t params[2];
    ErrorT error;
    uint32_t lcsCurrent;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);

    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_FAIL);
    mock().expectOneCall("CC_BsvFatalErrorSet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U);
    mock().expectOneCall("SystemMgrReportCryptoFault")
        .withParameter("fault", SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT)
        .withParameter("infoParam", 0U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)params);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementInvalidLcsCm)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    uint32_t params[2];
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_CHIP_MANUFACTURE_LCS;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)params);
    CHECK_EQUAL(ERROR_ACCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetNoHeap)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 1;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t));
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("MockFree").ignoreOtherParameters();
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_NOSPC, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetError)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;
    uint32_t enableRMA = 0;
    uint32_t buffer[CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4] = {0};

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 1;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t))
        .andReturnValue(buffer);
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", &message.entitlement)
        .withParameter("certPkgSize", message.blobSizeWords*2)
        .withOutputParameterReturning("pEnableRmaMode", (uint32_t *)&enableRMA, sizeof(enableRMA))
        .withParameter("workspaceSize", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES)
        .ignoreOtherParameters()
        .andReturnValue(CC_FAIL);
    mock().expectOneCall("MockFree").ignoreOtherParameters();
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_BADMSG, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetEnableRma)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;
    uint32_t enableRMA = 1;
    uint32_t buffer[CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4] = {0};

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 1;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t))
        .andReturnValue(buffer);
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", &message.entitlement)
        .withParameter("certPkgSize", message.blobSizeWords*2)
        .withOutputParameterReturning("pEnableRmaMode", (uint32_t *)&enableRMA, sizeof(enableRMA))
        .withParameter("workspaceSize", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES)
        .ignoreOtherParameters()
        .andReturnValue(CC_OK);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_RMA_PRESENTED)
        .withParameter("infoParam", 0U);

    mock().expectOneCall("MockFree").ignoreOtherParameters();
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetInvalidClock)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 0;

    mock().expectOneCall("DiagSetCheckpoint")
            .withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("SystemMgrSetClockSpeed")
            .withParameter("clockSpeedHz", message.clockSpeedHz)
            .andReturnValue(ERROR_RANGE);

    mock().expectOneCall("DiagSetCheckpoint")
            .withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_RANGE, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetSuccess)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_DEVICE_MANUFACTURE_LCS;
    uint32_t enableRMA = 0;
    uint32_t buffer[CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4] = {0};

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 0;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t))
        .andReturnValue(buffer);
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", &message.entitlement)
        .withParameter("certPkgSize", message.blobSizeWords*2)
        .withOutputParameterReturning("pEnableRmaMode", (uint32_t *)&enableRMA, sizeof(enableRMA))
        .withParameter("workspaceSize", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES)
        .ignoreOtherParameters()
        .andReturnValue(CC_OK);

    mock().expectOneCall("MockFree").ignoreOtherParameters();

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetSuccessSecureEnableInfo)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_SECURE_LCS;
    uint32_t enableRMA = 0;
    uint32_t buffer[CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4] = {0};

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 0;

    cryptoMemory[DX_HOST_DCU_EN0_REG_OFFSET] = 0xFF;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t))
        .andReturnValue(buffer);
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", &message.entitlement)
        .withParameter("certPkgSize", message.blobSizeWords*2)
        .withOutputParameterReturning("pEnableRmaMode", (uint32_t *)&enableRMA, sizeof(enableRMA))
        .withParameter("workspaceSize", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES)
        .ignoreOtherParameters()
        .andReturnValue(CC_OK);

    mock().expectOneCall("DiagEnableInfo").withParameter("enable", true);

    mock().expectOneCall("MockFree").ignoreOtherParameters();

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, lifecycleMgr.debugEntitlementApplied);
}


TEST(LifecycleMgrApiGroup, TestOnApplyDebugEntitlementBsvSecureDebugSetSuccessSecureDisableInfo)
{
    HostCommandCodeT commandCode = HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT;
    LifecycleMgrApplyDebugEntitlementHeaderT message;
    ErrorT error;
    uint32_t lcsCurrent = CC_BSV_SECURE_LCS;
    uint32_t enableRMA = 0;
    uint32_t buffer[CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4] = {0};

    message.clockSpeedHz = 5000000;
    message.blobSizeWords = 0;

    cryptoMemory[DX_HOST_DCU_EN0_REG_OFFSET] = 0x00U;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);
    mock().expectOneCall("CC_BsvLcsGet")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
    mock().expectOneCall("MockCalloc")
        .withParameter("nItems", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES/4)
        .withParameter("size", sizeof(uint32_t))
        .andReturnValue(buffer);
    mock().expectOneCall("SystemMgrSetClockSpeed").withParameter("clockSpeedHz", message.clockSpeedHz);
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", &message.entitlement)
        .withParameter("certPkgSize", message.blobSizeWords*2)
        .withOutputParameterReturning("pEnableRmaMode", (uint32_t *)&enableRMA, sizeof(enableRMA))
        .withParameter("workspaceSize", CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES)
        .ignoreOtherParameters()
        .andReturnValue(CC_OK);

    mock().expectOneCall("DiagEnableInfo").withParameter("enable", false);

    mock().expectOneCall("MockFree").ignoreOtherParameters();

    error = LifecycleMgrOnApplyDebugEntitlement(commandCode, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseInvalid)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_NUM_PHASES;

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseUnsupported)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_BOOT;

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseInitialize)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_INITIALIZE;

    lifecycleMgr.debugEntitlementApplied = true;

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))LifecycleMgrOnDiagFatalErrorDump);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseDebugInitFailed)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_DEBUG;
    OtpmDrvErrorT errorOtpm;

    errorOtpm.isCorrectable = true;
    lifecycleMgr.debugEntitlementApplied = false;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_FAIL);
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);

    mock().expectOneCall("SystemMgrReportFatalError")
       .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_INIT_FAILED)
       .withParameter("infoParam", ERROR_SYSERR);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseDebug)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_DEBUG;
    OtpmDrvErrorT errorOtpm;
    uint32_t lcsCurrent;

    errorOtpm.isCorrectable = true;
    lifecycleMgr.debugEntitlementApplied = false;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    mock().expectOneCall("CC_BsvInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .andReturnValue(CC_OK);
    errorOtpm.isCorrectable = false;
    mock().expectOneCall("OtpmDrvGetLastError")
        .withOutputParameterReturning("error", (OtpmDrvErrorT *)&errorOtpm, sizeof(errorOtpm))
        .andReturnValue(ERROR_NOENT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
    mock().expectOneCall("CC_BsvLcsGetAndInit")
            .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
            .withOutputParameterReturning("pLcs", (uint32_t *)&lcsCurrent, sizeof(lcsCurrent))
            .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
    mock().expectOneCall("CC_BsvCoreClkGatingEnable")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
    mock().expectOneCall("CC_BsvSecModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isSecAccessMode", CC_FALSE)
        .withParameter("isSecModeLock", CC_FALSE)
        .andReturnValue(CC_OK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS);
    mock().expectOneCall("CC_BsvPrivModeSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withParameter("isPrivAccessMode", CC_FALSE)
        .withParameter("isPrivModeLock", CC_FALSE)
        .andReturnValue(CC_OK);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhasePatch)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_PATCH;

    lifecycleMgr.debugEntitlementApplied = false;
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U)
        .andReturnValue(CC_BSV_ILLEGAL_INPUT_PARAM_ERR);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);

    lifecycleMgr.debugEntitlementApplied = true;

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseConfigure)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_CONFIGURE;

    lifecycleMgr.debugEntitlementApplied = false;
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U)
        .andReturnValue(CC_BSV_ILLEGAL_INPUT_PARAM_ERR);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);

    lifecycleMgr.debugEntitlementApplied = false;
    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U)
        .andReturnValue(CC_BSV_AO_WRITE_FAILED_ERR);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_CONFIG_FAILED)
        .withParameter("infoParam", CC_BSV_AO_WRITE_FAILED_ERR);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);

    lifecycleMgr.debugEntitlementApplied = true;

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(true, lifecycleMgr.debugEntitlementApplied);
}

TEST(LifecycleMgrApiGroup, TestOnPhaseEntryPhaseShutdown)
{
    ErrorT error;
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_SHUTDOWN;

    lifecycleMgr.debugEntitlementApplied = false;

    mock().expectOneCall("CC_BsvFatalErrorSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE);

    mock().expectOneCall("CC_BsvSecureDebugSet")
        .withParameter("hwBaseAddress", CRYPTOCELL_CORE_BASE)
        .withPointerParameter("pDebugCertPkg", NULL)
        .withParameter("certPkgSize", 0U)
        .withPointerParameter("pEnableRmaMode", NULL)
        .withPointerParameter("pWorkspace", NULL)
        .withParameter("workspaceSize", 0U)
        .andReturnValue(CC_OK);

    error = LifecycleMgrOnPhaseEntry(phase);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(false, lifecycleMgr.debugEntitlementApplied);
}

