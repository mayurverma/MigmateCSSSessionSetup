//***********************************************************************************
/// \file
///
/// System Manager unit test
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
#include "SystemMgr.h"
#include "SystemMgrInternal.h"
#include "CommandHandler.h"
#include "Diag.h"
#include "Interrupts.h"
}

// Mocks
extern "C"
{
    ErrorT AssetMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("AssetMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT NvmMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("NvmMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT SessionMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("SessionMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    // Local mock to force an error condition
    ErrorT CommandHandlerOnDoorbellEvent(HostCommandCodeT const commandCode)
    {
        mock().actualCall("CommandHandlerOnDoorbellEvent")
            .withParameter("commandCode", commandCode);

        // set flag to force SystemMgrMainLoop() to return
        systemMgrState.fatalErrorState.hasTerminated = true;

        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    // Local mock to set the debug-start event
    void SystemDrvWaitForInterrupt(void)
    {
        mock().actualCall("SystemDrvWaitForInterrupt");

        // Set the debug-start event to allow SystemMgrMainLoop() to return
        SystemMgrSetEvent(SYSTEM_MGR_EVENT_DEBUG_START);

        // and set flag to force SystemMgrMainLoop() to return
        systemMgrState.fatalErrorState.hasTerminated = true;
    }

#if defined(BUILD_TARGET_ARCH_x86)
    void InitMpuEnablePatchRegion(bool const enable)
    {
        mock().actualCall("InitMpuEnablePatchRegion")
            .withParameter("enable", enable);
    }
#endif
}

// Locals
void setupInitSuccess(void);


TEST_GROUP(SystemMgrInitGroup)
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

TEST_GROUP(SystemMgrApiGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        setupInitSuccess();
        SystemMgrInit();
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(SystemMgrInternalGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        setupInitSuccess();
        SystemMgrInit();
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

// Sets-up test for a successful call to SystemMgrNotifyEnterPhase()
void setupNotifyEnterPhase(SystemMgrPhaseT const phase)
{
    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("AssetMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("SessionMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("CryptoMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("NvmMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("HealthMonitorOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("PatchMgrOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("CommandHandlerOnPhaseEntry")
        .withParameter("phase", phase)
        .andReturnValue(ERROR_SUCCESS);

    mock().expectNCalls(8, "DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);
}

void setupInitSuccess(void)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectNCalls(32, "InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetExceptionPriority")
        .withParameter("exception", EXCEPTION_SVCALL)
        .withParameter("priority", INTERRUPT_PRIORITY_0);

    mock().expectOneCall("InterruptsSetInterruptPriority")
        .withParameter("interrupt", INTERRUPT_WATCHDOG)
        .withParameter("priority", INTERRUPT_PRIORITY_1);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_INITIALIZE);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_INITIALIZE);

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))SystemMgrOnDiagFatalErrorDump);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);
}

TEST(SystemMgrInitGroup, TestInitSuccess)
{
    setupInitSuccess();
    SystemMgrInit();

    bool pending;
    ErrorT status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_DEBUG_START, &pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(pending);

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_INITIALIZE, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitPlatformInitFailure)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit")
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_SYSERR);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_PLATFORM_INIT_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_BOOT, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitExceptionAndInterruptPrioritiesFailure1)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectOneCall("InterruptsSetInterruptPriority")
        .withParameter("interrupt", INTERRUPT_MIN)
        .withParameter("priority", INTERRUPT_PRIORITY_2)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_INVAL);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_EXCEPTION_INIT_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_BOOT, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitExceptionAndInterruptPrioritiesFailure2)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectNCalls(INTERRUPT_MAX + 1, "InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetExceptionPriority")
        .withParameter("exception", EXCEPTION_SVCALL)
        .withParameter("priority", INTERRUPT_PRIORITY_0)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_INVAL);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_EXCEPTION_INIT_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_BOOT, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitExceptionAndInterruptPrioritiesFailure3)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectNCalls(INTERRUPT_MAX + 1, "InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetExceptionPriority")
        .withParameter("exception", EXCEPTION_SVCALL)
        .withParameter("priority", INTERRUPT_PRIORITY_0)
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("InterruptsSetInterruptPriority")
        .withParameter("interrupt", INTERRUPT_WATCHDOG)
        .withParameter("priority", INTERRUPT_PRIORITY_1)
        .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_INVAL);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_EXCEPTION_INIT_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_BOOT, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitNotifyEnterPhaseInitializeFailure)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectNCalls(INTERRUPT_MAX + 1, "InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetExceptionPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_INITIALIZE);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .withParameter("info", 0);

    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_INITIALIZE)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_SYSERR);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_COMPONENT_INIT_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_INITIALIZE, currentPhase);
}

