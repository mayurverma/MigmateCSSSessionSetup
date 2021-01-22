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
#include <string.h>
#include "cc_pal_types.h"
#include "cc_pal_log.h"
#include "CC312HalPal.h"
#include "SystemMgr.h"
#include "AssetMgr.h"
#include "Diag.h"
//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

CcPalTrngT ccPalTrng =
{
    // Initialized here as CC_LibInit() calls CC_HalInit() during Configuration Phase
    // That is inconvenient if a patch calls CC_PalTrngParamSet()
    .isPalTrngParamSet = CC_FALSE
};

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// This function copies the input TRNG user parameters to a local static structure.
///
/// \param[in] pTrngParams    A pointer to the TRNG user parameters
/// \returns void
///
void CC_PalTrngParamSet(const AssetMgrCC312TrngCharT* pTrngParams)
{
    // Use isPalTrngParamSet flag to prevent the parameters being set more than once.
    // Because the TRNG or SDPU application patches might neet to override the TRNG char defaults
    // They can do it in the loader and the CryptoManager won't overwrite them during Configuration.
    if (CC_FALSE == ccPalTrng.isPalTrngParamSet)
    {
        ccPalTrng.isPalTrngParamSet = CC_TRUE;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio1 = pTrngParams->R0;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio2 = pTrngParams->R1;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio3 = pTrngParams->R2;
        ccPalTrng.cc312PalTrngParameter.SubSamplingRatio4 = pTrngParams->R3;
    }
}

/// This function return the TRNG user parameters.
///
/// \param[in] pTrngParams        A pointer to the TRNG user parameters.
/// \param[in,out] pParamsSize    A pointer to size of the TRNG user parameters structure used.
///                               As input: the function needs to verify its size is the same as CC_PalTrngParams_t.
///                               As output: return the size of CC_PalTrngParams_t for Library size verification.
/// \returns Success/Failure
/// \retval  CC_OK            success
/// \retval  CC_FAIL          failure
///
CCError_t CC_PalTrngParamGet(CC_PalTrngParams_t* pTrngParams, size_t* pParamsSize)
{
    CCError_t error = CC_OK;

    if ((pTrngParams == NULL) ||
        (pParamsSize == NULL) ||
        (*pParamsSize != sizeof(CC_PalTrngParams_t)))
    {
        error = (CCError_t)CC_FAIL;
    }
    else
    {
        *pParamsSize = sizeof(CC_PalTrngParams_t);

        if (CC_FALSE == ccPalTrng.isPalTrngParamSet)
        {
            SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_CC312_PAL_TRNG_PARAM_NOT_SET, (uint32_t)ERROR_SYSERR);
            error = (CCError_t)CC_FAIL;
        }
        else
        {
            pTrngParams->SubSamplingRatio1 = ccPalTrng.cc312PalTrngParameter.SubSamplingRatio1;
            pTrngParams->SubSamplingRatio2 = ccPalTrng.cc312PalTrngParameter.SubSamplingRatio2;
            pTrngParams->SubSamplingRatio3 = ccPalTrng.cc312PalTrngParameter.SubSamplingRatio3;
            pTrngParams->SubSamplingRatio4 = ccPalTrng.cc312PalTrngParameter.SubSamplingRatio4;
        }
    }

    return error;
}
/// @} endgroup cc312halpal

