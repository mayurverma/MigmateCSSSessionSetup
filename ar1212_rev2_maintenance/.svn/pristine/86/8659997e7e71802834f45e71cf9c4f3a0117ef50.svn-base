//***********************************************************************************
/// \file
///
/// SDPU Application default Mocks
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
#include "cc_cmpu.h"
#include "cc_dmpu.h"
#include "cc_pal_types.h"
}

// Mocks
extern "C"
{
    CCError_t CCProd_Cmpu(unsigned long ccHwRegBaseAddr,
                         CCCmpuData_t     *pCmpuData,
                         unsigned long    workspaceBaseAddr,
                         uint32_t         workspaceSize)
    {
        mock().actualCall("CCProd_Cmpu")
            .withParameter("ccHwRegBaseAddr", ccHwRegBaseAddr)
            .withParameter("pCmpuData", pCmpuData)
            .withParameter("workspaceBaseAddr", (uint8_t*)workspaceBaseAddr)
            .withParameter("workspaceSize", workspaceSize);

        return (CCError_t)mock().returnUnsignedIntValueOrDefault((CCError_t)CC_OK);
    }

    CCError_t CCProd_Dmpu(unsigned long ccHwRegBaseAddr,
                         CCDmpuData_t     *pDmpuData,
                         unsigned long    workspaceBaseAddr,
                         uint32_t         workspaceSize)
    {
        mock().actualCall("CCProd_Dmpu")
            .withParameter("ccHwRegBaseAddr", ccHwRegBaseAddr)
            .withParameter("pDmpuData", pDmpuData)
            .withParameter("workspaceBaseAddr", workspaceBaseAddr)
            .withParameter("workspaceSize", workspaceSize);

        return (CCError_t)mock().returnUnsignedIntValueOrDefault((CCError_t)CC_OK);
    }
}
