//******************************************************************************
/// \file
///
/// This file implements the AR0820 system initialisation functions
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
#include "InitSystem.h"

#if ((defined BUILD_TYPE_debug) && (defined BUILD_TARGET_PLATFORM_barebones))
#include "DebugUartDrv.h"
#endif

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

/// Initializes the system and the core (Cortex-M0+) so that the C library initialisation can be invoked
///
/// \returns void
///
#if defined(BUILD_TOOLCHAIN_arm_none_eabi_gcc)
// Declare as a constructor so that it will be invoked prior to main()
__attribute__((constructor))
#endif
void InitSystem(void)
{
    // INFO: <PH>: Code to initialize the system
    //             Do not use global variables because this function is called before
    //             reaching pre-main. RW section maybe overwritten afterwards.

#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
    // ARM-specific MPU initialisation
    InitMpu();
#endif
}

/// @} endgroup init

