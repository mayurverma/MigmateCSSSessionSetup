#if !defined(SRC__INTERRUPTS__INC__INTERRUPTS_H)
#define      SRC__INTERRUPTS__INC__INTERRUPTS_H
//******************************************************************************
/// \file
///
/// Provides interrupt/exceptions support
///
/// \defgroup platform Platform
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "Common.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

// INFO: <PH>: These allocations MUST match those in AR0820.h!!!
/// Exception allocations
typedef enum
{
    EXCEPTION_RESET = -15,         ///< Reset
    EXCEPTION_NMI = -14,           ///< Non-maskable interrupt
    EXCEPTION_HARDFAULT = -13,     ///< HardFault, all classes of fault
    EXCEPTION_RESERVED_M12 = -12,  ///< Reserved
    EXCEPTION_RESERVED_M11 = -11,  ///< Reserved
    EXCEPTION_RESERVED_M10 = -10,  ///< Reserved
    EXCEPTION_RESERVED_M9 = -9,    ///< Reserved
    EXCEPTION_RESERVED_M8 = -8,    ///< Reserved
    EXCEPTION_RESERVED_M7 = -7,    ///< Reserved
    EXCEPTION_RESERVED_M6 = -6,    ///< Reserved
    EXCEPTION_SVCALL = -5,         ///< System service call vis SVC
    EXCEPTION_RESERVED_M4 = -4,    ///< Reserved
    EXCEPTION_RESERVED_M3 = -3,    ///< Reserved
    EXCEPTION_PENDSV = -2,         ///< Pendable request for system service
    EXCEPTION_SYSTICK = -1,        ///< SysTick timer

    EXCEPTION_MAX = -15  ///< M0+ supports 15 exceptions
} ExceptionT;

// INFO: <PH>: These allocations MUST match those in AR0820.h!!!
/// Interrupt allocations
typedef enum
{
    INTERRUPT_DOORBELL = 0U,                  ///< Command register doorbell interrupt
    INTERRUPT_SOF = 1U,                       ///< Start-of-frame interrupt
    INTERRUPT_EOF = 2U,                       ///< End-of-frame interrupt
    INTERRUPT_SOL = 3U,                       ///< Start-of-line interrupt
    INTERRUPT_EOL = 4U,                       ///< End-of-line interrupt
    INTERRUPT_CC_IS_IDLE = 5U,                ///< CC312 is_idle signal
    INTERRUPT_CC_POWERDWN_READY = 6U,         ///< CC312 powerdown-ready signal
    INTERRUPT_CC_HOST_INT_REQ = 7U,           ///< CC312 host interrupt request signal
    INTERRUPT_PLL_LOCKED = 8U,                ///< Sensor PLL locked
    INTERRUPT_PLL_NOT_LOCKED = 9U,            ///< Sensor PLL not locked
    INTERRUPT_ICORE_ECC_DED_FAIL = 10U,       ///< Icore RAM ECC DED Failure interrupt
    INTERRUPT_ICORE_ECC_SEC_ERR = 11U,        ///< Icore RAM ECC SEC Correction interrupt
    INTERRUPT_RESERVED_12 = 12U,              ///< Reserved
    INTERRUPT_DUALTIMER_INT1 = 13U,           ///< Dual timer interrupt 1
    INTERRUPT_DUALTIMER_INT2 = 14U,           ///< Dual timer interrupt 2
    INTERRUPT_DUALTIMER_INTC = 15U,           ///< Dual timer interrupt C (both timers' interrupts combined)
    INTERRUPT_WATCHDOG = 16U,                 ///< Watchdog timer interrupt
    INTERRUPT_CYBER_ROM_DED_17 = 17U,         ///< Cyber ROM ECC DED Failure
    INTERRUPT_CYBER_ROM_SEC_18 = 18U,         ///< Cyber ROM ECC single bit correction
    INTERRUPT_CYBER_RAM_DED_19 = 19U,         ///< Cyber RAM ECC DED Failure
    INTERRUPT_CYBER_RAM_SEC_20 = 20U,         ///< Cyber RAM ECC single bit correction
    INTERRUPT_RESERVED_21 = 21U,              ///< Reserved
    INTERRUPT_RESERVED_22 = 22U,              ///< Reserved
    INTERRUPT_RESERVED_23 = 23U,              ///< Reserved
    INTERRUPT_RESERVED_24 = 24U,              ///< Reserved
    INTERRUPT_RESERVED_25 = 25U,              ///< Reserved
    INTERRUPT_RESERVED_26 = 26U,              ///< Reserved
    INTERRUPT_RESERVED_27 = 27U,              ///< Reserved
    INTERRUPT_RESERVED_28 = 28U,              ///< Reserved
    INTERRUPT_RESERVED_29 = 29U,              ///< Reserved
    INTERRUPT_RESERVED_30 = 30U,              ///< Reserved
    INTERRUPT_RESERVED_31 = 31U,              ///< Reserved

    INTERRUPT_MIN = 0U,                       ///< First interrupt
    INTERRUPT_MAX = 31U                       ///< M0+ supports 32 interrupts
} InterruptT;

/// Interrupt/exception priorities (for M0+)
typedef enum
{
    INTERRUPT_PRIORITY_0 = 0U,  ///< 0x00 (Highest-priority)
    INTERRUPT_PRIORITY_1 = 1U,  ///< 0x40
    INTERRUPT_PRIORITY_2 = 2U,  ///< 0x80
    INTERRUPT_PRIORITY_3 = 3U,  ///< 0xC0 (Lowest-priority)
} InterruptPriorityT;

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initialize Interrupts
extern ErrorT InterruptsInit(void);

// Enable all interrupts
extern void InterruptsEnableAll(void);

// Disable all interrupts
extern void InterruptsDisableAll(void);

// Enable or disable specified interrupt
extern ErrorT InterruptsEnableInterrupt(InterruptT const interrupt, bool const enable);

// Tests if the specified interrupt is pending
extern ErrorT InterruptsIsInterruptPending(InterruptT const interrupt,
    bool* isPending);

// Clear the specified pending interrupt
extern ErrorT InterruptsClearInterruptPending(InterruptT const interrupt);

// Set priority of specified interrupt
extern ErrorT InterruptsSetInterruptPriority(InterruptT const interrupt,
    InterruptPriorityT const priority);

// Set priority of specified exception
extern ErrorT InterruptsSetExceptionPriority(ExceptionT const exception,
    InterruptPriorityT const priority);

/// @} endgroup platform
#endif  // !defined(SRC__INTERRUPTS__INC__INTERRUPTS_H)

