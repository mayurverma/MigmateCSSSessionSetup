//***********************************************************************************
/// \file
///
/// Command Handler unit test
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
#include "CommandHandlerInternal.h"
#include "CommandHandler.h"
#include "Diag.h"
#include "CryptoMgr.h"
#include "SystemMgr.h"
#include "Interrupts.h"
#include "CommandDrv.h"
#include "SystemDrv.h"
}

// Mocks
extern "C"
{
    ErrorT HealthMonitorOnGetStatus(HostCommandCodeT const commandCode,
                                    CommandHandlerCommandParamsT params)
    {
        mock().actualCall("HealthMonitorOnGetStatus")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT PatchMgrOnLoadPatchChunk( HostCommandCodeT const commandCode,
                                            CommandHandlerCommandParamsT params)
    {
        mock().actualCall("PatchMgrOnLoadPatchChunk")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    // for patched RAM-based commands
    extern CommandHandlerCommandTableEntryT commandHandlerRAMCommandTable[];

    ErrorT SessionMgrOnSetSessionKeyPatched(HostCommandCodeT const commandCode,
                                            CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetSessionKeyPatched")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT HealthMonitorOnEnableMonitoringPatched( HostCommandCodeT const commandCode,
                                                   CommandHandlerCommandParamsT params)
    {
        mock().actualCall("HealthMonitorOnEnableMonitoringPatched")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT HealthMonitorOnGetStatusPatched( HostCommandCodeT const commandCode,
            CommandHandlerCommandParamsT params)
    {
        mock().actualCall("HealthMonitorOnGetStatusPatched")
        .withParameter("commandCode", commandCode)
        .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT SessionMgrOnSetEphemeralMasterSecretPatched( HostCommandCodeT const commandCode,
                                                        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetEphemeralMasterSecretPatched")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT ANewCommandHandlerFunction_1( HostCommandCodeT const commandCode,
                                       CommandHandlerCommandParamsT params)
    {
        mock().actualCall("ANewCommandHandlerFunction_1")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT ANewCommandHandlerFunction_2( HostCommandCodeT const commandCode,
                                       CommandHandlerCommandParamsT params)
    {
        mock().actualCall("ANewCommandHandlerFunction_2")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
}

// Helper functions
// Set the correct helper function expectation
#define TEST_PHASE_NONE                 (SystemMgrPhaseT)0

// some new commands
#define HOST_COMMAND_CODE_NEW_1         ((HostCommandCodeT)(0x8000 + 0x100))      // well above any existing command codes plus the doorbell bit
#define HOST_COMMAND_CODE_NEW_2         ((HostCommandCodeT)(0x8000 + 0x101))      // well above any existing command codes plus the doorbell bit


void setMockExpectOneCallForCommand( HostCommandCodeT CommandUnderTest )
{
    // each command will call a different handler
    switch ( CommandUnderTest )
    {
        case HOST_COMMAND_CODE_NULL:
            // Null command is allowed but won't do anything
            break;

        case HOST_COMMAND_CODE_GET_STATUS:
            mock().expectOneCall("HealthMonitorOnGetStatus")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT:
            mock().expectOneCall("LifecycleMgrOnApplyDebugEntitlement")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_AUTHORIZE_RMA:
            mock().expectOneCall("LifecycleMgrOnAuthorizeRMA")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_LOAD_PATCH_CHUNK:
            mock().expectOneCall("PatchMgrOnLoadPatchChunk")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_PROVISION_ASSET:
            mock().expectOneCall("AssetMgrOnProvisionAsset")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_SELF_TEST:
            mock().expectOneCall("CryptoMgrOnSelfTest")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_GET_SENSOR_ID:
            mock().expectOneCall("CryptoMgrOnGetSensorId")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_GET_CERTIFICATES:
            mock().expectOneCall("SessionMgrOnGetCertificates")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_SET_SESSION_KEYS:
            mock().expectOneCall("SessionMgrOnSetSessionKey")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS:
            mock().expectOneCall("SessionMgrOnSetPSKSessionKey")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET:
            mock().expectOneCall("SessionMgrOnSetEphemeralMasterSecret")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI:
            mock().expectOneCall("SessionMgrOnSetVideoAuthROI")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        case HOST_COMMAND_CODE_ENABLE_MONITORING:
            mock().expectOneCall("HealthMonitorOnEnableMonitoring")
                  .withParameter("commandCode", CommandUnderTest)
                  .withParameter("params", (void*)0x3fff1000UL);
            break;

        default:
            mock().crashOnFailure(true);        // test code error
            break;
    }
}

// Which phase(s) and command-handler are associated with each host command
typedef struct
{
    HostCommandCodeT            HostCommand;
    uint16_t                    NumAllowedPhases;
    SystemMgrPhaseT             AllowedPhase[7];    // up to 7 valid system phases
} CommandPhase_t;


CommandPhase_t      CommandPhaseTable[] =
{
    { HOST_COMMAND_CODE_NULL,                           0U, { TEST_PHASE_NONE }},
    { HOST_COMMAND_CODE_GET_STATUS,                     7U, { SYSTEM_MGR_PHASE_BOOT, SYSTEM_MGR_PHASE_INITIALIZE, SYSTEM_MGR_PHASE_DEBUG, SYSTEM_MGR_PHASE_PATCH, SYSTEM_MGR_PHASE_CONFIGURE, SYSTEM_MGR_PHASE_SESSION, SYSTEM_MGR_PHASE_SHUTDOWN }},
    { HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT,        1U, { SYSTEM_MGR_PHASE_DEBUG }},
    { HOST_COMMAND_CODE_AUTHORIZE_RMA,                  1U, { SYSTEM_MGR_PHASE_DEBUG }},
    { HOST_COMMAND_CODE_LOAD_PATCH_CHUNK,               2U, { SYSTEM_MGR_PHASE_DEBUG,  SYSTEM_MGR_PHASE_PATCH }},
    { HOST_COMMAND_CODE_PROVISION_ASSET,                1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_SELF_TEST,                      1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_GET_SENSOR_ID,                  1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_GET_CERTIFICATES,               1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_SET_SESSION_KEYS,               1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS,           1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET,    1U, { SYSTEM_MGR_PHASE_CONFIGURE }},
    { HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI,             1U, { SYSTEM_MGR_PHASE_SESSION }},
    { HOST_COMMAND_CODE_ENABLE_MONITORING,              1U, { SYSTEM_MGR_PHASE_SESSION }}
};

TEST_GROUP(CommandHandlerInitGroup)
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

TEST_GROUP(CommandHandlerGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        mock().expectOneCall("DiagSetCheckpoint")
            .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INIT);
        mock().expectOneCall("InterruptsClearInterruptPending")
                .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL);
        mock().expectOneCall("InterruptsEnableInterrupt")
              .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL)
              .withParameter("enable", true);

        ErrorT status = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(CommandHandlerInterruptGroup)
{
    // Option
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(CommandHandlerDoorbellEventGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        uint16_t LoopCount;

        // Need to clear the RAM-based command table - other tests may have used it
        for (LoopCount = 0; LoopCount < COMMAND_HANDLER_MAX_PATCH_COMMANDS; LoopCount++)
        {
            commandHandlerRAMCommandTable[LoopCount].commandCode = HOST_COMMAND_CODE_NULL;
            commandHandlerRAMCommandTable[LoopCount].phaseMask   = 0;
            commandHandlerRAMCommandTable[LoopCount].handler     = (CommandHandlerFunctionT)NULL;
        }
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(CommandHandlerOnPhaseEntryGroup)
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

TEST(CommandHandlerInitGroup, TestEnterInitPhase)
{
//    mock().expectOneCall("CommandDrvInit");
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INIT);
    mock().expectOneCall("InterruptsClearInterruptPending")
            .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL);
    mock().expectOneCall("InterruptsEnableInterrupt")
          .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL)
          .withParameter("enable", true);

    ErrorT status = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SUCCESS, status);
}

TEST(CommandHandlerInterruptGroup, TestDoorbellInterruptISR)
{
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_DOORBELL_ISR);

    mock().expectOneCall("SystemMgrSetEvent")
        .withParameter("event", SYSTEM_MGR_EVENT_DOORBELL);

    Doorbell_IrqHandler();
}

TEST(CommandHandlerInterruptGroup, TestDoorbellIcoreEccDedInterruptISR)
{
    mock().expectOneCall("SystemMgrReportCryptoFault")
          .withParameter("fault", SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE)
          .withParameter("context", 0)
          .withParameter("infoParam", 0);

    IcoreEccDed_IrqHandler();
}

// Call CommandHandlerOnDoorbellEvent() with all ROM commands and phases
TEST(CommandHandlerDoorbellEventGroup, TestROMCommandsInPhases)
{
    uint16_t            CommandCtr, NumberAllowedPhases, AllowedCmdPhase, TestPhase;
    ErrorT              ExpectedRetVal, ActualRetVal;
    HostCommandCodeT    CommandUnderTest;
    bool                IsAllowed = false;

    // Go through each command
    for (CommandCtr = 0; CommandCtr < (sizeof(CommandPhaseTable) / sizeof(CommandPhase_t)); CommandCtr++)
    {
        CommandUnderTest = CommandPhaseTable[CommandCtr].HostCommand;
        NumberAllowedPhases = CommandPhaseTable[CommandCtr].NumAllowedPhases;

        // and for each command go through each phase
        for (TestPhase = 0; TestPhase < SYSTEM_MGR_PHASE_NUM_PHASES; TestPhase++)
        {
            // is the current command allowed in this phase? Check the CommandPhaseTable[] above
            IsAllowed = false;
            for (AllowedCmdPhase = 0; AllowedCmdPhase < NumberAllowedPhases; AllowedCmdPhase++)
            {
                if (TestPhase == CommandPhaseTable[CommandCtr].AllowedPhase[AllowedCmdPhase])
                {
                    IsAllowed = true;
                }
            }

            // common expectations
            mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
                .withParameter("info", CommandUnderTest);

            mock().expectOneCall("DiagSetCheckpoint")
                .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

            mock().expectOneCall("SystemMgrGetCurrentPhase")
                .andReturnValue(TestPhase);

            // set different expectations depending on whether or not the command is allowed
            if (IsAllowed)
            {
                // command is allowed in this phase
                mock().expectOneCall("CommandDrvGetCommandParams");

                // each command will call a different handler, set appropriate expectation
                setMockExpectOneCallForCommand ( CommandUnderTest );

                mock().expectOneCall("DiagSetCheckpoint")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);

                mock().expectOneCall("CommandDrvSetResponse")
                    .withParameter("response", ERROR_SUCCESS);

                ExpectedRetVal = ERROR_SUCCESS;
            }
            else
            {
                // command is not allowed in this phase
                mock().expectOneCall("CommandDrvSetResponse")
                    .withParameter("response", ERROR_ACCESS);

                ExpectedRetVal = ERROR_ACCESS;
            }

            // call the function under test
            ActualRetVal = CommandHandlerOnDoorbellEvent( CommandUnderTest );
            LONGS_EQUAL(ExpectedRetVal, ActualRetVal);

            // all expectations must have been satisfied in this iteration
            mock().checkExpectations();

        }       // for TestPhase
    }       // for Command
}

