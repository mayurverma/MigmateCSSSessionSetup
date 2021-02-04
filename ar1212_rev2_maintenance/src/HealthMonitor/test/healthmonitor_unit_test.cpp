//***********************************************************************************
/// \file
///
/// Health Monitor unit test
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
#include <string.h>

#include "cpputest_support.h"

extern "C"
{
#include "Diag.h"
#include "LifecycleMgr.h"
#include "PatchMgr.h"
#include "Init.h"
#include "HealthMonitor.h"
#include "HealthMonitorInternal.h"
#include "Interrupts.h"
#include "crypto_vcore.h"
}

#if defined(BUILD_TARGET_ARCH_x86)
// To support MinGW32 unit testing model the stack as a memory buffer
uint8_t stack[4U * 1024U];
#endif

// Mocks
extern "C"
{
}

// helper function
uint32_t StackEmpty(uint32_t lengthLongWords)
{
    uint32_t protectedRegionSizeLongWords = 8U;
    uint32_t stackSizeLongWords = sizeof(stack)/sizeof(uint32_t);
    uint32_t usedStackLongWords;
    uint32_t* stackLongWords = (uint32_t*)&stack[0];

    usedStackLongWords = (lengthLongWords >= (stackSizeLongWords - protectedRegionSizeLongWords)) ?
            stackSizeLongWords : lengthLongWords;

    memset(stack, 0U, sizeof(stack));

    for (uint32_t i = 0; i < (stackSizeLongWords - lengthLongWords); i++)
    {
        *((uint32_t*)&stackLongWords[i]) = HEALTH_MONITOR_STACK_INIT_VALUE;
    }
    // printf(">>> Used %d <<<\n", usedStackLongWords);
    return usedStackLongWords;
}



TEST_GROUP(HealthMonitorInitGroup)
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

TEST_GROUP(HealthMonitorGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        mock().expectOneCall("DiagSetCheckpoint")
            .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_INIT);

        mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
            .withFunctionPointerParameter("notifier", (void (*)(void))HealthMonitorOnClockChange);

        ErrorT status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(HealthMonitorInitGroup, TestHealthMonitorOnPhaseEntryInit)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_INIT);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
          .withFunctionPointerParameter("notifier", (void (*)(void))HealthMonitorOnClockChange);

    ErrorT status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // test the globals have been initialised
    CHECK_EQUAL(healthMonitorState.wdgInterruptCtr, 0U);
    CHECK_EQUAL(healthMonitorState.wdgTimerIntervalMicrosecs, 50000U);
    CHECK_EQUAL(healthMonitorState.stackHwmDangerZoneSizeLongWords, 16);
    CHECK_EQUAL(healthMonitorState.wdgInterruptCtrMax, 19);    // 50ms timer interval x (healthMonitorState.wdgInterruptCtrMax + 1) = 1 Sec
}

TEST(HealthMonitorInitGroup, TestHealthMonitorOnPhaseEntryInitWithFailedRegisterClockSpeed)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_INIT);

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
          .withFunctionPointerParameter("notifier", (void (*)(void))HealthMonitorOnClockChange)
          .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_REGISTER_CLOCK_CHANGE_NOTIFIER_FAILED)
          .withParameter("infoParam", ERROR_SYSERR);

    ErrorT status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SYSERR, status);

    // test the globals have been initialised
    CHECK_EQUAL(healthMonitorState.wdgInterruptCtr, 0U);
    CHECK_EQUAL(healthMonitorState.wdgTimerIntervalMicrosecs, 50000U);
    CHECK_EQUAL(healthMonitorState.stackHwmDangerZoneSizeLongWords, 16);
    CHECK_EQUAL(healthMonitorState.wdgInterruptCtrMax, 19);    // 50ms timer interval x (healthMonitorState.wdgInterruptCtrMax + 1) = 1 Sec
}

