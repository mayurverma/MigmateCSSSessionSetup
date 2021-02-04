//***********************************************************************************
/// \file
///
/// HAL/PAL Configuration
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
#include "cc_pal_buff_attr.h"

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

/// Gets PAL data buffer attribute.
///
/// Function is supposed to get PAL data buffer attribute.
///
/// \param[in] pDataBuffer Address of the buffer to map. (not used)
/// \param[in] buffSize    Buffer size in bytes. (not used)
/// \param[in] buffType    Input for read / output for write. (not used)
/// \param[out] pBuffNs    HNONSEC buffer attribute (0 for secure, 1 for non-secure).
/// \returns CC_OK
///
CCError_t CC_PalDataBufferAttrGet(const unsigned char* pDataBuffer,
    size_t buffSize,
    uint8_t buffType,
    uint8_t* pBuffNs
    )
{
    CC_UNUSED_PARAM(pDataBuffer);
    CC_UNUSED_PARAM(buffSize);
    CC_UNUSED_PARAM(buffType);

    *pBuffNs = DATA_BUFFER_IS_SECURE;

    return CC_OK;
}
/// @} endgroup cc312halpal

