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
#include "CommandHandler.h"
#include "AR0820.h"
#include "Diag.h"
#include "NvmMgr.h"
#include "CryptoMgr.h"
#include "LifecycleMgr.h"
#include "AssetMgr.h"
#include "AssetMgrInternal.h"

}

#define member_size(type, member) sizeof(((type *)0)->member)


TEST_GROUP(AssetMgrGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(AssetMgrGroup, TestGetPublicKeyBasic)
{
    AssetMgrRsa2048PubCertT pubCert;
    uint8_t* pubKey;
    uint32_t pubKeySizeBytes;

    pubCert.header.typeIdentifier  = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    pubCert.header.signatureOffsetLongWords = (sizeof(pubCert.publicKey) + sizeof(pubCert.header)) / 4;
    pubCert.header.certificateFlags = 0;

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrGetPublicKeyReference((AssetMgrRsaPubCertT *)&pubCert, &pubKey, &pubKeySizeBytes));

    CHECK_EQUAL(pubCert.publicKey, pubKey);
    CHECK_EQUAL(ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES, pubKeySizeBytes)
}

TEST(AssetMgrGroup, TestGetPublicKeyError)
{
    AssetMgrRsa2048PubCertT pubCert;
    uint8_t* pubKey;
    uint32_t pubKeySizeBytes;

    pubCert.header.typeIdentifier  = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    pubCert.header.signatureOffsetLongWords = (sizeof(pubCert.publicKey) + sizeof(pubCert.header)) / 4;
    pubCert.header.certificateFlags = 0;

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrGetPublicKeyReference(NULL, &pubKey, &pubKeySizeBytes));

    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrGetPublicKeyReference((AssetMgrRsaPubCertT *)&pubCert, NULL, &pubKeySizeBytes));

    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrGetPublicKeyReference((AssetMgrRsaPubCertT *)&pubCert, &pubKey, NULL));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY)
                .withParameter("info", ERROR_RANGE);

    // Force error
    pubCert.header.signatureOffsetLongWords = (uint16_t)(pubCert.header.signatureOffsetLongWords + (uint16_t)1U);
    CHECK_EQUAL(ERROR_RANGE, AssetMgrGetPublicKeyReference((AssetMgrRsaPubCertT *)&pubCert, &pubKey, &pubKeySizeBytes));
}

TEST(AssetMgrGroup, TestRetrieveCC312TrngCharBasic)
{
    AssetMgrCC312TrngCharT CC312TrngChar;

    uint32_t assetLengthLongWords = sizeof(CC312TrngChar) / sizeof(uint32_t);

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

}


TEST(AssetMgrGroup, TestRetrieveCC312TrngCharError)
{
    AssetMgrCC312TrngCharT CC312TrngChar;
    uint32_t assetLengthLongWords = sizeof(CC312TrngChar) / sizeof(uint32_t);

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrRetrieveCC312TrngChar(NULL));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_IO)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_IO)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

    //Force size error
    assetLengthLongWords =  0xff;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrRetrieveCC312TrngChar(&CC312TrngChar));

}

TEST(AssetMgrGroup, TestRetrieveCertificateBasic)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrCertificatePurposeT certPurpose = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    AssetMgrRsa2048PubCertT pubCert;
    uint32_t certSizeLongWords = sizeof(AssetMgrRsa2048PubCertT);
    uint32_t certificateFlags = 0;

    certificateFlags += (uint32_t)certAuthId << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET;
    certificateFlags += (uint32_t)certPurpose << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT header = {};
    header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    header.signatureOffsetLongWords = (ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES + sizeof(AssetMgrRsaPubCertHeaderT)) / sizeof(uint32_t);
    header.certificateFlags = certificateFlags;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", sizeof(AssetMgrRsa2048PubCertT))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWords));

}

TEST(AssetMgrGroup, TestRetrieveCertificateFindNext)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrCertificatePurposeT certPurpose = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    AssetMgrRsa2048PubCertT pubCert;
    uint32_t certSizeLongWords = sizeof(AssetMgrRsa2048PubCertT);
    uint32_t certificateFlags = 0;

    AssetMgrRsaPubCertHeaderT header = {};
    header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    header.signatureOffsetLongWords = (ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES + sizeof(AssetMgrRsaPubCertHeaderT)) / sizeof(uint32_t);
    header.certificateFlags = certificateFlags;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    certificateFlags += (uint32_t)certAuthId << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET;
    certificateFlags += (uint32_t)certPurpose << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET;
    header.certificateFlags = certificateFlags;

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWords));

}

