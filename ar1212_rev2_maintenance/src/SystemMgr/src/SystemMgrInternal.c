//***********************************************************************************
/// \file
///
/// System Manager internal implementation
///
/// \addtogroup systemmgr
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

#include <stdlib.h>
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "crypto_icore.h"

#include "SystemMgrInternal.h"

#include "LifecycleMgr.h"
#include "AssetMgr.h"
#include "SessionMgr.h"
#include "CryptoMgr.h"
#include "NvmMgr.h"
#include "HealthMonitor.h"
#include "PatchMgr.h"
#include "CommandHandler.h"

#include "Interrupts.h"
#include "CommandDrv.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

/// Default event priorities, indexed by event identifier (MUST match systemMgrEventByPriority)
const SystemMgrEventPriorityT systemMgrPriorityByEventDefaults[SYSTEM_MGR_EVENT_MAX] =
{
    SYSTEM_MGR_EVENT_PRIORITY_0,    // SYSTEM_MGR_EVENT_WATCHDOG
    SYSTEM_MGR_EVENT_PRIORITY_1,    // SYSTEM_MGR_EVENT_FATAL_ERROR
    SYSTEM_MGR_EVENT_PRIORITY_2,    // SYSTEM_MGR_EVENT_SHUTDOWN_PENDING
    SYSTEM_MGR_EVENT_PRIORITY_3,    // SYSTEM_MGR_EVENT_DEBUG_START
    SYSTEM_MGR_EVENT_PRIORITY_4,    // SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE
    SYSTEM_MGR_EVENT_PRIORITY_5,    // SYSTEM_MGR_EVENT_SESSION_START
    SYSTEM_MGR_EVENT_PRIORITY_6,    // SYSTEM_MGR_EVENT_DOORBELL
    SYSTEM_MGR_EVENT_PRIORITY_7,    // SYSTEM_MGR_EVENT_CC312_IS_IDLE
    SYSTEM_MGR_EVENT_PRIORITY_8,    // SYSTEM_MGR_EVENT_CC312_PWRDOWN_READY
    SYSTEM_MGR_EVENT_PRIORITY_9,    // SYSTEM_MGR_EVENT_CC312_HOST_INT
    SYSTEM_MGR_EVENT_PRIORITY_10,   // SYSTEM_MGR_EVENT_GMAC_DONE
    SYSTEM_MGR_EVENT_PRIORITY_11,   // SYSTEM_MGR_EVENT_CMAC_DONE
    SYSTEM_MGR_EVENT_PRIORITY_12,   // SYSTEM_MGR_EVENT_RESERVED_12
    SYSTEM_MGR_EVENT_PRIORITY_13,   // SYSTEM_MGR_EVENT_RESERVED_13
    SYSTEM_MGR_EVENT_PRIORITY_14,   // SYSTEM_MGR_EVENT_RESERVED_14
    SYSTEM_MGR_EVENT_PRIORITY_15,   // SYSTEM_MGR_EVENT_RESERVED_15
    SYSTEM_MGR_EVENT_PRIORITY_16,   // SYSTEM_MGR_EVENT_RESERVED_16
    SYSTEM_MGR_EVENT_PRIORITY_17,   // SYSTEM_MGR_EVENT_RESERVED_17
    SYSTEM_MGR_EVENT_PRIORITY_18,   // SYSTEM_MGR_EVENT_RESERVED_18
    SYSTEM_MGR_EVENT_PRIORITY_19,   // SYSTEM_MGR_EVENT_RESERVED_19
    SYSTEM_MGR_EVENT_PRIORITY_20,   // SYSTEM_MGR_EVENT_RESERVED_20
    SYSTEM_MGR_EVENT_PRIORITY_21,   // SYSTEM_MGR_EVENT_RESERVED_21
    SYSTEM_MGR_EVENT_PRIORITY_22,   // SYSTEM_MGR_EVENT_RESERVED_22
    SYSTEM_MGR_EVENT_PRIORITY_23,   // SYSTEM_MGR_EVENT_RESERVED_23
    SYSTEM_MGR_EVENT_PRIORITY_24,   // SYSTEM_MGR_EVENT_RESERVED_24
    SYSTEM_MGR_EVENT_PRIORITY_25,   // SYSTEM_MGR_EVENT_RESERVED_25
    SYSTEM_MGR_EVENT_PRIORITY_26,   // SYSTEM_MGR_EVENT_RESERVED_26
    SYSTEM_MGR_EVENT_PRIORITY_27,   // SYSTEM_MGR_EVENT_RESERVED_27
    SYSTEM_MGR_EVENT_PRIORITY_28,   // SYSTEM_MGR_EVENT_RESERVED_28
    SYSTEM_MGR_EVENT_PRIORITY_29,   // SYSTEM_MGR_EVENT_RESERVED_29
    SYSTEM_MGR_EVENT_PRIORITY_30,   // SYSTEM_MGR_EVENT_RESERVED_30
    SYSTEM_MGR_EVENT_PRIORITY_31,   // SYSTEM_MGR_EVENT_RESERVED_31
};

