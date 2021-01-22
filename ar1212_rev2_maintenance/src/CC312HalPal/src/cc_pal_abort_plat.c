//***********************************************************************************
/// \file
///
/// HAL/PAL Abort Handler
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
#include "cc_pal_log.h"
#include "cc_pal_abort.h"
#include "stdlib.h"
#include "CryptoMgr.h"

/// This function performs the "Abort" operation.
///
/// \param[in] expression       (not used)
/// \returns void
///
void CC_PalAbort(const char* expression)
{
    CC_PAL_LOG_ERR("ASSERT:%s:%d: %s", __FILE__, __LINE__, expression);
    CC_UNUSED_PARAM(expression);   // To avoid compilation error in case DEBUG isn't defined.
    CryptoMgrOnPalAbort();
}
/// @} endgroup cc312halpal