// Call CommandHandlerOnDoorbellEvent() with a replacement RAM command and during all allowed phases
TEST(CommandHandlerDoorbellEventGroup, TestRAMCommandsReplacementAllowedPhases)
{
    ErrorT              RetVal;
    SystemMgrPhaseT     AllowedTestPhase[] = { SYSTEM_MGR_PHASE_DEBUG, SYSTEM_MGR_PHASE_SHUTDOWN };
    uint16_t            TestCount;

    // replace an existing command code with different allowed phases and new handler function
    commandHandlerRAMCommandTable[0].commandCode = HOST_COMMAND_CODE_SET_SESSION_KEYS;
    commandHandlerRAMCommandTable[0].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_DEBUG) | COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SHUTDOWN);
    commandHandlerRAMCommandTable[0].handler     = SessionMgrOnSetSessionKeyPatched;

    for ( TestCount = 0; TestCount < (sizeof(AllowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_SET_SESSION_KEYS);

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(AllowedTestPhase[TestCount]);

        mock().expectOneCall("CommandDrvGetCommandParams");

        mock().expectOneCall("SessionMgrOnSetSessionKeyPatched")
              .withParameter("commandCode", HOST_COMMAND_CODE_SET_SESSION_KEYS)
              .withParameter("params", (void*)0x3fff1000UL);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);

        mock().expectOneCall("CommandDrvSetResponse")
              .withParameter("response", ERROR_SUCCESS);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_SET_SESSION_KEYS );
        LONGS_EQUAL(ERROR_SUCCESS, RetVal);
        mock().checkExpectations();
    }
}

