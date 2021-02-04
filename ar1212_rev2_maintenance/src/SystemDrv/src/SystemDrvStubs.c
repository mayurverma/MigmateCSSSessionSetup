//***********************************************************************************
/// \file
///
/// System driver
///
/// \addtogroup systemdrv
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

#include <stdio.h>
#include <assert.h>

#include "AR0820.h"
#include "Common.h"

#include "SystemDrvInternal.h"

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

/// Puts the processor to sleep and waits for an interrupt
///
/// Interrupts MUST be disabled on entry.
/// Interrupts are STILL disabled on exit.
///
/// \returns void
///
void SystemDrvWaitForInterrupt(void)
{
    // Stub - do nothing
}

/// Terminate processor execution
///
/// \returns Never
void SystemDrvTerminate(void)
{
    // Stub - generate an assert
    do
    {
        assert(0);
    }
    while (true);
}

// @} endgroup systemdrv

