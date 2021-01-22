//***********************************************************************************
/// \file
///
/// Watchdog Driver default Mocks
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
#include "AR0820.h"
#include "WatchdogDrv.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_WATCHDOG_DRV_INIT)
    // Initialize the Watchdog Driver
    ErrorT WatchdogDrvInit(void)
    {
        mock().actualCall("WatchdogDrvInit");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_WATCHDOG_DRV_START)
    // Configure and start the watchdog
    ErrorT WatchdogDrvStart(uint32_t const timeOutClocks)
    {
        mock().actualCall("WatchdogDrvStart")
            .withParameter("timeOutClocks", timeOutClocks);
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_WATCHDOG_DRV_CLEAR_INTERRUPT)
    // Clear the watchdog interrupt ('pat' the watchdog)
    ErrorT WatchdogDrvClearInterrupt(void)
    {
        mock().actualCall("WatchdogDrvClearInterrupt");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_WATCHDOG_DRV_DISABLE)
    // Disable the watchdog
    ErrorT WatchdogDrvDisable(void)
    {
        mock().actualCall("WatchdogDrvDisable");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

}
