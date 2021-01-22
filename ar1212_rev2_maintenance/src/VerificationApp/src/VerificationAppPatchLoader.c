//***********************************************************************************
/// \file
///
/// Verification Application
///
/// \addtogroup verificationapp
/// @{
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

#include "Common.h"
#include "VerificationAppInternal.h"
#include "Diag.h"
#include "CommandHandler.h"
#include "CommandHandlerInternal.h"
#include "Init.h"
#include "NvmMgr.h"

#include "LifecycleMgr.h"
#include "LifecycleMgrInternal.h"

#include "AR0820.h"
#include "PatchMgr.h"
#include "patcher.h"
#include "VerificationAppDebugId.h"

#include "Interrupts.h"

#include "HealthMonitor.h"
#include "HealthMonitorInternal.h"

#include "SystemMgr.h"
#include "SystemMgrInternal.h"

#include "WatchdogDrv.h"
#include "Toolbox.h"
#include "OtpmDrv.h"
#include "crypto_otpm.h"

#include "cc_pal_types.h"
#include "cc_pal_apbc.h"
#include "cc_pal_pm.h"
#include "cc_pal_interrupt_ctrl_plat.h"
#include "cc_pal_mem.h"

#include "bsv_otp_api.h"
#include "cc_pal_abort.h"
#include "cc_pal_error.h"

#include "InitInternal.h"

extern CCError_t CC_PalSecMemCmp(const uint8_t* aTarget, const uint8_t* aSource, size_t aSize);

extern uint16_t SystemMgrGetRomVersionPatched(void);
extern uint16_t SystemMgrGetRomVersionBeefPatched(void);

extern const PatchMgrPatchedFunctionAddressT
    verificationAppPatchedFunctionTable[VERIFICATION_APP_NUM_PATCHED_FUNCTIONS];
extern const PatchMgrComparatorFunctionAddressT verificationAppComparatorTable[VERIFICATION_APP_NUM_PATCHED_FUNCTIONS];

extern HealthMonitorStateT healthMonitorState;
extern void _sys_exit(int return_code);
extern void _ttywrch(int ch);

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
uint32_t verificationAppDebugId;
uint32_t verificationAppDebugParameter;
uint32_t verificationAppDebugPacketLength;
uint32_t verificationAppDebugVar;
uint8_t buffer1[] = {1, 2, 3, 4};

ErrorT VerificationAppExecute(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params);
ErrorT VerificationAppFillStack(void);
void New_IrqHandler(void);
int VerificationAppTestMpu(VerificationAppDebugIdT verificationAppDebugId);

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// After all patch chunks are downloaded, this function is called to load/hook the patched functions
/// and patched-in new commands.
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Patch loaded successfully.
/// \retval ERROR_SYSERR                Error updating interrupt vector table
ErrorT VerificationAppLoader(void)
{
    ErrorT error;
    CommandHandlerCommandTableEntryT command;

    verificationAppDebugId = 0U;
    verificationAppDebugParameter = 0U;
    verificationAppDebugVar = 0U;

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_MAX, 0xAAAA);

    error = PatchMgrSetupComparatorTable(0U, VERIFICATION_APP_NUM_PATCHED_FUNCTIONS, verificationAppComparatorTable,
            VERIFICATION_APP_NUM_PATCHED_FUNCTIONS);

    if (ERROR_SUCCESS == error)
    {
        // Add command to aid testing
        // add 2 new TrngApp commands
        command.commandCode = HOST_COMMAND_CODE_EXECUTE_DEBUG;
        command.phaseMask = (COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE) |
                             COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) |
                             COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_BOOT) |
                             COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_INITIALIZE) |
                             COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SHUTDOWN));
        command.handler = VerificationAppExecute;
        error = CommandHandlerAddCommand(&command);
    }

    return error;
}

/// Generic handler function used to replace existing or add new
void New_IrqHandler(void)
{
    verificationAppDebugVar += 1U;
    SystemDrvSetInfo(verificationAppDebugVar);
}

