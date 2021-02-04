//***********************************************************************************
/// \file
///
/// HAL/PAL TRNG support
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

#include "CC312HalPal.h"
#include "CC312HalPalPatch.h"

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

/// Use this function to replace the patched TRNG function in ROM.
///
/// The original TRNG parameters working for real chips are: 5000, 1000, 500, 0.
/// The parameters in OnSemi's ROM and code below are for RTL simulation only.
/// 4 parameters needs to be tuned in order to generate random numbers.
///
/// \param[in] pTrngParams    A pointer to the TRNG user parameters
///
/// \returns void
///
void CC_PalTrngParamSetPatched(const AssetMgrCC312TrngCharT* pTrngParams)
{
    (void)pTrngParams;

    if (CC_FALSE == ccPalTrng.isPalTrngParamSet)
    {
        ccPalTrng.isPalTrngParamSet = CC_TRUE;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio1 = 5000;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio2 = 1000;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio3 = 500;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio4 = 0;
    }
}

/// @} endgroup cc312halpal

