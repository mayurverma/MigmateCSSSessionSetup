//***********************************************************************************
/// \file
///
/// OTPM Driver unit test
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
#include <string.h>
#include <math.h>
#include "Common.h"
#include "AR0820.h"
#include "crypto_otpm.h"
#include "DiagCheckpoint.h"
#include "OtpmDrv.h"
#include "OtpmDrvInternal.h"

}

#if defined(BUILD_TARGET_ARCH_x86)
uint32_t otpmArray[2048] = {0};
#endif

// Mocks
extern "C"
{

}

TEST_GROUP(OtpmDrvInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(OTPM_REGS, 0U, sizeof(CRYPTO_OTPM_REGS_T));
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(OtpmDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(otpmArray, 0U, sizeof(otpmArray));
    	memset(OTPM_REGS, 0U, sizeof(CRYPTO_OTPM_REGS_T));
        ErrorT status = OtpmDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(OtpmDrvGroupAccess)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(otpmArray, 0U, sizeof(otpmArray));
        memset(OTPM_REGS, 0U, sizeof(CRYPTO_OTPM_REGS_T));
        ErrorT status = OtpmDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(OtpmDrvGroupCalculation)
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

TEST_GROUP(OtpmDrvGroupConfig)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(OTPM_REGS, 0U, sizeof(CRYPTO_OTPM_REGS_T));
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(OtpmDrvInitGroup, TestInit)
{
    ErrorT status = OtpmDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_EQUAL(false ,otpmDrv.isConfigured);
}

TEST(OtpmDrvInitGroup, TestOtpmDrvGetUserSpaceAddress)
{
    OtpmDrvAddressT base = OtpmDrvGetUserSpaceAddress();
    LONGS_EQUAL(44U, base);
}

TEST(OtpmDrvGroup, TestGetLastError)
{
    OtpmDrvErrorT otpmError;
    ErrorT result;

    // Check no entry reported if no error is pending
    LONGS_EQUAL(0, OTPM_REGS->core_ecc_flags);
    result = OtpmDrvGetLastError(&otpmError);
    LONGS_EQUAL(ERROR_NOENT, result);


    // Force a single-bit error then check its reported
    OTPM_REGS->core_ecc_flags |= CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__MASK;

    result = OtpmDrvGetLastError(&otpmError);
    LONGS_EQUAL(ERROR_SUCCESS, result);
    CHECK_TRUE(otpmError.isCorrectable);
    LONGS_EQUAL(0, otpmError.errorAddress);

    // Check the error has been cleared
    LONGS_EQUAL(0, OTPM_REGS->core_ecc_flags);

    // Force an uncorrectable error then check its reported
    OTPM_REGS->core_ecc_flags |= CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK;
    OTPM_REGS->core_read_error = 0x1234U;
    otpmError.errorAddress = 0U;
    otpmError.isCorrectable = true;

    result = OtpmDrvGetLastError(NULL);
    LONGS_EQUAL(ERROR_INVAL, result);
    // Check the error has been cleared
    LONGS_EQUAL(0U, OTPM_REGS->core_ecc_flags);
    LONGS_EQUAL(0x1234U, OTPM_REGS->core_read_error);

    result = OtpmDrvGetLastError(&otpmError);
    LONGS_EQUAL(ERROR_NOENT, result);
    LONGS_EQUAL(0U, otpmError.errorAddress);
    CHECK_TRUE(otpmError.isCorrectable);

}

TEST(OtpmDrvGroup, TestIsHardwareFault)
{
    bool isFault = false;

    // Check no fault reported if no error is pending
    LONGS_EQUAL(0, OTPM_REGS->core_ecc_flags);
    isFault = OtpmDrvIsHardwareFault();
    CHECK_FALSE(isFault);

    // Force a single-bit error then check no fault reported
    OTPM_REGS->core_ecc_flags |= CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__MASK;
    isFault = OtpmDrvIsHardwareFault();
    CHECK_FALSE(isFault);

    // Force an uncorrectable error then check a fault is reported
    OTPM_REGS->core_ecc_flags |= CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK;
    isFault = OtpmDrvIsHardwareFault();
    CHECK_TRUE(isFault);

    // Clear the pending faults
    OTPM_REGS->core_ecc_flags = 0;
}

#define LONG_WORDS_TO_READ  16U

TEST(OtpmDrvGroupAccess, TestRead)
{
    uint32_t reference[LONG_WORDS_TO_READ];
    uint32_t buffer[LONG_WORDS_TO_READ];
    ErrorT result;
    uint32_t index;

    for (index = 0; index < LONG_WORDS_TO_READ; index++)
    {
        OTPM_BASE[index] = reference[index] = 0x20000000 + index;
    }
    result = OtpmDrvRead(0, buffer, LONG_WORDS_TO_READ);
    LONGS_EQUAL(ERROR_SUCCESS, result);
    MEMCMP_EQUAL(reference, buffer, sizeof(buffer));
}

TEST(OtpmDrvGroupAccess, TestReadError)
{
    uint32_t buffer[LONG_WORDS_TO_READ];
    ErrorT result;

    result = OtpmDrvRead((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), NULL, 1);
    LONGS_EQUAL(ERROR_INVAL, result);

    result = OtpmDrvRead((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), buffer, 0);
    LONGS_EQUAL(ERROR_INVAL, result);

    result = OtpmDrvRead((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), buffer, 1);
    LONGS_EQUAL(ERROR_RANGE, result);

    result = OtpmDrvRead((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t) - 1, buffer, 1);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    result = OtpmDrvRead(0, buffer, LONG_WORDS_TO_READ);
    LONGS_EQUAL(ERROR_SUCCESS, result);
}

TEST(OtpmDrvGroupAccess, TestWrite)
{
    uint32_t buffer[] = {1,2,3,4,5};
    uint32_t readback[16];
    uint32_t numLongWords = sizeof(buffer)/sizeof(buffer[0]);
    ErrorT result;

    otpmDrv.isConfigured = true;

    for (uint16_t index = 0; index < numLongWords; index++)
    {
        mock().expectOneCall("DiagSetCheckpointWithInfoUnprotected")
            .withParameter("checkpoint", DIAG_CHECKPOINT_OTPM_DRV_WRITE)
            .withParameter("info", index);
    }
    result = OtpmDrvWrite(0, buffer, numLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    result = OtpmDrvRead(0, readback, numLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    MEMCMP_EQUAL(readback, buffer, sizeof(buffer));
}

TEST(OtpmDrvGroupAccess, TestWriteNoneBlank)
{
    uint32_t buffer[] = {1,2,3,4,5,6};
    uint32_t numLongWords = sizeof(buffer)/sizeof(buffer[0]);
    ErrorT result;

    otpmDrv.isConfigured = true;
    otpmArray[3] =1U;

    for (uint16_t index = 0; index < 3; index++)
    {
        mock().expectOneCall("DiagSetCheckpointWithInfoUnprotected")
            .withParameter("checkpoint", DIAG_CHECKPOINT_OTPM_DRV_WRITE)
            .withParameter("info", index);
    }

    mock().expectOneCall("DiagSetCheckpointWithInfoUnprotected")
        .withParameter("checkpoint", DIAG_CHECKPOINT_OTPM_DRV_WRITE_ERROR)
        .withParameter("info", 3U);

    result = OtpmDrvWrite(0, buffer, numLongWords);
    LONGS_EQUAL(ERROR_IO, result);

}

TEST(OtpmDrvGroupAccess, TestWriteError)
{
    uint32_t buffer[LONG_WORDS_TO_READ];
    ErrorT result;

    otpmDrv.isConfigured = true;

    result = OtpmDrvWrite((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), NULL, 1);
    LONGS_EQUAL(ERROR_INVAL, result);

    result = OtpmDrvWrite((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), buffer, 0);
    LONGS_EQUAL(ERROR_INVAL, result);

    result = OtpmDrvWrite((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t), buffer, 1);
    LONGS_EQUAL(ERROR_RANGE, result);


    mock().expectOneCall("DiagSetCheckpointWithInfoUnprotected")
        .withParameter("checkpoint", DIAG_CHECKPOINT_OTPM_DRV_WRITE)
        .withParameter("info", 0U);
    result = OtpmDrvWrite((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA)/sizeof(uint32_t) - 1, buffer, 1);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    result = OtpmDrvRead(0, buffer, LONG_WORDS_TO_READ);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    otpmDrv.isConfigured = false;
    result = OtpmDrvWrite(0, buffer, 1);
    LONGS_EQUAL(ERROR_ACCESS, result);
}

TEST(OtpmDrvGroupAccess, TestRegWriteError)
{
    ErrorT result;
    OtpmRegisterWriteT const regWrite[1] = {33U*4U, 0x55U};

    result = OtpmDrvRegisterWrite(regWrite, 1U);
    LONGS_EQUAL(ERROR_RANGE, result);

}

TEST(OtpmDrvGroupCalculation, Test156M)
{
    uint32_t clocks;
    uint32_t inputClock = 156000000;

    clocks = OtpmDrvClockCycles(inputClock, 600000);
    LONGS_EQUAL(93600, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 300000);
    LONGS_EQUAL(46800, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 200000);
    LONGS_EQUAL(31200, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 10000);
    LONGS_EQUAL(1560, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1000);
    LONGS_EQUAL(156, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 17);
    LONGS_EQUAL(3, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 5);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 4);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 2);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1);
    LONGS_EQUAL(1, clocks);
}

TEST(OtpmDrvGroupCalculation, Test50M)
{
    uint32_t clocks;
    uint32_t inputClock = 50000000;

    clocks = OtpmDrvClockCycles(inputClock, 600000);
    LONGS_EQUAL(30000, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 300000);
    LONGS_EQUAL(15000, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 200000);
    LONGS_EQUAL(10000, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 10000);
    LONGS_EQUAL(500, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1000);
    LONGS_EQUAL(50, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 17);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 5);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 4);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 2);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1);
    LONGS_EQUAL(1, clocks);
}