/// Generic command used to interface to Patch to aid with testing and debug
///
/// \param[in] commandCode  Command Id
/// \param[in] params       Control/Configuration parameters
/// \returns Command error status
/// \retval ERROR_SUCCESS   Command accepted and executed.
/// \retval ERROR_SYSERR    Internal fault or error during execution
/// \retval ERROR_RANGE     Command id out of range
//
ErrorT  VerificationAppExecute(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params)
{
    ErrorT error = ERROR_SUCCESS;
    NvmMgrAddressT nvmOffset;
    uint16_t checksum;
    uint32_t value;
    void* aBuffer;
    void* oldBuffer;
    bool flag;

    void (* badfunc)(void) = (void (*)(void)) 0x5678;

    if (HOST_COMMAND_CODE_EXECUTE_DEBUG != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_BAD_COMMAND,
            (uint32_t)commandCode);
        error = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_BAD_PARAM, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        verificationAppDebugId = *(uint32_t*)params;
        verificationAppDebugParameter = *((uint32_t*)params + 1);
        verificationAppDebugPacketLength = *((uint32_t*)params + 2);

        switch (verificationAppDebugId)
        {
            case VERIFICATION_APP_DEBUG_ID_NO_ACTION:
                break;

            case VERIFICATION_APP_DEBUG_ID_FILL_STACK:
                error = VerificationAppFillStack();
                break;

            case VERIFICATION_APP_DEBUG_ID_DISABLE_PATCH:
                // Disable Comparator
                error = PatchMgrDisableComparator(verificationAppDebugParameter);
                break;

            case VERIFICATION_APP_DEBUG_ID_ENABLE_PATCH:
                // Add patch
                error = PatchMgrSetupComparator(verificationAppDebugParameter,
                (PatchMgrComparatorFunctionAddressT)SystemMgrGetRomVersion);
                break;

            case VERIFICATION_APP_DEBUG_ID_SWITCH_PATCH:
                // Switch patch
                (void)PatchMgrDisableComparator(verificationAppDebugParameter);
                error = PatchMgrSetupComparatorAndPatchedTable(verificationAppDebugParameter,
                (PatchMgrComparatorFunctionAddressT)SystemMgrGetRomVersion,
                (PatchMgrPatchedFunctionAddressT)SystemMgrGetRomVersionBeefPatched,
                (PatchMgrComparatorFunctionAddressT*)verificationAppPatchedFunctionTable,
                        1U);
                break;

            case VERIFICATION_APP_DEBUG_ID_NVM_FREE_SPACE_OFFSET:
                // Get free space address
                error = NvmMgrGetFreeSpaceAddress(&nvmOffset);
                *((NvmMgrAddressT*)params) = nvmOffset;
                break;

            case VERIFICATION_APP_DEBUG_ID_LIFECYCLE_MGR_SW_FATAL_ERROR:
                // Execute software fatal error
                LifecycleMgrSwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT, 0xBEEF);
                break;

            case VERIFICATION_APP_DEBUG_ID_TRIGGER_INTERRUPT:
                // Trigger Interrupt
                (void)InterruptsClearInterruptPending((InterruptT)verificationAppDebugParameter);
                (void)InterruptsEnableInterrupt((InterruptT)verificationAppDebugParameter, true);
                __NVIC_SetPendingIRQ((IRQn_Type)verificationAppDebugParameter);
                break;

            case VERIFICATION_APP_DEBUG_ID_SYS_INFO:
                // Get info value
                SystemDrvSetInfo((uint16_t)(*((uint32_t*)params + 1)) + 1U);
                *((uint16_t*)params) = SystemDrvGetInfo();
                break;

            case VERIFICATION_APP_DEBUG_ID_SYS_TICK:
                // Force SysTick interrupt
                SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;
                break;

            case VERIFICATION_APP_DEBUG_ID_NEW_IRQ_HANDER:
                // Install new handler for vector
                error = InitInstallNewIrqHandler((IRQn_Type)verificationAppDebugParameter,
                (InitVectorTableHandlerT)New_IrqHandler);
                *((uint16_t*)params) = (verificationAppDebugParameter - 1U);
                break;

            case VERIFICATION_APP_DEBUG_ID_RELOCATE_IRQ_VECTOR_TABLE:
                InitRelocateVectorTable();
                break;

            case VERIFICATION_APP_DEBUG_ID_SET_WATCHDOG_TIMER_INTERVAL:
                HealthMonitorSetWdogTimerInterval(verificationAppDebugParameter);
                // Force clock change to load new watchdog timer interval
                (void)SystemMgrSetClockSpeed(systemMgrState.clockSpeedHz ^ 1U);
                (void)WatchdogDrvClearInterrupt();          // Re-sync system for testing purposes...
                *((uint32_t*)params) = healthMonitorState.wdgTimerIntervalMicrosecs;
                break;

            case VERIFICATION_APP_DEBUG_ID_SET_WATCHDOG_DANGER_ZONE_SIZE:
                HealthMonitorSetStackCheckThreshold(verificationAppDebugParameter);
                *((uint32_t*)params) = healthMonitorState.stackHwmDangerZoneSizeLongWords;
                if (verificationAppDebugParameter != healthMonitorState.stackHwmDangerZoneSizeLongWords)
                {
                    error = ERROR_SYSERR;
                }
                break;

            case VERIFICATION_APP_DEBUG_ID_CALCULATE_CHECKSUM:
                checksum = ToolboxCalcChecksum((uint16_t* const)(((uint32_t*)params + 3U)),
                        verificationAppDebugPacketLength * 2U);
                *((uint16_t*)params) = checksum;
                break;

            case VERIFICATION_APP_DEBUG_ID_FORCE_OTPM_ECC_DED:
                if (true == OtpmDrvIsHardwareFault())
                {
                    error = ERROR_SYSERR;
                }
                else
                {
                    OTPM_REGS->core_ecc_flags |= CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK;
                    if (true != OtpmDrvIsHardwareFault())
                    {
                        error = ERROR_SYSERR;
                    }
                }
                *((uint32_t*)params) = OTPM_REGS->core_ecc_flags;
                break;

            case VERIFICATION_APP_DEBUG_ID_CC312_HAL_PAL_EMPTY:
                value = 0U;
                // Empty functions, no return values just check we return from them
                CC_PalApbcCntrInit();
                value += 1U;
                CC_PalPowerSaveModeInit();
                value += 1U;
                CC_PalPowerSaveModeStatus();
                value += 1U;
                CC_PalFinishIrq();
                value += 1U;
                // Empty function with fixed return value
                if (0U == CC_PalApbcCntrValue())
                {
                    value += 1U;
                    if (0U == CC_PalApbcModeSelect(CC_FALSE))
                    {
                        value += 1U;
                        if (0U == CC_PalInitIrq())
                        {
                            value += 1U;
                        }
                        else
                        {
                            error = ERROR_SYSERR;
                        }
                    }
                    else
                    {
                        error = ERROR_SYSERR;
                    }
                }
                else
                {
                    error = ERROR_SYSERR;
                }
                *((uint32_t*)params) = value;
                break;

            case VERIFICATION_APP_DEBUG_ID_CC312_HAL_PAL_MEMORY:
                value = 0U;
                if (NULL == (aBuffer = CC_PalMemMallocPlat(23U)))
                {
                    error = ERROR_SYSERR;
                }
                else
                {
                    value += 1U;
                    if (NULL == (aBuffer = CC_PalMemReallocPlat(aBuffer, 23U)))
                    {
                        error = ERROR_SYSERR;
                    }
                    else
                    {
                        value += 1U;
                        oldBuffer = aBuffer;
                        CC_PalMemFreePlat(aBuffer);
                        if (oldBuffer != aBuffer)
                        {
                            error = ERROR_SYSERR;
                        }
                        else
                        {
                            value += 1U;
                        }
                    }
                }
                *((uint32_t*)params) = value;
                *((uint32_t*)(params + 1U)) = (uint32_t)aBuffer;
                break;

            case VERIFICATION_APP_DEBUG_ID_CC312_HAL_PAL_OTPM_WRITE:
                // Initialise OTPM support by setting clock speed
                (void)SystemMgrSetClockSpeed(27000000U);
                *((uint32_t*)params + 1U) = (uint32_t)CC_BsvOTPWordWrite(0U, verificationAppDebugParameter, 0xCAFEBABE);
                *((uint32_t*)params + 2U) = (uint32_t)OtpmDrvRead((OtpmDrvAddressT const)verificationAppDebugParameter,
                        &value, 1U);
                *((uint32_t*)params + 3U) = value;

                if (0U != *((uint32_t*)params + 1U))
                {
                    error = ERROR_SYSERR;
                }
                break;

            case VERIFICATION_APP_DEBUG_ID_CC312_HAL_PAL_ABORT:
                value = 0U;
                CC_PalAbort("CC Abort");
                break;

            case VERIFICATION_APP_DEBUG_ID_CC312_HAL_PAL_COMPARE:
                if (CC_OK != CC_PalSecMemCmp(buffer1, (uint8_t*)((uint32_t*)params + 3), sizeof(buffer1)))
                {
                    error = ERROR_SYSERR;
                }
                break;

            case VERIFICATION_APP_DEBUG_ID_LIFECYCLE_MGR_AUTHORIZE_RMA:
                error = LifecycleMgrOnAuthorizeRMA(HOST_COMMAND_CODE_AUTHORIZE_RMA, params);
                break;

            case VERIFICATION_APP_DEBUG_ID_SYSTEM_DRV_GET_BOOT_OPTIONS:
                *((uint32_t*)params + 0U) = (uint32_t)SystemDrvGetBootOptionFlags();
                break;

            case VERIFICATION_APP_DEBUG_ID_SYSTEM_MGR_PATCH_FUNCTIONS:
                value = 0;
                (void)SystemMgrSetClockSpeed(27000000U);
                if (ERROR_SUCCESS == (error = SystemMgrGetClockSpeed(((uint32_t*)params + 1U))))
                {
                    value += 1U;
                    SystemMgrSetEvent(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING);
                    if (ERROR_SUCCESS == (error = SystemMgrClearPendingEvent(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING)))
                    {
                        value += 1U;
                    }
                }
                *((uint32_t*)params) = value;
                break;

            case VERIFICATION_APP_DEBUG_ID_SYSTEM_MGR_REQUEST_SHUTDOWN:
                value = 0U;
                SystemMgrRequestShutdown(0x55U, 0xCCU);
                if (ERROR_SUCCESS == (error = SystemMgrIsEventPending(SYSTEM_MGR_EVENT_SHUTDOWN_PENDING, &flag)))
                {
                    value += 1U;
                    if (true != flag)
                    {
                        error = ERROR_SYSERR;
                    }
                    else
                    {
                        value += 1U;
                    }
                }
                *((uint32_t*)params) = value;
                break;

            case VERIFICATION_APP_DEBUG_ID_SYSTEM_MGR_UNHANDLED_EVENT:
                error = SystemMgrOnUnhandledEvent(SYSTEM_MGR_EVENT_MAX);
                break;

            case VERIFICATION_APP_DEBUG_ID_SYS_EXIT:
                _sys_exit(0U);
                break;

            case VERIFICATION_APP_DEBUG_ID_SYS_TTY_CHR:
                _ttywrch(0x35U);
                break;

            case VERIFICATION_APP_DEBUG_ID_NMI_HANDLER:
                NMI_Handler();
                break;

            case VERIFICATION_APP_DEBUG_ID_RESERVED_HANDLER:
                Reserved_Handler();
                break;

            case VERIFICATION_APP_DEBUG_ID_AESCMAC_STATUS:
                aescmac_status = verificationAppDebugParameter;
                *((uint32_t*)params) = aescmac_status;
                break;

            case VERIFICATION_APP_DEBUG_ID_BAD_FUNCTION:
                badfunc();
                break;

            case VERIFICATION_APP_DEBUG_ID_UNALIGNED_ACCESS:
            {
                uint16_t* data_ptr = (uint16_t*)0x1233;
                *((uint16_t*)params) = data_ptr[0];
            }
            break;

            case VERIFICATION_APP_DEBUG_ID_PENDSV:
                SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
                break;

            case VERIFICATION_APP_DEBUG_ID_ILLEGAL_INSTRUCTION:
            {
                uint16_t* badFuncMem;
                void (* badFunc)(void) = NULL;

                // Generate an illegal instruction exception
                badFuncMem = (uint16_t*)malloc(4);
                if (NULL == badFuncMem)
                {
                    error = ERROR_NOSPC;
                }
                else
                {
                    badFuncMem[0] = 0x2000;  // MOV R0, #0
                    badFuncMem[1] = 0xBB00;  // Illegal instruction
                    badFuncMem[2] = 0x4770;  // BX LR

                    // Note: MUST set LSB as M0+ only operates in Thumb mode
                    badFunc = (void (*)(void))((uint32_t)badFuncMem | 1);
                    badFunc();

                    free(badFuncMem);
                }
            }
            break;

            case VERIFICATION_APP_DEBUG_ID_MPU_WRITE_START_ROM:
            case VERIFICATION_APP_DEBUG_ID_MPU_WRITE_END_ROM:
            case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_ROM:
            case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_END_ROM:
            case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_PATCH_RAM:
            case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_RAM:
            case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_STACK:
            case VERIFICATION_APP_DEBUG_ID_MPU_READ_STACK_GUARD:
            case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_SHARED_MEM:
            case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_SHARED_MEM:
            case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_VCORE_ADDR:
            case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_VCORE_ADDR:
                VerificationAppTestMpu((VerificationAppDebugIdT)verificationAppDebugId);
                break;

            case VERIFICATION_APP_DEBUG_ID_FW_LOCKUP:
                flag = true;        // stops compiler complaining about unreachable code
                while (true == flag)
                {
                    // Keep the CPU here, in an infinite loop.
                    // Interrupts are still enabled, and ISRs will be serviced
                }
                break;

            default:
                error = ERROR_RANGE;
                break;
        }
    }

    return error;
}

