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
#define WINDOWS
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#include "cpputest_support.h"

extern "C"
{
#include "PatchMgr.h"
#include "PatchMgrInternal.h"
#include "Diag.h"
#include "LifeCycleMgr.h"
#include "SystemMgr.h"

#if defined(BUILD_TARGET_ARCH_x86)
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */
#include "patcher.h"

PATCHER_REGS_T patcherRegsUnitTest = {};
#endif

PatchMgrPatchedFunctionAddressT patchFunctionTableUnitTest[PATCH_MGR_MAX_PATCH_TABLE_ENTRIES];

}

// Test vectors
typedef struct
{
    PatchMgrLoadPatchChunkCommandHeaderT  header;
    uint16_t dataWord[4];
    uint8_t padding[8];
    uint8_t assetTag[16];
} PatchMgrTestChunk4T;

uint32_t patchSection[200];
uint8_t zeroes[PATCH_MGR_MAX_PATCH_TABLE_ENTRIES*4] =  {0};
volatile PatchMgrPatchedFunctionAddressT patchMgrPatchTable[PATCH_MGR_MAX_PATCH_TABLE_ENTRIES];



// Asset needs to be a multiple of 16 bytes. 16 bytes for header thus chunk plus padding needs to be a multiple of 16
// bytes
PatchMgrTestChunk4T  patchChunk4 =
{
        0xAAAAAAAA,                         // assetId Unique identifier for the patch chunk
        0x11111111,                         // version Version of ROM code
        0x22222222,                         // token  Version of ROM code
        32,                                 // chunkLengthBytes Length of the chunk in bytes
        {0,1,2,3,4,5,6,7},                  // reserved[8] Version of ROM code
        {0,1,2,3,4,5,6,7,8,9,10,11},        // nonce[12] Provisioning nonce (a number used only once)
        0x00,                               // patchVersion Patch Version
        0x01,                               // romVersion Version of ROM code
        0x00,                               // lastChunk Id of last 'chunk' in sequence, Total chunks = lastChunk + 1
        0x00,                               // thisChunk Id of current chunk
        (uint32_t)&patchSection[0],         // loadAddress address in memory to copy payload data too
        0x00000000,                         // loaderAddress If last chunk and none zero, call function at this address
        0x00000004,                         // dataLengthWords Number of words (16-bit) in this 'chunk'
        {0x1111, 0x2222, 0x3333, 0x4444},   // Patch 'chunk'
        {1,1,1,1,1,1,1,1},                  // Padding (chunk plus padding needs to be a multiple of 16 bytes)
        {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},  // asset Tag.
};

uint16_t loaderCallCount = 0;

// Mocks
extern "C"
{
    ErrorT AppLoader(void)
    {
        loaderCallCount++;
        return ERROR_SUCCESS;
    }

    ErrorT AppLoaderError(void)
    {
        return ERROR_NOEXEC;
    }

}

TEST_GROUP(PatchMgrInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(&patchMgr, 0, sizeof(patchMgr));
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(PatchMgrGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))PatchMgrOnDiagFatalErrorDump);
        mock().expectOneCall("DiagSetCheckpoint")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_INIT);

        ErrorT status = PatchMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(PatchMgrGroupVector)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))PatchMgrOnDiagFatalErrorDump);
        mock().expectOneCall("DiagSetCheckpoint")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_INIT);

        ErrorT status = PatchMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(PatchMgrPatchSupport)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        memset(patchFunctionTableUnitTest, 0, sizeof(patchFunctionTableUnitTest));
        memset(&patcherRegsUnitTest, 0, sizeof(patcherRegsUnitTest));
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(PatchMgrInitGroup, TestInit)
{
    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))PatchMgrOnDiagFatalErrorDump);
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_INIT);

	CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);
    ErrorT status = PatchMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_EQUAL(PATCH_MGR_STATUS_INIT, patchMgr.status);
    MEMCMP_EQUAL(zeroes, (void*)patchMgrPatchTable, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES*sizeof(PatchMgrPatchedFunctionAddressT))
}

