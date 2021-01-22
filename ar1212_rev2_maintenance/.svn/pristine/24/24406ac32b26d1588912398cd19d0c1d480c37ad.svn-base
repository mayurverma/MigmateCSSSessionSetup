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
#include "PatchMgr.h"
}

// Mocks
extern "C"
{
    ErrorT PatchMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("PatchMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    PatchMgrPatchVersionT PatchMgrGetPatchVersion(void)
    {
        mock().actualCall("PatchMgrGetPatchVersion");
        return (PatchMgrPatchVersionT)mock().returnUnsignedIntValueOrDefault(0U);
    }
}