/// Default event identifiers, indexed by priority (MUST match systemMgrPriorityByEvent)
const SystemMgrEventT systemMgrEventByPriorityDefaults[SYSTEM_MGR_EVENT_MAX] =
{
    SYSTEM_MGR_EVENT_WATCHDOG,              // SYSTEM_MGR_EVENT_PRIORITY_0
    SYSTEM_MGR_EVENT_FATAL_ERROR,           // SYSTEM_MGR_EVENT_PRIORITY_1
    SYSTEM_MGR_EVENT_SHUTDOWN_PENDING,      // SYSTEM_MGR_EVENT_PRIORITY_2
    SYSTEM_MGR_EVENT_DEBUG_START,           // SYSTEM_MGR_EVENT_PRIORITY_3
    SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE,   // SYSTEM_MGR_EVENT_PRIORITY_4
    SYSTEM_MGR_EVENT_SESSION_START,         // SYSTEM_MGR_EVENT_PRIORITY_5
    SYSTEM_MGR_EVENT_DOORBELL,              // SYSTEM_MGR_EVENT_PRIORITY_6
    SYSTEM_MGR_EVENT_CC312_IS_IDLE,         // SYSTEM_MGR_EVENT_PRIORITY_7
    SYSTEM_MGR_EVENT_CC312_PWRDOWN_READY,   // SYSTEM_MGR_EVENT_PRIORITY_8
    SYSTEM_MGR_EVENT_CC312_HOST_INT,        // SYSTEM_MGR_EVENT_PRIORITY_9
    SYSTEM_MGR_EVENT_GMAC_DONE,             // SYSTEM_MGR_EVENT_PRIORITY_10
    SYSTEM_MGR_EVENT_CMAC_DONE,             // SYSTEM_MGR_EVENT_PRIORITY_11
    SYSTEM_MGR_EVENT_RESERVED_12,           // SYSTEM_MGR_EVENT_PRIORITY_12
    SYSTEM_MGR_EVENT_RESERVED_13,           // SYSTEM_MGR_EVENT_PRIORITY_13
    SYSTEM_MGR_EVENT_RESERVED_14,           // SYSTEM_MGR_EVENT_PRIORITY_14
    SYSTEM_MGR_EVENT_RESERVED_15,           // SYSTEM_MGR_EVENT_PRIORITY_15
    SYSTEM_MGR_EVENT_RESERVED_16,           // SYSTEM_MGR_EVENT_PRIORITY_16
    SYSTEM_MGR_EVENT_RESERVED_17,           // SYSTEM_MGR_EVENT_PRIORITY_17
    SYSTEM_MGR_EVENT_RESERVED_18,           // SYSTEM_MGR_EVENT_PRIORITY_18
    SYSTEM_MGR_EVENT_RESERVED_19,           // SYSTEM_MGR_EVENT_PRIORITY_19
    SYSTEM_MGR_EVENT_RESERVED_20,           // SYSTEM_MGR_EVENT_PRIORITY_20
    SYSTEM_MGR_EVENT_RESERVED_21,           // SYSTEM_MGR_EVENT_PRIORITY_21
    SYSTEM_MGR_EVENT_RESERVED_22,           // SYSTEM_MGR_EVENT_PRIORITY_22
    SYSTEM_MGR_EVENT_RESERVED_23,           // SYSTEM_MGR_EVENT_PRIORITY_23
    SYSTEM_MGR_EVENT_RESERVED_24,           // SYSTEM_MGR_EVENT_PRIORITY_24
    SYSTEM_MGR_EVENT_RESERVED_25,           // SYSTEM_MGR_EVENT_PRIORITY_25
    SYSTEM_MGR_EVENT_RESERVED_26,           // SYSTEM_MGR_EVENT_PRIORITY_26
    SYSTEM_MGR_EVENT_RESERVED_27,           // SYSTEM_MGR_EVENT_PRIORITY_27
    SYSTEM_MGR_EVENT_RESERVED_28,           // SYSTEM_MGR_EVENT_PRIORITY_28
    SYSTEM_MGR_EVENT_RESERVED_29,           // SYSTEM_MGR_EVENT_PRIORITY_29
    SYSTEM_MGR_EVENT_RESERVED_30,           // SYSTEM_MGR_EVENT_PRIORITY_30
    SYSTEM_MGR_EVENT_RESERVED_31,           // SYSTEM_MGR_EVENT_PRIORITY_31
};

