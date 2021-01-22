//******************************************************************************
/// \file
///
/// This file implements the 'unexpected' exception/interrupt handlers. Each
/// one calls a common function that triggers a hard-fault shutdown.
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

#include "Common.h"
#include "AR0820.h"
#include "SystemMgr.h"
#include "InitInternal.h"

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

/// Reports an unexpected exception or interrupt
/// params[in] Irq  Exception or interrupt identifier
/// returns never
///
void UnexpectedHandler(IRQn_Type const Irq)
{
    InitHardFaultShutdown((uint32_t)SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION, (uint32_t)Irq, 0U);
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Handles an unexpected Non-Maskable Interrupt exception
/// returns never
///
void NMI_Handler(void)
{
    UnexpectedHandler(NonMaskableInt_IRQn);
}

/// Handles an unexpected 'Reserved' exception
/// returns never
///
void Reserved_Handler(void)
{
    UnexpectedHandler(Exception4_IRQn);
}

/// Handles an unexpected System Tick exception
/// returns never
///
void SysTick_Handler(void)
{
    UnexpectedHandler(SysTick_IRQn);
}

/// Handles an unexpected Start-of-Frame interrupt
/// returns never
///
void SOF_IrqHandler(void)
{
    UnexpectedHandler(SOF_IRQn);
}

/// Handles an unexpected End-of-Frame interrupt
/// returns never
///
void EOF_IrqHandler(void)
{
    UnexpectedHandler(EOF_IRQn);
}

/// Handles an unexpected Start-of-Line interrupt
/// returns never
///
void SOL_IrqHandler(void)
{
    UnexpectedHandler(SOL_IRQn);
}

/// Handles an unexpected End-of-Line interrupt
/// returns never
///
void EOL_IrqHandler(void)
{
    UnexpectedHandler(EOL_IRQn);
}

/// Handles an unexpected CC312 Idle interrupt
/// returns never
///
void CCIsIdle_IrqHandler(void)
{
    UnexpectedHandler(CCIsIdle_IRQn);
}

/// Handles an unexpected CC312 Power-down Ready interrupt
/// returns never
///
void CCPowerdownReady_IrqHandler(void)
{
    UnexpectedHandler(CCPowerdownReady_IRQn);
}

/// Handles an unexpected CC312 Host interrupt
/// returns never
///
void CCHostIntReq_IrqHandler(void)
{
    UnexpectedHandler(CCHostIntReq_IRQn);
}

/// Handles an unexpected PLL locked Irq
/// returns never
///
void PllLocked_IrqHandler(void)
{
    UnexpectedHandler(PllLocked_IRQn);
}

/// Handles an unexpected PLL not locked Irq
/// returns never
///
void PllNotLocked_IrqHandler(void)
{
    UnexpectedHandler(PllNotLocked_IRQn);
}

/// Handles an unexpected ICore ECC SEC interrupt
/// returns never
///
void IcoreEccSec_IrqHandler(void)
{
    UnexpectedHandler(IcoreEccSec_IRQn);
}

// IcoreEccDed_IrqHandler() is defined in CommandHandler

/// Handles an unexpected Irq
/// returns never
///
void Reserved12_IrqHandler(void)
{
    UnexpectedHandler(Reserved12_IRQn);
}

/// Handles an unexpected Dual-Timer 1 interrupt
/// returns never
///
void DualTimerInt1_IrqHandler(void)
{
    UnexpectedHandler(DualTimerInt1_IRQn);
}

/// Handles an unexpected Dual-Timer 2 interrupt
/// returns never
///
void DualTimerInt2_IrqHandler(void)
{
    UnexpectedHandler(DualTimerInt2_IRQn);
}

/// Handles an unexpected Dual-Timer C interrupt
/// returns never
///
void DualTimerIntC_IrqHandler(void)
{
    UnexpectedHandler(DualTimerIntC_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void CyberRomDed_IrqHandler(void)
{
    UnexpectedHandler(CyberRomDed_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void CyberRomSec_IrqHandler(void)
{
    UnexpectedHandler(CyberRomSec_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void CyberRamDed_IrqHandler(void)
{
    UnexpectedHandler(CyberRamDed_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void CyberRamSec_IrqHandler(void)
{
    UnexpectedHandler(CyberRamSec_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved21_IrqHandler(void)
{
    UnexpectedHandler(Reserved21_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved22_IrqHandler(void)
{
    UnexpectedHandler(Reserved22_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved23_IrqHandler(void)
{
    UnexpectedHandler(Reserved23_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved24_IrqHandler(void)
{
    UnexpectedHandler(Reserved24_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved25_IrqHandler(void)
{
    UnexpectedHandler(Reserved25_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved26_IrqHandler(void)
{
    UnexpectedHandler(Reserved26_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved27_IrqHandler(void)
{
    UnexpectedHandler(Reserved27_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved28_IrqHandler(void)
{
    UnexpectedHandler(Reserved28_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved29_IrqHandler(void)
{
    UnexpectedHandler(Reserved29_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved30_IrqHandler(void)
{
    UnexpectedHandler(Reserved30_IRQn);
}

/// Handles an unexpected Irq
/// returns never
///
void Reserved31_IrqHandler(void)
{
    UnexpectedHandler(Reserved31_IRQn);
}

/// @} endgroup init