TEST(SystemMgrInitGroup, TestInitRegisterFatalErrorDumpHandlerFails)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_START);

    mock().expectOneCall("PlatformInit");

    mock().expectNCalls(32, "InterruptsSetInterruptPriority")
        .ignoreOtherParameters();

    mock().expectOneCall("InterruptsSetExceptionPriority")
        .withParameter("exception", EXCEPTION_SVCALL)
        .withParameter("priority", INTERRUPT_PRIORITY_0);

    mock().expectOneCall("InterruptsSetInterruptPriority")
        .withParameter("interrupt", INTERRUPT_WATCHDOG)
        .withParameter("priority", INTERRUPT_PRIORITY_1);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_INITIALIZE);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_INITIALIZE);

    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))SystemMgrOnDiagFatalErrorDump)
        .andReturnValue(ERROR_NOSPC);

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", ERROR_NOSPC);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL)
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_REGISTER_DUMP_HANDLER_FAILED);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_INIT_DONE);

    SystemMgrInit();

    SystemMgrPhaseT currentPhase = SystemMgrGetCurrentPhase();
    LONGS_EQUAL(SYSTEM_MGR_PHASE_INITIALIZE, currentPhase);
}

extern "C"
{
    uint32_t notifierClockSpeed[SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS];

    ErrorT TestGetSetClockSpeedNotifier0(uint32_t const clockSpeed)
    {
        notifierClockSpeed[0] = clockSpeed;
        return ERROR_SUCCESS;
    }

    ErrorT TestGetSetClockSpeedNotifier1(uint32_t const clockSpeed)
    {
        notifierClockSpeed[1] = clockSpeed;
        return ERROR_SUCCESS;
    }

    ErrorT TestGetSetClockSpeedNotifier2(uint32_t const clockSpeed)
    {
        notifierClockSpeed[2] = clockSpeed;
        return ERROR_SUCCESS;
    }

    ErrorT TestGetSetClockSpeedNotifier3(uint32_t const clockSpeed)
    {
        notifierClockSpeed[3] = clockSpeed;
        return ERROR_SUCCESS;
    }

    ErrorT TestGetSetClockSpeedNotifierBad(uint32_t const clockSpeed)
    {
        notifierClockSpeed[1] = clockSpeed;
        return ERROR_RANGE;
    }
}

TEST(SystemMgrApiGroup, TestSetGetClockSpeed)
{
    uint32_t clockSpeedHz;
    uint32_t returnedClockSpeedHz;

    // Check SystemMgr rejects if clock speed is unknown
    ErrorT status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_NOMSG, status);

    // Check SystemMgr rejects if clock speed is out-of-range
    clockSpeedHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ - 1;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_RANGE, status);

    clockSpeedHz = SYSTEM_MGR_MAX_CLOCK_RATE_HZ + 1;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_RANGE, status);

    // Set a valid clock speed
    clockSpeedHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // and check its returned
    status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(clockSpeedHz, returnedClockSpeedHz);

    clockSpeedHz = SYSTEM_MGR_MAX_CLOCK_RATE_HZ / 2;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // check a null parameter is just ignored
    status = SystemMgrGetClockSpeed(NULL);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Register a notifier
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier0);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Change the clock speed and check the notifier is called
    notifierClockSpeed[0] = 0;
    clockSpeedHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ * 2U;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(clockSpeedHz, returnedClockSpeedHz);
    LONGS_EQUAL(clockSpeedHz, notifierClockSpeed[0]);

    // Set the same clock speed and check the notifier is not called
    notifierClockSpeed[0] = 0;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(clockSpeedHz, returnedClockSpeedHz);
    LONGS_EQUAL(0, notifierClockSpeed[0]);

    // Register a 'bad' notifier and check clock speed change is rejected
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifierBad);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Change the clock speed and check its rejected
    clockSpeedHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ * 4;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_RANGE, status);

    status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(clockSpeedHz, returnedClockSpeedHz);

}

