//***********************************************************************************
/// \file
///
/// Asset Manager default Mocks
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
#include "AssetMgr.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_ASSET_MGR_RETRIEVE_CERTIFICATE)
    ErrorT AssetMgrRetrieveCertificate(AssetMgrCertificateAuthIdT const certAuthId,
        AssetMgrCertificatePurposeT const certPurpose,
        AssetMgrRsaPubCertT* const pubCert,
        uint32_t* const certSizeLongWords)
    {
        mock().actualCall("AssetMgrRetrieveCertificate")
            .withParameter("certAuthId", certAuthId)
            .withParameter("certPurpose", certPurpose)
            .withOutputParameter("pubCert", pubCert)
            .withOutputParameter("certSizeLongWords", certSizeLongWords);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
    ErrorT AssetMgrRetrieveCC312TrngChar(AssetMgrCC312TrngCharT* CC312TrngChar)
    {
        mock().actualCall("AssetMgrRetrieveCC312TrngChar")
            .withOutputParameter("CC312TrngChar", CC312TrngChar);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_RETRIEVE_OTPM_CONFIG)
    ErrorT AssetMgrRetrieveOtpmConfig(AssetMgrOtpmConfigT* OTPMConfig)
    {
        mock().actualCall("AssetMgrRetrieveOtpmConfig")
            .withOutputParameter("OTPMConfig", OTPMConfig);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_GET_PUBLIC_KEY_REFERENCE)
        ErrorT AssetMgrGetPublicKeyReference(AssetMgrRsaPubCertT* const pubCert,
            uint8_t** pubKey,
            uint32_t* const pubKeySizeBytes)
        {
            mock().actualCall("AssetMgrGetPublicKeyReference")
                .withParameter("pubCert", pubCert)
                .withOutputParameter("pubKey", pubKey)
                .withOutputParameter("pubKeySizeBytes", pubKeySizeBytes);

            return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
        }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
    ErrorT AssetMgrRetrievePrivateKey(AssetMgrCertificateAuthIdT const certAuthId,
        uint8_t* const privKey,
        uint32_t* const privKeySizeLongWords)
    {
        mock().actualCall("AssetMgrRetrievePrivateKey")
            .withParameter("certAuthId", certAuthId)
            .withOutputParameter("privKey", privKey)
            .withOutputParameter("privKeySizeLongWords", privKeySizeLongWords);

        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET)
    ErrorT AssetMgrRetrievePskMasterSecret(AssetMgrPskMasterSecretT* masterSecret,
        uint32_t* const masterSecretSizeLongWords)
    {
        mock().actualCall("AssetMgrRetrievePskMasterSecret")
            .withOutputParameter("masterSecret", masterSecret)
            .withOutputParameter("masterSecretSizeLongWords", masterSecretSizeLongWords);

        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_ASSET_MGR_ON_PROVISION_ASSET)
    ErrorT AssetMgrOnProvisionAsset(HostCommandCodeT const commandCode,
                                    CommandHandlerCommandParamsT params)
    {
        mock().actualCall("AssetMgrOnProvisionAsset")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

}
