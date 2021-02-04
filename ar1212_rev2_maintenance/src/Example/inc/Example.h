#if !defined(SRC__EXAMPLE__INC__EXAMPLE_H)
#define      SRC__EXAMPLE__INC__EXAMPLE_H

//***********************************************************************************
/// \file
/// Example component (boilerplate public header file).
///
/// \defgroup example   Example component
///
/// This is an example component (called 'Example') intended to provide the boilerplate
/// for real components. The example supports CppUTest, Doxygen and Eclipse.
///
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

#include <stdint.h>

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern int32_t Example_Init(int32_t Value);
extern int32_t Example_Shutdown(void);

// @} endgroup example
#endif  // !defined(SRC__EXAMPLE__INC__EXAMPLE_H)