// Call CommandHandlerOnDoorbellEvent() with a replacement RAM command and during dis-allowed phases
TEST(CommandHandlerDoorbellEventGroup, TestRAMCommandsReplacementDisallowedPhase)
{
    ErrorT              RetVal;
    SystemMgrPhaseT     DisallowedTestPhase[] = { SYSTEM_MGR_PHASE_DEBUG, SYSTEM_MGR_PHASE_SHUTDOWN };
    uint16_t            TestCount;

    // replace an existing command code with different allowed phases and new handler function
    commandHandlerRAMCommandTable[0].commandCode = HOST_COMMAND_CODE_ENABLE_MONITORING;
    commandHandlerRAMCommandTable[0].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_INITIALIZE) | COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_BOOT);
    commandHandlerRAMCommandTable[0].handler     = HealthMonitorOnEnableMonitoringPatched;

    for ( TestCount = 0; TestCount < (sizeof(DisallowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_ENABLE_MONITORING);

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(DisallowedTestPhase[TestCount]);

        mock().expectOneCall("CommandDrvSetResponse")
             .withParameter("response", ERROR_ACCESS);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_ENABLE_MONITORING );
        LONGS_EQUAL(ERROR_ACCESS, RetVal);
        mock().checkExpectations();
    }
}

// Call CommandHandlerOnDoorbellEvent() with two replacement RAM commands and two new RAM commands during allowed phases
TEST(CommandHandlerDoorbellEventGroup, TestRAMCommandsReplaceAndNewAllowedPhases)
{
    ErrorT              RetVal;
    SystemMgrPhaseT     AllowedTestPhase[] = { SYSTEM_MGR_PHASE_SESSION, SYSTEM_MGR_PHASE_PATCH };
    uint16_t            TestCount;

    // replace the existing HOST_COMMAND_CODE_GET_STATUS command
    commandHandlerRAMCommandTable[0].commandCode = HOST_COMMAND_CODE_GET_STATUS;
    commandHandlerRAMCommandTable[0].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH);
    commandHandlerRAMCommandTable[0].handler     = HealthMonitorOnGetStatusPatched;

    // add a completely new command and handler function
    commandHandlerRAMCommandTable[1].commandCode = HOST_COMMAND_CODE_NEW_1;
    commandHandlerRAMCommandTable[1].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
    commandHandlerRAMCommandTable[1].handler     = ANewCommandHandlerFunction_1;

    // replace an existing command code and handler function
    commandHandlerRAMCommandTable[2].commandCode = HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET;
    commandHandlerRAMCommandTable[2].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH);
    commandHandlerRAMCommandTable[2].handler     = SessionMgrOnSetEphemeralMasterSecretPatched;

    // add a completely new command and handler function
    commandHandlerRAMCommandTable[3].commandCode = HOST_COMMAND_CODE_NEW_2;
    commandHandlerRAMCommandTable[3].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
    commandHandlerRAMCommandTable[3].handler     = ANewCommandHandlerFunction_2;

    // call the second replaced command
    for ( TestCount = 0; TestCount < (sizeof(AllowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(AllowedTestPhase[TestCount]);

        mock().expectOneCall("CommandDrvGetCommandParams");

        mock().expectOneCall("SessionMgrOnSetEphemeralMasterSecretPatched")
              .withParameter("commandCode", HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET)
              .withParameter("params", (void*)0x3fff1000UL);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);

        mock().expectOneCall("CommandDrvSetResponse")
              .withParameter("response", ERROR_SUCCESS);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET );
        LONGS_EQUAL(ERROR_SUCCESS, RetVal);
        mock().checkExpectations();
    }

    // call the second new command
    for ( TestCount = 0; TestCount < (sizeof(AllowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_NEW_1);

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(AllowedTestPhase[TestCount]);

        mock().expectOneCall("CommandDrvGetCommandParams");

        mock().expectOneCall("ANewCommandHandlerFunction_1")
              .withParameter("commandCode", HOST_COMMAND_CODE_NEW_1)
              .withParameter("params", (void*)0x3fff1000UL);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);

        mock().expectOneCall("CommandDrvSetResponse")
              .withParameter("response", ERROR_SUCCESS);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_NEW_1 );
        LONGS_EQUAL(ERROR_SUCCESS, RetVal);
        mock().checkExpectations();
    }

    // call the first replaced command
    for ( TestCount = 0; TestCount < (sizeof(AllowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_GET_STATUS);

        mock().expectOneCall("CommandDrvGetCommandParams");

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(AllowedTestPhase[TestCount]);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INVOKE_HANDLER);

        mock().expectOneCall("HealthMonitorOnGetStatusPatched")
              .withParameter("commandCode", HOST_COMMAND_CODE_GET_STATUS)
              .withParameter("params", (void*)0x3fff1000UL);

        mock().expectOneCall("CommandDrvSetResponse")
              .withParameter("response", ERROR_SUCCESS);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_GET_STATUS );
        LONGS_EQUAL(ERROR_SUCCESS, RetVal);
        mock().checkExpectations();
    }
}