TEST(OtpmDrvGroupCalculation, Test39M)
{
    uint32_t clocks;
    uint32_t inputClock = 39000000;

    clocks = OtpmDrvClockCycles(inputClock, 600000);
    LONGS_EQUAL(23400, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 300000);
    LONGS_EQUAL(11700, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 200000);
    LONGS_EQUAL(7800, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 10000);
    LONGS_EQUAL(390, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1000);
    LONGS_EQUAL(39, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 17);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 5);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 4);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 2);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1);
    LONGS_EQUAL(1, clocks);
}

TEST(OtpmDrvGroupCalculation, Test27M)
{
    uint32_t clocks;
    uint32_t inputClock = 27000000;

    clocks = OtpmDrvClockCycles(inputClock, 600000);
    LONGS_EQUAL(16200, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 300000);
    LONGS_EQUAL(8100, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 200000);
    LONGS_EQUAL(5400, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 10000);
    LONGS_EQUAL(270, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1000);
    LONGS_EQUAL(27, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 17);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 5);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 4);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 2);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1);
    LONGS_EQUAL(1, clocks);
}

TEST(OtpmDrvGroupCalculation, Test6M)
{
    uint32_t clocks;
    uint32_t inputClock = 6000000;

    clocks = OtpmDrvClockCycles(inputClock, 600000);
    LONGS_EQUAL(3600, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 300000);
    LONGS_EQUAL(1800, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 200000);
    LONGS_EQUAL(1200, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 10000);
    LONGS_EQUAL(60, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1000);
    LONGS_EQUAL(6, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 17);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 5);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 4);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 2);
    LONGS_EQUAL(1, clocks);
    clocks = OtpmDrvClockCycles(inputClock, 1);
    LONGS_EQUAL(1, clocks);
}


