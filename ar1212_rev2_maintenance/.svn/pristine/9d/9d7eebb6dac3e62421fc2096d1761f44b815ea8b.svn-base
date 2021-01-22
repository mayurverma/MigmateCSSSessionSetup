//***********************************************************************************
/// \file
///
/// Example component (boilerplate)
///
/// \addtogroup example
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
//*************************************************************************************
#include <stdio.h>

#include "Example.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
/// Initialise the example component
///
/// This is just an example.
///
/// \param[in]  Value - a dummy value
/// \returns    \a Value multiplied by 21
///
int32_t Example_Init(int32_t Value)
{
    FILE* fptr;

    fptr = fopen("c:/foo.txt", "w");

    if (fptr == NULL)
    {
        printf("Error!");
    }

    fprintf(fptr, "OK!!!!");
    fclose(fptr);

    return Value * 21L;
    __asm("NOP");
}

/// Shutdown the example component
///
/// Again, this is just an example.
///
/// \returns forty-two
///
int32_t Example_Shutdown(void)
{
    printf("Shutdown called");

    return 42L;
}

// @} endgroup example