TEST(PatchMgrInitGroup, TestInitRegistrationFail)
{
    mock().expectOneCall("DiagRegisterFatalErrorDumpHandler")
        .withFunctionPointerParameter("handler", (void (*)(void))PatchMgrOnDiagFatalErrorDump)
        .andReturnValue(ERROR_NOSPC);
    mock().expectOneCall("DiagSetCheckpoint")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_INIT);

    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_INIT_FAILED)
        .withParameter("infoParam", ERROR_NOSPC);

    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);
    ErrorT status = PatchMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE);
    LONGS_EQUAL(ERROR_NOSPC, status);
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);
    MEMCMP_EQUAL(zeroes, (void*)patchMgrPatchTable, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES*sizeof(PatchMgrPatchedFunctionAddressT))
}

TEST(PatchMgrInitGroup, TestMgrOnPhaseEntryPhaseInvalid)
{
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_NUM_PHASES;

    mock().expectNoCall("DiagSetCheckpoint");
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);

    ErrorT status = PatchMgrOnPhaseEntry(phase);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);
}

TEST(PatchMgrInitGroup, TestMgrOnPhaseEntryPhaseUnsupported)
{
    SystemMgrPhaseT phase = SYSTEM_MGR_PHASE_BOOT;

    mock().expectNoCall("DiagSetCheckpoint");
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);

    ErrorT status = PatchMgrOnPhaseEntry(phase);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, patchMgr.status);
}

TEST(PatchMgrInitGroup, TestOnLoadPatchChunkUninitialized)
{
    ErrorT error;
    uint32_t    command[100];

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);
    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)command);
    CHECK_EQUAL(ERROR_ACCESS, error);
}

TEST(PatchMgrGroup, TestPatchGetPatchVersion)
{
    PatchMgrPatchVersionT version;

    patchMgr.patchVersion = 0x55;
    version = PatchMgrGetPatchVersion();
    CHECK_EQUAL(patchMgr.patchVersion, version);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkWrongCommand)
{
    ErrorT error;
    uint32_t    command[100];

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_ON_LOAD_PATCH_CHUNK_BAD_COMMAND)
        .withParameter("infoParam", HOST_COMMAND_CODE_NULL);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_NULL, (CommandHandlerCommandParamsT)command);
    CHECK_EQUAL(ERROR_SYSERR, error);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkNullParams)
{
    ErrorT error;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("SystemMgrReportFatalError")
        .withParameter("context", SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_HANDLE_COMMAND_BAD_PARAM)
        .withParameter("infoParam", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)NULL);
    CHECK_EQUAL(ERROR_SYSERR, error);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkWrongRomVersion)
{
    ErrorT error;
    PatchMgrLoadPatchChunkCommandHeaderT  message;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.chunk.thisChunk = 0U;
    message.chunk.lastChunk = 1U;
    message.chunk.romVersion =  (uint16_t)(SystemMgrGetRomVersion() + 1U);        // Force to different ROM version

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkInvalidPatchVersion)
{
    ErrorT error;
    PatchMgrLoadPatchChunkCommandHeaderT  message;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.chunk.thisChunk = 0U;
    message.chunk.lastChunk = 1U;
    message.chunk.romVersion = SystemMgrGetRomVersion();        // Set to correct ROM version
    message.chunk.patchVersion = 0U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_RANGE, error);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkInvalidFirstChunk)
{
    ErrorT error;
    PatchMgrLoadPatchChunkCommandHeaderT  message;
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.chunk.romVersion = SystemMgrGetRomVersion();
    message.chunk.patchVersion = 1U;
    message.chunk.thisChunk = 3U;                               // First chunk must be Zero

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_INVAL, error);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);
}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunk)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
            .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_CALL_LOADER)
            .withParameter("info", (uint16_t)((uint32_t)AppLoader));
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_COMPLETE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords)
    CHECK_EQUAL(1U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkLoaderError)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
            .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_CALL_LOADER)
            .withParameter("info", (uint16_t)((uint32_t)AppLoaderError));
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoaderError;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_NOEXEC, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords)
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkNoLoader)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = 0U;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_NOENT, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkEncryptedLcsError)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT *)&lcsCurrent, sizeof(lcsCurrent))
        .andReturnValue(ERROR_SYSERR);
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SYSERR, error);
    CHECK_EQUAL(0U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkEncrypted)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT *)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING);
    mock().expectOneCall("CryptoMgrAuthenticatePatchChunk")