TEST(HealthMonitorInitGroup, TestHealthMonitorOnPhaseEntryShutDown)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ENTER_SHUTDOWN);

    mock().expectOneCall("WatchdogDrvDisable");
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_WATCHDOG);

    ErrorT status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(HealthMonitorInitGroup, TestHealthMonitorOnPhaseEntryUnsupported)
{
    ErrorT status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_NUM_PHASES);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(HealthMonitorGroup, TestGetMonitorStatus)
{
    uint8_t commandParams[64];
    ErrorT status;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    HealthMonitorGetStatusResponseParamsT* responseParams =
        (HealthMonitorGetStatusResponseParamsT*)commandParams;
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("PatchMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_INITIALIZE);
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_INIT);
    status = PatchMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);
    mock().expectOneCall("SystemMgrGetCurrentPhase");
    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent))
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    mock().expectOneCall("SystemMgrRegisterClockSpeedChangeNotifier")
          .withFunctionPointerParameter("notifier", (void (*)(void))HealthMonitorOnClockChange);

    status = HealthMonitorOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = HealthMonitorOnGetStatus(  HOST_COMMAND_CODE_GET_STATUS,
                                        (CommandHandlerCommandParamsT)&commandParams);
    responseParams->currentLcs = lcsCurrent;
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_EQUAL(SYSTEM_MGR_PHASE_INITIALIZE, responseParams->currentPhase);
    CHECK_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE, responseParams->currentLcs);
    CHECK_EQUAL(0x1234, responseParams->romVersion);         // From the mock function call.
    CHECK_EQUAL(0, responseParams->patchVersion);
}

TEST(HealthMonitorGroup, TestGetStatus)
{
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    InitMpuStackConfigType stackConfig;
    HealthMonitorGetStatusResponseParamsT   getStatusResponse;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);

    mock().expectOneCall("SystemMgrGetCurrentPhase");
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");
    mock().expectOneCall("InitMpuGetStackInformation")
         .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    ErrorT status = HealthMonitorOnGetStatus(HOST_COMMAND_CODE_GET_STATUS,
                                            (CommandHandlerCommandParamsT)&getStatusResponse);
    // tests the responses
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(LIFECYCLE_MGR_LIFECYCLE_STATE_CM, getStatusResponse.currentLcs);
    LONGS_EQUAL(SYSTEM_MGR_PHASE_INITIALIZE, getStatusResponse.currentPhase);
    LONGS_EQUAL(0, getStatusResponse.patchVersion);
    LONGS_EQUAL(0x1234, getStatusResponse.romVersion);
    CHECK(getStatusResponse.stackUsedLongWords != 0);
}

TEST(HealthMonitorGroup, TestGetStatusWithBadCommand)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STATUS_BAD_COMMAND)
          .withParameter("infoParam", HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT);

    uint8_t response[64];
    ErrorT status = HealthMonitorOnGetStatus(HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT,
                                            (CommandHandlerCommandParamsT)&response);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(HealthMonitorGroup, TestGetStatusWithErrorFromLifecycleMgr)
{
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);

    mock().expectOneCall("SystemMgrGetCurrentPhase");
    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent))
        .andReturnValue(ERROR_SYSERR);

    HealthMonitorGetStatusResponseParamsT response;
    ErrorT status = HealthMonitorOnGetStatus(HOST_COMMAND_CODE_GET_STATUS,
                                            (CommandHandlerCommandParamsT)&response);
    LONGS_EQUAL(ERROR_SYSERR, status);
    LONGS_EQUAL(HEALTH_MONITOR_LIFE_CYCLE_INVALID, response.currentLcs);
}