TEST(SystemMgrApiGroup, TestRegisterClockSpeedChangeNotifier)
{
    uint32_t clockSpeedHz;
    uint32_t returnedClockSpeedHz;
    ErrorT status;

    // Attempt to register a NULL notifier - should be ignored
    status = SystemMgrRegisterClockSpeedChangeNotifier(NULL);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Register notifiers
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier0);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier1);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier2);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier3);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Change the clock speed and check the notifiers are called
    memset(notifierClockSpeed, 0, sizeof(notifierClockSpeed));
    clockSpeedHz = SYSTEM_MGR_MIN_CLOCK_RATE_HZ;
    status = SystemMgrSetClockSpeed(clockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SystemMgrGetClockSpeed(&returnedClockSpeedHz);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(clockSpeedHz, returnedClockSpeedHz);

    for (int i = 0; i < SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS; i++)
    {
        LONGS_EQUAL(clockSpeedHz, notifierClockSpeed[i]);
    }

    // Attempt to register another notifier
    status = SystemMgrRegisterClockSpeedChangeNotifier(TestGetSetClockSpeedNotifier0);
    LONGS_EQUAL(ERROR_NOSPC, status);
}

TEST(SystemMgrApiGroup, TestEnablePatchRegion)
{
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("InitMpuEnablePatchRegion")
        .withParameter("enable", true);
    mock().expectOneCall("PlatformExitCritical");

    SystemMgrEnablePatchRegion(true);

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("InitMpuEnablePatchRegion")
        .withParameter("enable", false);
    mock().expectOneCall("PlatformExitCritical");

    SystemMgrEnablePatchRegion(false);
}

TEST(SystemMgrApiGroup, TestMgrGetRomVersion)
{
    uint16_t romVersion = SystemMgrGetRomVersion();
    LONGS_EQUAL(SYSTEM_MGR_ROM_VERSION, romVersion);
}