void TestRetrieveCertificateWrongFields(AssetMgrCertificateAuthIdT certAuthId,
                                                  AssetMgrCertificatePurposeT certPurpose,
                                                  AssetMgrCertificateAuthIdT certAuthIdRead,
                                                  AssetMgrCertificatePurposeT certPurposeRead)
{
    AssetMgrRsa2048PubCertT pubCert;
    uint32_t certSizeLongWords = sizeof(AssetMgrRsa2048PubCertT);
    uint32_t certificateFlags = 0;

    certificateFlags += (uint32_t)certAuthIdRead << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET;
    certificateFlags += (uint32_t)certPurposeRead << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET;


    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT header = {};
    header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    header.signatureOffsetLongWords = (ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES + sizeof(AssetMgrRsaPubCertHeaderT)) / sizeof(uint32_t);
    header.certificateFlags = certificateFlags;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWords));

}

TEST(AssetMgrGroup, TestRetrieveCertificateNotFound)
{

    TestRetrieveCertificateWrongFields(ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A, ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR,
                                       ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A, ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR);

    TestRetrieveCertificateWrongFields(ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A, ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR,
                                       ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B, ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR);

    TestRetrieveCertificateWrongFields(ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A, ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR,
                                       ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B, ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR);

}

TEST(AssetMgrGroup, TestRetrieveCertificateError)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrCertificatePurposeT certPurpose = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    AssetMgrRsa2048PubCertT pubCert;
    uint32_t certSizeLongWords = sizeof(AssetMgrRsa2048PubCertT);
    uint32_t certificateFlags = 0;

    certificateFlags += (uint32_t)certAuthId << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET;
    certificateFlags += (uint32_t)certPurpose << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET;

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrRetrieveCertificate(certAuthId, certPurpose, NULL, NULL));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWords));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &certificateFlags, sizeof(uint32_t))
                .andReturnValue(ERROR_IO)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWords));

}

TEST(AssetMgrGroup, TestRetrieveCertificateSmallBuffer)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrCertificatePurposeT certPurpose = ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR;
    AssetMgrRsa2048PubCertT pubCert;
    uint32_t certSizeLongWords = sizeof(AssetMgrRsa2048PubCertT);
    uint32_t certificateFlags = 0;
    uint32_t certSizeLongWordsSmall = certSizeLongWords - 1;

    certificateFlags += (uint32_t)certAuthId << ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET;
    certificateFlags += (uint32_t)certPurpose << ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT header = {};
    header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    header.signatureOffsetLongWords = (ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES + sizeof(AssetMgrRsaPubCertHeaderT)) / sizeof(uint32_t);
    header.certificateFlags = certificateFlags;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &certSizeLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrRetrieveCertificate(certAuthId, certPurpose, (AssetMgrRsaPubCertT *)&pubCert, &certSizeLongWordsSmall));

}


TEST(AssetMgrGroup, TestRetrievePrivateKeyBasic)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrRsa2048PrivKeyT privKey;
    uint32_t const privKeySize = sizeof(AssetMgrRsa2048PrivKeyT) / sizeof(uint32_t);
    AssetMgrRsaPrivKeyHeaderT header = {0,0};
    uint32_t privKeySizeLongWords = privKeySize -(sizeof(header)/sizeof(uint32_t));

    header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY)
                .withOutputParameterReturning("assetLengthLongWords", &privKeySize, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", (uint32_t *)&header, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("offset", 1)
                .withParameter("assetLengthLongWords", privKeySizeLongWords)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWords));
    CHECK_EQUAL(privKeySize -(sizeof(header)/sizeof(uint32_t)), privKeySizeLongWords);
}

TEST(AssetMgrGroup, TestRetrievePrivateKeyErrors)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrRsa2048PrivKeyT privKey;
    uint32_t const privKeySize = sizeof(AssetMgrRsa2048PrivKeyT) / sizeof(uint32_t);
    AssetMgrRsaPrivKeyHeaderT header = {0,0};
    uint32_t privKeySizeLongWords = privKeySize -(sizeof(header)/sizeof(uint32_t));

    header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrRetrievePrivateKey(certAuthId, NULL, NULL));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWords));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY)
                .withOutputParameterReturning("assetLengthLongWords", &privKeySize, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", (uint32_t *)&header, sizeof(uint32_t))
                .andReturnValue(ERROR_IO)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWords));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY)
                .withOutputParameterReturning("assetLengthLongWords", &privKeySize, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", (uint32_t *)&header, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_IO)
                .withParameter("offset", 1)
                .withParameter("assetLengthLongWords", privKeySizeLongWords)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWords));

}

