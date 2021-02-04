#if !defined(CSS_REGISTERS_H)
#define      CSS_REGISTERS_H

//***********************************************************************************
/// \file
///
/// CSS Register types and field definitions
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

/// CSS (I2C) register addresses
typedef enum
{
    CSS_COMMAND = 0xF000,       ///< Command register
    CSS_ERROR = 0xF008,         ///< Error register
    CSS_INFO = 0xF00A,          ///< Info register
    CSS_PAGE = 0xF00C,          ///< Page register
    CSS_PARAMS_0 = 0xF100       ///< First address of shared-memory page
} CssRegisterT;

// CSS register fields
#define CSS_COMMAND_RESULT__SIZE        15U
#define CSS_COMMAND_RESULT__OFFSET      0U
#define CSS_COMMAND_RESULT__MASK        (((1U << CSS_COMMAND_RESULT__SIZE) - 1U) \
                                        << CSS_COMMAND_RESULT__OFFSET)

#define CSS_COMMAND_DOORBELL__SIZE      1U
#define CSS_COMMAND_DOORBELL__OFFSET    15U
#define CSS_COMMAND_DOORBELL__MASK      (((1U << CSS_COMMAND_DOORBELL__SIZE) - 1U) \
                                        << CSS_COMMAND_DOORBELL__OFFSET)

#define CSS_ERROR_FAULTCONTEXT__SIZE    10U
#define CSS_ERROR_FAULTCONTEXT__OFFSET  0U
#define CSS_ERROR_FAULTCONTEXT__MASK    (((1U << CSS_ERROR_FAULTCONTEXT__SIZE) - 1U) \
                                        << CSS_ERROR_FAULTCONTEXT__OFFSET)

#define CSS_ERROR_FAULTCODE__SIZE       6U
#define CSS_ERROR_FAULTCODE__OFFSET     10U
#define CSS_ERROR_FAULTCODE__MASK       (((1U << CSS_ERROR_FAULTCODE__SIZE) - 1U) \
                                        << CSS_ERROR_FAULTCODE__OFFSET)

#endif // !defined(CSS_REGISTERS_H)