//                    .withParameter("assetId", assetId)
//                    .withParameter("assetPackageBuffer", assetPackageBuffer)
//                    .withOutputParameter("decryptedPackageSizeBytes", decryptedPackageSizeBytes)
            .withOutputParameterReturning("decryptedPackage", &message.header.chunk, (sizeof(PatchMgrLoadPatchChunkCommandHeaderT) + 8U))
            .ignoreOtherParameters();
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
            .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_CALL_LOADER)
            .withParameter("info", (uint16_t)((uint32_t)AppLoader));
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunkLengthBytes = sizeof(PatchMgrLoadPatchChunkCommandHeaderT) + 8U;
    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_COMPLETE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords)
    CHECK_EQUAL(1U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkEncryptedNoHeap)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT *)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunkLengthBytes = 0xffffffff;
    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_NOSPC, error);
    CHECK_EQUAL(0U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkSingleChunkEncryptedCorrupted)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[10];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT *)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING);
    mock().expectOneCall("CryptoMgrAuthenticatePatchChunk").ignoreOtherParameters().andReturnValue(ERROR_BADMSG);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunkLengthBytes = 8U;
    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 0U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_BADMSG, error);
    CHECK_EQUAL(0U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkMultipleChunks)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[16];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(1U, patchMgr.expectedPatchVersion);
    CHECK_EQUAL(0U, patchMgr.patchVersion);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(0U, loaderCallCount);

    // Send Next chunk
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 1U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 1U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(2U, patchMgr.expectedChunk);
    CHECK_EQUAL(1U, patchMgr.expectedPatchVersion);
    CHECK_EQUAL(0U, patchMgr.patchVersion);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[4], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(0U, loaderCallCount);

    // Send Final chunk
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 2U);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
            .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_CALL_LOADER)
            .withParameter("info", (uint16_t)((uint32_t)AppLoader));
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 2U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[8]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(3U, patchMgr.expectedChunk);
    CHECK_EQUAL(1U, patchMgr.expectedPatchVersion);
    CHECK_EQUAL(1U, patchMgr.patchVersion);
    CHECK_EQUAL(PATCH_MGR_STATUS_COMPLETE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[8], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(1U, loaderCallCount);

    // Tag chunk to the end should be rejected
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 3U;                // Correct sequence, but greater then lastChunk
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[8]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_ACCESS, error);
    CHECK_EQUAL(3U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_COMPLETE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[8], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(1U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkMultipleChunksPatchVersionChange)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[16];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(1U, patchMgr.expectedPatchVersion);
    CHECK_EQUAL(0U, patchMgr.patchVersion);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(0U, loaderCallCount);

    // Send Next chunk
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 2U;
    message.header.chunk.thisChunk = 1U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_RANGE, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(1U, patchMgr.expectedPatchVersion);
    CHECK_EQUAL(0U, patchMgr.patchVersion);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkMultipleChunksWrongOrder)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[16];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(0U, loaderCallCount);

    // Send Next chunk
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 2U;                    // Wrong order
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_INVAL, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

    // Now send correct chunk - System should be locked out
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 1U;                    // Correct sequence
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_ACCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}

