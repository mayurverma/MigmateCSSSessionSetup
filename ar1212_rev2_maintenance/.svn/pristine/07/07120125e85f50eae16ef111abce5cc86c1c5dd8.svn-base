#if !defined(SRC__INIT__SRC__INITINTERNAL_H)
#define      SRC__INIT__SRC__INITINTERNAL_H
//******************************************************************************
/// \file
///
/// Initialization support internal API
///
/// \addtogroup init
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

#include "Init.h"

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

// Reports an unexpected exception or interrupt
extern void UnexpectedHandler(IRQn_Type const Irq);

// Handles a HardFault
extern void InitHardFaultHandler(uint32_t* FramePtr, uint32_t LinkReg, uint32_t ControlReg);

// Handles system shutdown in response to a HardFault or unexpected exception/interrupt
extern void InitHardFaultShutdown(uint32_t const fault, uint32_t const context, uint16_t const info);

// Handles an unexpected Non-Maskable Interrupt exception
extern void NMI_Handler(void);

// Handles an unexpected pendable Supervisor Call exception
extern void PendSV_Handler(void);

// Handles an unexpected 'Reserved' exception
extern void Reserved_Handler(void);

// Handles an unexpected System Tick exception
extern void SysTick_Handler(void);

// Handles an unexpected Start-of-Frame interrupt
extern void SOF_IrqHandler(void);

// Handles an unexpected End-of-Frame interrupt
extern void EOF_IrqHandler(void);

// Handles an unexpected Start-of-Line interrupt
extern void SOL_IrqHandler(void);

// Handles an unexpected End-of-Line interrupt
extern void EOL_IrqHandler(void);

// Handles an unexpected CC312 Idle interrupt
extern void CCIsIdle_IrqHandler(void);

// Handles an unexpected CC312 Power-down Ready interrupt
void CCPowerdownReady_IrqHandler(void);

// Handles an unexpected CC312 Host interrupt
extern void CCHostIntReq_IrqHandler(void);

// Handles an unexpected PllLocked Irq
extern void PllLocked_IrqHandler(void);

// Handles an unexpected PllNotLocked Irq
extern void PllNotLocked_IrqHandler(void);

// Handles an unexpected ICore ECC SEC interrupt
extern void IcoreEccSec_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved12_IrqHandler(void);

// Handles an unexpected Dual-Timer 1 interrupt
extern void DualTimerInt1_IrqHandler(void);

// Handles an unexpected Dual-Timer 2 interrupt
extern void DualTimerInt2_IrqHandler(void);

// Handles an unexpected Dual-Timer C interrupt
extern void DualTimerIntC_IrqHandler(void);

// Handles an unexpected Watchdog Timer interrupt
extern void Watchdog_IrqHandler(void);

// Handles an unexpected Irq
extern void CyberRomDed_IrqHandler(void);

// Handles an unexpected Irq
extern void CyberRomSec_IrqHandler(void);

// Handles an unexpected Irq
extern void CyberRamDed_IrqHandler(void);

// Handles an unexpected Irq
extern void CyberRamSec_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved21_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved22_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved23_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved24_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved25_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved26_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved27_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved28_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved29_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved30_IrqHandler(void);

// Handles an unexpected Irq
extern void Reserved31_IrqHandler(void);

/// @} endgroup init
#endif  // !defined(SRC__INIT__SRC__INITINTERNAL_H)

