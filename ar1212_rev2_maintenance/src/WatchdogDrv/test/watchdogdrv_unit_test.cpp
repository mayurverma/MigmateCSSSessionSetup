//***********************************************************************************
/// \file
///
/// Watchdog Driver unit test
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
#include "WatchdogDrv.h"
#include "WatchdogDrvInternal.h"
}

#if defined(BUILD_TARGET_ARCH_x86)
uint32_t watchdogArray[769] = {0};
#endif

// Mocks
extern "C"
{

}

TEST_GROUP(WatchdogDrvInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
#if defined(BUILD_TARGET_ARCH_x86)
        memset(WATCHDOG_REGS, 0U, sizeof(WDOG_TypeDef));
#endif
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(WatchdogDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
#if defined(BUILD_TARGET_ARCH_x86)
        memset(WATCHDOG_REGS, 0U, sizeof(WDOG_TypeDef));
#endif
        ErrorT status = WatchdogDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(WatchdogDrvInitGroup, TestInit)
{
    ErrorT status = WatchdogDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(WatchdogDrvGroup, TestDriverStart)
{
    ErrorT status = WatchdogDrvStart(9999);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(9999, WATCHDOG_REGS->Load);
    LONGS_EQUAL(3, WATCHDOG_REGS->Control);
#if defined(BUILD_TARGET_ARCH_x86)
    LONGS_EQUAL(0, WATCHDOG_REGS->Lock);
#else
    // In real HW or VP, a read from this register returns the lock status rather than the value
    // written: 0 -- NOt locked; 1 -- Locked.
    LONGS_EQUAL(1, WATCHDOG_REGS->Lock);
#endif
}

TEST(WatchdogDrvGroup, TestDriverClearInterrupt)
{
    ErrorT status = WatchdogDrvClearInterrupt();
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(0, WATCHDOG_REGS->IntClr);
#if defined(BUILD_TARGET_ARCH_x86)
    LONGS_EQUAL(0, WATCHDOG_REGS->Lock);
#else
    // In real HW or VP, a read from this register returns the lock status rather than the value
    // written: 0 -- NOt locked; 1 -- Locked.
    LONGS_EQUAL(1, WATCHDOG_REGS->Lock);
#endif
}

TEST(WatchdogDrvGroup, TestDriverDisable)
{
    ErrorT status = WatchdogDrvDisable();
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(0, WATCHDOG_REGS->Control);
#if defined(BUILD_TARGET_ARCH_x86)
    LONGS_EQUAL(0, WATCHDOG_REGS->Lock);
#else
    // In real HW or VP, a read from this register returns the lock status rather than the value
    // written: 0 -- NOt locked; 1 -- Locked.
    LONGS_EQUAL(1, WATCHDOG_REGS->Lock);
#endif
}


TEST(WatchdogDrvGroup, TestDriverRegisterUnlock)
{
    WATCHDOG_REGS->Lock = WDOG_LOCK_ENABLE_KEY;
    WATCHDOG_REGS->Load = 9494;
    WATCHDOG_REGS->Control = 3;

    LONGS_EQUAL(9494, WATCHDOG_REGS->Load);
    LONGS_EQUAL(3, WATCHDOG_REGS->Control);
#if defined(BUILD_TARGET_ARCH_x86)
    LONGS_EQUAL(WDOG_LOCK_ENABLE_KEY, WATCHDOG_REGS->Lock);
#else
    // In real HW or VP, a read from this register returns the lock status rather than the value
    // written: 0 -- Not locked; 1 -- Locked.
    LONGS_EQUAL(0, WATCHDOG_REGS->Lock);
#endif
}