TEST(PatchMgrGroup, TestOnLoadPatchChunkMultipleLastChunkError)
{
    ErrorT error;
    PatchMgrTestChunk4T  message;
    uint16_t    patchArea[16];
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 0U;
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[0]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x0001U;
    message.dataWord[1] = 0x0020U;
    message.dataWord[2] = 0x0300U;
    message.dataWord[3] = 0x4000U;

    loaderCallCount = 0;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    MEMCMP_EQUAL(&message.dataWord[0],&patchArea[0], 2*message.header.chunk.dataLengthWords);
    CHECK_EQUAL(0U, loaderCallCount);

    // Send Next chunk
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(2, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", false);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR);
    mock().expectOneCall("SystemMgrSetEvent").withParameter("event", SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 1U;                    // Wrong order
    message.header.chunk.lastChunk = 3U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_INVAL, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

    // Now send correct chunk - System should be locked out
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectNCalls(1, "SystemMgrGetRomVersion");
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR);

    message.header.chunk.romVersion = SystemMgrGetRomVersion();
    message.header.chunk.patchVersion = 1U;
    message.header.chunk.thisChunk = 1U;                    // Correct sequence
    message.header.chunk.lastChunk = 2U;
    message.header.chunk.dataLengthWords = 0x4U;
    message.header.chunk.loadAddress = (uint32_t)(&patchArea[4]);
    message.header.chunk.loaderAddress = (uint32_t)AppLoader;

    message.dataWord[0] = 0x000AU;
    message.dataWord[1] = 0x00B0U;
    message.dataWord[2] = 0x0C00U;
    message.dataWord[3] = 0xD000U;

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)&message);
    CHECK_EQUAL(ERROR_ACCESS, error);
    CHECK_EQUAL(1U, patchMgr.expectedChunk);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, patchMgr.status);
    CHECK_EQUAL(0U, loaderCallCount);

}


TEST(PatchMgrGroupVector, TestOnLoadPatchChunkGeneratedCommand)
{
    ErrorT error;
    uint16_t    patchArea[256] = {0};
    PatchMgrLoadPatchChunkCommandHeaderT* command;
    char rootPath[FILENAME_MAX];
    char patchFile[FILENAME_MAX] = {0};
    LifecycleMgrLifecycleStateT lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    CHECK((GetCurrentDir(rootPath, sizeof(rootPath))));
    rootPath[sizeof(rootPath) - 1] = '\0'; /* not really required */
    char * pos = strstr(rootPath, "PatchMgr");
    pos[sizeof("PatchMgr")] = '\0';
    strcpy(patchFile, rootPath);
    strcat(patchFile, "test\\patch_00.bin");


    printf ("Opening: %s", patchFile);

    FILE * file = fopen(patchFile, "rb+");
    CHECK((file != NULL));

    // Reading size of file
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    fclose(file);

    // Reading data to array of unsigned chars
    file = fopen(patchFile, "rb+");
    CHECK((file != NULL));
    unsigned char * in = (unsigned char *) malloc(size);
    (void)fread(in, sizeof(unsigned char), size, file);
    fclose(file);

    command = (PatchMgrLoadPatchChunkCommandHeaderT*)in;
    CHECK_EQUAL(0x20000000U, command->chunk.loadAddress);
    CHECK_EQUAL(0U, command->chunk.loaderAddress);

    command->chunk.loadAddress = (uint32_t)&patchArea[command->chunk.loadAddress & 0xFF];

    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS);
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
        .withOutputParameterReturning("lcsCurrent", (LifecycleMgrLifecycleStateT*)&lcsCurrent, sizeof(lcsCurrent));
    mock().expectNCalls(1, "SystemMgrGetRomVersion").andReturnValue(0x55U);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_PROCESSING);
    mock().expectOneCall("DiagSetCheckpointWithInfo")
        .withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_COPY_PATCH)
        .withParameter("info", 0U);
    mock().expectOneCall("SystemMgrEnablePatchRegion").withParameter("enable", true);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_PROCESS_CHUNK_EXIT);
    mock().expectOneCall("DiagSetCheckpoint").withParameter("checkpoint", DIAG_CHECKPOINT_PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT);

    error = PatchMgrOnLoadPatchChunk(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK, (CommandHandlerCommandParamsT)in);
    CHECK_EQUAL(ERROR_SUCCESS, error);
    CHECK_EQUAL(PATCH_MGR_STATUS_ACTIVE, patchMgr.status);
    CHECK_EQUAL(0x1111, patchArea[0]);
    CHECK_EQUAL(0x1111, patchArea[1]);
    CHECK_EQUAL(0x2222, patchArea[2]);
    CHECK_EQUAL(0x2222, patchArea[3]);
    CHECK_EQUAL(0x3333, patchArea[4]);
    CHECK_EQUAL(0x3333, patchArea[5]);
    CHECK_EQUAL(0xBBBB, patchArea[6]);
    CHECK_EQUAL(0xBBBB, patchArea[7]);
    CHECK_EQUAL(0xB510, patchArea[8]);
    CHECK_EQUAL(0xF000, patchArea[9]);

}

