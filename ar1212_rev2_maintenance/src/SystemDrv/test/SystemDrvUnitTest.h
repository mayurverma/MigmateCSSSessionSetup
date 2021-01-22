#if !defined(SRC__SYSTEMDRV__TEST__SYSTEMDRVUNITTEST_H)
#define      SRC__SYSTEMDRV__TEST__SYSTEMDRVUNITTEST_H
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

extern CRYPTO_ICORE_REGS_T icoreRegsUnitTest;
extern MCU_SYSCTRL_REGS_T mcuSysctrlRegsUnitTest;


//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#undef CRYPTO_ICORE_REGS
#define CRYPTO_ICORE_REGS           (&icoreRegsUnitTest)

#undef MCU_SYSCTRL_REGS
#define MCU_SYSCTRL_REGS            (&mcuSysctrlRegsUnitTest)

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

#endif  // !defined(SRC__SYSTEMDRV__TEST__SYSTEMDRVUNITTEST_H)