TEST(AssetMgrGroup, TestRetrievePrivateKeyNoAuthId)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrRsa2048PrivKeyT privKey;
    uint32_t const privKeySize = sizeof(AssetMgrRsa2048PrivKeyT) / sizeof(uint32_t);
    AssetMgrRsaPrivKeyHeaderT header = {0,0};
    uint32_t privKeySizeLongWords = privKeySize -(sizeof(header)/sizeof(uint32_t));

    header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY)
                .withOutputParameterReturning("assetLengthLongWords", &privKeySize, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", (uint32_t *)&header, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWords));

}

TEST(AssetMgrGroup, TestRetrievePrivateKeySmallBuffer)
{
    AssetMgrCertificateAuthIdT certAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;
    AssetMgrRsa2048PrivKeyT privKey;
    uint32_t const privKeySize = sizeof(AssetMgrRsa2048PrivKeyT) / sizeof(uint32_t);
    AssetMgrRsaPrivKeyHeaderT header = {0,0};
    uint32_t privKeySizeLongWords = privKeySize -(sizeof(header)/sizeof(uint32_t));
    uint32_t privKeySizeLongWordsSmall = privKeySizeLongWords - 1;

    header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A;

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("assetType", ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY)
                .withOutputParameterReturning("assetLengthLongWords", &privKeySize, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", (uint32_t *)&header, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrRetrievePrivateKey(certAuthId, privKey.exponent, &privKeySizeLongWordsSmall));
}

TEST(AssetMgrGroup, TestRetrievePSKMasterSecretBasic)
{
    AssetMgrPskMasterSecret128T masterSecret;
    uint32_t assetLengthLongWords = sizeof(AssetMgrPskMasterSecret128T) / sizeof(uint32_t);
    uint32_t NvmRecordLengthLongWords = sizeof(AssetMgrPskMasterSecret128T) / sizeof(uint32_t);

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("offset", 0U)
                .withParameter("assetLengthLongWords", assetLengthLongWords)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrievePskMasterSecret((AssetMgrPskMasterSecretT *)&masterSecret, &assetLengthLongWords));
    CHECK_EQUAL(NvmRecordLengthLongWords, assetLengthLongWords);
}

TEST(AssetMgrGroup, TestRetrievePSKMasterSecretError)
{
    AssetMgrPskMasterSecret128T masterSecret;
    uint32_t assetLengthLongWords = (sizeof(AssetMgrPskMasterSecret128T) / sizeof(uint32_t)) - 1;
    uint32_t NvmRecordLengthLongWords = sizeof(AssetMgrPskMasterSecret128T) / sizeof(uint32_t);

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrRetrievePskMasterSecret(NULL, NULL));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrievePskMasterSecret((AssetMgrPskMasterSecretT *)&masterSecret, &assetLengthLongWords));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrRetrievePskMasterSecret((AssetMgrPskMasterSecretT *)&masterSecret, &assetLengthLongWords));

}

TEST(AssetMgrGroup, TestRetrieveOTPMConfigBasic)
{
    AssetMgrOtpmConfigT OTPMConfig;
    uint32_t assetLengthLongWords = sizeof(AssetMgrOtpmConfigT) / sizeof(uint32_t);
    uint32_t NvmRecordLengthLongWords = sizeof(AssetMgrOtpmConfigT) / sizeof(uint32_t);

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .andReturnValue(ERROR_SUCCESS)
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", assetLengthLongWords)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_OTPM_CONFIG)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrRetrieveOtpmConfig((AssetMgrOtpmConfigT *)&OTPMConfig));
}

TEST(AssetMgrGroup, TestRetrieveOTPMConfigError)
{
    AssetMgrOtpmConfigT OTPMConfig;
    uint32_t NvmRecordLengthLongWords = sizeof(AssetMgrOtpmConfigT) / sizeof(uint32_t);

    // Force error by setting NULL
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_OTPM_CONFIG)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrRetrieveOtpmConfig( NULL));

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .withOutputParameterReturning("assetLengthLongWords", &NvmRecordLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_OTPM_CONFIG)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrRetrieveOtpmConfig((AssetMgrOtpmConfigT *)&OTPMConfig));
}