TEST(PatchMgrGroup, TestOnDiagFatalErrorDumpParamError)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);
    PatchMgrT*  dump = ( PatchMgrT*)buffer;

    patchMgr.status = PATCH_MGR_STATUS_ERROR;
    patchMgr.patchVersion = 0xAAAAU;
    patchMgr.lastChunk = 0xBBBBU;
    patchMgr.expectedChunk = 0xCCCCU;

    PatchMgrOnDiagFatalErrorDump(NULL, bufferSizeLongWords);
    CHECK_EQUAL(PATCH_MGR_STATUS_UNINIT, dump->status);
    CHECK_EQUAL(0U, dump->patchVersion);
    CHECK_EQUAL(0U, dump->expectedChunk);
    CHECK_EQUAL(0U, dump->lastChunk);

    PatchMgrOnDiagFatalErrorDump(buffer, 1U);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, dump->status);
    CHECK_EQUAL(0U, dump->patchVersion);
    CHECK_EQUAL(0U, dump->expectedChunk);
    CHECK_EQUAL(0U, dump->lastChunk);

}


TEST(PatchMgrGroup, TestOnDiagFatalErrorDumpParam)
{
    uint32_t buffer[64] = {0U};
    uint32_t bufferSizeLongWords = sizeof(buffer)/sizeof(buffer[0]);
    PatchMgrT*  dump = ( PatchMgrT*)buffer;

    patchMgr.status = PATCH_MGR_STATUS_ERROR;
    patchMgr.patchVersion = 0xAAAAU;
    patchMgr.lastChunk = 0xBBBBU;
    patchMgr.expectedChunk = 0xCCCCU;

    PatchMgrOnDiagFatalErrorDump(buffer, bufferSizeLongWords);
    CHECK_EQUAL(PATCH_MGR_STATUS_ERROR, dump->status);
    CHECK_EQUAL(0xAAAAU, dump->patchVersion);
    CHECK_EQUAL(0xCCCCU, dump->expectedChunk);
    CHECK_EQUAL(0xBBBBU, dump->lastChunk);
}

void CheckExpectedEnableRegister(uint32_t value)
{
    //printf ("patcherRegsUnitTest.enable: %x vs %x", value, patcherRegsUnitTest.enable);
    CHECK_EQUAL(value, patcherRegsUnitTest.enable);
}

void CheckPatchTables(uint32_t index, PatchMgrComparatorFunctionAddressT comparator, PatchMgrPatchedFunctionAddressT patched)
{
    volatile PatchMgrComparatorFunctionAddressT * comparators = (PatchMgrComparatorFunctionAddressT *)&patcherRegsUnitTest.comparator_00;
    CHECK_EQUAL(comparator, comparators[index]);
    CHECK_EQUAL(patched, patchFunctionTableUnitTest[index]);
}

