//***********************************************************************************
/// \file
///
/// Patch Manager unit test
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
#include "cc_pal_init.h"
#include "cc_pal_dma_plat.h"
#include "cc_pal_log.h"
#include "cc_pal_mutex.h"
#include "cc_pal_pm.h"
#include "dx_reg_base_host.h"
#include "cc_hal.h"
#include "Diag.h"
#include "SystemMgr.h"
#include "CC312HalPal.h"
#include "cc_pal_trng.h"
#include "bsv_error.h"
#include "bsv_otp_api.h"
#include "cc_error.h"
#include "cc_pal_buff_attr.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_error.h"
#include "cc_pal_interrupt_ctrl_plat.h"

#define CC_TEE
#include "cc_regs.h"
#include "dx_host.h"

extern CcPalTrngT ccPalTrng;
extern CCError_t CC_PalSecMemCmp(const uint8_t* aTarget, const uint8_t* aSource, size_t aSize);
extern CCError_t CC_BsvErrorConversion(ErrorT error);

#define LONG_WORDS_TO_READ  16U

// To support MinGW32 based unit testing HW register accessing
uint32_t cc_reg_array[0x4000];

}   // End of extern "C"

TEST_GROUP(CC312HalPalInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        CC_HalInit();
        ccPalTrng.isPalTrngParamSet  = CC_FALSE;
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(CC312HalPalGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        AssetMgrCC312TrngCharT CC312TrngChar;  // Set to defaults

        // Something wrong with NVM or Asset not present.
        // Set the default values that work on RTL Sims.
        CC312TrngChar.R0 = CC_CONFIG_SAMPLE_CNT_ROSC_1;
        CC312TrngChar.R1 = CC_CONFIG_SAMPLE_CNT_ROSC_2;
        CC312TrngChar.R2 = CC_CONFIG_SAMPLE_CNT_ROSC_3;
        CC312TrngChar.R3 = CC_CONFIG_SAMPLE_CNT_ROSC_4;
        ccPalTrng.isPalTrngParamSet  = CC_FALSE;

        CC_PalTrngParamSet(&CC312TrngChar);

    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(CC312HalPalInitGroup, TestInit)
{
    AssetMgrCC312TrngCharT CC312TrngChar;  // Set to defaults

    // Something wrong with NVM or Asset not present.
    // Set the default values that work on RTL Sims.
    CC312TrngChar.R0 = CC_CONFIG_SAMPLE_CNT_ROSC_1;
    CC312TrngChar.R1 = CC_CONFIG_SAMPLE_CNT_ROSC_2;
    CC312TrngChar.R2 = CC_CONFIG_SAMPLE_CNT_ROSC_3;
    CC312TrngChar.R3 = CC_CONFIG_SAMPLE_CNT_ROSC_4;

    CHECK_EQUAL(CC_FALSE, ccPalTrng.isPalTrngParamSet);

    CC_PalTrngParamSet(&CC312TrngChar);

    CHECK_EQUAL(DX_BASE_CC, gCcRegBase);
    CHECK_EQUAL(CC_TRUE, ccPalTrng.isPalTrngParamSet);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_1, ccPalTrng.cc312PalTrngParameter.SubSamplingRatio1);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_2, ccPalTrng.cc312PalTrngParameter.SubSamplingRatio2);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_3, ccPalTrng.cc312PalTrngParameter.SubSamplingRatio3);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_4, ccPalTrng.cc312PalTrngParameter.SubSamplingRatio4);

    // Check trying to load a second time (shouldn't change the internal TRNG values)
    CC312TrngChar.R0 = 0;
    CC_PalTrngParamSet(&CC312TrngChar);
    CHECK_EQUAL(CC_TRUE, ccPalTrng.isPalTrngParamSet);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_1, ccPalTrng.cc312PalTrngParameter.SubSamplingRatio1);

}


TEST(CC312HalPalGroup, TestPalTrngParamGet)
{
    CCError_t error = CC_OK;
    CC_PalTrngParams_t PalTrngParamSet;
    size_t ParamSize = sizeof(CC_PalTrngParams_t);

    error = CC_PalTrngParamGet(&PalTrngParamSet, &ParamSize);
    CHECK_EQUAL(CC_OK, error);
    CHECK_EQUAL(sizeof(CC_PalTrngParams_t), ParamSize);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_1, PalTrngParamSet.SubSamplingRatio1);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_2, PalTrngParamSet.SubSamplingRatio2);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_3, PalTrngParamSet.SubSamplingRatio3);
    CHECK_EQUAL(CC_CONFIG_SAMPLE_CNT_ROSC_4, PalTrngParamSet.SubSamplingRatio4);
}