TEST(AssetMgrGroup, TestMgrOnPhaseEntryBasic)
{
    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnPhaseEntry(SYSTEM_MGR_PHASE_INITIALIZE));
    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));
    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnPhaseEntry(SYSTEM_MGR_PHASE_NUM_PHASES));
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetError)
{
    uint32_t sharedmem[1024*4] = {0};
    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;

    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    // Wrong command code
    mock().expectOneCall("SystemMgrReportFatalError")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_INVAL);

    CHECK_EQUAL(ERROR_INVAL, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_GET_SENSOR_ID, (uint32_t *)params));

    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    // Fail to retrieve LCS
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs))
                .andReturnValue(ERROR_SYSERR);

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    // Invalid LCS
    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_ACCESS);

    CHECK_EQUAL(ERROR_ACCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    // Attempt to use DM root-of-trust in DM lifecycle state
    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_ACCESS);

    CHECK_EQUAL(ERROR_ACCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    // Return error from CryptoMgrAuthenticateAsset
    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_NOENT);

    CHECK_EQUAL(ERROR_NOENT, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    // Wrong assetType
    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    asset->header.assetType = ASSET_MGR_ASSET_TYPE_EMPTY;
    asset->header.assetLengthLongWords = sizeof(AssetMgrRsa2048PubCertT) / sizeof(uint32_t);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    // big asset size
    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    asset->header.assetType = ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE;
    asset->header.assetLengthLongWords = (sizeof(AssetMgrRsa2048PubCertT) / sizeof(uint32_t)) + 16;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    // Find First returns error in NVM
    params->assetSize = sizeof(AssetMgrRsa2048PubCertT) + sizeof(AssetMgrAssetHeaderT);
    params->rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    asset->header.assetType = ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE;
    asset->header.assetLengthLongWords = (sizeof(AssetMgrRsa2048PubCertT) / sizeof(uint32_t));
    asset->cert.header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_2048_BIT;
    asset->cert.header.signatureOffsetLongWords = (offsetof(AssetMgrRsa2048PubCertT, signature)) / sizeof(uint32_t);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_SYSERR)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

void SetupSharedMemtoRsaAsset(CryptoMgrRootOfTrustT rootOfTrust,
                                     AssetMgrProvisionAssetRequestParamsT * params,
                                     AssetMgrCertificateTypeT type
                                     )
{
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE;
    params->assetId = 0x12345678;
    params->clockSpeedHz = 6000000;
    params->rootOfTrust = rootOfTrust;
    uint32_t size = 0;
    uint8_t *publicKey = asset->cert.publicKey;
    uint8_t *signature = NULL;
    switch(type)
    {
        case ASSET_MGR_CERTIFICATE_TYPE_2048_BIT:
            params->assetSize = sizeof(AssetMgrRsa2048PubCertAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa2048PubCertT) / sizeof(uint32_t);
            asset->cert.header.signatureOffsetLongWords = (offsetof(AssetMgrRsa2048PubCertT, signature)) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES;
            signature = ((AssetMgrRsa2048PubCertT *)&asset->cert)->signature;
            break;
        case ASSET_MGR_CERTIFICATE_TYPE_3072_BIT:
            params->assetSize = sizeof(AssetMgrRsa3072PubCertAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa3072PubCertT) / sizeof(uint32_t);
            asset->cert.header.signatureOffsetLongWords = (offsetof(AssetMgrRsa3072PubCertT, signature)) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_3072_PUBLIC_KEY_SIZE_BYTES;
            signature = ((AssetMgrRsa3072PubCertT *)&asset->cert)->signature;
            break;
        case ASSET_MGR_CERTIFICATE_TYPE_4096_BIT:
            params->assetSize = sizeof(AssetMgrRsa4096PubCertAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa4096PubCertT) / sizeof(uint32_t);
            asset->cert.header.signatureOffsetLongWords = (offsetof(AssetMgrRsa4096PubCertT, signature)) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;
            signature = ((AssetMgrRsa4096PubCertT *)&asset->cert)->signature;
            break;
        case ASSET_MGR_CERTIFICATE_TYPE_RESERVED:
        default:
            CHECK(false);
            break;
    }

    asset->header.assetType = assetType;
    asset->cert.header.certificateFlags = (3 << ASSET_MGR_CERT_FLAGS_VERSION__OFFSET) +
                                          (0xBEEF << ASSET_MGR_CERT_FLAGS_NUMBER__OFFSET);
    asset->cert.header.typeIdentifier = type;

    for(uint32_t i = 0; i < size; i++)
    {
        publicKey[i] = (uint8_t)i;
        signature[i] = (uint8_t)i;
    }

}