// Call CommandHandlerOnDoorbellEvent() with a new RAM command and during dis-allowed phases
TEST(CommandHandlerDoorbellEventGroup, TestRAMCommanddNewDisallowedPhase)
{
    ErrorT              RetVal;
    SystemMgrPhaseT     DisallowedTestPhase[] = { SYSTEM_MGR_PHASE_DEBUG, SYSTEM_MGR_PHASE_SHUTDOWN };
    uint16_t            TestCount;

    // add a completely new command and handler function
    commandHandlerRAMCommandTable[0].commandCode = HOST_COMMAND_CODE_NEW_1;
    commandHandlerRAMCommandTable[0].phaseMask   = COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH) |
                                                   COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE);
    commandHandlerRAMCommandTable[0].handler     = ANewCommandHandlerFunction_1;

    for ( TestCount = 0; TestCount < (sizeof(DisallowedTestPhase) / sizeof(SystemMgrPhaseT)); TestCount++)
    {
        // set expectations
        mock().expectOneCall("DiagSetCheckpointWithInfo")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT)
              .withParameter("info", HOST_COMMAND_CODE_NEW_1);

        mock().expectOneCall("SystemMgrGetCurrentPhase")
              .andReturnValue(DisallowedTestPhase[TestCount]);

        mock().expectOneCall("CommandDrvSetResponse")
             .withParameter("response", ERROR_ACCESS);

        mock().expectOneCall("DiagSetCheckpoint")
              .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE);

        RetVal = CommandHandlerOnDoorbellEvent( HOST_COMMAND_CODE_NEW_1 );
        LONGS_EQUAL(ERROR_ACCESS, RetVal);
        mock().checkExpectations();
    }
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryInitializeWithFailedClearEccInterrupt)
{
    ErrorT      RetVal;

    // set expectations
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL)
          .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_ICORE_ECC_DED_INTERRUPT_FAILED)
          .withParameter("infoParam", INTERRUPT_ICORE_ECC_DED_FAIL);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INIT);

    // call the function under test
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_INVAL, RetVal);
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryInitializeWithFailedEnableEccInterrupt)
{
    ErrorT      RetVal;

    // set expectations
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL);

    mock().expectOneCall("InterruptsEnableInterrupt")
                  .withParameter("interrupt", INTERRUPT_ICORE_ECC_DED_FAIL)
                  .withParameter("enable", true)
                  .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_ICORE_ECC_DED_INTERRUPT_FAILED)
          .withParameter("infoParam", INTERRUPT_ICORE_ECC_DED_FAIL);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_INIT);

    // call the function under test
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_INVAL, RetVal);
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryDebug)
{
    ErrorT      RetVal;

    // set expectations
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_DOORBELL);

    mock().expectOneCall("InterruptsEnableInterrupt")
          .withParameter("interrupt", INTERRUPT_DOORBELL)
          .withParameter("enable", true);

    mock().expectOneCall("CommandDrvSetResponse")
          .withParameter("response", ERROR_SUCCESS);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ENTER_DEBUG);

    // call the function under test
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_SUCCESS, RetVal);
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryDebugWithFailedClearDoorbellInterrupt)
{
    ErrorT      RetVal;

    // set expectations
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_DOORBELL)
          .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_DOORBELL_INTERRUPT_FAILED)
          .withParameter("infoParam", INTERRUPT_DOORBELL);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ENTER_DEBUG);

    // call the function under test
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_INVAL, RetVal);
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryDebugWithFailedEnableDoorbellInterrupt)
{
    ErrorT      RetVal;

    // set expectations
    mock().expectOneCall("InterruptsClearInterruptPending")
          .withParameter("interrupt", INTERRUPT_DOORBELL);

    mock().expectOneCall("InterruptsEnableInterrupt")
          .withParameter("interrupt", INTERRUPT_DOORBELL)
          .withParameter("enable", true)
          .andReturnValue(ERROR_INVAL);

    mock().expectOneCall("SystemMgrReportFatalError")
          .withParameter("context", SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_DOORBELL_INTERRUPT_FAILED)
          .withParameter("infoParam", INTERRUPT_DOORBELL);

    mock().expectOneCall("DiagSetCheckpoint")
          .withParameter("checkpoint", DIAG_CHECKPOINT_COMMAND_HANDLER_ENTER_DEBUG);

    // call the function under test
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_DEBUG);
    LONGS_EQUAL(ERROR_INVAL, RetVal);
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryBenignPhases)
{
    uint16_t    LoopCount;
    ErrorT      RetVal;

    SystemMgrPhaseT BenignPhase[] =
    {
        SYSTEM_MGR_PHASE_PATCH, SYSTEM_MGR_PHASE_CONFIGURE,
        SYSTEM_MGR_PHASE_SESSION, SYSTEM_MGR_PHASE_SHUTDOWN
    };

    // Call the function for each benign phase - nothing will happen
    for (LoopCount = 0; LoopCount < (sizeof(BenignPhase) / sizeof(SystemMgrPhaseT)); LoopCount++)
    {
        RetVal = CommandHandlerOnPhaseEntry(BenignPhase[LoopCount]);
        LONGS_EQUAL(ERROR_SUCCESS, RetVal);
    }
}

