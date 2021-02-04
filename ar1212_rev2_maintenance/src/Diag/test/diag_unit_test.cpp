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
#include "SystemMgr.h"
#include "Diag.h"
#include "DiagInternal.h"

}

// Mocks
extern "C"
{
    void PlatformEnterCritical(void)
    {
        mock().actualCall("PlatformEnterCritical");
    }

    void PlatformExitCritical(void)
    {
        mock().actualCall("PlatformExitCritical");
    }

    bool CommandHandlerIsHostCommandInProgress(void)
    {
        mock().actualCall("CommandHandlerIsHostCommandInProgress");
        return (bool)mock().returnBoolValueOrDefault(false);
    }

    void * CommandDrvGetCommandParams(void)
    {
        mock().actualCall("CommandDrvGetCommandParams");
        return (void *)mock().returnUnsignedLongIntValueOrDefault(0x3FFF0000UL);
    }
}

void DiagOnWaitPausedNCalls(uint32_t pause)
{
    mock().expectOneCall("SystemDrvSetPaused");
    mock().expectNCalls(pause, "SystemDrvIsPaused").andReturnValue(true);
    mock().expectOneCall("SystemDrvIsPaused");
}

TEST_GROUP(DiagGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT status;

        status = DiagInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(DiagGroup, TestInit)
{
    ErrorT status;

    CHECK_EQUAL(true, diagControl.infoEnabled);

    status = DiagInit();
    for (uint32_t i = 0; i < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; i++)
    {
        FUNCTIONPOINTERS_EQUAL(NULL, diagFatalErrorDumpHandlers[i]);
    }
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_EQUAL(true, diagControl.infoEnabled);
}

TEST(DiagGroup, TestDiagOnWaitPaused)
{
    mock().expectOneCall("SystemDrvSetPaused");
    mock().expectOneCall("SystemDrvIsPaused");
    DiagOnWaitPaused();

    DiagOnWaitPausedNCalls(5);
    DiagOnWaitPaused();
}

TEST(DiagGroup, TestDiagSetCheckpoint)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint");
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpoint(checkpoint);
}

TEST(DiagGroup, TestDiagSetCheckpointWithPause)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint").andReturnValue(checkpoint);
    DiagOnWaitPausedNCalls(6);
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpoint(checkpoint);
}

TEST(DiagGroup, TestDiagSetCheckpointWithInfo)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;
    uint16_t info =  0x55U;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetInfo").withParameter("info", info);
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint");
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpointWithInfo(checkpoint, info);
}

TEST(DiagGroup, TestDiagSetCheckpointWithInfoDisabled)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;
    uint16_t info =  0x55U;

    DiagEnableInfo(false);
    CHECK_EQUAL(false, diagControl.infoEnabled);

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectNoCall("SystemDrvSetInfo");
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint");
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpointWithInfo(checkpoint, info);
}

TEST(DiagGroup, TestDiagSetCheckpointWithInfoWithPause)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;
    uint16_t info =  0x55U;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetInfo").withParameter("info", info);
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint").andReturnValue(checkpoint);
    DiagOnWaitPausedNCalls(6);
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpointWithInfo(checkpoint, info);
}

TEST(DiagGroup, TestDiagSetCheckpointWithInfoUnprotected)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;
    uint16_t info =  0x55U;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetInfo").withParameter("info", info);
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint");
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpointWithInfoUnprotected(checkpoint, info);
}

TEST(DiagGroup, TestDiagSetCheckpointWithInfoUnprotectedWithPause)
{
    DiagCheckpointT const checkpoint = DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING;
    uint16_t info =  0x55U;

    mock().expectOneCall("PlatformEnterCritical");
    mock().expectOneCall("SystemDrvSetInfo").withParameter("info", info);
    mock().expectOneCall("SystemDrvSetCheckpoint").withParameter("checkpoint", checkpoint);
    mock().expectOneCall("SystemDrvGetPausepoint").andReturnValue(checkpoint);
    DiagOnWaitPausedNCalls(6);
    mock().expectOneCall("PlatformExitCritical");
    DiagSetCheckpointWithInfoUnprotected(checkpoint, info);
}

