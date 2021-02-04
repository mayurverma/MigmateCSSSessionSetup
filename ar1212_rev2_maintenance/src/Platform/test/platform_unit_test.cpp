//***********************************************************************************
/// \file
///
/// Platform unit test
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
#include "Platform.h"
#include "SystemMgr.h"
#include "Diag.h"
#include "PlatformInternal.h"
}

// Mocks
extern "C"
{
    ErrorT SystemDrvInit(void)
    {
        mock().actualCall("SystemDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT WatchdogDrvInit(void)
    {
        mock().actualCall("WatchdogDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT OtpmDrvInit(void)
    {
        mock().actualCall("OtpmDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT CommandDrvInit(void)
    {
        mock().actualCall("CommandDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT MacDrvInit(void)
    {
        mock().actualCall("MacDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
}

void setupInitSuccess(void)
{
    mock().expectNCalls(3, "DiagSetCheckpoint")
        .ignoreOtherParameters();

    mock().expectNCalls(5, "DiagSetCheckpointWithInfo")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsInit")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsDisableAll")
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("WatchdogDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("MacDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("OtpmDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("CommandDrvInit")
        .ignoreOtherParameters();
}

TEST_GROUP(PlatformInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        platformState.criticalSectionNestLevel = 0;
    }
};

TEST_GROUP(PlatformApiGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        setupInitSuccess();

        ErrorT status = PlatformInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        platformState.criticalSectionNestLevel = 0;
    }
};

TEST(PlatformInitGroup, TestInit)
{
    setupInitSuccess();

    ErrorT status = PlatformInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(PlatformInitGroup, TestInitFailsIfInterruptInitFails)
{
    mock().expectNCalls(3, "DiagSetCheckpoint")
        .ignoreOtherParameters();

    mock().expectNCalls(5, "DiagSetCheckpointWithInfo")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsInit")
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("InterruptsDisableAll")
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("WatchdogDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("MacDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("OtpmDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("CommandDrvInit")
        .ignoreOtherParameters();

    ErrorT status = PlatformInit();
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(PlatformInitGroup, TestInitFailsIfPlatformInitDriversFails)
{
    mock().expectNCalls(3, "DiagSetCheckpoint")
        .ignoreOtherParameters();

    mock().expectNCalls(5, "DiagSetCheckpointWithInfo")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsInit")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsDisableAll")
        .ignoreOtherParameters();

    mock().expectOneCall("SystemDrvInit")
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("WatchdogDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("MacDrvInit")
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("OtpmDrvInit")
        .ignoreOtherParameters();

    mock().expectOneCall("CommandDrvInit")
        .ignoreOtherParameters();

    ErrorT status = PlatformInit();
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(PlatformInitGroup, TestInitFailsIfCriticalSectionNextLevelIsNonZero)
{
    mock().expectNCalls(2, "DiagSetCheckpoint")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsDisableAll");

    // Enter critical section but don't exit
    PlatformEnterCritical();

    ErrorT status = PlatformInit();
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(PlatformApiGroup, TestEnterExitCritical)
{
    mock().expectOneCall("InterruptsDisableAll");
    PlatformEnterCritical();

    PlatformExitCritical();

    mock().expectOneCall("InterruptsEnableAll");
    PlatformExitCritical();

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_PLATFORM_CRITICAL_SECTION_NESTING_NOT_ZERO)
        .withParameter("infoParam", 0);
    PlatformExitCritical();
}