TEST(CC312HalPalGroup, TestPalTrngParamGetErrors)
{
    CCError_t error = CC_OK;
    CC_PalTrngParams_t PalTrngParamSet;
    size_t ParamSize = sizeof(CC_PalTrngParams_t);
    size_t ParamSizeBad = ParamSize + 1;

    error = CC_PalTrngParamGet(NULL, &ParamSize);
    CHECK_EQUAL(CC_FAIL, error);

    error = CC_PalTrngParamGet(&PalTrngParamSet, NULL);
    CHECK_EQUAL(CC_FAIL, error);

    error = CC_PalTrngParamGet(&PalTrngParamSet, &ParamSizeBad);
    CHECK_EQUAL(CC_FAIL, error);

    ccPalTrng.isPalTrngParamSet  = CC_FALSE;
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();
    error = CC_PalTrngParamGet(&PalTrngParamSet, &ParamSize);
    CHECK_EQUAL(CC_FAIL, error);
}


TEST(CC312HalPalGroup, TestBsvErrorConversion)
{
    CCError_t ccError = CC_OK;

    ccError = CC_BsvErrorConversion(ERROR_NOENT);
    CHECK_EQUAL(CC_BSV_OTP_WRITE_CMP_FAIL_ERR, ccError);

    ccError = CC_BsvErrorConversion(ERROR_INVAL);
    CHECK_EQUAL(CC_BSV_ILLEGAL_INPUT_PARAM_ERR, ccError);

    ccError = CC_BsvErrorConversion(ERROR_ALREADY);
    CHECK_EQUAL(CC_BSV_BASE_ERROR, ccError);
}

TEST(CC312HalPalGroup, TestBsvOtpmRead)
{
    uint32_t valueInOtpm = 55;
    CCError_t error = CC_OK;

    mock().expectOneCall("OtpmDrvRead")
          .withParameter("address", 0)
          .withOutputParameterReturning("buffer", &valueInOtpm, sizeof(uint32_t))
          .withParameter("numLongWords", 1);

    error = CC_BsvOTPWordRead(0, 0, &valueInOtpm);
    CHECK_EQUAL(CC_OK, error);
    CHECK_EQUAL(55, valueInOtpm);
}


TEST(CC312HalPalGroup, TestBsbOtpmWrite)
{
    uint32_t valueToOtpm = 55;
    uint32_t valueReadFromOtpm = 44;
    CCError_t error = CC_OK;

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 0)
        .withParameter("numLongWords", 1)
        .ignoreOtherParameters()
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("OtpmDrvRead")
          .withParameter("address", 0)
          .withOutputParameterReturning("buffer", &valueToOtpm, sizeof(uint32_t))
          .withParameter("numLongWords", 1);

    error = CC_BsvOTPWordWrite(0, 0, valueToOtpm);
    CHECK_EQUAL(CC_OK, error);

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 0)
        .withParameter("numLongWords", 1)
        .ignoreOtherParameters()
        .andReturnValue(ERROR_RANGE);

    error = CC_BsvOTPWordWrite(0, 0, valueToOtpm);
    CHECK_EQUAL(CC_BSV_BASE_ERROR, error);

    mock().expectOneCall("OtpmDrvWrite")
        .withParameter("address", 0)
        .withParameter("numLongWords", 1)
        .ignoreOtherParameters()
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("OtpmDrvRead")
          .withParameter("address", 0)
          .withOutputParameterReturning("buffer", &valueReadFromOtpm, sizeof(uint32_t))
          .withParameter("numLongWords", 1);

    error = CC_BsvOTPWordWrite(0, 0, valueToOtpm);
    CHECK_EQUAL(CC_BSV_OTP_WRITE_CMP_FAIL_ERR, error);
}


TEST(CC312HalPalGroup, TestPalInitTerminate)
{
    int32_t error = CC_OK;

    error = CC_PalInit();
    CHECK_EQUAL(CC_OK, error);

    CC_PalTerminate();
}

TEST(CC312HalPalGroup, TestPalPowerSave)
{
    int32_t status = CC_OK;
    CCError_t error = CC_OK;

    CC_PalPowerSaveModeInit();

    status = CC_PalPowerSaveModeStatus();
    CHECK_EQUAL(CC_OK, status);

    error = CC_PalPowerSaveModeSelect(CC_TRUE);
    CHECK_EQUAL(CC_OK, error);
}

TEST(CC312HalPalGroup, TestPalMutex)
{
    CCError_t error = CC_OK;
    CC_PalMutex MutexId = 1;

    error = CC_PalMutexLock(&MutexId, 1000);
    CHECK_EQUAL(CC_OK, error);

    error = CC_PalMutexUnlock(&MutexId);
    CHECK_EQUAL(CC_OK, error);
}


