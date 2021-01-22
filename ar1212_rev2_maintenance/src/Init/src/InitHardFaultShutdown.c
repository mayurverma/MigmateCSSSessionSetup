//******************************************************************************
/// \file
///
/// This file implements the Hard Fault shutdown routine. Shuts-down the CSS
/// in response to a HardFault exception, or an unexpected exception or
/// interrupt.
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

#include <string.h>

#include "bsv_api.h"

#include "Common.h"
#include "AR0820.h"
#include "SystemMgr.h"
#include "Diag.h"
#include "MacDrv.h"
#include "OtpmDrv.h"
#include "CommandDrv.h"
#include "WatchdogDrv.h"

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

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Handles system shutdown in response to a HardFault
///
/// \param[in] fault        CRYPTOERROR fault code
/// \param[in] context      CRYPTOERROR context
/// \param[in] info         INFO context
/// \returns Never
///
void InitHardFaultShutdown(uint32_t const fault, uint32_t const context, uint16_t const info)
{
    uint16_t checkpoint;

    //
    // Shut-down all CSS operations
    //

    // 1. Disable video authentication
    MacDrvResetToIdle();
    (void)SystemDrvEnableClock(SYSTEM_DRV_CSS_CLOCK_VCORE, false);

    // 2. Signal a fatal error to the CC312
    (void)CC_BsvFatalErrorSet((unsigned long)CRYPTOCELL_CORE_BASE);

    // 3. Disable watchdog
    (void)WatchdogDrvDisable();

    // 4. Set the CHECKPOINT, INFO and CRYPTOERROR registers
    checkpoint = SystemDrvGetCheckpoint();
    SystemDrvSetCheckpoint(((uint16_t)1U << 15U) | checkpoint);
    SystemDrvSetInfo(info);
    SystemDrvSetCryptoError(fault, context);

    // 5. Complete host command with SYSERR
    (void)CommandDrvSetResponse((uint16_t)ERROR_SYSERR);

    // 6. Terminate (does not return)
    SystemDrvTerminate();
}

/// @} endgroup init

