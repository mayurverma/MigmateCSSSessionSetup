//******************************************************************************
/// \file
///
/// File used for unit testing, returns default value unless overridden by a mock
///
/// \addtogroup prodtestapp
/// @{
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

#include "ProdTestAppInternal.h"
#define MAX_ERROR_ARRAY_LENGTH_LONG_WORDS 2044U;

uint32_t ProdTestAppGetMaxErrorArrayLengthLongWords(void)
{
    uint32_t maxError = MAX_ERROR_ARRAY_LENGTH_LONG_WORDS;

    return maxError;
}

// @} endgroup prodtestapp

