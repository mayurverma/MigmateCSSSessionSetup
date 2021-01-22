#if !defined(SRC__CC312HALPAL__INC__CC312HALPAL_H)
#define      SRC__CC312HALPAL__INC__CC312HALPAL_H

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
#include "AssetMgr.h"
#include "cc_pal_trng.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define CC_CONFIG_SAMPLE_CNT_ROSC_1     50U
#define CC_CONFIG_SAMPLE_CNT_ROSC_2     10U
#define CC_CONFIG_SAMPLE_CNT_ROSC_3     10U
#define CC_CONFIG_SAMPLE_CNT_ROSC_4     10U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// Represent internal control and state of HAL/PAL TRNG
typedef struct
{
    /// Make sure the TRNG parameters have been set.
    CCBool isPalTrngParamSet;
    /// TRNG parameters to be obtained from HW calibration. Boot-up code must copy these from NVM.
    CC_PalTrngParams_t cc312PalTrngParameter;
} CcPalTrngT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// This function copies the input TRNG user parameters to a local static structure
extern void CC_PalTrngParamSet(const AssetMgrCC312TrngCharT* pTrngParams);

/// @} endgroup cc312halpal
#endif  // !defined(SRC__CC312HALPAL__INC__CC312HALPAL_H)