TEST(HealthMonitorGroup, TestGetStatusWithErrorFromGetStackInformation)
{
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    InitMpuStackConfigType stackConfig;
    HealthMonitorGetStatusResponseParamsT   getStatusResponse;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_GET_STATUS);

    mock().expectOneCall("SystemMgrGetCurrentPhase");
    mock().expectOneCall("SystemMgrGetRomVersion");
    mock().expectOneCall("PatchMgrGetPatchVersion");
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig))
        .andReturnValue(ERROR_INVAL);
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_INFORMATION_ERROR)
        .withParameter("infoParam", ERROR_INVAL);

    ErrorT status = HealthMonitorOnGetStatus(HOST_COMMAND_CODE_GET_STATUS,
                                            (CommandHandlerCommandParamsT)&getStatusResponse);
    LONGS_EQUAL(ERROR_INVAL, status);
    LONGS_EQUAL(HEALTH_MONITOR_STACK_USED_INVALID, getStatusResponse.stackUsedLongWords);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringExtClock)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    InitMpuStackConfigType          stackConfig;
    uint32_t                        expectedWdgTimerLoadValueExt, expectedWdgTimerLoadValuePll;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =  10123000 ;   // 10.123 MHz
    decryptedParams.pllFreqHz    = 100100000;    // 100.1 MHz

    printf("Enable Monitoring Test Clock Frequency  EXT: %d Hz\n",
            decryptedParams.extClkFreqHz);

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(false);                       // using EXT clock

    mock().expectOneCall("SystemMgrSetClockSpeed")
            .withParameter("clockSpeedHz", decryptedParams.extClkFreqHz);

    mock().expectOneCall("WatchdogDrvInit");
    mock().expectOneCall("WatchdogDrvDisable");

    mock().expectOneCall("WatchdogDrvStart")
          .withParameter("timeOutClocks", (uint32_t)((uint64_t)decryptedParams.extClkFreqHz * 50000U / 1000000U));  // gives 50ms WDG period

    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_WATCHDOG);

    mock().expectOneCall("SystemDrvEnableCpuWait")
          .withParameter("enable", true);

    mock().expectOneCall("InterruptsEnableInterrupt")
          .withParameter("interrupt", INTERRUPT_WATCHDOG)
          .withParameter("enable", true);

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_SUCCESS, status);

    // check some globals are set correctly
    expectedWdgTimerLoadValueExt = (uint32_t)((uint64_t)decryptedParams.extClkFreqHz * 50000U / 1000000U);   // 50000 us wdg timer period
    expectedWdgTimerLoadValuePll = (uint32_t)((uint64_t)decryptedParams.pllFreqHz * 50000U / 1000000U);   // 50000 us wdg timer period
    CHECK(healthMonitorState.isWatchdogActive == true);
    LONGS_EQUAL(expectedWdgTimerLoadValueExt, healthMonitorState.wdgTimerReloadValueExt);
    LONGS_EQUAL(expectedWdgTimerLoadValuePll, healthMonitorState.wdgTimerReloadValuePll);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringPllClock)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    InitMpuStackConfigType          stackConfig;
    uint32_t                        expectedWdgTimerLoadValueExt, expectedWdgTimerLoadValuePll;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =   9900000;    // 9.9 MHz
    decryptedParams.pllFreqHz    = 100500000;    // 100.5 MHz

    printf("Enable Monitoring Test Clock Frequency  PLL: %d Hz\n",
            decryptedParams.pllFreqHz);

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(true);                       // using PLL clock

    mock().expectOneCall("SystemMgrSetClockSpeed")
            .withParameter("clockSpeedHz", decryptedParams.pllFreqHz);

    mock().expectOneCall("WatchdogDrvInit");
    mock().expectOneCall("WatchdogDrvDisable");

    mock().expectOneCall("WatchdogDrvStart")
          .withParameter("timeOutClocks", (uint32_t)((uint64_t)decryptedParams.pllFreqHz * 50000U / 1000000U));  // gives 50ms WDG period

    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_WATCHDOG);

    mock().expectOneCall("SystemDrvEnableCpuWait")
          .withParameter("enable", true);

    mock().expectOneCall("InterruptsEnableInterrupt")
          .withParameter("interrupt", INTERRUPT_WATCHDOG)
          .withParameter("enable", true);

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_SUCCESS, status);

    // check some globals are set correctly
    expectedWdgTimerLoadValueExt = (uint32_t)((uint64_t)decryptedParams.extClkFreqHz * 50000U / 1000000U);   // 50000 us wdg timer period
    expectedWdgTimerLoadValuePll = (uint32_t)((uint64_t)decryptedParams.pllFreqHz * 50000U / 1000000U);   // 50000 us wdg timer period
    CHECK(healthMonitorState.isWatchdogActive == true);
    LONGS_EQUAL(expectedWdgTimerLoadValueExt, healthMonitorState.wdgTimerReloadValueExt);
    LONGS_EQUAL(expectedWdgTimerLoadValuePll, healthMonitorState.wdgTimerReloadValuePll);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringInvalidCommand)
{
    ErrorT status;
    uint8_t expected[64] = {0};
    uint8_t response[64] = {0};

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_ENABLE_MONITORING_BAD_COMMAND)
        .withParameter("infoParam", HOST_COMMAND_CODE_NULL);

    status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_NULL ,
                                            (CommandHandlerCommandParamsT)&response);
    LONGS_EQUAL(ERROR_SYSERR, status);
    MEMCMP_EQUAL(expected, response, sizeof(expected));
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithFailedDecryptionOfCommandParams)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =   7900000;    // 7.9MHz
    decryptedParams.pllFreqHz    =  95000000;    // 95MHz

    printf("Enable Monitoring Test Clock Frequency  EXT: %d Hz\n",
            decryptedParams.extClkFreqHz);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_BADMSG);

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithFailedSetClockSpeed)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    InitMpuStackConfigType          stackConfig;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =  11110000;    // 11.11 MHz
    decryptedParams.pllFreqHz    = 123450000;    // 123.45 MHz

    printf("Enable Monitoring Test Clock Frequency  EXT: %d Hz\n",
            decryptedParams.extClkFreqHz);

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(false);                       // using EXT clock

    mock().expectOneCall("SystemMgrSetClockSpeed")
            .withParameter("clockSpeedHz", decryptedParams.extClkFreqHz)
            .andReturnValue(ERROR_INVAL);       // eg one of the component notifiers fails

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_INVAL, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithExtClockParamTooLow)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;     // anything except SECURE

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321UL;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432UL;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =   5999999UL;    // 5.9 MHz, too low
    decryptedParams.pllFreqHz    = 100000000UL;    // 100MHz

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &lcsCurrent, sizeof(lcsCurrent));

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithExtClockParamTooHigh)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;     // anything except SECURE

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321UL;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432UL;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz =  50000001UL;    // just over 50 MHz, too high
    decryptedParams.pllFreqHz    = 100000000UL;    // 100MHz, OK

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &lcsCurrent, sizeof(lcsCurrent));

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithPllClockParamTooLow)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_RMA;     // anything except SECURE

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321UL;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432UL;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz = 7000000UL;    // 7 MHz, OK
    decryptedParams.pllFreqHz    = 5999999UL;    // 5.9 MHz, too low

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &lcsCurrent, sizeof(lcsCurrent));

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithPllClockParamTooHigh)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321U;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432U;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz = 7000000UL;       // 7 MHz, OK
    decryptedParams.pllFreqHz    = 156000001UL;     // Just over 156 MHz, too high

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &lcsCurrent, sizeof(lcsCurrent));

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_RANGE, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringWithPllClockParamTooHighSecure)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;
    HealthMonitorSystemClockFreqT   decryptedParams;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    // clear the global
    healthMonitorState.isWatchdogActive = false;

    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321U;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432U;      // Not a real freq, just big number for testing

    decryptedParams.extClkFreqHz = 7000000UL;       // 7 MHz, OK
    decryptedParams.pllFreqHz    = 156000001UL;     // Just over 156 MHz, too high

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    mock().expectOneCall("SessionMgrDecryptCommandRequestParams")
          .withParameter("commandCode", HOST_COMMAND_CODE_ENABLE_MONITORING)
          .withParameter("encryptedParams", &encryptedParams)
          .withOutputParameterReturning("decryptedParams", &decryptedParams, sizeof(HealthMonitorSystemClockFreqT))
          .withParameter("paramSizeBytes", sizeof(HealthMonitorSystemClockFreqT))
          .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
          .withOutputParameterReturning("lcsCurrent", &lcsCurrent, sizeof(lcsCurrent));

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_BADMSG, status);
    CHECK(healthMonitorState.isWatchdogActive == false);
}

