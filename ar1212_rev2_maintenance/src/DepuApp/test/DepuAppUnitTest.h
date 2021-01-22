#if !defined(SRC__DEPUAPP__TEST__DEPUAPPUNITTEST_H)
#define      SRC__DEPUAPP__TEST__DEPUAPPUNITTEST_H
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

extern MCU_SYSCTRL_REGS_T sysCtrlRegsUnitTest;
extern uint16_t depuRomContentsUnitTest[0x7FFF];
//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#undef MCU_SYSCTRL_REGS
#define MCU_SYSCTRL_REGS               &sysCtrlRegsUnitTest

#undef CRYPTO_CCORE_ROM_BASE
#define CRYPTO_CCORE_ROM_BASE               &depuRomContentsUnitTest;


//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
#endif  // !defined(SRC__DEPUAPP__TEST__DEPUAPPUNITTEST_H)