TEST(SystemMgrApiGroup, TestRequestShutdown)
{
    uint32_t context = 0xAAAA;
    uint32_t info = 0xBBBB;
    bool is_pending;
    uint32_t status;

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    SystemMgrRequestShutdown(context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    LONGS_EQUAL(context, systemMgrState.shutdownState.context);
    LONGS_EQUAL(info, systemMgrState.shutdownState.infoParam);

    // Retrigger the request and check the new context is ignored
    context = 0xCCCC;
    info = 0xDDDD;

    SystemMgrRequestShutdown(context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    LONGS_EQUAL(0xAAAA, systemMgrState.shutdownState.context);
    LONGS_EQUAL(0xBBBB, systemMgrState.shutdownState.infoParam);

}

TEST(SystemMgrApiGroup, TestReportFatalError)
{
    SystemMgrCssFaultT fault = SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL;
    SystemMgrFatalErrorT context = SYSTEM_MGR_FATAL_ERROR_HARD_FAULT;
    uint32_t info = 0xBBBB;
    bool is_pending;
    ErrorT status;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    SystemMgrReportFatalError(context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    LONGS_EQUAL(fault, systemMgrState.fatalErrorState.fault);
    LONGS_EQUAL(context, systemMgrState.fatalErrorState.context);
    LONGS_EQUAL(info, systemMgrState.fatalErrorState.infoParam);
    CHECK_FALSE(systemMgrState.fatalErrorState.hasTerminated);
}

TEST(SystemMgrApiGroup, TestReportCryptoFault)
{
    SystemMgrCssFaultT fault = SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION;
    uint32_t context = 0xAAAA;
    uint32_t info = 0xBBBB;
    bool is_pending;
    ErrorT status;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    SystemMgrReportCryptoFault(fault, context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    LONGS_EQUAL(fault, systemMgrState.fatalErrorState.fault);
    LONGS_EQUAL(context, systemMgrState.fatalErrorState.context);
    LONGS_EQUAL(info, systemMgrState.fatalErrorState.infoParam);
    CHECK_FALSE(systemMgrState.fatalErrorState.hasTerminated);

    // Retrigger the request and check the new context is ignored
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("PlatformExitCritical");

    fault = SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE;
    context = 0xCCCC;
    info = 0xDDDD;

    SystemMgrReportCryptoFault(fault, context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    LONGS_EQUAL(SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION, systemMgrState.fatalErrorState.fault);
    LONGS_EQUAL(0xAAAA, systemMgrState.fatalErrorState.context);
    LONGS_EQUAL(0xBBBB, systemMgrState.fatalErrorState.infoParam);
    CHECK_FALSE(systemMgrState.fatalErrorState.hasTerminated);

    // Retrigger with Cryptocell SW fault
    // - should trigger a HardFault but for unit test we have to skip this
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("PlatformExitCritical");

    fault = SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR;
    context = 0xEEEE;
    info = 0xFFFF;

    SystemMgrReportCryptoFault(fault, context, info);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_FALSE(is_pending);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_FATAL_ERROR, &is_pending);
    LONGS_EQUAL(status, ERROR_SUCCESS);
    CHECK_TRUE(is_pending);

    LONGS_EQUAL(SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION, systemMgrState.fatalErrorState.fault);
    LONGS_EQUAL(0xAAAA, systemMgrState.fatalErrorState.context);
    LONGS_EQUAL(0xBBBB, systemMgrState.fatalErrorState.infoParam);
    CHECK_FALSE(systemMgrState.fatalErrorState.hasTerminated);

}

TEST(SystemMgrApiGroup, TestClearPendingEvent)
{
    ErrorT status;
    bool is_pending;

    SystemMgrSetEvent(SYSTEM_MGR_EVENT_RESERVED_30);
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_30, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(is_pending);

    // Clear a non-existing event
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_NONE);
    LONGS_EQUAL(ERROR_RANGE, status);

    // Clear a non-pending event
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_RESERVED_31);
    LONGS_EQUAL(ERROR_ALREADY, status);
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_30, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(is_pending);

    // Clear a pending event
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_RESERVED_30);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_30, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_FALSE(is_pending);
}

TEST(SystemMgrApiGroup, TestSetEvent)
{
    // Clear the pending SYSTEM_MGR_EVENT_DEBUG_START event
    ErrorT status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_DEBUG_START);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Check no pending events
    LONGS_EQUAL(0, systemMgrState.currentPriorityMask)

    // Set a valid event
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_RESERVED_29);
    LONGS_EQUAL(1U << SYSTEM_MGR_EVENT_RESERVED_29, systemMgrState.currentPriorityMask);

    // Set an invalid event, we expect a fatal error
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    SystemMgrSetEvent(SYSTEM_MGR_EVENT_MAX);
    LONGS_EQUAL((1U << SYSTEM_MGR_EVENT_RESERVED_29) | (1U << SYSTEM_MGR_EVENT_FATAL_ERROR),
        systemMgrState.currentPriorityMask);

    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_FATAL_ERROR);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_RESERVED_29);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(0, systemMgrState.currentPriorityMask)

    // change the priority of the event
    SystemMgrEventT newEventByPriority[SYSTEM_MGR_EVENT_MAX];
    SystemMgrEventPriorityT newPriorityByEvent[SYSTEM_MGR_EVENT_MAX];

    // copy the defaults
    memcpy(newEventByPriority, systemMgrEventByPriorityDefaults, sizeof(systemMgrEventByPriorityDefaults));
    memcpy(newPriorityByEvent, systemMgrPriorityByEventDefaults, sizeof(systemMgrPriorityByEventDefaults));

    // switch 2 and 29
    SystemMgrEventT eventAt2 = systemMgrEventByPriorityDefaults[SYSTEM_MGR_EVENT_PRIORITY_2];
    SystemMgrEventPriorityT priorityAt2 = systemMgrPriorityByEventDefaults[eventAt2];
    LONGS_EQUAL(eventAt2, SYSTEM_MGR_EVENT_SHUTDOWN_PENDING);
    LONGS_EQUAL(priorityAt2, SYSTEM_MGR_EVENT_PRIORITY_2);

    newPriorityByEvent[SYSTEM_MGR_EVENT_RESERVED_29] = SYSTEM_MGR_EVENT_PRIORITY_2;
    newEventByPriority[SYSTEM_MGR_EVENT_PRIORITY_2] = SYSTEM_MGR_EVENT_RESERVED_29;
    newPriorityByEvent[eventAt2] = SYSTEM_MGR_EVENT_PRIORITY_29;
    newEventByPriority[SYSTEM_MGR_EVENT_PRIORITY_29] = eventAt2;

    // replace the System Manager's tables
    systemMgrState.eventByPriority = newEventByPriority;
    systemMgrState.priorityByEvent = newPriorityByEvent;

    SystemMgrSetEvent(SYSTEM_MGR_EVENT_RESERVED_29);
    LONGS_EQUAL(1U << SYSTEM_MGR_EVENT_PRIORITY_2, systemMgrState.currentPriorityMask);

    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_RESERVED_29);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(0, systemMgrState.currentPriorityMask)

    // restore the tables
    systemMgrState.eventByPriority = (SystemMgrEventT*)systemMgrEventByPriorityDefaults;
    systemMgrState.priorityByEvent = (SystemMgrEventPriorityT*)systemMgrPriorityByEventDefaults;

}