TEST(CommandHandlerOnPhaseEntryGroup, TestOnPhaseEntryWithInvalidPhase)
{
    ErrorT      RetVal;
    bool        isInProgress;

    // call function with illegal phase
    RetVal = CommandHandlerOnPhaseEntry(SYSTEM_MGR_PHASE_NUM_PHASES);
    LONGS_EQUAL(ERROR_SUCCESS, RetVal);

    // Check that no command is in progress
    mock().expectOneCall("CommandDrvIsCommandInProgress");

    isInProgress = CommandHandlerIsHostCommandInProgress();
    CHECK_FALSE(isInProgress);
}

TEST(CommandHandlerGroup,TestCommandHandlerAddCommandNullPointer)
{
    ErrorT err = CommandHandlerAddCommand(NULL);
    CHECK_EQUAL(ERROR_INVAL,err);
}

TEST(CommandHandlerGroup,TestCommandHandlerAddCommandNullCommandCode)
{
    ErrorT err = ERROR_SUCCESS;
    //use random function with matching parameters to put in host command table
    //function never read so actual function used doesn't matter
    CommandHandlerFunctionT commandHandler = CryptoMgrOnGetSensorId;

    CommandHandlerCommandTableEntryT testEntry = {HOST_COMMAND_CODE_NULL,0,commandHandler};
    err = CommandHandlerAddCommand(&testEntry);
    CHECK_EQUAL(ERROR_INVAL,err);
}

