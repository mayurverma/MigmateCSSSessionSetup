#if !defined(I2C_DRIVER_H)
#define      I2C_DRIVER_H

//***********************************************************************************
/// \file
///
/// Stub I2C driver function declarations
///
//***********************************************************************************
// Copyright 2019 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "css_registers.h"

/// Writes an unsigned 16-bit value to the specified register
///
/// \param[in]  reg     16-bit address of register to write
/// \param[in]  value   16-bit value to write
///
/// \returns void
///
extern void I2cWriteReg16(CssRegisterT const reg, uint16_t const value);

/// Reads an unsigned 16-bit value from the specified register
///
/// \param[in]  reg     16-bit address of register to read
///
/// \returns 16-bit value read from register
///
extern uint16_t I2cReadReg16(CssRegisterT const reg);

#endif // !defined(I2C_DRIVER_H)