void TestMgrOnProvisionAssetPubCertTypes(AssetMgrCertificateTypeT type)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_CM;

    SetupSharedMemtoRsaAsset(rootOfTrust, params, type);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("SystemMgrSetClockSpeed")
                .withParameter("clockSpeedHz", 6000000);

    mock().expectOneCall("NvmMgrWriteAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPubCertBasic)
{
    TestMgrOnProvisionAssetPubCertTypes(ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);
    TestMgrOnProvisionAssetPubCertTypes(ASSET_MGR_CERTIFICATE_TYPE_3072_BIT);
    TestMgrOnProvisionAssetPubCertTypes(ASSET_MGR_CERTIFICATE_TYPE_4096_BIT);
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPubCertBadType)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    AssetMgrRsa4096PubCertAssetT * asset = (AssetMgrRsa4096PubCertAssetT *)params->encAsset;

    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);
    // Change type
    asset->cert.header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_RESERVED;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_BADMSG);

    CHECK_EQUAL(ERROR_BADMSG, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPubCertMaxCount)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;
    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    for (uint32_t i = 1; i < ASSET_MGR_MAX_PUB_CERT_2048_3072_ASSETS_ALLOWED; i++)
    {
        // check same version
        AssetMgrRsaPubCertHeaderT header = asset->cert.header;
        // change first bit of the number
        header.certificateFlags ^= (1 << (ASSET_MGR_CERT_FLAGS_VERSION__OFFSET + ASSET_MGR_CERT_FLAGS_VERSION__SIZE));
        mock().expectOneCall("NvmMgrReadAsset")
                    .withOutputParameterReturning("buffer", &header, sizeof(AssetMgrRsaPubCertHeaderT))
                    .withParameter("offset", 0)
                    .withParameter("assetLengthLongWords", 2)
                    .andReturnValue(ERROR_SUCCESS)
                    .ignoreOtherParameters();

        mock().expectOneCall("NvmMgrFindNext")
                    .andReturnValue(ERROR_SUCCESS)
                    .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                    .ignoreOtherParameters();
    }

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_NOSPC);

    CHECK_EQUAL(ERROR_NOSPC, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPubCertWrongVersion)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);
    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    // check same version
    AssetMgrRsaPubCertHeaderT same_version = asset->cert.header;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &same_version, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_ALREADY);

    CHECK_EQUAL(ERROR_ALREADY, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT lower_otpm_version = asset->cert.header;
    lower_otpm_version.certificateFlags &= ~ASSET_MGR_CERT_FLAGS_VERSION__MASK;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &lower_otpm_version, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT diff_otpm_number_flags = asset->cert.header;
    diff_otpm_number_flags.certificateFlags ^= ASSET_MGR_CERT_FLAGS_NUMBER__MASK;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &diff_otpm_number_flags, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT diff_otpm_purpose_flags = asset->cert.header;
    diff_otpm_purpose_flags.certificateFlags ^= ASSET_MGR_CERT_FLAGS_PURPOSE__MASK;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &diff_otpm_purpose_flags, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    AssetMgrRsaPubCertHeaderT diff_otpm_authority_flags = asset->cert.header;
    diff_otpm_authority_flags.certificateFlags ^= ASSET_MGR_CERT_FLAGS_AUTH_ID__MASK;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &diff_otpm_authority_flags, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindNext")
                .andReturnValue(ERROR_SUCCESS)
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .ignoreOtherParameters();

    // check lower version
    AssetMgrRsaPubCertHeaderT higher_otpm_version = asset->cert.header;
    higher_otpm_version.certificateFlags |= ASSET_MGR_CERT_FLAGS_VERSION__MASK;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &higher_otpm_version, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_ALREADY);

    CHECK_EQUAL(ERROR_ALREADY, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPubCertWrongOtpmType)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);
    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    // check bad type header in OTPM
    AssetMgrRsaPubCertHeaderT bad_type_header = asset->cert.header;
    bad_type_header.typeIdentifier = ASSET_MGR_CERTIFICATE_TYPE_RESERVED;
    mock().expectOneCall("NvmMgrReadAsset")
                .withOutputParameterReturning("buffer", &bad_type_header, sizeof(AssetMgrRsaPubCertHeaderT))
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetRsaError)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    AssetMgrRsa2048PubCertAssetT * asset = (AssetMgrRsa2048PubCertAssetT *)params->encAsset;
    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    // Fail comparing the certificate
    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;
    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset").andReturnValue(ERROR_SYSERR)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);
    // Wrong signatureOffset
    asset->cert.header.signatureOffsetLongWords = (offsetof(AssetMgrRsa2048PubCertT, signature)) / sizeof(uint32_t) + 1;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    SetupSharedMemtoRsaAsset(rootOfTrust, params, ASSET_MGR_CERTIFICATE_TYPE_2048_BIT);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset")
                .withParameter("offset", 0)
                .withParameter("assetLengthLongWords", 2)
                .andReturnValue(ERROR_SYSERR)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SYSERR);

    CHECK_EQUAL(ERROR_SYSERR, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));


}

