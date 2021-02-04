//***********************************************************************************
/// \file
///
/// Command Handler default Mocks
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
#include "HealthMonitor.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_HEALTH_MONITOR_ON_PHASE_ENTRY)
    ErrorT HealthMonitorOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("HealthMonitorOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_HEALTH_MONITOR_ON_WATCHDOG_EVENT)
    ErrorT HealthMonitorOnWatchdogEvent(void)
    {
        mock().actualCall("HealthMonitorOnWatchdogEvent");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_HEALTH_MONITOR_ON_ENABLE_MONITORING)
    ErrorT HealthMonitorOnEnableMonitoring(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("HealthMonitorOnEnableMonitoring")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
}