TEST(HealthMonitorGroup, TestOnEnableMonitoringTwiceErrorAlready)
{
    HealthMonitorEnableMonitoringRequestParamsT encryptedParams;

    // set the global
    healthMonitorState.isWatchdogActive = true;
    encryptedParams.encryptedParams.extClkFreqHz = 0x87654321;      // Not a real freq, just big number for testing
    encryptedParams.encryptedParams.pllFreqHz    = 0x98765432;      // Not a real freq, just big number for testing

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_HEALTH_MONITOR_ON_ENABLE_MONITORING);

    ErrorT status = HealthMonitorOnEnableMonitoring(HOST_COMMAND_CODE_ENABLE_MONITORING, (CommandHandlerCommandParamsT)&encryptedParams);

    LONGS_EQUAL(ERROR_ALREADY, status);

    // check some globals are set correctly
    CHECK(healthMonitorState.isWatchdogActive == true);
}

TEST(HealthMonitorGroup, TestWatchdog_IrqHandlerWdgInactive)
{
    healthMonitorState.isWatchdogActive = false;

    // the expectation is that nothing will happen

    Watchdog_IrqHandler();
}

TEST(HealthMonitorGroup, TestWatchdog_IrqHandlerWdgActiveFreqChangeToExt)
{
#define TEST_FREQUENCY1_EXT_CLK_HZ     6100000U   // 6.1 MHz
#define TEST_FREQUENCY1_PLL_HZ       101700000U   // 101.7 MHz

    healthMonitorState.isWatchdogActive = true;
    healthMonitorState.wdgInterruptCtr = 7U;                                  // 0 < any value < 19
    healthMonitorState.wdgTimerIntervalMicrosecs = 50000;                     // 50ms wdg timer interval
    healthMonitorState.wdgInterruptCtrMax = 19U;      // 50ms wdg timer interval x (healthMonitorState.wdgInterruptCtrMax + 1) = 1 Sec
    healthMonitorState.extClkFreqHz = TEST_FREQUENCY1_EXT_CLK_HZ;
    healthMonitorState.pllFreqHz = TEST_FREQUENCY1_PLL_HZ;
    healthMonitorState.currentPllLockState = true;                            // was running on PLL CLOCK
    healthMonitorState.wdgTimerReloadValueExt = (uint32_t)((uint64_t)TEST_FREQUENCY1_EXT_CLK_HZ * healthMonitorState.wdgTimerIntervalMicrosecs / 1000000U);

    mock().expectOneCall("WatchdogDrvClearInterrupt");
    mock().expectOneCall("SystemMgrSetEvent")
          .withParameter("event", SYSTEM_MGR_EVENT_WATCHDOG);
    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(false);                                       // now using EXT clock - changed since last time

    mock().expectOneCall("SystemMgrSetClockSpeed")
          .withParameter("clockSpeedHz", TEST_FREQUENCY1_EXT_CLK_HZ);

    Watchdog_IrqHandler();
    CHECK_EQUAL(8U, healthMonitorState.wdgInterruptCtr);
    CHECK_EQUAL(false, healthMonitorState.currentPllLockState);
}