/// Function used to fill stack and cause exception
///
//
ErrorT VerificationAppFillStack(void)
{
    ErrorT error = ERROR_SUCCESS;
    InitMpuStackConfigType stackConfig;
    uint32_t* writePointer;
    uint16_t i = 0;

    error = InitMpuGetStackInformation(&stackConfig);
    if (ERROR_SUCCESS != error)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_FILL_STACK_ERROR,
            (uint32_t)error);
    }
    else
    {
        writePointer = (stackConfig.base + stackConfig.stackGuardRegionSizeLongWords);
        while ((0xfeedface == writePointer[i]) && (i < 512))
        {
            writePointer[i] = 0x55555500 + i;
            i += 1;
        }
    }

    return error;
}

/// @cond ARM_LINKER_COMMANDS
// This externs are excluded from the Doxygen as it is generating the error: symbol was not declared or defined.
extern unsigned int Image$$CODE$$Base;
extern unsigned int Image$$CODE$$ZI$$Limit;
extern unsigned int Image$$DATA$$Base;
extern unsigned int Image$$HEAP$$Base;
extern unsigned int Image$$MAIN_STACK$$Base;
extern unsigned int Image$$MAIN_STACK$$Limit;
extern unsigned int Image$$ICORE_REGISTERS$$Base;
extern unsigned int Image$$SHARED_MEMORY$$Base;
/// @endcond