TEST(CommandHandlerGroup,TestCommandHandlerAddCommandNullHandler)
{
    ErrorT err = ERROR_SUCCESS;
    //Host command code has no significance, use to check if null function pointer
    CommandHandlerCommandTableEntryT testEntry = {HOST_COMMAND_CODE_SELF_TEST,0,NULL};
    err = CommandHandlerAddCommand(&testEntry);
    CHECK_EQUAL(ERROR_INVAL,err);
}

TEST(CommandHandlerGroup,TestCommandHandlerAddCommandFullTable)
{
    ErrorT err = ERROR_SUCCESS;
    //use random function with matching parameters to put in host command table
    //function never read so actual function used doesn't matter
    CommandHandlerFunctionT commandHandler = &CryptoMgrOnGetSensorId;
    CommandHandlerCommandTableEntryT garbageEntry = {HOST_COMMAND_CODE_SELF_TEST,14,commandHandler};
    CommandHandlerCommandTableEntryT newEntry = {HOST_COMMAND_CODE_GET_SENSOR_ID,30,commandHandler};
    for(uint16_t idx = 0;idx<COMMAND_HANDLER_MAX_PATCH_COMMANDS;idx++)
    {
        commandHandlerRAMCommandTable[idx] = garbageEntry;
    }
    err = CommandHandlerAddCommand(&newEntry);
    CHECK_EQUAL(ERROR_NOSPC,err);
}

