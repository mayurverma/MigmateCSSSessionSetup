//***********************************************************************************
/// \file
///
/// SysTick component unit test
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
#include "SysTickDrv.h"
#include "Diag.h"
}

TEST_GROUP(SysTickDrvInitGroup)
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

TEST_GROUP(SysTickDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT status = SysTickDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        ErrorT status = SysTickDrvStop();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }
};

TEST(SysTickDrvInitGroup, TestInit)
{
    ErrorT status = SysTickDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SysTickDrvGroup, TestStart)
{
    uint32_t num_ticks;

    ErrorT status = SysTickDrvStart(10000);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Wait for 20 ticks
    do
    {
        num_ticks = SysTickDrvGetCurrentTicks();
    } while (20 != num_ticks);
    LONGS_EQUAL(20, SysTickDrvGetCurrentTicks())
}

volatile uint32_t SysTickDrvGroup_UserTicks;

void SysTickDrvGroup_UserHandler(uint32_t const NumTicks)
{
    (void)NumTicks;
    SysTickDrvGroup_UserTicks += 1;
}

TEST(SysTickDrvGroup, TestUserHandler)
{
    SysTickDrvHandler CurrentHandler = SysTickDrvSetHandler(SysTickDrvGroup_UserHandler);
    LONGS_EQUAL(NULL, CurrentHandler);

    SysTickDrvGroup_UserTicks = 0;

    ErrorT status = SysTickDrvStart(10000);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Wait for 20 ticks
    do
    {
    } while (20 != SysTickDrvGroup_UserTicks);
    LONGS_EQUAL(20, SysTickDrvGroup_UserTicks);

    CurrentHandler = SysTickDrvSetHandler(CurrentHandler);
    LONGS_EQUAL(CurrentHandler, SysTickDrvGroup_UserHandler);
}