void SetupSharedMemtoPrivateKeyAsset(CryptoMgrRootOfTrustT rootOfTrust,
                                              AssetMgrProvisionAssetRequestParamsT * params,
                                              AssetMgrSensorAuthPrivKeyIdT type)
{
    AssetMgrRsa4096PrivKeyAssetT * asset = (AssetMgrRsa4096PrivKeyAssetT *)params->encAsset;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY;
    uint32_t size = 0;
    switch(type)
    {
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048:
            params->assetSize = sizeof(AssetMgrRsa2048PrivKeyAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa2048PrivKeyT) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_2048_PRIVATE_EXPONENT_SIZE_BYTES;
            break;
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_3072:
            params->assetSize = sizeof(AssetMgrRsa3072PrivKeyAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa3072PrivKeyT) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_3072_PRIVATE_EXPONENT_SIZE_BYTES;
            break;
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_4096:
            params->assetSize = sizeof(AssetMgrRsa4096PrivKeyAssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrRsa4096PrivKeyT) / sizeof(uint32_t);
            size = ASSET_MGR_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES;
            break;
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED0:
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED4:
        default:
            CHECK(false);
            break;
    }
    params->assetId = 0x12345678;
    params->clockSpeedHz = 30000000;
    params->rootOfTrust = rootOfTrust;

    asset->header.assetType = assetType;
    asset->key.header.certificateAuthId = ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B;
    asset->key.header.authPrivKeyId = type;

    for(uint32_t i = 0; i < size; i++)
    {
        asset->key.exponent[i] = (uint8_t)i;
    }
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPrivKeyBasic)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;

    for (AssetMgrSensorAuthPrivKeyIdT type = ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048;
        type < ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED4;
        type = (AssetMgrSensorAuthPrivKeyIdT)((uint32_t)type + 1))
    {
        SetupSharedMemtoPrivateKeyAsset(rootOfTrust, params, type);

        mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                    .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

        mock().expectOneCall("CryptoMgrAuthenticateAsset")
                    .withParameter("assetId", params->assetId)
                    .withParameter("rootOfTrust", rootOfTrust)
                    .ignoreOtherParameters();

        mock().expectOneCall("NvmMgrFindFirst")
                    .andReturnValue(ERROR_NOENT)
                    .ignoreOtherParameters();

        mock().expectOneCall("SystemMgrSetClockSpeed")
                    .withParameter("clockSpeedHz", 30000000);

        mock().expectOneCall("NvmMgrWriteAsset")
                    .andReturnValue(ERROR_SUCCESS)
                    .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpointWithInfo")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                    .withParameter("info", ERROR_SUCCESS);

        CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
    }
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPrivKeyError)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    AssetMgrRsa4096PrivKeyAssetT * asset = (AssetMgrRsa4096PrivKeyAssetT *)params->encAsset;

    SetupSharedMemtoPrivateKeyAsset(rootOfTrust, params, ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048);
    asset->key.header.authPrivKeyId = ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RESERVED0;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_RANGE);

    CHECK_EQUAL(ERROR_RANGE, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

    SetupSharedMemtoPrivateKeyAsset(rootOfTrust, params, ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;
    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrReadAsset").andReturnValue(ERROR_IO)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_IO);

    CHECK_EQUAL(ERROR_IO, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPrivKeySameInOTPM)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    SetupSharedMemtoPrivateKeyAsset(rootOfTrust, params, ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048);
    AssetMgrRsa2048PrivKeyAssetT * asset = (AssetMgrRsa2048PrivKeyAssetT *)params->encAsset;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;
    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    uint32_t * buffer = (uint32_t *)&asset->key;
    for (uint32_t i = 0; i < (sizeof(asset->key)/sizeof(uint32_t)); i++)
    {
        mock().expectOneCall("NvmMgrReadAsset").andReturnValue(ERROR_SUCCESS)
                    .withOutputParameterReturning("buffer", &buffer[i], sizeof(uint32_t))
                    .withParameter("offset", 0)
                    .withParameter("assetLengthLongWords", 1)
                    .ignoreOtherParameters();
    }

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_ALREADY);

    CHECK_EQUAL(ERROR_ALREADY, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPrivKeyMaxCount)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    SetupSharedMemtoPrivateKeyAsset(rootOfTrust, params, ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048);
    AssetMgrRsa2048PrivKeyAssetT * asset = (AssetMgrRsa2048PrivKeyAssetT *)params->encAsset;

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    uint32_t assetLengthLongWords = asset->header.assetLengthLongWords;
    mock().expectOneCall("NvmMgrFindFirst")
                .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    for (uint32_t i = 1; i < ASSET_MGR_MAX_PRIVATE_RSA_2048_3072_ASSETS_ALLOWED; i++)
    {
        uint32_t * buffer = (uint32_t *)&asset->key;
        mock().expectOneCall("NvmMgrReadAsset")
                    .withOutputParameterReturning("buffer", &buffer[0], sizeof(uint32_t))
                    .withParameter("offset", 0)
                    .withParameter("assetLengthLongWords", 1)
                    .andReturnValue(ERROR_SUCCESS)
                    .ignoreOtherParameters();

        uint32_t data = buffer[1] ^ 0xCAFE; // change some bits
        mock().expectOneCall("NvmMgrReadAsset")
                    .withOutputParameterReturning("buffer", &data, sizeof(uint32_t))
                    .withParameter("offset", 0)
                    .withParameter("assetLengthLongWords", 1)
                    .andReturnValue(ERROR_SUCCESS)
                    .ignoreOtherParameters();

        mock().expectOneCall("NvmMgrFindNext")
                    .andReturnValue(ERROR_SUCCESS)
                    .withOutputParameterReturning("assetLengthLongWords", &assetLengthLongWords, sizeof(uint32_t))
                    .ignoreOtherParameters();
    }

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_NOSPC);

    CHECK_EQUAL(ERROR_NOSPC, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));

}

