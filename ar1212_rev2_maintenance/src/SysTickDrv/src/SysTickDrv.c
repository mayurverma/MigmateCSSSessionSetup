//***********************************************************************************
/// \file
///
/// SysTick driver
///
/// \addtogroup systickdrv
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
//*************************************************************************************

#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "SysTickDrv.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

/// SysTick driver internal state
typedef struct
{
    volatile uint32_t numTicks;
    SysTickDrvHandler userHandler;
} SysTickDrvStateT;

SysTickDrvStateT sysTickDrvState;

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

/// Default SysTick interrupt handler
/// Calls an optional user handler if registered
void SysTick_Handler(void)
{
    // Read the control and status register to reset the 'COUNTFLAG' field
    (void)SysTick->CTRL;

    sysTickDrvState.numTicks += 1;

    if (NULL != sysTickDrvState.userHandler)
    {
        sysTickDrvState.userHandler(sysTickDrvState.numTicks);
    }
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialises the SysTick driver
/// \returns Success or failure
ErrorT SysTickDrvInit(void)
{
    (void)memset(&sysTickDrvState, 0, sizeof(SysTickDrvStateT));

    return ERROR_SUCCESS;
}

/// Configures and starts the SysTick timer
///
/// \param[in]  clocks - the number of (processor) clocks per 'tick' (interrupt)
/// \returns    Success or failure
/// \retval     #ERROR_SUCCESS - configured successfully and timer running
/// \retval     #ERROR_INVAL - invalid configuration
///
ErrorT SysTickDrvStart(uint32_t const clocks)
{
    ErrorT status = ERROR_SUCCESS;
    int res;

    res = SysTick_Config(clocks);
    if (0 != res)
    {
        status = ERROR_INVAL;
    }
    else
    {
        sysTickDrvState.numTicks = 0;

        // Enable the SysTick exception
        NVIC_ClearPendingIRQ(SysTick_IRQn);
        NVIC_EnableIRQ(SysTick_IRQn);
    }

    return status;
}

/// Stops the SysTick timer
/// \returns Success or failure
///
ErrorT SysTickDrvStop(void)
{
    // Disable the timer and interrupt
    SysTick->CTRL = 0;
    NVIC_DisableIRQ(SysTick_IRQn);

    return ERROR_SUCCESS;
}

uint32_t SysTickDrvGetCurrentTicks(void)
{
    return sysTickDrvState.numTicks;
}

/// Install a user SysTick interrupt handler
/// \param[in] handler  The new handler
/// \returns The current handler
SysTickDrvHandler SysTickDrvSetHandler(SysTickDrvHandler handler)
{
    SysTickDrvHandler currentHandler = sysTickDrvState.userHandler;

    sysTickDrvState.userHandler = handler;

    return currentHandler;
}

// @} endgroup systickdrv