TEST(SystemMgrApiGroup, TestIsEventPending)
{
    ErrorT status;
    bool is_pending;

    // Check valid event when event is not pending
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_20, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_FALSE(is_pending);

    // Check valid event when event is pending
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_RESERVED_20);
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_20, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(is_pending);

    // Check invalid event
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_MAX, NULL);
    LONGS_EQUAL(ERROR_RANGE, status);

    // Check valid event with null
    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_20, NULL);
    LONGS_EQUAL(ERROR_INVAL, status);

    // Clear and check again
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_RESERVED_20);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_RESERVED_20, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_FALSE(is_pending);

}

TEST(SystemMgrApiGroup, TestMainLoop)
{
    // After SystemMgrInit() returns, the SYSTEM_MGR_EVENT_DEBUG_START event
    // should be pending
    ErrorT status;
    bool is_pending;

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_DEBUG_START, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(is_pending);

    // Set a second, third and fourth pending event (lower-priority)
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_SESSION_START);
    SystemMgrSetEvent(SYSTEM_MGR_EVENT_DOORBELL);

    // Note the main loop does not exit unless a fatal error occurs.

    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DEBUG_START);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);

    mock().expectOneCall("PlatformEnterCritical");

    // SystemMgr should now process the patch-load-complete event and switch to the
    // Configure phase
    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_PATCH_LOAD_COMPLETE);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_CONFIGURE);

    mock().expectOneCall("PlatformEnterCritical");

    // System Mgr should now process the SessionStart event
    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_SESSION_START);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    // Force phase entry to fail
    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SESSION)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    mock().expectOneCall("PlatformEnterCritical");

    // System Mgr should now process the Doorbell event
    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("CommandDrvGetCommand")
        .andReturnValue(HOST_COMMAND_CODE_GET_STATUS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    mock().expectOneCall("CommandHandlerOnDoorbellEvent")
        .withParameter("commandCode", HOST_COMMAND_CODE_GET_STATUS)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("PlatformEnterCritical");

    SystemMgrMainLoop();
}

TEST(SystemMgrApiGroup, TestMainLoopWithNoPendingEvents)
{
    // After SystemMgrInit() returns, the SYSTEM_MGR_EVENT_DEBUG_START event
    // should be pending
    ErrorT status;
    bool is_pending;

    status = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_DEBUG_START, &is_pending);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(is_pending);

    // Clear the debug-start event
    status = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_DEBUG_START);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // Note the main loop does not exit unless a fatal error occurs.

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT);

    // This (local) mock will set the debug-start event
    mock().expectOneCall("SystemDrvWaitForInterrupt");

    mock().expectOneCall("PlatformExitCritical");
    mock().expectOneCall("PlatformEnterCritical");

    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DEBUG_START);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    // Force phase entry to fail
    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_DEBUG)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    mock().expectOneCall("PlatformEnterCritical");

    SystemMgrMainLoop();
}

