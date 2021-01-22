//***********************************************************************************
/// \file
///
/// LifecycleMgr default Mocks
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
#include "LifecycleMgr.h"
}

// Mocks
extern "C"
{
    ErrorT LifecycleMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("LifecycleMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT LifecycleMgrGetCurrentLcs(LifecycleMgrLifecycleStateT* lcsCurrent)
    {
        mock().actualCall("LifecycleMgrGetCurrentLcs")
           .withOutputParameter("lcsCurrent", lcsCurrent);

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT LifecycleMgrOnApplyDebugEntitlement(  HostCommandCodeT const commandCode,
                                                 CommandHandlerCommandParamsT params)
    {
        mock().actualCall("LifecycleMgrOnApplyDebugEntitlement")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT LifecycleMgrOnAuthorizeRMA(  HostCommandCodeT const commandCode,
                                        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("LifecycleMgrOnAuthorizeRMA")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
    ErrorT LifecycleMgrCc312Init(void)
    {
        mock().actualCall("LifecycleMgrCc312Init");

        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }

}