/// Table of application-component phase entry handlers
const SystemMgrOnPhaseEntryHandlerT systemMgrROMPhaseEntryHandlers[] =
{
    LifecycleMgrOnPhaseEntry,
    AssetMgrOnPhaseEntry,
    SessionMgrOnPhaseEntry,
    CryptoMgrOnPhaseEntry,
    NvmMgrOnPhaseEntry,
    HealthMonitorOnPhaseEntry,
    PatchMgrOnPhaseEntry,
    CommandHandlerOnPhaseEntry,
};

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

SystemMgrStateT systemMgrState;

/// Table of patch phase entry handlers
SystemMgrOnPhaseEntryHandlerT systemMgrRAMPhaseEntryHandlers[] =
{
    (SystemMgrOnPhaseEntryHandlerT)NULL,
    (SystemMgrOnPhaseEntryHandlerT)NULL
};

/// Helper function to initialize the exception and interrupt priorities

/// \returns Error status
/// \retval ERROR_SUCCESS   Exceptions and interrupt priorities initialised successfully
/// \retval ERROR_INVAL     Invalid interrupt or exception
///
ErrorT SystemMgrInitExceptionAndInterruptPriorities(void)
{
    ErrorT status;
    uint32_t interrupt;

    // Set all interrupt priorities (priority 0x80)
    for (interrupt = (uint32_t)INTERRUPT_MIN; interrupt <= (uint32_t)INTERRUPT_MAX; interrupt += 1U)
    {
        status = InterruptsSetInterruptPriority((InterruptT)interrupt, INTERRUPT_PRIORITY_2);
        if (ERROR_SUCCESS != status)
        {
            break;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        // Prioritize the SVCall exception over all interrupts (priority 0x00)
        status = InterruptsSetExceptionPriority(EXCEPTION_SVCALL, INTERRUPT_PRIORITY_0);
        if (ERROR_SUCCESS == status)
        {
            // Prioritize the Watchdog interrupt over all other interrupts (priority 0x40)
            status = InterruptsSetInterruptPriority(INTERRUPT_WATCHDOG, INTERRUPT_PRIORITY_1);
        }
    }

    return status;
}

/// Helper function to notify all registered components of the new phase
///
/// Note, when entering the SHUTDOWN phase System Manager ignores all
/// errors. This ensures that all components are notified of the impending
/// shutdown.
///
/// \param[in] newPhase     The phase being entered
/// \returns Error status
/// \retval ERROR_SUCCESS   All components notified
/// \retval Component error code
///
ErrorT SystemMgrNotifyEnterPhase(SystemMgrPhaseT const newPhase)
{
    ErrorT status = ERROR_SUCCESS;
    bool abortOnError = (SYSTEM_MGR_PHASE_SHUTDOWN != newPhase);
    uint32_t idx;

    if (newPhase == systemMgrState.currentPhase)
    {
        // Cannot re-enter the same phase
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_NO_PHASE_REENTRY, 0U);
        status = ERROR_SYSERR;
    }
    else if (SYSTEM_MGR_PHASE_SHUTDOWN < newPhase)
    {
        // Invalid phase
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_PHASE_INVALID, (uint32_t)newPhase);
        status = ERROR_SYSERR;
    }
    else
    {
        systemMgrState.currentPhase = newPhase;

        for (idx = 0U;
             idx < (sizeof(systemMgrRAMPhaseEntryHandlers) / sizeof((systemMgrRAMPhaseEntryHandlers)[0]));
             idx += 1U)
        {
            if (NULL != systemMgrRAMPhaseEntryHandlers[idx])
            {
                DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE, (uint16_t)idx);

                status = systemMgrRAMPhaseEntryHandlers[idx](newPhase);
                if (ERROR_SUCCESS != status)
                {
                    // Patch has detected a fatal error and set the fatal error event.
                    if (abortOnError)
                    {
                        break;
                    }
                    else
                    {
                        status = ERROR_SUCCESS;
                    }
                }
            }
        }
    }

    if (ERROR_SUCCESS == status)
    {
        for (idx = 0U;
             idx < (sizeof(systemMgrROMPhaseEntryHandlers) / sizeof((systemMgrROMPhaseEntryHandlers)[0]));
             idx += 1U)
        {
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE, (uint16_t)idx);

            status = systemMgrROMPhaseEntryHandlers[idx](newPhase);
            if (ERROR_SUCCESS != status)
            {
                // Component has detected a fatal error and set the fatal error event.
                if (abortOnError)
                {
                    break;
                }
                else
                {
                    status = ERROR_SUCCESS;
                }
            }
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE);

    return status;
}

