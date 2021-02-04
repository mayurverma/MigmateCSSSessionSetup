//***********************************************************************************
/// \file
///
/// Platform default Mocks
//
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

#include "cpputest_support.h"

extern "C"
{
#include "Common.h"
#include "Platform.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_PLATFORM_INIT)
    ErrorT PlatformInit(void)
    {
        mock().actualCall("PlatformInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_PLATFORM_ENTER_CRITICAL)
    void PlatformEnterCritical(void)
    {
        mock().actualCall("PlatformEnterCritical");
    }
#endif
#if !defined(MOCK_DISABLE_PLATFORM_EXIT_CRITICAL)
    void PlatformExitCritical(void)
    {
        mock().actualCall("PlatformExitCritical");
    }
#endif
}
