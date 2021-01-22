//***********************************************************************************
/// \file
///
/// HAL/PAL Configuration
///
/// \addtogroup cc312halpal
/// @{
//***********************************************************************************
//
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.
//       ALL RIGHTS RESERVED
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//
//***********************************************************************************
//
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
#include "cc_regs.h"
#include "cc_pal_memmap.h"
#include "cc_hal.h"
#include "dx_crys_kernel.h"
#include "cc_pal_abort.h"
#include "cc_error.h"
#include "cc_regs.h"

#include "cc_pal_interrupt_ctrl_plat.h"
#include "dx_rng.h"

#include "AR0820.h"
//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
unsigned long gCcRegBase = 0U;

/// Initialises global variable gCcRegBase to base of CC register set
///
/// \returns void
///
int CC_HalInit(void)
{
    gCcRegBase = DX_BASE_CC;

    return 0;
}

/// HAL exit point, Unmaps ARM CryptoCell registers.
///
/// \returns command success/error
///
int CC_HalTerminate(void)
{
    gCcRegBase = 0;

    return (int)CC_HAL_OK;
}

/// This function is used to clear the interrupt vector.
///
/// \param[in] data    Value to write to ICR register
/// \returns void
///
void CC_HalClearInterruptBit(uint32_t data)
{
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ICR), data);
}

/// This function is used to mask IRR interrupts.
///
/// \param[in] data    Value to write to IMR register
/// \returns void
///
void CC_HalMaskInterrupt(uint32_t data)
{
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IMR), data);
}

/// Wait upon Interrupt Request Register (IRR) signals.
///
/// This function notifies for any ARM CryptoCell interrupt, it is the caller responsibility
/// to verify and prompt the expected case interrupt source.
///
/// \param[in] data    Value to write to IMR register
/// \returns CCError_t        Indicate success or failure result.
/// \retval  CC_OK            Upon success
/// \retval  CC_FAIL          The expected interrupt never comes.
/// \retval  CC_FATAL_ERROR   Invalid input data.
///
CCError_t CC_HalWaitInterrupt(uint32_t data)
{
    return (CC_PalWaitInterrupt(data));
}

/// This function is used to wait for the IRR interrupt signal.
///
/// The existing implementation performs a "busy wait" on the IRR.
///
/// \param[in] data    Value to write to IMR register
/// \returns CCError_t        Indicate success or failure result.
/// \retval  CC_OK            Upon success
/// \retval  CC_FATAL_ERROR   Invalid input data or the expected interrupt never comes.
///
CCError_t CC_HalWaitInterruptRND(uint32_t data)
{
    CCError_t error = CC_OK;

    // Call the PAL function which has the same functionality.
    error = CC_PalWaitInterrupt(data);
    // This function used to return CC_FATAL_ERROR in the original ARM's code.
    // The following error conversion is for returning the identical error code as before.
    if ((CCError_t)CC_FAIL == error)
    {
        error = CC_FATAL_ERROR;
    }

    return error;
}

/// @} endgroup cc312halpal

