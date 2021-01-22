//***********************************************************************************
/// \file
///
/// HAL/PAL MUTEX Handling
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

#include "cc_pal_mutex.h"

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
/// This function purpose is to Wait for Mutex with a timeout. aTimeOut is
/// specified in milliseconds. (CC_INFINITE is blocking)
///
/// \param[in] pMutexId     pointer to Mutex handle (not used)
/// \param[in] timeOut      timeout in mSec, or CC_INFINITE (not used)
/// \returns Command success/failure
/// \retval CC_OK                            Success (OK) definition.
///
CCError_t CC_PalMutexLock(CC_PalMutex* pMutexId, uint32_t timeOut)
{
    CC_UNUSED_PARAM(pMutexId);
    CC_UNUSED_PARAM(timeOut);

    return 0;
}

///
/// This function purpose is to release the mutex.
///
/// \param[in] pMutexId     pointer to Mutex handle (not used)
/// \returns Command success/failure
/// \retval CC_OK                            Success (OK) definition.
///
CCError_t CC_PalMutexUnlock(CC_PalMutex* pMutexId)
{
    CC_UNUSED_PARAM(pMutexId);

    return 0;
}
/// @} endgroup cc312halpal