void SetupSharedMemtoPSKAsset(CryptoMgrRootOfTrustT rootOfTrust,
                                     AssetMgrProvisionAssetRequestParamsT * params,
                                     CryptoMgrSymmetricKeyTypeT type)
{
    AssetMgrPskMasterSecret256AssetT * asset = (AssetMgrPskMasterSecret256AssetT *)params->encAsset;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_PSK_MASTER_SECRET;
    uint32_t size = 0;

    switch(type)
    {
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_128:
            params->assetSize = sizeof(AssetMgrPskMasterSecret128AssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrPskMasterSecret128T) / sizeof(uint32_t);
            size = ASSET_MGR_PSK_MASTER_SECRET_128_KEY_SIZE_BYTES;
            break;
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_192:
            params->assetSize = sizeof(AssetMgrPskMasterSecret192AssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrPskMasterSecret192T) / sizeof(uint32_t);
            size = ASSET_MGR_PSK_MASTER_SECRET_192_KEY_SIZE_BYTES;
            break;
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_256:
            params->assetSize = sizeof(AssetMgrPskMasterSecret256AssetT);
            asset->header.assetLengthLongWords = sizeof(AssetMgrPskMasterSecret256T) / sizeof(uint32_t);
            size = ASSET_MGR_PSK_MASTER_SECRET_256_KEY_SIZE_BYTES;
            break;
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED:
        default:
            CHECK(false);
            break;
    }

    params->assetId = 0x12345678;
    params->clockSpeedHz = 100000000;
    params->rootOfTrust = rootOfTrust;

    asset->header.assetType = assetType;
    asset->masterSecret.header.masterSecretKeyType = type;

    for(uint32_t i = 0; i < size; i++)
    {
        asset->masterSecret.masterSecret[i] = (uint8_t)i;
    }
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetPSKBasic)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    for (CryptoMgrSymmetricKeyTypeT type = CRYPTO_MGR_SYMMETRIC_KEY_AES_128;
        type < CRYPTO_MGR_SYMMETRIC_KEY_AES_RESERVED;
        type = (CryptoMgrSymmetricKeyTypeT)((uint32_t)type + 1))
    {
        SetupSharedMemtoPSKAsset(rootOfTrust, params, type);

        mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                    .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

        mock().expectOneCall("CryptoMgrAuthenticateAsset")
                    .withParameter("assetId", params->assetId)
                    .withParameter("rootOfTrust", params->rootOfTrust)
                    .ignoreOtherParameters();

        mock().expectOneCall("NvmMgrFindFirst")
                    .andReturnValue(ERROR_NOENT)
                    .ignoreOtherParameters();

        mock().expectOneCall("SystemMgrSetClockSpeed")
                    .withParameter("clockSpeedHz", 100000000);

        mock().expectOneCall("NvmMgrWriteAsset")
                    .andReturnValue(ERROR_SUCCESS)
                    .ignoreOtherParameters();

        mock().expectOneCall("DiagSetCheckpointWithInfo")
                    .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                    .withParameter("info", ERROR_SUCCESS);

        CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
    }

}