TEST(HealthMonitorGroup, TestWatchdog_IrqHandlerWdgActiveFreqChangeToPll)
{
#define TEST_FREQUENCY2_EXT_CLK_HZ     6000000U   // 6 MHz
#define TEST_FREQUENCY2_PLL_HZ       101900000U   // 101.9 MHz

    healthMonitorState.isWatchdogActive = true;
    healthMonitorState.wdgInterruptCtr = 0U;                                // 0 < any value < 19
    healthMonitorState.wdgTimerIntervalMicrosecs = 50000;                   // 50ms wdg timer interval
    healthMonitorState.wdgInterruptCtrMax = 19U;      // 50ms wdg timer interval x (healthMonitorState.wdgInterruptCtrMax + 1) = 1 Sec
    healthMonitorState.extClkFreqHz = TEST_FREQUENCY2_EXT_CLK_HZ;           // 6 MHz EXT
    healthMonitorState.pllFreqHz = TEST_FREQUENCY2_PLL_HZ;                  // 101 MHz PLL
    healthMonitorState.currentPllLockState = false;                         // was running on EXT CLOCK
    healthMonitorState.wdgTimerReloadValuePll = (uint32_t)((uint64_t)TEST_FREQUENCY2_PLL_HZ * healthMonitorState.wdgTimerIntervalMicrosecs / 1000000U);

    mock().expectOneCall("WatchdogDrvClearInterrupt");
    mock().expectOneCall("SystemMgrSetEvent")
          .withParameter("event", SYSTEM_MGR_EVENT_WATCHDOG);
    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(true);                                            // now using PLL clock - changed since last time

    mock().expectOneCall("SystemMgrSetClockSpeed")
          .withParameter("clockSpeedHz", TEST_FREQUENCY2_PLL_HZ);

    Watchdog_IrqHandler();
    CHECK_EQUAL(1U, healthMonitorState.wdgInterruptCtr);
    CHECK_EQUAL(true, healthMonitorState.currentPllLockState);
}

