//***********************************************************************************
/// \file
///
/// Debug Uart Driver implementation
///
/// \addtogroup debuguartdrv
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

/// Need to provide our own version of fputc() which has been removed from the ARM lib
/// by __use_no_semihosting in the debug build.
/// \param[in] ch       The 8-bit char to be transmitted
/// \param[in] f        Pointer to FILE type (unused)
/// \returns the value written to the debug uart
///
int fputc(int ch, FILE* f)
{
    (void)f;

#if (defined BUILD_TYPE_debug)
    DebugUartDrvPutc((char)ch);
#endif

    return ch;
}

/// @} endgroup debuguartdrv

