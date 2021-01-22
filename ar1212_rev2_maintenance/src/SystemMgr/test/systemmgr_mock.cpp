//***********************************************************************************
/// \file
///
/// System Manager default Mocks
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
#include "SystemMgr.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_SYSTEM_MGR_GET_CLOCK_SPEED)
    ErrorT SystemMgrGetClockSpeed(uint32_t* clockSpeedHz)
    {
        mock().actualCall("SystemMgrGetClockSpeed")
            .withOutputParameter("clockSpeedHz", clockSpeedHz);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_SET_CLOCK_SPEED)
    ErrorT SystemMgrSetClockSpeed(uint32_t clockSpeedHz)
    {
        mock().actualCall("SystemMgrSetClockSpeed")
            .withParameter("clockSpeedHz", clockSpeedHz);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_REPORT_CRYPTO_FAULT)
    void SystemMgrReportCryptoFault(SystemMgrCssFaultT const fault,
        uint32_t const context,
        uint32_t const infoParam)
    {
        mock().actualCall("SystemMgrReportCryptoFault")
            .withParameter("fault", fault)
            .withParameter("context", context)
            .withParameter("infoParam", infoParam);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_REPORT_FATAL_ERROR)
    void SystemMgrReportFatalError(SystemMgrFatalErrorT const context,
        uint32_t const infoParam)
    {
        mock().actualCall("SystemMgrReportFatalError")
            .withParameter("context", context)
            .withParameter("infoParam", infoParam);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_GET_ROM_VERSION)
    uint16_t SystemMgrGetRomVersion(void)
    {
        mock().actualCall("SystemMgrGetRomVersion");
        return (uint16_t)mock().returnUnsignedIntValueOrDefault(0x1234);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_REGISTER_CLOCK_SPEED_CHANGE_NOTIFIER)
    ErrorT SystemMgrRegisterClockSpeedChangeNotifier(SystemMgrClockSpeedChangeNotifierT notifier)
    {
        mock().actualCall("SystemMgrRegisterClockSpeedChangeNotifier")
            .withFunctionPointerParameter("notifier", (void (*)(void))notifier);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_SET_EVENT)
    void SystemMgrSetEvent(SystemMgrEventT const event)
    {
        mock().actualCall("SystemMgrSetEvent")
            .withParameter("event", event);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_GET_CURRENT_PHASE)
    SystemMgrPhaseT SystemMgrGetCurrentPhase(void)
    {
        mock().actualCall("SystemMgrGetCurrentPhase");
        return (SystemMgrPhaseT)mock().returnLongIntValueOrDefault(SYSTEM_MGR_PHASE_INITIALIZE);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_MGR_ENABLE_PATCH_REGION)
    void SystemMgrEnablePatchRegion(bool const enable)
    {
        mock().actualCall("SystemMgrEnablePatchRegion")
            .withParameter("enable", enable);

        printf("\nSystemMgrEnablePatchRegion(%d)", enable);
    }
#endif
}
