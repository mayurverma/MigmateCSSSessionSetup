//***********************************************************************************
/// \file
///
/// Watchdog driver
///
/// \addtogroup watchdogdrv
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

#include "AR0820.h"
#include "Common.h"
#include "WatchdogDrv.h"
#include "WatchdogDrvInternal.h"

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

/// Initialize the Watchdog Driver
/// \returns Error status
/// \retval ERROR_SUCCESS   Watchdog driver initialized successfully
///
ErrorT WatchdogDrvInit(void)
{
    // There is nothing to do in the initialization as the register writings are disabled by default.
    return ERROR_SUCCESS;
}

/// Configure and start the watchdog
///
/// Watchdog is 'locked' to prevent spurious disable of the watchdog
///
/// \param[in] timeOutClocks    Watchdog time-out in clocks
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Watchdog started successfully
///
ErrorT WatchdogDrvStart(uint32_t const timeOutClocks)
{
    // Writing 0x1ACCE551 to this register enables write access to all other registers.
    WATCHDOG_REGS->Lock = WDOG_LOCK_ENABLE_KEY;

    // The watchdog reload value.
    WATCHDOG_REGS->Load = timeOutClocks;

    // Configure and start the watchdog
    // Enable Watchdog module reset output
    // Enable the counter and the interrupt
    // Enable reloading the counter from the value in the watchdog Load register.
    WATCHDOG_REGS->Control = ((1UL << WATCHDOG_CONTROL_INTEN__SHIFT) | (1UL << WATCHDOG_CONTROL_RESEN__SHIFT));

    // Writing any other value disables write access to all other registers.
    WATCHDOG_REGS->Lock = 0U;

    return ERROR_SUCCESS;
}

/// Clear the watchdog interrupt ('pat' the watchdog)
///
/// Clears the watchdog interrupt, and restarts the watchdog timer
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Watchdog petted successfully
///
ErrorT WatchdogDrvClearInterrupt(void)
{
    // Writing 0x1ACCE551 to this register enables write access to all other registers.
    WATCHDOG_REGS->Lock = WDOG_LOCK_ENABLE_KEY;

    // Write any value to this location clears the Watchdog module interrupt, and reloads
    // the counter from the value in the watchdog Load Register.
    WATCHDOG_REGS->IntClr = 0U;

    // Disables write access after interrupt clearing.
    WATCHDOG_REGS->Lock = 0U;

    return ERROR_SUCCESS;
}

/// Disable the watchdog
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Watchdog disabled successfully
///
ErrorT WatchdogDrvDisable(void)
{
    // Writing 0x1ACCE551 to this register enables write access to all other registers.
    WATCHDOG_REGS->Lock = WDOG_LOCK_ENABLE_KEY;

    // Configure and stop the watchdog
    // Disable Watchdog module reset output
    // Disable the counter and the interrupt
    WATCHDOG_REGS->Control = ((0UL << WATCHDOG_CONTROL_INTEN__SHIFT) | (0UL << WATCHDOG_CONTROL_RESEN__SHIFT));

    // Writing any other value disables write access to all other registers.
    WATCHDOG_REGS->Lock = 0U;

    return ERROR_SUCCESS;
}

/// @} endgroup watchdogdrv

