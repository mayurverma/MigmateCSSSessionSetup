//***********************************************************************************
/// \file
///
/// HAL/PAL Power management
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

#include "cc_pal_types.h"
#include "cc_pal_pm.h"

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

/// Initialises the power save mode feature of the CryptoCell
void CC_PalPowerSaveModeInit(void)
{
}

/// Returns the power save mode of the CryptoCell
///
/// \returns Command Success/Failure
/// \retval CC_OK
///
int32_t CC_PalPowerSaveModeStatus(void)
{
    return 0;
}

/// This function updates the atomic counter on each call to CryptoCell
///
/// On each call to CryptoCell, the counter is increased. At the end of each operation
/// the counter is decreased. Once the counter is zero, an external callback is called.
/// \param[in] isPowerSaveMode    Update value (not used)
/// \returns Command Success/Failure
/// \retval CC_OK
///
CCError_t CC_PalPowerSaveModeSelect(CCBool isPowerSaveMode)
{
    CC_UNUSED_PARAM(isPowerSaveMode);

    return 0;
}

/// @} endgroup cc312halpal

