#if !defined(SRC__SYSTEMMGR__INC__SYSTEMMGR_H)
#define      SRC__SYSTEMMGR__INC__SYSTEMMGR_H

//***********************************************************************************
/// \file
/// System Manager public API
///
/// \defgroup systemmgr   System Manager
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

#include "Common.h"
#include "SystemDrv.h"

// CSS fault codes (auto-generated)
#include "SystemMgrCssFault.h"

// CSS ROM firmware fatal error context codes (auto-generated)
#include "SystemMgrFatalError.h"

// System phases
#include "SystemMgrPhase.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#if defined(BUILD_TYPE_debug) || !defined(SYSTEM_MGR_ROM_VERSION)
// Default ROM version is zero to indicate an uncontrolled build
#define SYSTEM_MGR_ROM_VERSION  0x0000U  ///< ROM version (format determined by build system)
#endif

/// Maximum number of registered clock speed change notifiers
#define SYSTEM_MGR_MAX_CLOCK_SPEED_CHANGE_NOTIFIERS 4

#define SYSTEM_MGR_MIN_CLOCK_RATE_HZ      6000000UL     ///< Minimum supported system clock frequency
#define SYSTEM_MGR_MAX_CLOCK_RATE_HZ    156000000UL     ///< Maximum supported system clock frequency
#define SYSTEM_MGR_MIN_EXTCLOCK_RATE_HZ   6000000UL     ///< Minimum supported input EXTCLK frequency
#define SYSTEM_MGR_MAX_EXTCLOCK_RATE_HZ  50000000UL     ///< Maximum supported input EXTCLK frequency

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Identifies an event - zero-indexed to allow priority-lookup by event identifier
typedef enum
{
    SYSTEM_MGR_EVENT_WATCHDOG = 0U,             ///< Watchdog interrupt
    SYSTEM_MGR_EVENT_FATAL_ERROR = 1U,          ///< Firmware has detected a fatal error
    SYSTEM_MGR_EVENT_SHUTDOWN_PENDING = 2U,     ///< Firmware shutdown is pending
    SYSTEM_MGR_EVENT_DEBUG_START = 3U,          ///< Debug phase can start
    SYSTEM_MGR_EVENT_PATCH_LOAD_COMPLETE = 4U,  ///< Patch loading is complete
    SYSTEM_MGR_EVENT_SESSION_START = 5U,        ///< Secure session has started
    SYSTEM_MGR_EVENT_DOORBELL = 6U,             ///< Doorbell interrupt
    SYSTEM_MGR_EVENT_CC312_IS_IDLE = 7U,        ///< CC312 idle interrupt
    SYSTEM_MGR_EVENT_CC312_PWRDOWN_READY = 8U,  ///< CC312 powerdown ready interrupt
    SYSTEM_MGR_EVENT_CC312_HOST_INT = 9U,       ///< CC312 host interrupt
    SYSTEM_MGR_EVENT_GMAC_DONE = 10U,           ///< GMAC AES engine completed
    SYSTEM_MGR_EVENT_CMAC_DONE = 11U,           ///< CMAC AES engine completed
    SYSTEM_MGR_EVENT_RESERVED_12 = 12U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_13 = 13U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_14 = 14U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_15 = 15U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_16 = 16U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_17 = 17U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_18 = 18U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_19 = 19U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_20 = 20U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_21 = 21U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_22 = 22U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_23 = 23U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_24 = 24U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_25 = 25U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_26 = 26U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_27 = 27U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_28 = 28U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_29 = 29U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_30 = 30U,         ///< Reserved
    SYSTEM_MGR_EVENT_RESERVED_31 = 31U,         ///< Reserved

    SYSTEM_MGR_EVENT_NONE = 32U,                ///< No event
    SYSTEM_MGR_EVENT_MAX = 32U,                 ///< Max number of events
} SystemMgrEventT;

/// Pointer to a Clock Speed change notifier function
typedef ErrorT (* SystemMgrClockSpeedChangeNotifierT)(uint32_t const clockSpeedHz);

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initializes the system
extern void SystemMgrInit(void);

// Main loop, should never return
extern void SystemMgrMainLoop(void);

// Sets an event flag
extern void SystemMgrSetEvent(SystemMgrEventT const event);

// Indicates if the specified event is pending
extern ErrorT SystemMgrIsEventPending(SystemMgrEventT const event, bool* const isPending);

// Clears a pending event
extern ErrorT SystemMgrClearPendingEvent(SystemMgrEventT const event);

// Reports a firmware fatal error
extern void SystemMgrReportFatalError(SystemMgrFatalErrorT const context,
    uint32_t const infoParam);

// Reports a crypto fault
extern void SystemMgrReportCryptoFault(SystemMgrCssFaultT const fault,
    uint32_t const context,
    uint32_t const infoParam);

// Requests shutdown of the cybersecurity subsystem
extern void SystemMgrRequestShutdown(uint32_t const context,
    uint32_t const infoParam);

// Returns the current phase
extern SystemMgrPhaseT SystemMgrGetCurrentPhase(void);

// Returns the ROM version
extern uint16_t SystemMgrGetRomVersion(void);

// Enables read/write and code access to the patch region
extern void SystemMgrEnablePatchRegion(bool const enable);

// Tells System Manager the current cryptosubsystem clock speed
extern ErrorT SystemMgrSetClockSpeed(uint32_t clockSpeedHz);

// Retrieves the current cryptosubsystem clock speed (if known)
extern ErrorT SystemMgrGetClockSpeed(uint32_t* clockSpeedHz);

// Registers a notification handler for clock speed changes
extern ErrorT SystemMgrRegisterClockSpeedChangeNotifier(SystemMgrClockSpeedChangeNotifierT notifier);

// Indicates whether a CC312 abort (fatal error) has occurred
extern bool SystemMgrHasCC312AbortOccurred(void);

/// @} endgroup systemmgr
#endif  // !defined(SRC__SYSTEMMGR__INC__SYSTEMMGR_H)

