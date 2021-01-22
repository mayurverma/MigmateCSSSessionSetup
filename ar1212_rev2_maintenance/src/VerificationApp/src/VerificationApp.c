//***********************************************************************************
/// \file
///
/// Verification Application
///
/// \addtogroup verificationapp
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

#include "VerificationAppInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
const uint16_t verificationAppRoData[] =
{
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000,
    0x000A, 0x00B0, 0x0C00, 0xD000
};
uint16_t verificationAppRwDataInit[] = {0x000E, 0x00E0, 0x0E00, 0xE000};
uint16_t verificationAppRwDataZero[16] = {0};
uint16_t verificationAppRwDataUnint[16];
const char* verificationAppHello = "Hello World";
uint32_t aescmac_status = 0;

//////////////////////////////////////////////////////////////////////////////////////////////
// This implementation should come from SystemMgrPatch.c if it was a patch in maintenance branch
//////////////////////////////////////////////////////////////////////////////////////////////
/// Returns the ROM version
/// \returns Patched ROM version
uint16_t SystemMgrGetRomVersionPatched(void)
{
    return 0xCAFE;
}
/// Returns the ROM version
/// \returns Patched ROM version
uint16_t SystemMgrGetRomVersionBeefPatched(void)
{
    return 0xBEEF;
}

/// Return the value of the Vcore aescmac_status register
///
/// \returns Vcore CMAC status register
/// \retval CMAC status register value
uint32_t MacDrvGetCmacStatusPatched(void)
{
    return ((CRYPTO_VCORE_REGS->aescmac_status) | aescmac_status);
}
//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

// @} endgroup verificationapp

