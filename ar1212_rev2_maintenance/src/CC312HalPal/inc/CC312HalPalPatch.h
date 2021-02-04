#if !defined(SRC__CC312HALPAL__INC__CC312HALPALPATCH_H)
#define      SRC__CC312HALPAL__INC__CC312HALPALPATCH_H

//***********************************************************************************
/// \file
/// CC312 HAL/PAL public API
///
/// \defgroup cc312halpal   CC312 HAL PAL Driver
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

#include "Common.h"
#include "CC312HalPal.h"
#include "cc_pal_types.h"
#include "cc_pal_trng.h"

#include "AssetMgr.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// 4 TRNG parameters that are obtained from ring-oscillator calibration.
extern CcPalTrngT ccPalTrng;

// This function copies the input TRNG user parameters to a local static structure
extern void CC_PalTrngParamSetPatched(const AssetMgrCC312TrngCharT* pTrngParams);

/// @} endgroup cc312halpal
#endif  // !defined(SRC__CC312HALPAL__INC__CC312HALPALPATCH_H)

