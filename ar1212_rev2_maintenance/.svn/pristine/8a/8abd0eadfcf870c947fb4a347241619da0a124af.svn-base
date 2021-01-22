//***********************************************************************************
/// \file
///
/// HAL/PAL OPTM access
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

/************* Include Files ****************/

#include "cc_pal_types.h"
#include "cc_pal_types_plat.h"
#include "bsv_error.h"
#include "bsv_otp_api.h"
#include "Common.h"
#include "OtpmDrv.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
// Converts AR0820 error codes (ErrorT) to CC312 error codes (CCError_t)
CCError_t CC_BsvErrorConversion(ErrorT error);

/// Converts AR0820 error codes (ErrorT) to CC312 error codes (CCError_t)
///
/// \returns CC312 error codes
/// \retval CC_OK                            Success (OK) definition.
/// \retval CC_BSV_ILLEGAL_INPUT_PARAM_ERR   Illegal input parameter error.
/// \retval CC_BSV_BASE_ERROR                Defines BSV base error.
/// \retval CC_BSV_OTP_WRITE_CMP_FAIL_ERR    OTP write compare failure error
///
CCError_t CC_BsvErrorConversion(ErrorT error)
{
    CCError_t retError = CC_OK;

    switch (error)
    {
        case ERROR_INVAL:
            retError = CC_BSV_ILLEGAL_INPUT_PARAM_ERR;
            break;

        case ERROR_RANGE:
            retError = CC_BSV_BASE_ERROR;
            break;

        case ERROR_NOENT:
            retError = CC_BSV_OTP_WRITE_CMP_FAIL_ERR;
            break;

        case ERROR_SUCCESS:
            retError = CC_OK;
            break;

        default:
            retError = CC_BSV_BASE_ERROR;
            break;
    }

    return retError;
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Read Long Word from OTPM array
///
/// \param[in] hwBaseAddress    Base address of hardware (not used)
/// \param[in] otpAddress       OTPM address to read
/// \param[out] pOtpWord        Pointer to location to be used to store read value
/// \returns Command success/failure
///
CCError_t CC_BsvOTPWordRead(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t* pOtpWord)
{
    ErrorT result = ERROR_SUCCESS;
    CCError_t retError = CC_OK;
    uint32_t regVal = 0U;

    (void)hwBaseAddress;

    /* read OTP word */
    result = OtpmDrvRead(otpAddress, &regVal, 1U);
    *pOtpWord = regVal;
    retError = CC_BsvErrorConversion(result);

    return retError;
}

/// Write Long Word to OTPM array
///
/// \param[in] hwBaseAddress    Base address of hardware (not used)
/// \param[in] otpAddress       OTPM address for write
/// \param[in] otpWord          Value to write
/// \returns Command success/failure
/// \retval CC_OK                            Success (OK) definition.
/// \retval CC_BSV_OTP_WRITE_CMP_FAIL_ERR    OTP write compare failure error
///
CCError_t CC_BsvOTPWordWrite(unsigned long hwBaseAddress, uint32_t otpAddress, uint32_t otpWord)
{
    ErrorT result = ERROR_SUCCESS;
    CCError_t retError = CC_OK;
    uint32_t otpVal = 0U;

    (void)hwBaseAddress;

    /* write new word (a write request to the NVM is translated to read-modify by HW) */
    result = OtpmDrvWrite(otpAddress, &otpWord, 1U);
    retError = CC_BsvErrorConversion(result);

    if ((CCError_t)CC_OK == retError)
    {
        // read new word and verify operation
        result = OtpmDrvRead(otpAddress, &otpVal, 1U);
        retError = CC_BsvErrorConversion(result);

        if (otpVal != otpWord)
        {
            retError = CC_BSV_OTP_WRITE_CMP_FAIL_ERR;
        }
    }

    return retError;
}
/// @} endgroup cc312halpal

