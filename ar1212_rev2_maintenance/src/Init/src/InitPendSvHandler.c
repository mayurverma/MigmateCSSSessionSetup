//******************************************************************************
/// \file
///
/// Initialisation and Exception support: PendSV handler
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
#include "Diag.h"
#include "Interrupts.h"

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

/// Handles a PendSV exception
///
/// The PendSV exception can only be (correctly) signalled by System Manager, in
/// response to a CC312 software abort. This handler checks that System Manager
/// did signal the abort.
///
/// \returns Never
///
void PendSV_Handler(void)
{
    if (SystemMgrHasCC312AbortOccurred())
    {
        DiagPrintf("CC312 abort\n");
        InitHardFaultShutdown((uint32_t)SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR, 0U, 0U);
    }
    else
    {
        InitHardFaultShutdown((uint32_t)SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION, (uint32_t)PendSV_IRQn, 0U);
    }
}

/// @} endgroup init

