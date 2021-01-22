//***********************************************************************************
/// \file
///
/// Patch for Provisioning Apps, the wrapper of OtpmDrv
///
/// \addtogroup cc312halpal
/// @{
//***********************************************************************************
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
//*************************************************************************************
#include "AR0820.h"
#include "CommandHandler.h"
#include "cc_error.h"
#include "Diag.h"
#include "OtpmDrv.h"
#include "ccProdOtpPatch.h"

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

/// Component state to save error information
ccProdOtpT ccProdOtp;

/// Get the last error in OTPM hardware register.
/// If an ECC error happened, the error would be logged in a register.
///
/// Note that OtpmDrvGetLastError() output errorAddress of byte address.
/// SdpuApp needs to return 32-bit word address. Therefore, we need /4 conversion.
///
/// Also note that OnSemi's ErrorT is casted to ARM's CC error type uint32_t as many more
/// error codes must be passed back to the caller of CCProd_Cmpu() and CCProd_Dmpu().
///
/// \returns Error status
/// \retval ERROR_SUCCESS               No errors to get the last error from CC312 hardware.
/// \retval ERROR_BADMSG                Uncorrectable double-bit error encountered in OTPM.
/// \retval ERROR_IO                    OTPM read/write error.
///
ErrorT CC_ProdOtpGetLastError(void)
{
    ErrorT error = ERROR_SUCCESS;
    OtpmDrvErrorT otpmDrvError;

    error = OtpmDrvGetLastError(&otpmDrvError);
    if (ERROR_SUCCESS == error)
    {
        if (true == otpmDrvError.isCorrectable)
        {
            ccProdOtp.numberSoftErrors++;
        }
        else
        {
            // If the ECC error is not correctable,
            ccProdOtp.errorAddress = (uint16_t)(otpmDrvError.errorAddress / sizeof(uint32_t));
            error = ERROR_BADMSG;
        }
    }
    else if (ERROR_NOENT == error)
    {
        // This is not an error condition. It just means no ECC errors were found.
        error = ERROR_SUCCESS;
    }
    else
    {
        // Simply returns otpmError from the OTPM driver.
    }

    return error;
}

/// Read a long word from OTPM.
///
/// This function protocol is to satisfy ARM's macro CC_PROD_OTP_READ(otpData, otpWordOffset, rc)
/// definition. The macro obtains one long word from OTPM.
///
/// \param[in]      otpLongWordOffset   OTPM offset address from its base address to read.
/// \param[out]     rc                  Error return code to indicate OK, CC_PROD_XXPU_READ_ERROR
///
/// \returns otpReadLongWord
/// \retval uint32_t                    The long word that was read out from the OTPM offset.
///
uint32_t CC_ProdOtpRead(uint32_t otpLongWordOffset, uint32_t* rc)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t readBuff[1];

    *rc = CC_OK;
    error = OtpmDrvRead(otpLongWordOffset, readBuff, 1U);

    if (ERROR_SUCCESS != error)
    {
        ccProdOtp.lastError = error;
        *rc = CC_PROD_XXPU_READ_ERROR;
    }

    return readBuff[0];
}

/// Write a long word to OTPM, read it back and verify the long word.
/// Also get the last error from hardware register and store in the global data structure
/// for reporting the results to the host command.
///
/// \param[in]      otpData             The long word to be written to OTPM.
/// \param[in]      otpLongWordOffset   OTPM offset address from its base address to write.
///
/// \returns Error status
/// \retval CC_OK                       No errors in read/write OTPM.
/// \retval CC_PROD_XXPU_WRITE_ERROR    Errors detetected when writing to OTPM
///
uint32_t CC_ProdOtpWrite(uint32_t otpData, uint32_t otpLongWordOffset)
{
    uint32_t rc = CC_OK;
    ErrorT error = ERROR_SUCCESS;
    uint32_t wordBuff[1];
    uint32_t readBuff[1];

    readBuff[0] = otpData;
    wordBuff[0] = otpData;

    error = OtpmDrvWrite(otpLongWordOffset, wordBuff, 1U);
    if (ERROR_SUCCESS != error)
    {
        ccProdOtp.errorAddress = (uint16_t)otpLongWordOffset;
    }
    else
    {
        error = OtpmDrvRead(otpLongWordOffset, readBuff, 1U);
        if (ERROR_SUCCESS == error)
        {
            error = CC_ProdOtpGetLastError();
            if (ERROR_SUCCESS == error)
            {
                if (readBuff[0] != wordBuff[0])
                {
                    ccProdOtp.errorAddress = (uint16_t)otpLongWordOffset;
                    error = ERROR_NOMSG;
                }
            }
        }
    }

    if (ERROR_SUCCESS != error)
    {
        ccProdOtp.lastError = error;
        rc = CC_PROD_XXPU_WRITE_ERROR;
    }

    return rc;
}

/// OTPM write and verify a number of long words.
///
/// \param[in]      otpLongWordOffset   OTPM offset address from its base address.
/// \param[in]      longWordBuff        Contents to be written to OTPM.
/// \param[in]      buffLongWordSize    Number of 32-bit words to be written to OTPM.
///
/// \returns Error status
/// \retval CC_OK                       No errors in read/write OTPM.
/// \retval CC_PROD_XXPU_WRITE_ERROR    Errors detetected when writing to OTPM
///
uint32_t CC_ProdOtpWriteVerifyWordBuff(uint32_t otpLongWordOffset, const uint32_t* longWordBuff,
    uint32_t const buffLongWordSize)
{
    uint32_t rc = CC_OK;
    uint32_t i;

    for (i = 0U; i < buffLongWordSize; i++)
    {
        if (0U != longWordBuff[i])  // Only write to OTPM if the word has non-zero bits.
        {
            rc = CC_ProdOtpWrite(longWordBuff[i], (otpLongWordOffset + i));
            if ((uint32_t)CC_OK != rc)
            {
                break;        // Drop out if an error happened.
            }
        }
    }

    return rc;
}

/// @} endgroup cc312halpal

