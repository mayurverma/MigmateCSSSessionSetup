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
#include "CryptoMgr.h"
#include "LifecycleMgr.h"
#include "CryptoMgrInternal.h"
#include "PatchMgr.h"
#include "cc_util_error.h"
#include "cc_pal_trng.h"
#include "AssetMgr.h"
#include "CC312HalPal.h"

ErrorT AssetMgrRetrieveCC312TrngChar(AssetMgrCC312TrngCharT* CC312TrngChar)
{
    (void)CC312TrngChar;
    return ERROR_IO;
}

}

TEST_GROUP(CryptoMgrGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        cryptoMgr.status = CRYPTO_MGR_UNINIT;
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(CryptoMgrGroup, TestIncrementNumberBasic)
{
    const uint32_t dataLen = 4;
    uint8_t data[dataLen] = {0,0,0,0};
    uint8_t expected[dataLen] = {1,0,0,0};

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);

    expected[0] = 2;
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);

    memset(data, 0xFF, dataLen);
    memset(expected, 0, dataLen);
    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrIncrementNumber(dataLen, data));
    MEMCMP_EQUAL(expected, data, dataLen);
}

TEST(CryptoMgrGroup, TestAeadDecryptBasic)
{
    CryptoMgrSymmetricAECipherModeT cipherMode = CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM;
    uint32_t keySizeBytes = 16;

    uint8_t key[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    uint8_t ivNonce[12] = {0,0,0,0, 0,0,0,0, 0,0,0,0};
    size_t associatedDataSizeBytes = 2;
    uint8_t associatedData[2] = {0, 0};
    size_t encryptedMsgSizeBytes = 16;
    uint8_t encryptedMsgAndTag[32] = {0x6e, 0xc7, 0x5f, 0xb2, 0xe2, 0xb4, 0x87, 0x46, 0x1e, 0xdd, 0xcb, 0xb8, 0x97, 0x11, 0x92, 0xba,
                                      0xcc, 0xc2, 0xe4, 0xfa, 0x6e, 0x76, 0xce, 0x44, 0x27, 0x50, 0xeb, 0x13, 0x84, 0x64, 0x1a, 0x16}; //tag
    uint8_t decryptedMsg[24];

    mock().disable();

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAeadDecrypt(
        cipherMode,
        keySizeBytes,
        key,
        ivNonce,
        associatedDataSizeBytes,
        associatedData,
        encryptedMsgSizeBytes,
        encryptedMsgAndTag,
        decryptedMsg));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));

    mock().enable();
}

TEST(CryptoMgrGroup, TestDerivationBasic)
{
    uint32_t const masterSecretSizeBytes = 16;
    uint8_t masterSecret[masterSecretSizeBytes] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    uint32_t const hostSaltSizeBytes = 16;
    uint8_t  hostSalt[hostSaltSizeBytes] = {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    uint32_t const sensorSaltSizeBytes = 16;
    uint8_t  sensorSalt[sensorSaltSizeBytes] = {2,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    uint32_t const authSessionKeySizeBytes = 16;
    uint8_t  authSessionKey[authSessionKeySizeBytes];
    uint32_t const controlChannelKeySizeBytes = 16;
    uint8_t  controlChannelKey[controlChannelKeySizeBytes];

    uint8_t  const expauthSessionKey[authSessionKeySizeBytes]       = {0x8f, 0x3f, 0xea, 0xd2, 0xc1, 0xd6, 0xa1, 0x70, 0xa0, 0x31, 0x27, 0xee, 0xd6, 0x9a, 0x34, 0x35};
    uint8_t  const expcontrolChannelKey[controlChannelKeySizeBytes] = {0x38, 0xc5, 0xec, 0x86, 0x16, 0x54, 0xe8, 0x7b, 0xa2, 0xd6, 0x0e, 0xa9, 0x9a, 0x60, 0x80, 0x08};


    mock().disable();

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrDeriveKeys(
            masterSecretSizeBytes,
            masterSecret,
            hostSaltSizeBytes,
            hostSalt,
            sensorSaltSizeBytes,
            sensorSalt,
            authSessionKeySizeBytes,
            authSessionKey,
            controlChannelKeySizeBytes,
            controlChannelKey));

    MEMCMP_EQUAL(expauthSessionKey, authSessionKey, authSessionKeySizeBytes);
    MEMCMP_EQUAL(expcontrolChannelKey, controlChannelKey, controlChannelKeySizeBytes);

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));

    mock().enable();

}

