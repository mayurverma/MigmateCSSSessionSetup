//***********************************************************************************
/// \file
///
/// Crypto Manager unit test
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
#include "bsv_api.h"
#include "cc_asset_prov.h"
#include "cc_lib.h"
}


// Mocks
extern "C"
{
    CCError_t CC_BsvSocIDCompute(unsigned long hwBaseAddress,      CCHashResult_t hashResult)
    {
        mock().actualCall("CC_BsvSocIDCompute")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withOutputParameter("hashResult", hashResult);
        return (CCError_t)mock().returnLongIntValueOrDefault(CC_OK);
    }

    CCError_t CC_BsvIcvAssetProvisioningOpen(unsigned long hwBaseAddress,
                         uint32_t         assetId,
                         uint32_t         *pAssetPkgBuff,
                         size_t           assetPackageLen,
                         uint8_t          *pOutAssetData,
                         size_t           *pAssetDataLen)
    {
        mock().actualCall("CC_BsvIcvAssetProvisioningOpen")
            .withParameter("hwBaseAddress", hwBaseAddress)
            .withParameter("assetId", assetId)
            .withParameter("pAssetPkgBuff", pAssetPkgBuff)
            .withParameter("assetPackageLen", assetPackageLen)
            .withOutputParameter("pOutAssetData", pOutAssetData)
            .withOutputParameter("pAssetDataLen", pAssetDataLen);

        return (CCError_t)mock().returnUnsignedIntValueOrDefault((CCError_t)CC_OK);
    }

    CClibRetCode_t CC_LibInit(CCRndContext_t *rndContext_ptr,    CCRndWorkBuff_t  *rndWorkBuff_ptr)
    {
        (void)rndContext_ptr;
        (void)rndWorkBuff_ptr;
        mock().actualCall("CC_LibInit");

        return (CClibRetCode_t)mock().returnUnsignedIntValueOrDefault(CC_LIB_RET_OK);
    }

    CClibRetCode_t CC_LibFini(CCRndContext_t *rndContext_ptr)
    {
        (void)rndContext_ptr;
        mock().actualCall("CC_LibFini");

        return (CClibRetCode_t)mock().returnUnsignedIntValueOrDefault(CC_LIB_RET_OK);
    }
}