void TestConfigCheck(uint32_t inputClock)
{
    uint32_t data;

    data = OTPM_REGS->core_mr;
    LONGS_EQUAL(otpmDrvDefaultConfig.mr_pgm, data & 0xFFFF);
    LONGS_EQUAL(otpmDrvDefaultConfig.mr_pgm_soak, (data >> 16));
    data = OTPM_REGS->core_mr_mra;
    LONGS_EQUAL(otpmDrvDefaultConfig.mr_read, data & 0xFFFF);
    LONGS_EQUAL(otpmDrvDefaultConfig.mra_pgm, (data >> 16));
    data = OTPM_REGS->core_mra;
    LONGS_EQUAL(otpmDrvDefaultConfig.mra_prg_soak, data & 0xFFFF);
    LONGS_EQUAL(otpmDrvDefaultConfig.mra_read, (data >> 16));
    data = OTPM_REGS->core_mrb;
    LONGS_EQUAL(otpmDrvDefaultConfig.mrb_pgm, data & 0xFFFF);
    LONGS_EQUAL(otpmDrvDefaultConfig.mrb_prg_soak, (data >> 16));
    data = OTPM_REGS->core_mrb_tds;
    LONGS_EQUAL(otpmDrvDefaultConfig.mrb_read, data & 0xFFFF);
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tds_ns*inputClock/1000000000)), (data >> 16));
    data = OTPM_REGS->core_prog_time;
    LONGS_EQUAL(otpmDrvDefaultConfig.tpas_ns, data & 0xFFFF);
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tpp_ns*inputClock/1000000000)), (data >> 16));
    data = OTPM_REGS->core_recovery;
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tpr_ns*inputClock/1000000000)), data & 0xFFFF);
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tpsr_ns*inputClock/1000000000)), (data >> 16));
    data = OTPM_REGS->core_soak_cycles;
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tspp_ns*inputClock/1000000000)), data & 0xFFFF);
    data = OTPM_REGS->core_program;
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tvpps_ns*inputClock/1000000000)), data & 0xFFFF);
    LONGS_EQUAL(otpmDrvDefaultConfig.soak_count_max, (data >> 16));
    data = OTPM_REGS->core_write_time;
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.twr_ns*inputClock/1000000000)), data & 0xFFFF);
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.twp_ns*inputClock/1000000000)), (data >> 16));
    data = OTPM_REGS->core_read_time;
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.tras_ns*inputClock/1000000000)), data & 0xFFFF);
    LONGS_EQUAL((uint32_t)(ceil((double)(uint64_t)otpmDrvDefaultConfig.trr_ns*inputClock/1000000000)), (data >> 16));
}

