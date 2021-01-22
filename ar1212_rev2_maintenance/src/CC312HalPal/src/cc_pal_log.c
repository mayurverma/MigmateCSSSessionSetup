//***********************************************************************************
/// \file
///
/// HAL/PAL Loggong Interface
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
#include <stdio.h>
#include "cc_pal_types.h"
#include "cc_pal_log.h"
#include <stdarg.h>

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
int CC_PAL_logLevel = CC_PAL_MAX_LOG_LEVEL;
uint32_t CC_PAL_logMask = 0xFFFFFFFF;

/// Log initialization function - platform dependent.
///
/// \returns void
///
void CC_PalLogInit(void)
{
}

/// Sets the component masking in case of debug.
///
/// \param[in] setMask       Logging mask
/// \returns void
///
void CC_PalLogMaskSet(uint32_t setMask)
{
    CC_PAL_logMask = setMask;
}

/// Logging Function
///
/// This function notifies for any ARM CryptoCell interrupt, it is the caller responsibility
/// to verify and prompt the expected case interrupt source.
///
/// \param[in] level       Logging level (not used)
/// \param[in] format      Formatted string
/// \returns void
///
void CC_PalLog(int level, const char* format, ...)
{
    va_list args;

    CC_UNUSED_PARAM(level);
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
/// @} endgroup cc312halpal

