#if !defined(INC__AR0820_H)
#define      INC__AR0820_H
//******************************************************************************
/// \file
///
/// CMSIS Device Header file for the AR0820
///
/// \note This file is based on the CMSIS framework provided by ARM and is therefore
///       provided under a separate license - please see license details below.
///
/// \defgroup ar0820_cmsis AR0820 CMSIS Support
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
//******************************************************************************
// brief    CMSIS Cortex-M0+ Core Peripheral Access Layer Header File for
//          Device AR0820
// version  V5.00
// date     02. March 2016
//******************************************************************************
//
// Copyright (c) 2009-2016 ARM Limited. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the License); you may
// not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifdef __cplusplus
extern "C" {
#endif

// CMSIS only supports C99. We include stdint.h here on the assumption that all compilers will support C99.
#include <stdint.h>
#include <stdbool.h>

// Include the top level memory map definition
#include "AR0820_memmap.h"

// ===========================================================================================================================
// ================                                Interrupt Number Definition                                ================
// ===========================================================================================================================

/// Exceptions and Interrupt numbers
typedef enum IRQn
{
    // =======================================  ARM Cortex-M0+ Processor Exception Numbers  ======================================
    Reset_IRQn = -15,           //!<  1  Reset Vector, invoked on Power up and warm reset
    NonMaskableInt_IRQn = -14,  //!<  2  Non maskable Interrupt, cannot be stopped or preempted
    HardFault_IRQn = -13,       //!<  3  Hard Fault, all classes of Fault
    Exception4_IRQn = -12,      //!<  4  Reserved
    Exception5_IRQn = -11,      //!<  5  Reserved
    Exception6_IRQn = -10,      //!<  6  Reserved
    Exception7_IRQn = -9,       //!<  7  Reserved
    Exception8_IRQn = -8,       //!<  8  Reserved
    Exception9_IRQn = -7,       //!<  9  Reserved
    Exception10_IRQn = -6,      //!< 10  Reserved
    SVCall_IRQn = -5,           //!< 11  System Service Call via SVC instruction
    Exception12_IRQn = -4,      //!< 12  Reserved
    Exception13_IRQn = -3,      //!< 13  Reserved
    PendSV_IRQn = -2,           //!< 14  Pendable request for system service
    SysTick_IRQn = -1,          //!< 15  System Tick Timer

    // ===========================================  AR0820 Specific Interrupt Numbers  ===========================================

    // INFO: <SC>: Keep the interrupt table and the ones in the HW document in sync
    // Notes : - IMAGINGPRJ-2992
    //         - <SP> Updated as per Micro Arch Doc v0.6 (24/5/18)
    //         - <PH> Updated as per Micro Arch Doc v52.0 (2/Jul/18)
    //
    //         - These allocations MUST match those in InitStartup.s/InitVectors.c/Interrupts.h!!!
    //
    Doorbell_IRQn = 0,          //!< Command register doorbell interrupt (pulsed)
    SOF_IRQn = 1,               //!< Start-of-frame interrupt (pulsed)
    EOF_IRQn = 2,               //!< End-of-frame interrupt (pulsed)
    SOL_IRQn = 3,               //!< Start-of-line interrupt (pulsed)
    EOL_IRQn = 4,               //!< End-of-line interrupt (pulsed)
    CCIsIdle_IRQn = 5,          //!< CC312 is_idle signal (pulsed)
    CCPowerdownReady_IRQn = 6,  //!< CC312 powerdown-ready signal (pulsed)
    CCHostIntReq_IRQn = 7,      //!< CC312 host interrupt request signal (pulsed)
    PllLocked_IRQn = 8,         //!< Sensor PLL locked (pulsed)
    PllNotLocked_IRQn = 9,      //!< Sensor PLL not locked (pulsed)
    IcoreEccDed_IRQn = 10,      //!< Icore RAM ECC DED Failure interrupt
    IcoreEccSec_IRQn = 11,      //!< Icore RAM ECC SEC Correction interrupt
    Reserved12_IRQn = 12,       //!< Reserved
    DualTimerInt1_IRQn = 13,    //!< Dual timer interrupt 1 (pulsed)
    DualTimerInt2_IRQn = 14,    //!< Dual timer interrupt 2 (pulsed)
    DualTimerIntC_IRQn = 15,    //!< Dual timer interrupt C (common) (pulsed)
    Watchdog_IRQn = 16,         //!< Watchdog timer interrupt (pulsed)
    CyberRomDed_IRQn = 17,      //!< Cyber ROM ECC DED Failure (pulsed)
    CyberRomSec_IRQn = 18,      //!< Cyber ROM ECC single bit correction (pulsed)
    CyberRamDed_IRQn = 19,      //!< Cyber RAM ECC DED Failure (pulsed)
    CyberRamSec_IRQn = 20,      //!< Cyber RAM ECC single bit correction (pulsed)
    Reserved21_IRQn = 21,       //!< Reserved
    Reserved22_IRQn = 22,       //!< Reserved
    Reserved23_IRQn = 23,       //!< Reserved
    Reserved24_IRQn = 24,       //!< Reserved
    Reserved25_IRQn = 25,       //!< Reserved
    Reserved26_IRQn = 26,       //!< Reserved
    Reserved27_IRQn = 27,       //!< Reserved
    Reserved28_IRQn = 28,       //!< Reserved
    Reserved29_IRQn = 29,       //!< Reserved
    Reserved30_IRQn = 30,       //!< Reserved
    Reserved31_IRQn = 31,       //!< Reserved
} IRQn_Type;

#define AR0820_MAX_IRQ                  32  // CM0+ supports max 32 IRQs

// ===========================================================================================================================
// ================                           Processor and Core Peripheral Section                           ================
// ===========================================================================================================================

// ===========================  Configuration of the ARM Cortex-M0+ Processor and Core Peripherals  ===========================
#define __CM0PLUS_REV             0x0001U   //!< Core revision r0p1
#define __MPU_PRESENT             1         //!< Set to 1 if MPU is present
#define __VTOR_PRESENT            1         //!< Set to 1 if VTOR is present
#define __NVIC_PRIO_BITS          2         //!< Number of bits used for Priority Levels
#define __Vendor_SysTickConfig    0         //!< Set to 1 if different SysTick Config is used

#include <core_cm0plus.h>                   // ARM Cortex-M0+ processor and core peripherals

// ===========================================================================================================================
// ================                            Device Specific Peripheral Section                             ================
// ===========================================================================================================================

// ===========================================================================================================================
// ================                                            WDOG                                           ================
// ===========================================================================================================================

/// Watchdog (WDOG) - ARM Cortex System Design Kit watchdog (cmsdk_apb_watchdog.v)
typedef struct
{
    __IOM uint32_t Load;              //!< Offset: 0x000 (R/W)  Load register
    __IM uint32_t Value;              //!< Offset: 0x004 (R/ )  Current value
    __IOM uint32_t Control;           //!< Offset: 0x008 (R/W)  Control register
    __OM uint32_t IntClr;             //!< Offset: 0x00C ( /W)  Interrupt clear
    __IM uint32_t RIS;                //!< Offset: 0x010 (R/ )  Raw interrupt status
    __IM uint32_t MIS;                //!< Offset: 0x014 (R/ )  Masked interrupt status
    __IM uint32_t Reserved[0x2FA];    //!< Offset: 0x018 (R/ )  Reserved
    __IOM uint32_t Lock;              //!< Offset: 0xC00 (R/W)  Lock register
} WDOG_TypeDef;

// Watchdog control interrupt enable
#define WATCHDOG_CONTROL_INTEN__SHIFT        0
#define WATCHDOG_CONTROL_INTEN__SIZE         1
#define WATCHDOG_CONTROL_INTEN__MASK         (WATCHDOG_CONTROL_INTEN__SIZE << WATCHDOG_CONTROL_INTEN__SHIFT)

// Watchdog control reset enable
#define WATCHDOG_CONTROL_RESEN__SHIFT        1
#define WATCHDOG_CONTROL_RESEN__SIZE         1
#define WATCHDOG_CONTROL_RESEN__MASK         (WATCHDOG_CONTROL_RESEN__SIZE << WATCHDOG_CONTROL_RESEN__SHIFT)

// PLL/clock support?

// ===========================================================================================================================
// ================                                  Peripheral declaration                                   ================
// ===========================================================================================================================

#define CRYPTO_ICORE_REGS  ((CRYPTO_ICORE_REGS_T*) CRYPTO_ICORE_REG_BASE)
#define CRYPTO_VCORE_REGS  ((CRYPTO_VCORE_REGS_T*) CRYPTO_VCORE_REG_BASE)
#define DEBUG_UART_REGS    ((DEBUG_UART_REGS_T*)   DEBUG_UART_REG_BASE)
#define MCU_SYSCTRL_REGS   ((MCU_SYSCTRL_REGS_T*)  MCU_SYSCTRL_REG_BASE)
#define PATCHER_REGS       ((PATCHER_REGS_T*)      PATCHER_REG_BASE)
#define WATCHDOG_REGS      ((WDOG_TypeDef*)        WATCHDOG_REG_BASE)
#define OTPM_BASE          ((uint32_t*)            CRYPTO_OTPM_DATA)
#define OTPM_REGS          ((CRYPTO_OTPM_REGS_T*)  CRYPTO_OTPM_REG_BASE)

#ifdef __cplusplus
}
#endif

/// @} endgroup ar0820_cmsis
#endif  // !defined(INC__AR0820_H)

