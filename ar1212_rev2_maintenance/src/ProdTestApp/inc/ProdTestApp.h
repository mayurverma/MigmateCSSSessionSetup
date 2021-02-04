#if !defined(SRC__PRODTESTAPP__INC__PRODTESTAPP_H)
#define      SRC__PRODTESTAPP__INC__PRODTESTAPP_H

//***********************************************************************************
/// \file
/// Production Test Application public API
///
/// \defgroup prodtestapp   Production Test Application
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
//***********************************************************************************

#include "Common.h"
#include "CommandHandler.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// 16 bit int representing an OTPM error location and type (correctable or non-correctable)
typedef uint16_t ProdTestAppErrorT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Blank check the OTPM before programming
extern ErrorT ProdTestAppOnOtpmBlankCheck(HostCommandCodeT const command, CommandHandlerCommandParamsT params);

// Verify the OTPM post-programming
extern ErrorT ProdTestAppOnOtpmVerify(HostCommandCodeT const command, CommandHandlerCommandParamsT params);

// @} endgroup prodtestapp
#endif  //SRC__PRODTESTAPP__INC__PRODTESTAPP_H

