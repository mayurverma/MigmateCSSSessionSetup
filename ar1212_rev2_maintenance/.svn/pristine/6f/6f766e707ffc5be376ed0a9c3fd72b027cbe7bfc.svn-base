//***********************************************************************************
/// \file
///
/// System Driver unit test
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
#include "SystemDrv.h"
#include "AR0820.h"
#include "crypto_icore.h"
#include "mcu_sysctrl.h"

#if defined(BUILD_TARGET_ARCH_x86)
CRYPTO_ICORE_REGS_T icoreRegsUnitTest = {};
MCU_SYSCTRL_REGS_T mcuSysctrlRegsUnitTest = {};
#endif
}

#if defined(BUILD_TARGET_ARCH_x86)
CRYPTO_ICORE_REGS_T* icore_regs = &icoreRegsUnitTest;
MCU_SYSCTRL_REGS_T* sysctrl_regs = &mcuSysctrlRegsUnitTest;

#else

// Get access to the icore registers
CRYPTO_ICORE_REGS_T* icore_regs = (CRYPTO_ICORE_REGS_T*)CRYPTO_ICORE_REG_BASE;

// Get access to the SysCtrl registers
MCU_SYSCTRL_REGS_T* sysctrl_regs = (MCU_SYSCTRL_REGS_T*)MCU_SYSCTRL_REG_BASE;

#endif