TEST(SystemMgrApiGroup, TestHasCC312AbortOccurred)
{
    bool hasOccurred = SystemMgrHasCC312AbortOccurred();
    CHECK_FALSE(hasOccurred);

    systemMgrState.fatalErrorState.isCC312Abort = true;
    hasOccurred = SystemMgrHasCC312AbortOccurred();
    CHECK_TRUE(hasOccurred);
}

TEST(SystemMgrInternalGroup, TestGetNextEvent)
{
    SystemMgrEventT event;

    systemMgrState.currentPriorityMask = 0;
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(SYSTEM_MGR_EVENT_NONE, event);

    for (uint32_t idx = 0; idx < SYSTEM_MGR_EVENT_MAX; idx++)
    {
        systemMgrState.currentPriorityMask = 1U << idx;
        event = SystemMgrGetNextEvent();
        LONGS_EQUAL(SYSTEM_MGR_EVENT_WATCHDOG + idx, event);
    }

    systemMgrState.currentPriorityMask = 0xAA000000;
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(25, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(27, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(29, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(31, event);

    systemMgrState.currentPriorityMask = 0x00005500;
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(8, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(10, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(12, event);
    event = SystemMgrGetNextEvent();
    LONGS_EQUAL(14, event);

}

TEST(SystemMgrInternalGroup, TestOnShutdownPendingEvent)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_SHUTDOWN);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_SHUTDOWN);

    systemMgrState.shutdownState.context = 0xABCD;
    systemMgrState.shutdownState.infoParam = 0x1234;

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", 0x1234);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_FIRMWARE_TERMINATED)
        .withParameter("context", 0xABCD);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    ErrorT status = SystemMgrHandleEvent(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SystemMgrInternalGroup, TestOnFatalErrorEvent)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_SHUTDOWN);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_SHUTDOWN);

    systemMgrState.fatalErrorState.fault = SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE;
    systemMgrState.fatalErrorState.context = 0xCAFE;
    systemMgrState.fatalErrorState.infoParam = 0xBABE;

    mock().expectOneCall("SystemDrvGetCheckpoint");
    mock().expectOneCall("SystemDrvSetCheckpoint")
        .withParameter("checkpoint", 0x8000);
    mock().expectOneCall("SystemDrvSetInfo")
        .withParameter("info", 0xBABE);
    mock().expectOneCall("SystemDrvSetCryptoError")
        .withParameter("errorId", SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE)
        .withParameter("context", 0xCAFE);
    mock().expectOneCall("CommandHandlerFatalErrorStop");

    ErrorT status = SystemMgrHandleEvent(SYSTEM_MGR_EVENT_FATAL_ERROR);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SystemMgrInternalGroup, TestOnUnhandledEvent)
{
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    ErrorT status = SystemMgrHandleEvent(SYSTEM_MGR_EVENT_RESERVED_31);
    LONGS_EQUAL(ERROR_NOENT, status);
}

bool testPhaseEntryHandlerCalled;

extern "C"
{
    extern SystemMgrOnPhaseEntryHandlerT systemMgrRAMPhaseEntryHandlers[];

    ErrorT TestOnPhaseEntryHandlerGood(SystemMgrPhaseT const phase)
    {
        (void)phase;
        testPhaseEntryHandlerCalled = true;
        return ERROR_SUCCESS;
    }

    ErrorT TestOnPhaseEntryHandlerBad(SystemMgrPhaseT const phase)
    {
        (void)phase;
        testPhaseEntryHandlerCalled = true;
        return ERROR_SYSERR;
    }
}

TEST(SystemMgrInternalGroup, TestNotifyEnterPhaseWithSuccessfulPatch)
{
    mock().expectNCalls(2, "DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    // Force phase entry to fail
    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_DEBUG)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    systemMgrRAMPhaseEntryHandlers[0] = TestOnPhaseEntryHandlerGood;
    testPhaseEntryHandlerCalled = false;

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_TRUE(testPhaseEntryHandlerCalled);

    systemMgrRAMPhaseEntryHandlers[0] = NULL;
}