TEST(HealthMonitorGroup, TestWatchdog_IrqHandlerWdgActiveTimedOut)
{
#define TEST_FREQUENCY3_EXT_CLK_HZ     6000000U   // 6 MHz
#define TEST_FREQUENCY3_PLL_HZ       101000000U   // 101 MHz

    healthMonitorState.isWatchdogActive = true;
    healthMonitorState.wdgInterruptCtr = 18U;                                 // will soon timeout
    healthMonitorState.wdgTimerIntervalMicrosecs = 50000;                     // 50ms wdg timer interval
    healthMonitorState.wdgInterruptCtrMax = 19U;      // 50ms wdg timer interval x (healthMonitorState.wdgInterruptCtrMax + 1) = 1 Sec
    healthMonitorState.extClkFreqHz = TEST_FREQUENCY3_EXT_CLK_HZ;   // 6 MHz EXT
    healthMonitorState.pllFreqHz = TEST_FREQUENCY3_PLL_HZ;          // 101 MHz PLL
    healthMonitorState.currentPllLockState = false;                           // was running on EXT CLOCK

    mock().expectOneCall("SystemDrvIsPllLocked")
          .andReturnValue(false);                                       // still using EXT clock

    Watchdog_IrqHandler();
    CHECK_EQUAL(19U, healthMonitorState.wdgInterruptCtr);
    CHECK_EQUAL(false, healthMonitorState.currentPllLockState);
}

TEST(HealthMonitorGroup, TestGetVcoreStatusGood)
{
    ErrorT  retVal;
    HealthMonitorVcoreStatus vCoreStatus;

    mock().expectOneCall("MacDrvGetCmacStatus");

    retVal = HealthMonitorGetVCoreStatus(&vCoreStatus);
    LONGS_EQUAL(ERROR_SUCCESS, retVal);
    LONGS_EQUAL(HEALTH_MONITOR_VCORE_ERROR_STATUS_GOOD , vCoreStatus.errorStatus);
    LONGS_EQUAL(0, vCoreStatus.statusCode);
}

TEST(HealthMonitorGroup, TestGetVcoreStatusWithNullPtr)
{
    ErrorT  retVal;
    HealthMonitorVcoreStatus* vCoreStatusPtr = NULL;

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_VCORE_STATUS_NULL_PTR)
          .withParameter("infoParam", ERROR_SYSERR);

    retVal = HealthMonitorGetVCoreStatus(vCoreStatusPtr);
    LONGS_EQUAL(ERROR_SYSERR, retVal);
}