TEST_GROUP(SystemDrvInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
#if defined(BUILD_TARGET_ARCH_x86)
        memset(&icoreRegsUnitTest, 0, sizeof(icoreRegsUnitTest));
        memset(&mcuSysctrlRegsUnitTest, 0, sizeof(mcuSysctrlRegsUnitTest));
#endif
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(SystemDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
#if defined(BUILD_TARGET_ARCH_x86)
        memset(&icoreRegsUnitTest, 0, sizeof(icoreRegsUnitTest));
        memset(&mcuSysctrlRegsUnitTest, 0, sizeof(mcuSysctrlRegsUnitTest));

        // Set RESET_CONTROL default
        sysctrl_regs->reset_control = MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK;
#endif
        ErrorT status = SystemDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(SystemDrvInitGroup, TestInit)
{
    ErrorT status = SystemDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SystemDrvGroup, TestSetGetCheckpoint)
{
	icore_regs->checkpoint = 0;
    LONGS_EQUAL(0, icore_regs->checkpoint);

    SystemDrvSetCheckpoint(0x1234U);
    uint16_t checkpoint = SystemDrvGetCheckpoint();
    CHECK_EQUAL(0x1234U, checkpoint);

    SystemDrvSetCheckpoint(0xABCDU);
    checkpoint = SystemDrvGetCheckpoint();
    CHECK_EQUAL(0xABCDU, checkpoint);
    LONGS_EQUAL(0xABCDU, icore_regs->checkpoint);

    SystemDrvSetCheckpoint(0);
    LONGS_EQUAL(0x0, icore_regs->checkpoint);
}

TEST(SystemDrvGroup, TestGetPausepoint)
{
	icore_regs->pausepoint = 0;
    LONGS_EQUAL(0, icore_regs->pausepoint);

    uint16_t pausepoint = SystemDrvGetPausepoint();
    CHECK_EQUAL(0x0U, pausepoint);

#if defined(BUILD_TARGET_ARCH_x86)
	icore_regs->pausepoint = 0x42U;

    pausepoint = SystemDrvGetPausepoint();
    CHECK_EQUAL(0x42U, pausepoint);
#else
    // Cannot be tested under MinGW32 as relies on hardware functionality

    // Force pausepoint to 0xF234
    icore_regs->pausepoint = 0xF234U;
    // But check that the processor cannot write to the pausepoint field
    LONGS_EQUAL(0x8000U, icore_regs->pausepoint);

    // Get pausepoint, check all zero
    pausepoint = SystemDrvGetPausepoint();
    CHECK_EQUAL(0x0U, pausepoint);
#endif

    icore_regs->pausepoint = 0;
}

TEST(SystemDrvGroup, TestSetPausedIsPaused)
{
	icore_regs->pausepoint = 0;
    LONGS_EQUAL(0, icore_regs->pausepoint);

    // Set the pausepoint, but clear the paused bit
    icore_regs->pausepoint = 0x1234U;

    bool isPaused = SystemDrvIsPaused();
    CHECK_FALSE(isPaused);

    SystemDrvSetPaused();
    isPaused = SystemDrvIsPaused();
    CHECK_TRUE(isPaused);
    LONGS_EQUAL(0x8000U, icore_regs->pausepoint);

    icore_regs->pausepoint = 0;
}

TEST(SystemDrvGroup, TestGetBootOptions)
{
    uint16_t bootoptions = SystemDrvGetBootOptionFlags();
    CHECK_EQUAL(0x0U, bootoptions);
}

TEST(SystemDrvGroup, TestSetInfo)
{
	icore_regs->info = 0;
    LONGS_EQUAL(0, icore_regs->info);

    SystemDrvSetInfo(0xA546U);
    LONGS_EQUAL(0xA546U, icore_regs->info);

    icore_regs->info = 0;
}

#if !defined(BUILD_TARGET_ARCH_x86)
// Cannot be tested under MinGW32

// Counts the number of system ticks
uint32_t num_sys_ticks;

extern "C"
{
	// Overrides the default SysTick handler
	void SysTick_Handler(void)
	{
		num_sys_ticks += 1;
	}
}

TEST(SystemDrvGroup, TestWaitForInterrupt)
{
	// Set-up and enable the SysTick timer to generate an interrupt
	uint32_t res = SysTick_Config(5000);
	LONGS_EQUAL(0, res);
	NVIC_EnableIRQ(SysTick_IRQn);

	// Wait for an interrupt, and check it occurred
	num_sys_ticks = 0;
	SystemDrvWaitForInterrupt();
	LONGS_EQUAL(1, num_sys_ticks);

	NVIC_DisableIRQ(SysTick_IRQn);
}
#endif // defined(BUILD_TARGET_ARCH_x86)

TEST(SystemDrvGroup, TestEnableClockFailsIfInvalidClock)
{
    int clock = SYSTEM_DRV_CSS_CLOCK_VCORE;
    clock = clock + 1;
    ErrorT status = SystemDrvEnableClock((SystemDrvCssClockT)clock, true);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(SystemDrvGroup, TestEnableClockSucceedsForVcoreClock)
{
    uint32_t val = sysctrl_regs->reset_control;
    LONGS_EQUAL(MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK,
        val & MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK);

    SystemDrvCssClockT clock = SYSTEM_DRV_CSS_CLOCK_VCORE;
    ErrorT status = SystemDrvEnableClock(clock, false);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(0,
        val & MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK);

    status = SystemDrvEnableClock(clock, true);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK,
        val & MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK);
}

TEST(SystemDrvGroup, TestEnableCpuWait)
{
    uint32_t val = sysctrl_regs->reset_control;
    LONGS_EQUAL(0,
        val & MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK);

    SystemDrvEnableCpuWait(true);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK,
        val & MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK);

    SystemDrvEnableCpuWait(false);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(0,
        val & MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK);
}

TEST(SystemDrvGroup, TestEnableI2cWriteCrcReset)
{
    uint32_t val = sysctrl_regs->reset_control;
    LONGS_EQUAL(0,
        val & MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK);

    SystemDrvEnableI2cWriteCrcReset(false);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK,
        val & MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK);

    SystemDrvEnableI2cWriteCrcReset(true);

    val = sysctrl_regs->reset_control;
    LONGS_EQUAL(0,
        val & MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK);
}

TEST(SystemDrvGroup, TestSetCryptoError)
{
    uint32_t errorId = 0x3F;
    uint32_t context = 0x3FF;

    uint32_t val = icore_regs->cryptoerror;
    LONGS_EQUAL(0, val);

    SystemDrvSetCryptoError(errorId, context);

    val = icore_regs->cryptoerror;
    LONGS_EQUAL(0xFFFF, val);
}

TEST(SystemDrvGroup, TestIsPllLocked)
{
	bool is_locked;
	
#if defined(BUILD_TARGET_ARCH_x86)
    // Cannot be tested under MinGW32 as relies on hardware functionality

    sysctrl_regs->reset_control |= MCU_SYSCTRL_RESET_CONTROL_PLL_LOCK_STATUS__MASK;

    is_locked = SystemDrvIsPllLocked();
    CHECK_TRUE(is_locked);

    sysctrl_regs->reset_control &= ~MCU_SYSCTRL_RESET_CONTROL_PLL_LOCK_STATUS__MASK;
#endif

    is_locked = SystemDrvIsPllLocked();
    CHECK_FALSE(is_locked);
}