TEST(OtpmDrvGroupConfig, TestConfigNull6M)
{
    ErrorT result;
    uint32_t inputClock = 6000000U;

    result = OtpmDrvConfigure(NULL, inputClock);
    LONGS_EQUAL(ERROR_SUCCESS, result);
    CHECK_EQUAL(true ,otpmDrv.isConfigured);
    TestConfigCheck(inputClock);
}

TEST(OtpmDrvGroupConfig, TestConfigNull)
{
    ErrorT result;
    uint32_t inputClock = 6000000U;

    result = OtpmDrvConfigure((AssetMgrOtpmConfigT *)&otpmDrvDefaultConfig, inputClock);
    LONGS_EQUAL(ERROR_SUCCESS, result);
    CHECK_EQUAL(true ,otpmDrv.isConfigured);
    TestConfigCheck(inputClock);
}

TEST(OtpmDrvGroupConfig, TestConfigClockError)
{
    ErrorT result;
    otpmDrv.isConfigured = false;

    result = OtpmDrvConfigure((AssetMgrOtpmConfigT *)&otpmDrvDefaultConfig, 1000000U);
    LONGS_EQUAL(ERROR_RANGE, result);
    CHECK_EQUAL(false ,otpmDrv.isConfigured);

    result = OtpmDrvConfigure((AssetMgrOtpmConfigT *)&otpmDrvDefaultConfig, 156000001);
    LONGS_EQUAL(ERROR_RANGE, result);
    CHECK_EQUAL(false ,otpmDrv.isConfigured);
}