TEST(CommandHandlerGroup,TestCommandHandlerAddCommandAddNewCommand)
{
    ErrorT err = ERROR_SUCCESS;
    //use random function with matching parameters to put in host command table
    //function never read so actual function used doesn't matter
    CommandHandlerFunctionT commandHandler = &CryptoMgrOnGetSensorId;
    CommandHandlerCommandTableEntryT nullEntry = {HOST_COMMAND_CODE_NULL,0,NULL};
    CommandHandlerCommandTableEntryT newEntry = {HOST_COMMAND_CODE_GET_SENSOR_ID,30,commandHandler};
    for(uint16_t idx = 0;idx<COMMAND_HANDLER_MAX_PATCH_COMMANDS;idx++)
    {
        commandHandlerRAMCommandTable[idx] = nullEntry;
    }
    err = CommandHandlerAddCommand(&newEntry);
    CHECK_EQUAL(ERROR_SUCCESS,err);
    CHECK_EQUAL(HOST_COMMAND_CODE_GET_SENSOR_ID,commandHandlerRAMCommandTable[0].commandCode);
    CHECK_EQUAL(30,commandHandlerRAMCommandTable[0].phaseMask);
    CHECK_EQUAL(commandHandler,commandHandlerRAMCommandTable[0].handler);
}

TEST(CommandHandlerGroup, TestFatalErrorStopNoCommandInProgress)
{
    mock().expectOneCall("CommandDrvIsCommandInProgress")
        .andReturnValue(false);
    mock().expectOneCall("SystemDrvTerminate");
    CommandHandlerFatalErrorStop();
}

TEST(CommandHandlerGroup, TestFatalErrorStopWithCommandInProgress)
{
    mock().expectOneCall("CommandDrvIsCommandInProgress")
        .andReturnValue(true);
    mock().expectOneCall("CommandDrvSetResponse")
        .withParameter("response", ERROR_SYSERR);
    mock().expectOneCall("SystemDrvTerminate");
    CommandHandlerFatalErrorStop();
}