TEST(PatchMgrPatchSupport, TestPatchSupport)
{
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorAndPatchedTable(0, (PatchMgrComparatorFunctionAddressT)0xCAFE0, (PatchMgrPatchedFunctionAddressT)0xDEAD0, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CheckExpectedEnableRegister(1 << 0);
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorAndPatchedTable(1, (PatchMgrComparatorFunctionAddressT)0xCAFE1, (PatchMgrPatchedFunctionAddressT)0xDEAD1, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorAndPatchedTable(8, (PatchMgrComparatorFunctionAddressT)0xCAFE8, (PatchMgrPatchedFunctionAddressT)0xDEAD8, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorAndPatchedTable(16,(PatchMgrComparatorFunctionAddressT)0xCAF16, (PatchMgrPatchedFunctionAddressT)0xDEA16, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorAndPatchedTable(31,(PatchMgrComparatorFunctionAddressT)0xCAF31, (PatchMgrPatchedFunctionAddressT)0xDEA31, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));

    CheckPatchTables(0,  (PatchMgrComparatorFunctionAddressT)0xCAFE0, (PatchMgrPatchedFunctionAddressT)0xDEAD0);
    CheckPatchTables(1,  (PatchMgrComparatorFunctionAddressT)0xCAFE1, (PatchMgrPatchedFunctionAddressT)0xDEAD1);
    CheckPatchTables(8,  (PatchMgrComparatorFunctionAddressT)0xCAFE8, (PatchMgrPatchedFunctionAddressT)0xDEAD8);
    CheckPatchTables(16, (PatchMgrComparatorFunctionAddressT)0xCAF16, (PatchMgrPatchedFunctionAddressT)0xDEA16);
    CheckPatchTables(31, (PatchMgrComparatorFunctionAddressT)0xCAF31, (PatchMgrPatchedFunctionAddressT)0xDEA31);

    CheckExpectedEnableRegister((1U << 0)|(1U << 1)|(1U << 8)|(1U << 16)|(1U << 31));

    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrDisableComparator(0));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrDisableComparator(1));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrDisableComparator(8));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrDisableComparator(16));
    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrDisableComparator(31));

    CheckExpectedEnableRegister(0);


}


TEST(PatchMgrPatchSupport, TestPatchSupportError)
{
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorAndPatchedTable(32, (PatchMgrComparatorFunctionAddressT)0xCAFEBABE, (PatchMgrPatchedFunctionAddressT)0xDEADBEEF, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorAndPatchedTable(32, (PatchMgrComparatorFunctionAddressT)0xCAFEBABE, (PatchMgrPatchedFunctionAddressT)0xDEADBEEF, patchFunctionTableUnitTest, 33));
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorAndPatchedTable(31, NULL, (PatchMgrPatchedFunctionAddressT)0xDEADBEEF, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorAndPatchedTable(31, (PatchMgrComparatorFunctionAddressT)0xCAFEBABE, NULL, patchFunctionTableUnitTest, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorAndPatchedTable(31, (PatchMgrComparatorFunctionAddressT)0xCAFEBABE, (PatchMgrPatchedFunctionAddressT)0xDEADBEEF, NULL, PATCH_MGR_MAX_PATCH_TABLE_ENTRIES));
    CHECK_EQUAL(ERROR_SYSERR, PatchMgrDisableComparator(32));
}

TEST(PatchMgrPatchSupport, TestPatchSupportSetupComparatorTable)
{
    const uint32_t num_functions = 2;

    /// The contents of this table (comparators) should be in sync with the patchFunctionTable
    const PatchMgrComparatorFunctionAddressT patchComparatorTable[num_functions] =
    {
        (PatchMgrComparatorFunctionAddressT)0xAAAA,
        (PatchMgrComparatorFunctionAddressT)0xBBBB,
    };

    CHECK_EQUAL(ERROR_SUCCESS, PatchMgrSetupComparatorTable(0, num_functions, patchComparatorTable, num_functions));
    CheckPatchTables(0,  (PatchMgrComparatorFunctionAddressT)0xAAAA, (PatchMgrPatchedFunctionAddressT)0);
    CheckPatchTables(1,  (PatchMgrComparatorFunctionAddressT)0xBBBB, (PatchMgrPatchedFunctionAddressT)0);

    CHECK_EQUAL(ERROR_SYSERR, PatchMgrSetupComparatorTable(0, num_functions+1, patchComparatorTable, num_functions));
}



