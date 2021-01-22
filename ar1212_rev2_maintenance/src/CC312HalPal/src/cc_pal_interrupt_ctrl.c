//***********************************************************************************
/// \file
///
/// HAL/PAL Interrupt control
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
#include "CC312HalPal.h"
#include "cc_pal_types.h"
#include "cc_pal_mutex.h"
#include "cc_pal_interrupt_ctrl_plat.h"
#include "cc_error.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "cc_hal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
// INFO: <RW>: Loop takes 19 cycles (assuming zero wait state memory, so will be slower)
// Set the loop timeout to 4M  (4M*19)/156e6 which corresponds to 0.49 Sec @ 156MHz
#define PAL_WAIT_TIMEOUT   4000000U

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
/// PAL initialise IRQ
///
/// \returns Command execution success/failure
/// \retval CC_SUCCESS                            Success (OK) definition.
///
CCError_t CC_PalInitIrq(void)
{
    return (CCError_t)CC_SUCCESS;
}

/// This function removes the interrupt handler for cryptocell interrupts
///
/// \returns void
///
void CC_PalFinishIrq(void)
{
}

/// Busy wait upon Interrupt Request Register (IRR) signals.
///
/// This function notifies for any ARM CryptoCell interrupt, it is the caller responsibility
/// to verify and prompt the expected case interrupt source.
///
/// \param[in] data     input data
/// \returns            Command execution success/failure
/// \retval CC_OK       Upon success
/// \retval CC_FAIL          The expected interrupt never comes.
/// \retval CC_FATAL_ERROR   Invalid input data.
///
CCError_t CC_PalWaitInterrupt(uint32_t data)
{
    uint32_t irr = 0U;
    uint32_t count = 0U;
    CCError_t error = CC_OK;
    CCBool isAbort = CC_FALSE;
    uint32_t localData = data;

    if (0U == data)
    {
        error = CC_FATAL_ERROR;
    }
    else
    {
        // busy wait upon IRR signal
        do
        {
            irr = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR));
            // check APB bus error from HOST.
            if (0U != CC_REG_FLD_GET(0U, HOST_IRR, AHB_ERR_INT, irr))
            {
                error = (CCError_t)CC_FAIL;
                // set data for clearing bus error
                CC_REG_FLD_SET(HOST_RGF, HOST_ICR, AXI_ERR_CLEAR, localData, 1U);
                isAbort = CC_TRUE;
            }

            if (PAL_WAIT_TIMEOUT < count)
            {
                error = (CCError_t)CC_FAIL;
                isAbort = CC_TRUE;
            }

            if (CC_TRUE == isAbort)
            {
                break;
            }
            count++;
        }
        while (0U == (irr & data));
    }

    // clear interrupt
    // IRR and ICR bit map is the same use data to clear interrupt in ICR
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ICR), data);

    return error;
}

/// @} endgroup cc312halpal