TEST(SystemMgrInternalGroup, TestNotifyEnterPhaseWithFailingPatch)
{
    mock().expectNCalls(1, "DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    systemMgrRAMPhaseEntryHandlers[0] = TestOnPhaseEntryHandlerBad;
    testPhaseEntryHandlerCalled = false;

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_SYSERR, status);
    CHECK_TRUE(testPhaseEntryHandlerCalled);

    systemMgrRAMPhaseEntryHandlers[0] = NULL;
}

TEST(SystemMgrInternalGroup, TestNotifyEnterShutdownPhaseWithFailingPatch)
{
    mock().expectNCalls(9, "DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    // Force phase entry to fail
    mock().expectOneCall("LifecycleMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("AssetMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("SessionMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("CryptoMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("NvmMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("HealthMonitorOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("PatchMgrOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);
    mock().expectOneCall("CommandHandlerOnPhaseEntry")
        .withParameter("phase", SYSTEM_MGR_PHASE_SHUTDOWN)
        .andReturnValue(ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    systemMgrRAMPhaseEntryHandlers[0] = TestOnPhaseEntryHandlerBad;
    testPhaseEntryHandlerCalled = false;

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_SHUTDOWN);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(testPhaseEntryHandlerCalled);

    systemMgrRAMPhaseEntryHandlers[0] = NULL;
}

TEST(SystemMgrInternalGroup, TestNotifyEnterShutdownPhaseWithAllPatches)
{
    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);

    mock().expectNCalls(2, "DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE)
        .ignoreOtherParameters();

    systemMgrRAMPhaseEntryHandlers[0] = TestOnPhaseEntryHandlerGood;
    systemMgrRAMPhaseEntryHandlers[1] = TestOnPhaseEntryHandlerGood;
    testPhaseEntryHandlerCalled = false;

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(testPhaseEntryHandlerCalled);

    systemMgrRAMPhaseEntryHandlers[0] = NULL;
    systemMgrRAMPhaseEntryHandlers[1] = NULL;
}

TEST(SystemMgrInternalGroup, TestNotifyEnterPhaseWithSamePhase)
{
    // We expect a fatal error
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SystemMgrInternalGroup, TestNotifyEnterPhaseWithInvalidPhase)
{
    // We expect a fatal error
    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    ErrorT status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_NUM_PHASES);
    LONGS_EQUAL(ERROR_SYSERR, status);
}

TEST(SystemMgrInternalGroup, TestOnDoorbellEventInBadPhase)
{
    mock().expectOneCall("CommandDrvGetCommand")
        .andReturnValue(HOST_COMMAND_CODE_GET_STATUS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("DiagOnFatalError");
    mock().expectOneCall("PlatformExitCritical");

    systemMgrState.currentPriorityMask = 0;
    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_NUM_PHASES;
    ErrorT status = SystemMgrOnDoorbellEvent();
    LONGS_EQUAL(status, ERROR_SYSERR);
    LONGS_EQUAL(systemMgrState.currentPhase, SYSTEM_MGR_PHASE_NUM_PHASES);
    CHECK_TRUE((1 << SYSTEM_MGR_EVENT_FATAL_ERROR) == systemMgrState.currentPriorityMask);
}

TEST(SystemMgrInternalGroup, TestOnDoorbellEventInDebugPhase)
{
    mock().expectOneCall("CommandDrvGetCommand")
        .andReturnValue(HOST_COMMAND_CODE_GET_STATUS);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    mock().expectOneCall("CommandHandlerOnDoorbellEvent")
        .withParameter("commandCode", HOST_COMMAND_CODE_GET_STATUS)
        .andReturnValue(ERROR_SUCCESS);

    systemMgrState.currentPriorityMask = 0;
    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_DEBUG;
    ErrorT status = SystemMgrOnDoorbellEvent();
    LONGS_EQUAL(status, ERROR_SUCCESS);
    LONGS_EQUAL(systemMgrState.currentPhase, SYSTEM_MGR_PHASE_DEBUG);
    CHECK_TRUE(0 == systemMgrState.currentPriorityMask);
}

TEST(SystemMgrInternalGroup, TestOnDoorbellEventInDebugPhaseWithLoadPatchChunk)
{
    mock().expectOneCall("CommandDrvGetCommand")
        .andReturnValue(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_PATCH_START);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_PATCH);

    mock().expectOneCall("CommandHandlerOnDoorbellEvent")
        .withParameter("commandCode", HOST_COMMAND_CODE_LOAD_PATCH_CHUNK)
        .andReturnValue(ERROR_SUCCESS);

    systemMgrState.currentPriorityMask = 0;
    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_DEBUG;
    ErrorT status = SystemMgrOnDoorbellEvent();
    LONGS_EQUAL(status, ERROR_SUCCESS);
    LONGS_EQUAL(systemMgrState.currentPhase, SYSTEM_MGR_PHASE_PATCH);
    CHECK_TRUE(0 == systemMgrState.currentPriorityMask);
}

TEST(SystemMgrInternalGroup, TestOnDoorbellEventInDebugPhaseWithGetSensorId)
{
    mock().expectOneCall("CommandDrvGetCommand")
        .andReturnValue(HOST_COMMAND_CODE_GET_SENSOR_ID);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_SYSTEM_MGR_ON_CONFIGURE_START);

    setupNotifyEnterPhase(SYSTEM_MGR_PHASE_CONFIGURE);

    mock().expectOneCall("CommandHandlerOnDoorbellEvent")
        .withParameter("commandCode", HOST_COMMAND_CODE_GET_SENSOR_ID)
        .andReturnValue(ERROR_SUCCESS);

    systemMgrState.currentPriorityMask = 0;
    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_DEBUG;
    ErrorT status = SystemMgrOnDoorbellEvent();
    LONGS_EQUAL(status, ERROR_SUCCESS);
    LONGS_EQUAL(systemMgrState.currentPhase, SYSTEM_MGR_PHASE_CONFIGURE);
    CHECK_TRUE(0 == systemMgrState.currentPriorityMask);
}

TEST(SystemMgrInternalGroup, TestHandleEventWatchdog)
{
    mock().expectOneCall("HealthMonitorOnWatchdogEvent")
        .andReturnValue(ERROR_SUCCESS);

    ErrorT status = SystemMgrHandleEvent(SYSTEM_MGR_EVENT_WATCHDOG);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(SystemMgrInternalGroup, TestFatalErrorDumpHandler)
{
    uint32_t dump_buffer[64];
    SystemMgrStateT* ptr_state = (SystemMgrStateT*)dump_buffer;

    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_PATCH;
    systemMgrState.clockSpeedHz = 0x1000U;

    SystemMgrOnDiagFatalErrorDump(dump_buffer, 64UL);
    CHECK_EQUAL(systemMgrState.currentPhase, ptr_state->currentPhase);
    CHECK_EQUAL(systemMgrState.clockSpeedHz, ptr_state->clockSpeedHz);

    // Call with NULL buffer
    ptr_state->currentPhase = SYSTEM_MGR_PHASE_SESSION;
    ptr_state->clockSpeedHz = 0U;
    SystemMgrOnDiagFatalErrorDump(NULL, 64UL);
    CHECK_EQUAL(SYSTEM_MGR_PHASE_SESSION, ptr_state->currentPhase);
    CHECK_EQUAL(0U, ptr_state->clockSpeedHz);

    // Call with small buffer
    ptr_state->currentPhase = SYSTEM_MGR_PHASE_CONFIGURE;
    ptr_state->clockSpeedHz = 2000U;
    systemMgrState.currentPhase = SYSTEM_MGR_PHASE_INITIALIZE;
    systemMgrState.clockSpeedHz = 4000U;
    SystemMgrOnDiagFatalErrorDump(dump_buffer, 1UL);
    CHECK_EQUAL(systemMgrState.currentPhase, ptr_state->currentPhase);
    CHECK_EQUAL(2000U, ptr_state->clockSpeedHz);
}