// Tests the MPU regions
int VerificationAppTestMpu(VerificationAppDebugIdT verificationAppDebugId)
{
    uint32_t codeBase = (uint32_t)&Image$$CODE$$Base;
    uint32_t codeEnd = (uint32_t)&Image$$CODE$$ZI$$Limit;
    uint32_t dataBase = (uint32_t)&Image$$DATA$$Base;
    uint32_t stackBase = (uint32_t)&Image$$MAIN_STACK$$Base;
    uint32_t stackEnd = (uint32_t)&Image$$MAIN_STACK$$Limit;
    uint32_t icoreBase = (uint32_t)&Image$$ICORE_REGISTERS$$Base;
    uint32_t shrMemBase = (uint32_t)&Image$$SHARED_MEMORY$$Base;
    uint32_t vCoreBase = (uint32_t)CRYPTO_VCORE_BASE;
    uint32_t peripheralEnd = 0x60000000UL;

    uint32_t* dataPtr;
    volatile uint32_t initTestVal;

    (void)icoreBase;

    // Note MUST set the LSB as the M0 only operates in Thumb mode
    void (* funcPtr)(void) = (void (*)(void))(codeEnd | 1);

    switch (verificationAppDebugId)
    {
        //
        // Region 0: ROM
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_WRITE_START_ROM:
            // Attempt to write to start of ROM
            dataPtr = (uint32_t*)codeBase;
            *dataPtr = 0x12345678UL;  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_WRITE_END_ROM:
            // Attempt to write to end of ROM
            dataPtr = (uint32_t*)(codeEnd - 4U);
            *dataPtr = 0x12345678UL;  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_ROM:
            // Attempt to read past end of ROM
            dataPtr = (uint32_t*)codeEnd;
            initTestVal = *dataPtr;  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_END_ROM:
            // Attempt to execute past end of ROM
            funcPtr = (void (*)(void))(codeEnd | 1);
            funcPtr();  // Access should cause a HardFault
            break;

        //
        // Region 1: Patch RAM
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_PATCH_RAM:
            InitMpuEnablePatchRegion(false);
            break;

        //
        // Region 2: Data RAM
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_RAM:
            // Attempt to execute from data RAM
            funcPtr = (void (*)(void))(dataBase | 1);
            funcPtr();  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_STACK:
            // Attempt to read the top of the stack
            dataPtr = (uint32_t*)(stackEnd - 4);
            initTestVal = *dataPtr;

            // Attempt to read beyond the end of the stack
            dataPtr = (uint32_t*)stackEnd;
            initTestVal = *dataPtr;  // Access should cause a HardFault
            break;

        //
        // Region 3: Stack guard
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_READ_STACK_GUARD:
            // Attempt to read just above the stack guard
            dataPtr = (uint32_t*)(stackBase + 32UL);
            initTestVal = *dataPtr;

            // Attempt to read within the stack guard
            dataPtr = (uint32_t*)(stackBase + 28UL);
            initTestVal = *dataPtr;  // Access should cause a HardFault
            break;

        //
        // Region 4: ICore registers and shared-memory
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_SHARED_MEM:
            // Attempt to execute from ICore shared-memory
            funcPtr = (void (*)(void))(shrMemBase | 1);
            funcPtr();  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_SHARED_MEM:
            // Attempt to read beyond the end of the shared-memory
            dataPtr = (uint32_t*)(shrMemBase + 0x1000);
            initTestVal = *dataPtr;
            break;

        //
        // Region 5: Peripherals
        //

        case VERIFICATION_APP_DEBUG_ID_MPU_EXECUTE_VCORE_ADDR:
            // Attempt to execute from the Peripherals region (Vcore registers)
            funcPtr = (void (*)(void))(vCoreBase | 1);
            funcPtr();  // Access should cause a HardFault
            break;

        case VERIFICATION_APP_DEBUG_ID_MPU_READ_END_VCORE_ADDR:
            // Attempt to read beyond the end of the peripherals
            dataPtr = (uint32_t*)peripheralEnd;
            initTestVal = *dataPtr;
            break;

        default:
            break;
    }

    // Should never be here..
    return ERROR_IO;
}

/// @} endgroup verificationapp

