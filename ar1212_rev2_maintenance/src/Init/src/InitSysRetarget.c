//***********************************************************************************
/// \file
///
/// Debug Uart Driver implementation
///
/// \addtogroup init
/// @{
//***********************************************************************************
// Copyright 2017 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//*************************************************************************************

#include <stdio.h>
#include <rt_sys.h>

#include "Common.h"
#include "DebugUartDrv.h"

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

// this pragma disables semihosting so any standard C library calls that
// require a low-level target specific implementation will fail at link;
// the low-level function can then be implemented here
#pragma import(__use_no_semihosting)

/// Need to provide our own version of _ttywrch() which has been removed from the ARM lib
/// by __use_no_semihosting in the debug build.
/// \param[in] ch       The 8-bit char to be transmitted
/// \returns void
///
void _ttywrch(int ch)
{
#if (defined BUILD_TYPE_debug)
    DebugUartDrvPutc((char)ch);
#else
    (void)ch;
#endif
}

/// Our own version of _sys_exit() replaces the ARM lib version in the debug build
/// This function is just an infinite wait loop. It does not return.
/// \param[in] return_code unused
/// \returns void
///
void _sys_exit(int return_code)
{
    (void)return_code;

    /* endless loop for debugging */
    while (true)
    {
        // do nothing
    }
}

/// Need to provide our own version of _sys_open() which has been removed from the ARM lib
/// by __use_no_semihosting. It calls debug uart initialisation in the debug build
/// \param[in] name unused
/// \param[in] openmode unused
/// \returns a fixed value of 1
///
FILEHANDLE $Sub$$_sys_open(const char* name, int openmode)
{
    (void)name;
    (void)openmode;

    // Initialise the debug uart
#if (defined BUILD_TYPE_debug)
    DebugUartDrvInit();
#endif

    return (FILEHANDLE)1;
}
/// @} endgroup init