TEST(HealthMonitorGroup, TestGetVcoreStatusWithMemFullError)
{
    ErrorT  retVal;
    HealthMonitorVcoreStatus vCoreStatus;

    mock().expectOneCall("MacDrvGetCmacStatus")
          .andReturnValue(CRYPTO_VCORE_AESCMAC_STATUS_AESCMAC_MEMFULL__MASK);

    retVal = HealthMonitorGetVCoreStatus(&vCoreStatus);
    LONGS_EQUAL(ERROR_SUCCESS, retVal);
    LONGS_EQUAL(HEALTH_MONITOR_VCORE_ERROR_STATUS_CMAC_MEMFULL , vCoreStatus.errorStatus);
    LONGS_EQUAL(0, vCoreStatus.statusCode);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnWatchdogEvent)
{
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    // usually set by EnableMonitoring
    healthMonitorState.stackHwmDangerZoneSizeLongWords = 6U;
    healthMonitorState.stackMaxAllowedSizeLongWords =
        stackConfig.sizeLongWords -
        stackConfig.stackGuardRegionSizeLongWords -
        healthMonitorState.stackHwmDangerZoneSizeLongWords;

    healthMonitorState.wdgInterruptCtr = 17;   // a successful call to HealthMonitorOnWatchdogEvent() will clear this to zero

    StackEmpty(1U);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    mock().expectOneCall("MacDrvGetCmacStatus");

    ErrorT errorStatus = HealthMonitorOnWatchdogEvent();
    LONGS_EQUAL(ERROR_SUCCESS, errorStatus);
    LONGS_EQUAL(0, healthMonitorState.wdgInterruptCtr);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnWatchdogEventWithBadVcoreStatus)
{
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    // usually set by EnableMonitoring
    healthMonitorState.stackHwmDangerZoneSizeLongWords = 6U;
    healthMonitorState.stackMaxAllowedSizeLongWords =
        stackConfig.sizeLongWords -
        stackConfig.stackGuardRegionSizeLongWords -
        healthMonitorState.stackHwmDangerZoneSizeLongWords;

    StackEmpty(1U);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    mock().expectOneCall("MacDrvGetCmacStatus")
          .andReturnValue(CRYPTO_VCORE_AESCMAC_STATUS_AESCMAC_MEMFULL__MASK);
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_VCORE_ERROR)
        .withParameter("infoParam", HEALTH_MONITOR_VCORE_ERROR_STATUS_CMAC_MEMFULL);

    ErrorT errorStatus = HealthMonitorOnWatchdogEvent();
    LONGS_EQUAL(ERROR_SYSERR, errorStatus);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnWatchdogEventStackInDangerZone)
{
    InitMpuStackConfigType stackConfig;
    uint32_t    stackUsedLongWords;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    // usually set by EnableMonitoring
    healthMonitorState.stackHwmDangerZoneSizeLongWords = 6U;
    healthMonitorState.stackMaxAllowedSizeLongWords =
        stackConfig.sizeLongWords -
        stackConfig.stackGuardRegionSizeLongWords -
        healthMonitorState.stackHwmDangerZoneSizeLongWords;

    // set stack HWM just in danger zone
    stackUsedLongWords = healthMonitorState.stackMaxAllowedSizeLongWords + 1U;
    StackEmpty(stackUsedLongWords);

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_STACK_OVERRUN)
        .withParameter("infoParam", stackUsedLongWords);

    ErrorT errorStatus = HealthMonitorOnWatchdogEvent();
    LONGS_EQUAL(ERROR_SYSERR, errorStatus);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnWatchdogEventWithFailedGetStack)
{
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)NULL;
    stackConfig.sizeLongWords = 0;
    stackConfig.stackGuardRegionSizeLongWords = 0;

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig))
          .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_INFORMATION_ERROR)
        .withParameter("infoParam", ERROR_INVAL);

    ErrorT errorStatus = HealthMonitorOnWatchdogEvent();
    LONGS_EQUAL(ERROR_INVAL, errorStatus);
}

TEST(HealthMonitorGroup, TestHealthMonitorGetStackUsedSizeLongWordsNullPointer)
{
    ErrorT status;

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_USED_SIZE_NULL_PTR)
        .withParameter("infoParam", ERROR_SYSERR);

    status = HealthMonitorGetStackUsedSizeLongWords(NULL);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(HealthMonitorGroup, TestHealthMonitorGetStackUsedSizeLongWordsUsed0)
{
    ErrorT status;
    uint16_t stackUsedLongWords;
    uint32_t watermark;
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    watermark = StackEmpty(0U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status = HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);
}