/// Handles an 'unhandled' event - intended for patching
///
/// A patch can patch this function to extend the number of
/// events supported by System Manager
///
/// \param[in] event    The unhandled event
/// \returns Error status
///
ErrorT SystemMgrOnUnhandledEvent(SystemMgrEventT const event)
{
    SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_UNHANDLED_EVENT,
        (uint32_t)event);

    return ERROR_NOENT;
}

/// Handles a CC312 library fatal abort
///
/// The CC312 library functions do not expect CC_PalAbort() to return. If it does return
/// it may cause indeterminate operation, therefore System Manager has to ensure it does
/// not return. It does this by triggering a PendSV exception.
///
/// \returns Never
///
void SystemMgrOnCc312Abort(void)
{
    // Flag that the CC312 has aborted
    systemMgrState.fatalErrorState.isCC312Abort = true;

    // INFO: <PH>: For unit testing we cannot emulate a HardFault - skip
#if !defined(BUILD_TARGET_ARCH_x86)
    // Trigger the PendSV exception to indicate a CC312 abort
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
#endif
}

/// Terminates due to a fatal error
///
/// Reports the fatal error, then terminates execution. Does not return in
/// production systems. For unit test, returns to allow test runner to regain
/// control.
///
/// \param[in]  fault       Crypto fault identifier
/// \param[in]  context     context code (or zero)
/// \param[in]  infoParam   Optional information parameter for diagnostics (or zero)
/// \returns Never
///
void SystemMgrFatalErrorStop(SystemMgrCssFaultT const fault,
    uint32_t const context,
    uint32_t const infoParam)
{
    // Set the firmware-fatal error bit in the checkpoint register
    uint16_t checkpoint = SystemDrvGetCheckpoint();

    checkpoint |= CRYPTO_ICORE_CHECKPOINT_FATAL_ERROR__MASK;
    SystemDrvSetCheckpoint(checkpoint);

    // Set the Info and CryptoError registers
    SystemDrvSetInfo((uint16_t)(infoParam & 0xFFFFU));
    SystemDrvSetCryptoError((uint32_t)fault, context);

    // Complete the current host command, does not return (in production systems)
    CommandHandlerFatalErrorStop();
}

