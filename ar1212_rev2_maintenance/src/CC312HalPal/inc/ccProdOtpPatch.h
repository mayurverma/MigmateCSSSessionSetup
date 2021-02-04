#if !defined(SRC__CC312HALPAL__INC__CCPRODOTPPATCH_H)
#define      SRC__CC312HALPAL__INC__CCPRODOTPPATCH_H

//***********************************************************************************
/// \file
/// CC312 HAL/PAL public API for Provisioning apps
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

#include "Common.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

/// Choose an error base address that is free in CC312 lib for SDPU/CMPU/DMPU use - 0x00F40000
#define CC_PROD_XXPU_MODULE_ERROR_BASE  0x00F40000U
/// Obtain the error base code to check the group of errors.
#define CC_PROD_ERROR_BASE_MASK         0xFFFF0000U

#define CC_PROD_XXPU_READ_ERROR   (CC_PROD_XXPU_MODULE_ERROR_BASE + 1U)  /// Error occurred in CC_ProdOtpRead
#define CC_PROD_XXPU_WRITE_ERROR  (CC_PROD_XXPU_MODULE_ERROR_BASE + 2U)  /// Error occurred in CC_ProdOtpWrite

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents the error information when accessing the OTPM
typedef struct
{
    uint16_t errorAddress;              ///< The error address in OTPM (long word aligned)
    uint16_t numberSoftErrors;          ///< The number of soft errors.
    /// Last error recorded:
    /// ERROR_RANGE         Parameter address (offset) is outside permitted size
    /// ERROR_ACCESS        OTPM has not been configured so writes are not allowed
    /// ERROR_NOMSG         OTPM write has not been succesful as the read value is not the expected
    /// ERROR_IO            OTPM location not blank.
    ErrorT lastError;
} ccProdOtpT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern ccProdOtpT ccProdOtp;

// Get the last error in OTPM hardware register.
extern ErrorT CC_ProdOtpGetLastError(void);

// OTPM write and verify a number of long words.
uint32_t CC_ProdOtpWriteVerifyWordBuff(uint32_t otpLongWordOffset, const uint32_t* longWordBuff,
    uint32_t const buffLongWordSize);

// Read a long word from OTPM.
extern uint32_t CC_ProdOtpRead(uint32_t otpLongWordOffset, uint32_t* rc);

// Write a long word to OTPM.
extern uint32_t CC_ProdOtpWrite(uint32_t otpData, uint32_t otpLongWordOffset);

/// @} endgroup cc312halpal
#endif  // !defined(SRC__CC312HALPAL__INC__CCPRODOTPPATCH_H)