TEST(CryptoMgrGroup, TestRSABasic)
{
    uint32_t const keySizeBytes = 128;

    // 0x2dc678e0908167434e4046f4c556765bc611aba0632991ebcad712ebd1e938b6c395f9ccf397e2ca70733ac7bf2d6a4ad57dc9c11447962c45b7aaf7cbecbadafb26016a7fe4ba0b9b04035597907511fa172596e32fcbc274355801fdccca4d0e88265c5e3f6bda8d26cd70169294cc21ad448f2924f5d5e136714f49296ac9
    uint8_t privateExponent[keySizeBytes] = {0x2d, 0xc6, 0x78, 0xe0, 0x90, 0x81, 0x67, 0x43, 0x4e, 0x40, 0x46, 0xf4, 0xc5, 0x56, 0x76, 0x5b,
                                             0xc6, 0x11, 0xab, 0xa0, 0x63, 0x29, 0x91, 0xeb, 0xca, 0xd7, 0x12, 0xeb, 0xd1, 0xe9, 0x38, 0xb6,
                                             0xc3, 0x95, 0xf9, 0xcc, 0xf3, 0x97, 0xe2, 0xca, 0x70, 0x73, 0x3a, 0xc7, 0xbf, 0x2d, 0x6a, 0x4a,
                                             0xd5, 0x7d, 0xc9, 0xc1, 0x14, 0x47, 0x96, 0x2c, 0x45, 0xb7, 0xaa, 0xf7, 0xcb, 0xec, 0xba, 0xda,
                                             0xfb, 0x26, 0x01, 0x6a, 0x7f, 0xe4, 0xba, 0x0b, 0x9b, 0x04, 0x03, 0x55, 0x97, 0x90, 0x75, 0x11,
                                             0xfa, 0x17, 0x25, 0x96, 0xe3, 0x2f, 0xcb, 0xc2, 0x74, 0x35, 0x58, 0x01, 0xfd, 0xcc, 0xca, 0x4d,
                                             0x0e, 0x88, 0x26, 0x5c, 0x5e, 0x3f, 0x6b, 0xda, 0x8d, 0x26, 0xcd, 0x70, 0x16, 0x92, 0x94, 0xcc,
                                             0x21, 0xad, 0x44, 0x8f, 0x29, 0x24, 0xf5, 0xd5, 0xe1, 0x36, 0x71, 0x4f, 0x49, 0x29, 0x6a, 0xc9};

    // 0x98c187c6a9aab85ab174fb3ace56ae42403b0bbedc23adbd30e81a6bd00b314299e77c5461ee970147d5494f65babfe8a3506e50df1189d1fbe7f8cc2b98cbfa7adc994f83e221910691026cc083769fc91b1e36e30db0f5fe40bb584ede201f670252baa19bb3ec07da2a28af4fc08e91e10d92d932651fbc871f55b658ab7b
    uint8_t modulus[keySizeBytes] =         {0x98, 0xc1, 0x87, 0xc6, 0xa9, 0xaa, 0xb8, 0x5a, 0xb1, 0x74, 0xfb, 0x3a, 0xce, 0x56, 0xae, 0x42,
                                             0x40, 0x3b, 0x0b, 0xbe, 0xdc, 0x23, 0xad, 0xbd, 0x30, 0xe8, 0x1a, 0x6b, 0xd0, 0x0b, 0x31, 0x42,
                                             0x99, 0xe7, 0x7c, 0x54, 0x61, 0xee, 0x97, 0x01, 0x47, 0xd5, 0x49, 0x4f, 0x65, 0xba, 0xbf, 0xe8,
                                             0xa3, 0x50, 0x6e, 0x50, 0xdf, 0x11, 0x89, 0xd1, 0xfb, 0xe7, 0xf8, 0xcc, 0x2b, 0x98, 0xcb, 0xfa,
                                             0x7a, 0xdc, 0x99, 0x4f, 0x83, 0xe2, 0x21, 0x91, 0x06, 0x91, 0x02, 0x6c, 0xc0, 0x83, 0x76, 0x9f,
                                             0xc9, 0x1b, 0x1e, 0x36, 0xe3, 0x0d, 0xb0, 0xf5, 0xfe, 0x40, 0xbb, 0x58, 0x4e, 0xde, 0x20, 0x1f,
                                             0x67, 0x02, 0x52, 0xba, 0xa1, 0x9b, 0xb3, 0xec, 0x07, 0xda, 0x2a, 0x28, 0xaf, 0x4f, 0xc0, 0x8e,
                                             0x91, 0xe1, 0x0d, 0x92, 0xd9, 0x32, 0x65, 0x1f, 0xbc, 0x87, 0x1f, 0x55, 0xb6, 0x58, 0xab, 0x7b};

    //   238b2ecd21a5cef334a34e425547fe06a940465f77e4099c2fdda636fbee4c2ab39d7741b3260da9205ced7db80bfbdbf6c2d1f4070ba2cfc7864da9ecdaedbeb3f2d45f3331f4bac05db709f27d6ee8ef5a505899781358fbb85d52259787a02bd2012b0b97841e06e66afc922d13ea124e2de83ee01ad22949412931975140
    uint8_t encryptedMsg[keySizeBytes] =    {0x23, 0x8b, 0x2e, 0xcd, 0x21, 0xa5, 0xce, 0xf3, 0x34, 0xa3, 0x4e, 0x42, 0x55, 0x47, 0xfe, 0x06,
                                             0xa9, 0x40, 0x46, 0x5f, 0x77, 0xe4, 0x09, 0x9c, 0x2f, 0xdd, 0xa6, 0x36, 0xfb, 0xee, 0x4c, 0x2a,
                                             0xb3, 0x9d, 0x77, 0x41, 0xb3, 0x26, 0x0d, 0xa9, 0x20, 0x5c, 0xed, 0x7d, 0xb8, 0x0b, 0xfb, 0xdb,
                                             0xf6, 0xc2, 0xd1, 0xf4, 0x07, 0x0b, 0xa2, 0xcf, 0xc7, 0x86, 0x4d, 0xa9, 0xec, 0xda, 0xed, 0xbe,
                                             0xb3, 0xf2, 0xd4, 0x5f, 0x33, 0x31, 0xf4, 0xba, 0xc0, 0x5d, 0xb7, 0x09, 0xf2, 0x7d, 0x6e, 0xe8,
                                             0xef, 0x5a, 0x50, 0x58, 0x99, 0x78, 0x13, 0x58, 0xfb, 0xb8, 0x5d, 0x52, 0x25, 0x97, 0x87, 0xa0,
                                             0x2b, 0xd2, 0x01, 0x2b, 0x0b, 0x97, 0x84, 0x1e, 0x06, 0xe6, 0x6a, 0xfc, 0x92, 0x2d, 0x13, 0xea,
                                             0x12, 0x4e, 0x2d, 0xe8, 0x3e, 0xe0, 0x1a, 0xd2, 0x29, 0x49, 0x41, 0x29, 0x31, 0x97, 0x51, 0x40};
    uint32_t decryptedMsgSizeBytes = keySizeBytes;
    uint8_t expectedMsg[] =    {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

    uint8_t decryptedMsg[keySizeBytes] = {};

    mock().disable();

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrRsaDecrypt(
            keySizeBytes,
            privateExponent,
            modulus,
            encryptedMsg,
            &decryptedMsgSizeBytes,
            decryptedMsg));

    MEMCMP_EQUAL(expectedMsg, decryptedMsg, sizeof(expectedMsg));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_SHUTDOWN));

    mock().enable();

}