void FatalErrorHandler(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords)
{
    for (uint32_t i = 0; i < sizeLongWords; i++)
    {
        ((uint32_t *)dumpBuffer)[i] = i;
    }
}

TEST(DiagGroup, TestDiagRegisterFatalErrorDumpHandler)
{
    ErrorT error = DiagInit();
    LONGS_EQUAL(ERROR_SUCCESS, error);

    for (uint32_t i = 0; i < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; i++)
    {
        error = DiagRegisterFatalErrorDumpHandler(FatalErrorHandler);
        LONGS_EQUAL(ERROR_SUCCESS, error);
        FUNCTIONPOINTERS_EQUAL(FatalErrorHandler, diagFatalErrorDumpHandlers[i]);
    }

    error = DiagRegisterFatalErrorDumpHandler(FatalErrorHandler);
    LONGS_EQUAL(ERROR_NOSPC, error);
}

TEST(DiagGroup, TestDiagOnFatalError)
{
    uint32_t buffer[DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS*DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS] = {0};
    ErrorT error = DiagInit();
    LONGS_EQUAL(ERROR_SUCCESS, error);

    mock().expectOneCall("CommandHandlerIsHostCommandInProgress").andReturnValue(false);
    DiagOnFatalError();

    mock().expectOneCall("CommandHandlerIsHostCommandInProgress").andReturnValue(true);
    mock().expectOneCall("CommandDrvGetCommandParams").andReturnValue((uint32_t)&buffer[0]);
    DiagOnFatalError();

    // Register handler and try again
    error = DiagRegisterFatalErrorDumpHandler(FatalErrorHandler);
    LONGS_EQUAL(ERROR_SUCCESS, error);

    mock().expectOneCall("CommandHandlerIsHostCommandInProgress").andReturnValue(true);
    mock().expectOneCall("CommandDrvGetCommandParams").andReturnValue((uint32_t)&buffer[0]);
    DiagOnFatalError();

    for (uint32_t i = 0; i < DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS; i++)
    {
        UNSIGNED_LONGS_EQUAL(i, buffer[i]);
    }
}

TEST(DiagGroup, TestDiagOnFatalErrorAllHandlers)
{
    uint32_t buffer[DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS*DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS] = {0};
    ErrorT error = DiagInit();
    LONGS_EQUAL(ERROR_SUCCESS, error);
    uint32_t offset;

    for (uint32_t i = 0; i < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; i++)
    {
        error = DiagRegisterFatalErrorDumpHandler(FatalErrorHandler);
        LONGS_EQUAL(ERROR_SUCCESS, error);
        FUNCTIONPOINTERS_EQUAL(FatalErrorHandler, diagFatalErrorDumpHandlers[i]);
    }

    mock().expectOneCall("CommandHandlerIsHostCommandInProgress").andReturnValue(true);
    mock().expectOneCall("CommandDrvGetCommandParams").andReturnValue((uint32_t)&buffer[0]);
    DiagOnFatalError();

    for (uint32_t i = 0; i < DIAG_MAX_FATAL_ERROR_DUMP_HANDLERS; i++)
    {
        offset = i*(DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS);
        for (uint32_t j = 0; j < DIAG_FATAL_ERROR_DUMP_REGION_SIZE_LONG_WORDS; j++)
        {
            UNSIGNED_LONGS_EQUAL(j, buffer[offset + j]);
        }
    }
}

TEST(DiagGroup, TestDiagEnableInfo)
{

    DiagEnableInfo(true);
    CHECK_EQUAL(true, diagControl.infoEnabled);

    DiagEnableInfo(false);
    CHECK_EQUAL(false, diagControl.infoEnabled);

    DiagEnableInfo(true);
    CHECK_EQUAL(true, diagControl.infoEnabled);
}