TEST(HealthMonitorGroup, TestHealthMonitorGetStackUsedSizeLongWordsUsed1To9)
{
    ErrorT status;
    uint16_t stackUsedLongWords;
    uint32_t watermark;
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    watermark = StackEmpty(1U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status = HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(9U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);
}

TEST(HealthMonitorGroup, TestHealthMonitorGetStackUsedSizeLongWordsFree0To8)
{
    ErrorT status;
    uint16_t stackUsedLongWords;
    uint32_t watermark;
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    watermark = StackEmpty(1024U - 0U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(1024U - 1U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(1024U - 8U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);
}

TEST(HealthMonitorGroup, TestHealthMonitorGetStackUsedSizeLongWordsFree9To12)
{
    ErrorT status;
    uint16_t stackUsedLongWords;
    uint32_t watermark;
    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    watermark = StackEmpty(1024U - 9U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(1024U - 10U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(1024U - 11U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);

    watermark = StackEmpty(1024U - 12U);

    mock().expectOneCall("InitMpuGetStackInformation")
        .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));
    status =  HealthMonitorGetStackUsedSizeLongWords(&stackUsedLongWords);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(watermark, stackUsedLongWords);
}

TEST(HealthMonitorGroup, TestHealthMonitorSetWdogTimerInterval)
{
    uint32_t newTimerInterval = 0x87654321;
    healthMonitorState.wdgTimerIntervalMicrosecs = 42;

    HealthMonitorSetWdogTimerInterval(newTimerInterval);
    LONGS_EQUAL(newTimerInterval, healthMonitorState.wdgTimerIntervalMicrosecs);
}

TEST(HealthMonitorGroup, TestHealthMonitorSetStackCheckThreshold)
{
    uint32_t newMaxAllowedSize;
    uint16_t newSafetyZoneSize = 0x9876;

    InitMpuStackConfigType stackConfig;

    stackConfig.base = (uint32_t*)&stack[0];
    stackConfig.sizeLongWords = 1024U;
    stackConfig.stackGuardRegionSizeLongWords = 8U;

    healthMonitorState.stackHwmDangerZoneSizeLongWords  = 13;

    mock().expectOneCall("InitMpuGetStackInformation")
          .withOutputParameterReturning("stackConfig", (InitMpuStackConfigType*)&stackConfig, sizeof(stackConfig));

    HealthMonitorSetStackCheckThreshold(newSafetyZoneSize);
    LONGS_EQUAL(newSafetyZoneSize, healthMonitorState.stackHwmDangerZoneSizeLongWords);

    newMaxAllowedSize = stackConfig.sizeLongWords -
            stackConfig.stackGuardRegionSizeLongWords -
            newSafetyZoneSize;
    LONGS_EQUAL(newMaxAllowedSize, healthMonitorState.stackMaxAllowedSizeLongWords);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnClockChangeFreqTooHigh)
{
    ErrorT      retVal;
    uint32_t    newClockFrequencyHz = SYSTEM_MGR_MAX_CLOCK_RATE_HZ + 1U;    // out-of-range high

    healthMonitorState.isWatchdogActive = true;
    retVal = HealthMonitorOnClockChange(newClockFrequencyHz);

    LONGS_EQUAL(ERROR_RANGE, retVal);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnClockChangeFreqTooLow)
{
    ErrorT      retVal;
    uint32_t    newClockFrequencyHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ - 1U;    // out-of-range low

    healthMonitorState.isWatchdogActive = true;
    retVal = HealthMonitorOnClockChange(newClockFrequencyHz);

    LONGS_EQUAL(ERROR_RANGE, retVal);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnClockChangeWdogInactive)
{
    ErrorT      retVal;
    uint32_t    newClockFrequencyHz = 17;    // out-of-range value will be ignored because wdg is inactive

    healthMonitorState.isWatchdogActive = false;
    retVal = HealthMonitorOnClockChange(newClockFrequencyHz);

    LONGS_EQUAL(ERROR_SUCCESS, retVal);
}

TEST(HealthMonitorGroup, TestHealthMonitorOnClockChange)
{
    ErrorT      retVal;
    uint32_t    newClockFrequencyHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ;    // minimum allowed frequency
    uint32_t    wdgTimerLoadValue;

    // input conditions
    healthMonitorState.isWatchdogActive = true;
    healthMonitorState.wdgTimerIntervalMicrosecs = 50000U;

    // expectations
    wdgTimerLoadValue = (uint32_t)(((uint64_t)newClockFrequencyHz * 50000U) / HEALTH_MONITOR_ONE_MILLION_DIVISOR);  // 50ms timer period
    mock().expectOneCall("WatchdogDrvDisable");
    mock().expectOneCall("WatchdogDrvStart")
          .withParameter("timeOutClocks", wdgTimerLoadValue);

    retVal = HealthMonitorOnClockChange(newClockFrequencyHz);
    LONGS_EQUAL(ERROR_SUCCESS, retVal);
}

