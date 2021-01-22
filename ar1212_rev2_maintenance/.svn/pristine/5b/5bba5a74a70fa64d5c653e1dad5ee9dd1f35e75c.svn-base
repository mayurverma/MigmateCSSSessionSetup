#if !defined(SRC__VERIFICATIONAPP__INC__VERIFICATION_APP_INTERNAL_H)
#define      SRC__VERIFICATIONAPP__INC__VERIFICATION_APP_INTERNAL_H
//***********************************************************************************
/// \file
///
/// \defgroup verificationapp Verification Application
///
/// Verification Application used to extend test coverage of ROM
///
/// @{
//***********************************************************************************
// Copyright 2019 ON Semiconductor.  All rights reserved.
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
#include "AR0820.h"
#include "Common.h"
#include "AR0820_memmap.h"
#include "crypto_vcore.h"

#include "Patch.h"
#include "PatchMgr.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#define VERIFICATION_APP_NUM_PATCHED_FUNCTIONS 2U   ///< No of functions to be patched (using HW comparators, max is 32)

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////
// This declaration should come from SystemMgrPatch.h if it was a patch in maintenance branch
//////////////////////////////////////////////////////////////////////////////////////////////
// Returns Patched ROM version
extern uint16_t SystemMgrGetRomVersionPatched(void);

// Returns modifed Vcore CMAC status register
extern uint32_t MacDrvGetCmacStatusPatched(void);

/// Returns CMAC status register value
extern uint32_t MacDrvGetCmacStatus(void);

extern const PatchMgrComparatorFunctionAddressT verificationAppComparatorTable[VERIFICATION_APP_NUM_PATCHED_FUNCTIONS];

extern uint32_t aescmac_status;

/// @} endgroup verificationapp
#endif  // !defined(SRC__VERIFICATIONAPP__INC__VERIFICATION_APP_INTERNAL_H)