void SetupSharedMemtoTrngAsset(CryptoMgrRootOfTrustT rootOfTrust,
                                     AssetMgrProvisionAssetRequestParamsT * params)
{
    AssetMgrCC312TrngCharAssetT * asset = (AssetMgrCC312TrngCharAssetT *)params->encAsset;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;

    params->assetSize = sizeof(AssetMgrCC312TrngCharAssetT);
    asset->header.assetLengthLongWords = sizeof(AssetMgrCC312TrngCharT) / sizeof(uint32_t);
    params->assetId = 0x12345678;
    params->clockSpeedHz = 70000000;
    params->rootOfTrust = rootOfTrust;

    asset->header.assetType = assetType;
    asset->TRNGChar.R0 = 0;
    asset->TRNGChar.R1 = 1;
    asset->TRNGChar.R2 = 2;
    asset->TRNGChar.R3 = 3;
}

TEST(AssetMgrGroup, TestMgrOnProvisionAssetTrngBasic)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    SetupSharedMemtoTrngAsset(rootOfTrust, params);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("SystemMgrSetClockSpeed")
                .withParameter("clockSpeedHz", 70000000);

    mock().expectOneCall("NvmMgrWriteAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
}

void SetupSharedMemtoOtpmAsset(CryptoMgrRootOfTrustT rootOfTrust,
                                     AssetMgrProvisionAssetRequestParamsT * params)
{
    AssetMgrOtpmConfigAssetT * asset = (AssetMgrOtpmConfigAssetT *)params->encAsset;
    AssetMgrAssetTypeT assetType = ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION;

    params->assetSize = sizeof(AssetMgrOtpmConfigAssetT);
    asset->header.assetLengthLongWords = sizeof(AssetMgrOtpmConfigT) / sizeof(uint32_t);
    params->assetId = 0x12345678;
    params->clockSpeedHz = 50000000;
    params->rootOfTrust = rootOfTrust;

    asset->header.assetType = assetType;
    asset->OTPMConfig.tvpps_ns = 0xcafebabe;
    asset->OTPMConfig.mrb_read = 0xbeef;
}
TEST(AssetMgrGroup, TestMgrOnProvisionAssetOtpmCfgBasic)
{
    uint32_t sharedmem[1024*4] = {0};

    AssetMgrProvisionAssetRequestParamsT *params = (AssetMgrProvisionAssetRequestParamsT *)sharedmem;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
    CryptoMgrRootOfTrustT const rootOfTrust = CRYPTO_MGR_ROOT_OF_TRUST_DM;
    SetupSharedMemtoOtpmAsset(rootOfTrust, params);

    mock().expectOneCall("LifecycleMgrGetCurrentLcs")
                .withOutputParameterReturning("lcsCurrent", &currentLcs, sizeof(currentLcs));

    mock().expectOneCall("CryptoMgrAuthenticateAsset")
                .withParameter("assetId", params->assetId)
                .withParameter("rootOfTrust", params->rootOfTrust)
                .ignoreOtherParameters();

    mock().expectOneCall("NvmMgrFindFirst")
                .andReturnValue(ERROR_NOENT)
                .ignoreOtherParameters();

    mock().expectOneCall("SystemMgrSetClockSpeed")
                .withParameter("clockSpeedHz", 50000000);

    mock().expectOneCall("NvmMgrWriteAsset")
                .andReturnValue(ERROR_SUCCESS)
                .ignoreOtherParameters();

    mock().expectOneCall("DiagSetCheckpointWithInfo")
                .withParameter("checkpoint", DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET)
                .withParameter("info", ERROR_SUCCESS);

    CHECK_EQUAL(ERROR_SUCCESS, AssetMgrOnProvisionAsset(HOST_COMMAND_CODE_PROVISION_ASSET, (uint32_t *)params));
}