/// Handles a fatal error event
///
/// Switches to the shutdown phase, reports the fatal error, and terminates
/// execution. Does not return in production systems. For unit test, sets
/// the hasTerminated flag and returns ERROR_SYSERR to allow test runner
/// to regain control.
///
/// \returns ERROR_SYSERR   Indicates that a fatal error occurred
///
ErrorT SystemMgrOnFatalErrorEvent(void)
{
    // Inform all application-level components about the impending shutdown
    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_SHUTDOWN);
    (void)SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_SHUTDOWN);

    SystemMgrFatalErrorStop(systemMgrState.fatalErrorState.fault,
        systemMgrState.fatalErrorState.context,
        systemMgrState.fatalErrorState.infoParam);

    systemMgrState.fatalErrorState.hasTerminated = true;

    return ERROR_SYSERR;
}

/// Handles a shutdown-pending event
///
/// Switches to the shutdown phase, reports the shutdown, and terminates
/// execution. Does not return in production systems. For unit test, sets
/// the hasTerminated flags and returns ERROR_SYSERR to allow test runner
/// to regain control.
///
/// \returns ERROR_SYSERR   Indicates that a fatal error occurred
///
ErrorT SystemMgrOnShutdownPendingEvent(void)
{
    // Inform all application-level components about the impending shutdown
    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_SHUTDOWN);
    (void)SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_SHUTDOWN);

    SystemMgrFatalErrorStop(SYSTEM_MGR_CSS_FAULT_FIRMWARE_TERMINATED,
        systemMgrState.shutdownState.context,
        systemMgrState.shutdownState.infoParam);

    systemMgrState.fatalErrorState.hasTerminated = true;

    return ERROR_SYSERR;
}

/// Handles a doorbell event
///
/// Retrieves the host command code from the Command register, and
/// determines if an operation phase change is required. If so, the
/// phase is changed and the components/drivers notified. Then invokes
/// Command Handler to handle command.
///
/// \returns Error status
ErrorT SystemMgrOnDoorbellEvent(void)
{
    HostCommandCodeT commandCode = (HostCommandCodeT)CommandDrvGetCommand();
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_DOORBELL_EVENT);

    switch (systemMgrState.currentPhase)
    {
        case SYSTEM_MGR_PHASE_DEBUG:
            switch (commandCode)
            {
                case HOST_COMMAND_CODE_GET_STATUS:
                case HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT:
                case HOST_COMMAND_CODE_AUTHORIZE_RMA:
                    // No phase change required
                    break;

                case HOST_COMMAND_CODE_LOAD_PATCH_CHUNK:
                    // Switch to the Patch phase
                    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_PATCH_START);
                    status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_PATCH);
                    break;

                default:
                    // Switch to the Configure phase
                    DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_CONFIGURE_START);
                    status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_CONFIGURE);
                    break;
            }
            break;

        case SYSTEM_MGR_PHASE_PATCH:
        case SYSTEM_MGR_PHASE_CONFIGURE:
        case SYSTEM_MGR_PHASE_SESSION:
            // Nothing to do here...
            break;

        case SYSTEM_MGR_PHASE_INITIALIZE:
        case SYSTEM_MGR_PHASE_SHUTDOWN:
        default:
            // Should never happen - report a fault
            SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_DOORBELL_BAD_PHASE,
            (uint32_t)systemMgrState.currentPhase);
            status = ERROR_SYSERR;
            break;
    }

    if (ERROR_SUCCESS == status)
    {
        status = CommandHandlerOnDoorbellEvent(commandCode);
    }

    return status;
}