TEST(CC312HalPalGroup, TestPalBuffAttr)
{
    CCError_t error = CC_OK;
    unsigned char DataBuffer[10];
    uint8_t BuffNSecure[1];

    error = CC_PalDataBufferAttrGet(&DataBuffer[0], 10, 1, &BuffNSecure[0]);
    CHECK_EQUAL(CC_OK, error);
    CHECK_EQUAL(BuffNSecure[0], DATA_BUFFER_IS_SECURE);
}


TEST(CC312HalPalGroup, TestPalAbort)
{
    char expression[] = "This test";

    mock().expectOneCall("CryptoMgrOnPalAbort");

    CC_PalAbort(expression);
}

TEST(CC312HalPalGroup, TestPalMemFunctions)
{
    char source[] = "This is a string";
    char dest[17];
    char *strPtr = NULL;
    uint8_t source1[] = {0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U};
    uint8_t dest1[17];
    int32_t result;
    CCError_t error;

    strPtr = (char *)CC_PalMemMallocPlat(sizeof(source));
    CHECK(NULL != strPtr);

    strPtr = (char *)CC_PalMemReallocPlat(strPtr, 20);
    CHECK(NULL != strPtr);

    CC_PalMemFreePlat(strPtr);

    CC_PalMemCopyPlat(dest, source, sizeof(source));
    result = CC_PalMemCmpPlat(dest, source, sizeof(source));
    CHECK_EQUAL(0, result);

    CC_PalMemSetPlat(dest, 'A', sizeof(source));

    CC_PalMemMovePlat(dest1, source1, sizeof(source1));
    error = CC_PalSecMemCmp(dest1, source1, sizeof(source1));
    CHECK_EQUAL(CC_SUCCESS, error);

    CC_PalMemSetZeroPlat(dest1, sizeof(source1));
    error = CC_PalSecMemCmp(dest1, source1, sizeof(source1));
    CHECK_EQUAL(CC_PAL_MEM_BUF2_GREATER, error);

    CC_PalMemMovePlat(dest1, source1, sizeof(source1));
    CC_PalMemSetZeroPlat(source1, sizeof(source1));
    error = CC_PalSecMemCmp(dest1, source1, sizeof(source1));
    CHECK_EQUAL(CC_PAL_MEM_BUF1_GREATER, error);
}


TEST(CC312HalPalGroup, TestPalInterrupt)
{
    CCError_t error = CC_OK;
    gCcRegBase = (unsigned long)&cc_reg_array[0];

    error = CC_PalInitIrq();
    CHECK_EQUAL(CC_SUCCESS, error);

    CC_PalFinishIrq();

    error = CC_PalWaitInterrupt(0x01U);
    CHECK_EQUAL(CC_FAIL, error);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR), (1U << DX_HOST_IRR_AHB_ERR_INT_BIT_SHIFT));
    error = CC_PalWaitInterrupt(0x01U << DX_HOST_IRR_AHB_ERR_INT_BIT_SHIFT);
    CHECK_EQUAL(CC_FAIL, error);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR), (1U << DX_HOST_IRR_RNG_INT_BIT_SHIFT));
    error = CC_PalWaitInterrupt(0x01U << DX_HOST_IRR_RNG_INT_BIT_SHIFT);
    CHECK_EQUAL(CC_OK, error);

    error = CC_PalWaitInterrupt(0x01U);
    CHECK_EQUAL(CC_FAIL, error);

    gCcRegBase = DX_BASE_CC;
}

TEST(CC312HalPalGroup, TestHalInterrupt)
{
    CCError_t error = CC_OK;
    int result;

    result = CC_HalTerminate();
    CHECK_EQUAL(CC_HAL_OK, result);

    gCcRegBase = (unsigned long)&cc_reg_array[0];

    CC_HalClearInterruptBit(0x40);

    CC_HalMaskInterrupt(0x40);

    error = CC_HalWaitInterrupt(0x40U);
    CHECK_EQUAL(CC_FAIL, error);

    error = CC_HalWaitInterrupt(0x00U);
    CHECK_EQUAL(CC_FATAL_ERROR, error);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR), 0);
    error = CC_HalWaitInterruptRND(0x01U);
    CHECK_EQUAL(CC_FATAL_ERROR, error);

    error = CC_HalWaitInterruptRND(0x00U);
    CHECK_EQUAL(CC_FATAL_ERROR, error);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR), (1U << DX_HOST_IRR_AHB_ERR_INT_BIT_SHIFT));
    error = CC_HalWaitInterruptRND(0x01U << DX_HOST_IRR_AHB_ERR_INT_BIT_SHIFT);
    CHECK_EQUAL(CC_FATAL_ERROR, error);

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR), (1U << DX_HOST_IRR_RNG_INT_BIT_SHIFT));
    error = CC_HalWaitInterruptRND(0x01U << DX_HOST_IRR_RNG_INT_BIT_SHIFT);
    CHECK_EQUAL(CC_OK, error);

    gCcRegBase = DX_BASE_CC;
}

