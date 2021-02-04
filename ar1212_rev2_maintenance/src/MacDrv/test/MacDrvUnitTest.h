#if !defined(SRC__MACDRV__TEST__MACDRVUNITTEST_H)
#define      SRC__MACDRV__TEST__MACDRVUNITTEST_H
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

extern CRYPTO_VCORE_REGS_T vcoreRegsUnitTest;


//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#undef CRYPTO_VCORE_REGS
#define CRYPTO_VCORE_REGS           (&vcoreRegsUnitTest)


//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

#endif  // !defined(SRC__MACDRV__TEST__MACDRVUNITTEST_H)

