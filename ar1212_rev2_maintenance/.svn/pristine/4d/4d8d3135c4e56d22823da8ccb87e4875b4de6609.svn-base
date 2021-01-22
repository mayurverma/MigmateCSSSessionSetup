#if !defined(SRC__SYSTEMMGR__SRC__SYSTEMMGRINTERNAL_H)
#define      SRC__SYSTEMMGR__SRC__SYSTEMMGRINTERNAL_H

//***********************************************************************************
/// \file
/// System Manager internal API and types
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
//***********************************************************************************

#include "SystemMgr.h"
#include "Diag.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Event priorities (lower = higher-priority)
typedef enum
{
    SYSTEM_MGR_EVENT_PRIORITY_0 = 0U,     ///< Priority 0 (highest)
    SYSTEM_MGR_EVENT_PRIORITY_1 = 1U,     ///< Priority 1
    SYSTEM_MGR_EVENT_PRIORITY_2 = 2U,     ///< Priority 2
    SYSTEM_MGR_EVENT_PRIORITY_3 = 3U,     ///< Priority 3
    SYSTEM_MGR_EVENT_PRIORITY_4 = 4U,     ///< Priority 4
    SYSTEM_MGR_EVENT_PRIORITY_5 = 5U,     ///< Priority 5
    SYSTEM_MGR_EVENT_PRIORITY_6 = 6U,     ///< Priority 6
    SYSTEM_MGR_EVENT_PRIORITY_7 = 7U,     ///< Priority 7
    SYSTEM_MGR_EVENT_PRIORITY_8 = 8U,     ///< Priority 8
    SYSTEM_MGR_EVENT_PRIORITY_9 = 9U,     ///< Priority 9
    SYSTEM_MGR_EVENT_PRIORITY_10 = 10U,   ///< Priority 10
    SYSTEM_MGR_EVENT_PRIORITY_11 = 11U,   ///< Priority 11
    SYSTEM_MGR_EVENT_PRIORITY_12 = 12U,   ///< Priority 12
    SYSTEM_MGR_EVENT_PRIORITY_13 = 13U,   ///< Priority 13
    SYSTEM_MGR_EVENT_PRIORITY_14 = 14U,   ///< Priority 14
    SYSTEM_MGR_EVENT_PRIORITY_15 = 15U,   ///< Priority 15
    SYSTEM_MGR_EVENT_PRIORITY_16 = 16U,   ///< Priority 16
    SYSTEM_MGR_EVENT_PRIORITY_17 = 17U,   ///< Priority 17
    SYSTEM_MGR_EVENT_PRIORITY_18 = 18U,   ///< Priority 18
    SYSTEM_MGR_EVENT_PRIORITY_19 = 19U,   ///< Priority 19
    SYSTEM_MGR_EVENT_PRIORITY_20 = 20U,   ///< Priority 20
    SYSTEM_MGR_EVENT_PRIORITY_21 = 21U,   ///< Priority 21
    SYSTEM_MGR_EVENT_PRIORITY_22 = 22U,   ///< Priority 22
    SYSTEM_MGR_EVENT_PRIORITY_23 = 23U,   ///< Priority 23
    SYSTEM_MGR_EVENT_PRIORITY_24 = 24U,   ///< Priority 24
    SYSTEM_MGR_EVENT_PRIORITY_25 = 25U,   ///< Priority 25
    SYSTEM_MGR_EVENT_PRIORITY_26 = 26U,   ///< Priority 26
    SYSTEM_MGR_EVENT_PRIORITY_27 = 27U,   ///< Priority 27
    SYSTEM_MGR_EVENT_PRIORITY_28 = 28U,   ///< Priority 28
    SYSTEM_MGR_EVENT_PRIORITY_29 = 29U,   ///< Priority 29
    SYSTEM_MGR_EVENT_PRIORITY_30 = 30U,   ///< Priority 30
    SYSTEM_MGR_EVENT_PRIORITY_31 = 31U,   ///< Priority 31 (lowest)

    SYSTEM_MGR_EVENT_PRIORITY_MIN = 31U,  ///< Priority 31 (lowest)
    SYSTEM_MGR_EVENT_PRIORITY_MAX = 0U,   ///< Priority 0  (highest)
} SystemMgrEventPriorityT;

/// Pointer to a Phase entry handler function
typedef ErrorT (* SystemMgrOnPhaseEntryHandlerT)(SystemMgrPhaseT const phase);

/// Represents the fatal error state
typedef struct
{
    SystemMgrCssFaultT fault;  ///< Crypto fault identifier
    uint32_t context;          ///< Fault context
    uint32_t infoParam;        ///< Optional information parameter
    bool hasTerminated;        ///< Flags if a fatal error has caused the firmware to terminate (for unit test purposes)
    bool isCC312Abort;         ///< Flags if a fatal error was due to a CC312 abort
} SystemMgrFatalErrorStateT;

/// Represents the shutdown state
typedef struct
{
    uint32_t context;    ///< Shutdown context
    uint32_t infoParam;  ///< Optional information parameter
} SystemMgrShutdownStateT;

/// Represents the System Manager state
typedef struct
{
    SystemMgrPhaseT currentPhase;                       ///< The current operating phase
    uint32_t currentPriorityMask;                       ///< Mask of the current set event priorities
    SystemMgrFatalErrorStateT fatalErrorState;          ///< Fatal error state buffer
    SystemMgrShutdownStateT shutdownState;              ///< Shutdown state buffer
    uint32_t clockSpeedHz;                              ///< Cryptosubsystem clock speed (if known)
    SystemMgrEventPriorityT const* priorityByEvent;     ///< Current event priority lookup table
    SystemMgrEventT const* eventByPriority;             ///< Current event lookup table

    /// table of notifiers to call in the event of a clock speed change
    SystemMgrClockSpeedChangeNotifierT clockSpeedChangeNotifier[SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS];
} SystemMgrStateT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern SystemMgrStateT systemMgrState;
extern const SystemMgrEventPriorityT systemMgrPriorityByEventDefaults[SYSTEM_MGR_EVENT_MAX];
extern const SystemMgrEventT systemMgrEventByPriorityDefaults[SYSTEM_MGR_EVENT_MAX];

// Helper function to initialize the exception and interrupt priorities
extern ErrorT SystemMgrInitExceptionAndInterruptPriorities(void);

// Helper function to notify all registered components of the new phase
extern ErrorT SystemMgrNotifyEnterPhase(SystemMgrPhaseT const newPhase);

// Handles an 'unhandled' event - intended for patching
extern ErrorT SystemMgrOnUnhandledEvent(SystemMgrEventT const event);

// Handles a CC312 library fatal abort
extern void SystemMgrOnCc312Abort(void);

// Terminates due to a fatal error
extern void SystemMgrFatalErrorStop(SystemMgrCssFaultT const fault,
    uint32_t const context,
    uint32_t const infoParam);

// Handles a fatal error event
extern ErrorT SystemMgrOnFatalErrorEvent(void);

// Handles a shutdown-pending event
extern ErrorT SystemMgrOnShutdownPendingEvent(void);

// Handles a doorbell event
extern ErrorT SystemMgrOnDoorbellEvent(void);

// Handles an event
extern ErrorT SystemMgrHandleEvent(SystemMgrEventT const event);

// Returns the highest-priority pending event
extern SystemMgrEventT SystemMgrGetNextEvent(void);

// Dumps System Manager state in the event of a fatal error
extern void SystemMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer,
    uint32_t const sizeLongWords);

/// @} endgroup systemmgr
#endif  // !defined(SRC__SYSTEMMGR__SRC__SYSTEMMGRINTERNAL_H)

