#if !defined(SRC__OTPMDRV__TEST__OTPMDRVUNITTEST_H)
#define      SRC__OTPMDRV__TEST__OTPMDRVUNITTEST_H
//******************************************************************************
/// \file
///
/// Provides definitions needed to support Mingw32 based unit testing of the OTPM driver
///
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "Common.h"
#include "OtpmDrv.h"

#undef OTPM_BASE
#undef OTPM_REGS
#undef CRYPTO_OTPM_REG_BASE
#undef CRYPTO_OTPM_DATA

extern uint32_t otpmArray[];
//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define OTPM_BASE               ((uint32_t*)            &otpmArray[0])
#define OTPM_REGS               ((CRYPTO_OTPM_REGS_T*)  &otpmArray[2048 - 32])
#define CRYPTO_OTPM_REG_BASE    ((uint32_t)&otpmArray[2048 - 32])
#define CRYPTO_OTPM_DATA        ((uint32_t)&otpmArray[0])
//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
#endif  // !defined(SRC__OTPMDRV__TEST__OTPMDRVUNITTEST_H)