TEST(CryptoMgrGroup, TestAuthAssetBasic)
{
    uint32_t assetId = 0x12345678;
    // Key = [0x00]x16
    // nonce = [0x02]x12
    // asset = [0x01]x16
    uint8_t assetPakageBuffer[] = {0x74,0x65,0x73,0x41,0x0,0x0,0x1,0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x4e,0x7f,0x7,0x84,0x39,0x30,0x56,0xaa,0xf,0xe1,0xf0,0xbd,0x77,0x29,0xb,0x7d,0x38,0xe3,0x28,0xc3,0x36,0x25,0xd9,0x36,0xd,0x27,0x89,0x55,0xc3,0xf7,0x65,0x42};
    uint8_t expectedAsset[] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    mock().disable();

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAuthenticateAsset(assetId, CRYPTO_MGR_ROOT_OF_TRUST_CM, (uint32_t *)assetPakageBuffer));

    MEMCMP_EQUAL(expectedAsset, assetPakageBuffer + 32, sizeof(expectedAsset));

    mock().enable();
}

TEST(CryptoMgrGroup, TestAuthPatchBasic)
{
    uint32_t assetId = 0x12345678;
    // Key = [0x00]x16
    // nonce = [0x02]x12
    // asset = [0x01]x16
    uint8_t assetPakageBuffer[] = {0x74,0x65,0x73,0x41,0x0,0x0,0x1,0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x2,0x4e,0x7f,0x7,0x84,0x39,0x30,0x56,0xaa,0xf,0xe1,0xf0,0xbd,0x77,0x29,0xb,0x7d,0x38,0xe3,0x28,0xc3,0x36,0x25,0xd9,0x36,0xd,0x27,0x89,0x55,0xc3,0xf7,0x65,0x42};
    uint32_t decryptedPackageSizeBytes = 16;
    uint8_t decryptedPackage[16] = {};
    uint8_t expectedAsset[] = {1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1};
    mock().disable();

    CHECK_EQUAL(CC_OK, CC_BsvInit(CRYPTOCELL_CORE_BASE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrAuthenticatePatchChunk(assetId, (uint32_t *)assetPakageBuffer, &decryptedPackageSizeBytes, decryptedPackage));

    CHECK_EQUAL(16, decryptedPackageSizeBytes);
    MEMCMP_EQUAL(expectedAsset, decryptedPackage, sizeof(decryptedPackageSizeBytes));

    mock().enable();
}

TEST(CryptoMgrGroup, TestRandomBasic)
{
	uint32_t randomNumberSizeBytes = 16;
    uint8_t  randomNumber[16];
    mock().disable();

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrOnPhaseEntry(SYSTEM_MGR_PHASE_CONFIGURE));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));

    CHECK_EQUAL(ERROR_SUCCESS, CryptoMgrGenRandomNumber(randomNumberSizeBytes, randomNumber));

    mock().enable();
}