/// Handles an event
///
/// \param[in] event   Event to handle
/// \returns
/// \retval ERROR_SUCCESS   Event handled successfully
/// \retval Component error status
ErrorT SystemMgrHandleEvent(SystemMgrEventT const event)
{
    ErrorT status = ERROR_SUCCESS;

    switch (event)
    {
        case SYSTEM_MGR_EVENT_WATCHDOG:
            status = HealthMonitorOnWatchdogEvent();
            break;

        case SYSTEM_MGR_EVENT_FATAL_ERROR:
            // A fatal error has been detected. Shutdown all operations.
            status = SystemMgrOnFatalErrorEvent();
            break;

        case SYSTEM_MGR_EVENT_SHUTDOWN_PENDING:
            // Normal termination requested. Shutdown all operations.
            status = SystemMgrOnShutdownPendingEvent();
            break;

        case SYSTEM_MGR_EVENT_DEBUG_START:
            // Enter the Debug phase
            DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_DEBUG_START);
            status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_DEBUG);
            break;

        case SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE:
            // Patch loading has completed, or failed. In either case switch to the Configure phase
            DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_PATCH_LOAD_COMPLETE);
            status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_CONFIGURE);
            break;

        case SYSTEM_MGR_EVENT_SESSION_START:
            // The host has established a secure session. Switch to the Session phase
            DiagSetCheckpoint(DIAG_CHECKPOINT_SYSTEM_MGR_ON_SESSION_START);
            status = SystemMgrNotifyEnterPhase(SYSTEM_MGR_PHASE_SESSION);
            break;

        case SYSTEM_MGR_EVENT_DOORBELL:
            status = SystemMgrOnDoorbellEvent();
            break;

        default:
            // Unsupported event - call the patchable handler
            status = SystemMgrOnUnhandledEvent(event);
            break;
    }

    return status;
}

/// Returns the highest-priority pending event
///
/// Implements a 'count-trailing-zeros' function using a
/// logarithmic number of operations and branches.
///
/// \returns Event flag, or 0 (none set)
///
SystemMgrEventT SystemMgrGetNextEvent(void)
{
    uint32_t currentPriorityMask = systemMgrState.currentPriorityMask;
    SystemMgrEventT nextEvent = SYSTEM_MGR_EVENT_NONE;
    uint32_t numZeros = 0;

    if (0U != currentPriorityMask)
    {
        if (0U == (currentPriorityMask & 0x0000FFFFUL))
        {
            numZeros += 16U;
            currentPriorityMask >>= 16U;
        }

        if (0U == (currentPriorityMask & 0x000000FFUL))
        {
            numZeros += 8U;
            currentPriorityMask >>= 8U;
        }

        if (0U == (currentPriorityMask & 0x0000000FUL))
        {
            numZeros += 4U;
            currentPriorityMask >>= 4U;
        }

        if (0U == (currentPriorityMask & 0x00000003UL))
        {
            numZeros += 2U;
            currentPriorityMask >>= 2U;
        }

        if (0U == (currentPriorityMask & 0x00000001UL))
        {
            numZeros += 1U;
        }

        // Clear the event
        systemMgrState.currentPriorityMask &= ~(1U << numZeros);

        // Look-up which event is at this priority
        nextEvent = systemMgrState.eventByPriority[numZeros];
    }

    return nextEvent;
}

/// Dumps System Mgr state in response to a fatal error
///
/// \param[in] dumpBuffer       Pointer to buffer to contain the dump
/// \param[in] sizeLongWords    Size of the dump buffer
/// \returns void
///
void SystemMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer,
    uint32_t const sizeLongWords)
{
    uint32_t sizeBytes = sizeLongWords * (sizeof(uint32_t));

    if (NULL != dumpBuffer)
    {
        uint32_t dumpSizeBytes = sizeof(systemMgrState);

        if (dumpSizeBytes > sizeBytes)
        {
            dumpSizeBytes = sizeBytes;
        }

        (void)memcpy((void*)dumpBuffer, (void*)&systemMgrState, dumpSizeBytes);
    }
}

/// @} endgroup systemmgr

