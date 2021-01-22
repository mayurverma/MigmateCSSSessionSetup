//***********************************************************************************
/// \file
///
/// User Guide Examples unit test
//
//***********************************************************************************
// Copyright 2019 ON Semiconductor.  All rights reserved.
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
#include <stdio.h>
#include <stdlib.h>

#include "css_host_command.h"

/// Example of how to issue the GetStatus host command
///
/// \returns void
///
void GetStatusExample(void)
{
    CssGetStatusResponseT getStatusResponse;
    CssErrorT result;

    result = CssGetStatus(&getStatusResponse);
    printf("Result is 0x%02x\n", (uint8_t)result);
    if (ERROR_SUCCESS == result)
    {
        printf("Current phase: 0x%04x\n", getStatusResponse.currentPhase);
        printf("Current LCS:   0x%04x\n", getStatusResponse.currentLcs);
        printf("ROM version:   0x%04x\n", getStatusResponse.romVersion);
        printf("Patch version: 0x%04x\n", getStatusResponse.patchVersion);
    }
}

/// Example of how to issue the ApplyDebugEntitlement host command
///
/// \param[in] blobFileName     Name of the file containing the BLOB
///
/// \returns void
///
void ApplyDebugEntitlementExample(char* const blobFileName)
{
    uint32_t const clockSpeedHz = 27000000; // 27 MHz clock
    FILE* blobFile;
    CssErrorT result;

    blobFile = fopen(blobFileName, "rb");
    if (NULL != blobFile)
    {
        uint8_t* blob;
        size_t blobFileSize;

        // Get the size of the BLOB
        fseek(blobFile, 0, SEEK_END);
        blobFileSize = (size_t)ftell(blobFile);
        fseek(blobFile, 0, SEEK_SET);
        printf("BLOB file is %d bytes\n", blobFileSize);

        // Read the BLOB and apply to the sensor
        blob = (uint8_t*)malloc(blobFileSize);
        if (NULL != blob)
        {
            size_t bytesRead = fread(blob, 1U, blobFileSize, blobFile);
            if (bytesRead == blobFileSize)
            {
                result = CssApplyDebugEntitlement(clockSpeedHz, blob, blobFileSize / 2U);
                printf("Result is 0x%02x\n", (uint8_t)result);
            }
        }
        free(blob);
    }
    else
    {
        printf("Failed to open %s\n", blobFileName);
    }

    fclose(blobFile);
}

/// Loads a patch chunk
///
/// \param[in] chunkFileName    Chunk file name
///
/// \returns Status of the load
/// \retval ERROR_SUCCESS   Chunk loaded successfully
/// \retval ERROR_NOSPC     Failed to malloc chunk
/// \retval ERROR_IO        Failed to read chunk file
/// \retval ERROR_NOENT     Chunk file not found
///
CssErrorT LoadChunk(char* const chunkFileName)
{
    CssErrorT result = ERROR_SUCCESS;
    FILE* chunkFile;
    size_t chunkFileSize;
    uint8_t* chunk;

    chunkFile = fopen(chunkFileName, "rb");
    if (NULL != chunkFile)
    {
        // Get the size of the chunk
        fseek(chunkFile, 0, SEEK_END);
        chunkFileSize = (size_t)ftell(chunkFile);
        fseek(chunkFile, 0, SEEK_SET);
        printf("Chunk file is %d bytes\n", chunkFileSize);

        // Read the chunl and apply to the sensor
        chunk = (uint8_t*)malloc(chunkFileSize);
        if (NULL != chunk)
        {
            size_t bytesRead = fread(chunk, 1U, chunkFileSize, chunkFile);
            if (bytesRead == chunkFileSize)
            {
                result = CssLoadPatchChunk(chunk, chunkFileSize / 2U);
                printf("Result is 0x%02x\n", (uint8_t)result);
            }
            else
            {
                printf("Failed to read chunk file\n");
                result = ERROR_IO;
            }
        }
        else
        {
            printf("Failed to malloc chunk\n");
            result = ERROR_NOSPC;
        }

        free(chunk);
    }
    else
    {
        printf("Failed to open chunk file: %s\n", chunkFileName);
        result = ERROR_NOENT;
    }

    fclose(chunkFile);
    return result;
}

/// Example of how to load a patch
///
/// \param[in] patchFileRootName    Root of the patch file name e.g. "Patchp12345r123"
/// \param[in] numChunks            Number of chunks to load
///
/// \returns void
///
void LoadPatchExample(char* const patchFileRootName, size_t const numChunks)
{
    CssErrorT result;
    size_t currentChunk;

    for (currentChunk = 0; currentChunk < numChunks; currentChunk++)
    {
        char chunkFileName[80];
        sprintf(chunkFileName, "%s_%02u.bin", patchFileRootName, currentChunk);

        printf("Loading chunk %s\n", chunkFileName);
        result = LoadChunk(chunkFileName);
        if (ERROR_SUCCESS != result)
        {
            printf("Failed to load chunk: %d\n", result);
            break;
        }
    }
}

/// Example of how to issue the GetSensorId host command
///
/// \returns void
///
void GetSensorIdExample(void)
{
    CssSensorIdT sensorId;
    CssErrorT result;

    result = CssGetSensorId(sensorId);
    printf("Result is 0x%02x\n", (uint8_t)result);
    if (ERROR_SUCCESS == result)
    {
        printf("Sensor ID:\n");
        for (size_t i = 0; i < sizeof(sensorId); i += 4U)
        {
            uint8_t* index = &sensorId[i];
            printf("\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }
    }
}

/// Example of how to issue the GetCertificates host command
///
/// Assumes the VendorA trust identity uses 2048-bit RSA keys
///
/// \returns void
///
void GetCertificatesExample(void)
{
    CssCertificateAuthIdT const certificateAuthId = CSS_CERTIFICATE_AUTH_ID_VENDOR_A;
    CssRsa2048PubCertT sensorCert;
    uint32_t sensorCertSizeWords;
    CssRsa2048PubCertT vendorCert;
    uint32_t vendorCertSizeWords;
    CssErrorT result;

    result = CssGetCertificates(certificateAuthId,
                                (uint8_t* const)&sensorCert,
                                &sensorCertSizeWords,
                                (uint8_t* const)&vendorCert,
                                &vendorCertSizeWords);
    printf("Result is 0x%02x\n", (uint8_t)result);
    if (ERROR_SUCCESS == result)
    {
        printf("SensorCert is %d words\n", sensorCertSizeWords);
        printf("\tTypeIdentifier: 0x%04x\n", sensorCert.header.typeIdentifier);
        printf("\tCertFlags:      0x%04x\n", sensorCert.header.certificateFlags);

        printf("\tPublic key:\n");
        for (size_t i = 0; i < sizeof(sensorCert.publicKey); i += 4U)
        {
            uint8_t* index = &sensorCert.publicKey[i];
            printf("\t\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }

        printf("VendorCert is %d words\n", vendorCertSizeWords);
        printf("\tTypeIdentifier: 0x%04x\n", vendorCert.header.typeIdentifier);
        printf("\tCertFlags:      0x%04x\n", vendorCert.header.certificateFlags);

        printf("\tPublic key:\n");
        for (size_t i = 0; i < sizeof(vendorCert.publicKey); i += 4U)
        {
            uint8_t* index = &vendorCert.publicKey[i];
            printf("\t\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }
    }
}

/// Example of how to issue the SetSessionKeys host command
///
/// \param[in] hostSalt                 random salt
/// \param[in] encryptedParams          RSA-encrypted SessionParametersT object
/// \param[in] encryptedParamsSizeWords size of the encrypted params in words
/// \param[out] sensorSalt              buffer to contain returned sensor salt
/// \param[out] videoAuthGmacIv         buffer to contain returned GMAC IV
/// \param[out] controlChannelNonce     buffer to contain returned GCM/CCM nonce
///
/// \returns Error status
///
CssErrorT SetSessionKeysExample(uint8_t const hostSalt[CSS_SALT_SIZE_BYTES],
                                uint8_t* const encryptedParams,
                                size_t const encryptedParamsSizeWords,
                                uint8_t sensorSalt[CSS_SALT_SIZE_BYTES],
                                uint8_t videoAuthGmacIv[CSS_IV_SIZE_BYTES],
                                uint8_t controlChannelNonce[CSS_NONCE_SIZE_BYTES])
{
    CssSymmetricAuthCipherModeT const videoAuthCipherMode =
        CSS_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    CssSymmetricKeyTypeT const videoAuthKeyType =
        CSS_SYMMETRIC_KEY_AES_128;
    CssSymmetricAECipherModeT const controlChannelCipherMode =
        CSS_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    CssSymmetricKeyTypeT const controlChannelKeyType =
        CSS_SYMMETRIC_KEY_AES_128;
    CssAsymmetricCipherT const sessionParamsCipher =
        CSS_ASYMMETRIC_CIPHER_RSAES_OAEP;
    CssErrorT result;

    result = CssSetSessionKeys( videoAuthCipherMode,
                                videoAuthKeyType,
                                controlChannelCipherMode,
                                controlChannelKeyType,
                                hostSalt,
                                sessionParamsCipher,
                                (uint32_t)encryptedParamsSizeWords,
                                encryptedParams,
                                sensorSalt,
                                videoAuthGmacIv,
                                controlChannelNonce);
    printf("Result is 0x%02x\n", (uint8_t)result);
    if (ERROR_SUCCESS == result)
    {
        printf("\tSensor salt:\n");
        for (size_t i = 0; i < CSS_SALT_SIZE_BYTES; i += 4U)
        {
            uint8_t* index = &sensorSalt[i];
            printf("\t\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }

        printf("\tVideo Auth GMAC IV:\n");
        for (size_t i = 0; i < CSS_IV_SIZE_BYTES; i += 4U)
        {
            uint8_t* index = &videoAuthGmacIv[i];
            printf("\t\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }

        printf("\tControl Channel Nonce:\n");
        for (size_t i = 0; i < CSS_NONCE_SIZE_BYTES; i += 4U)
        {
            uint8_t* index = &controlChannelNonce[i];
            printf("\t\t0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                index[0], index[1], index[2], index[3]);
        }
    }

    return result;
}

extern EncryptConfigParams( CssVideoAuthConfigParamsT* const params,
                            CssVideoAuthConfigParamsT* encryptedParams,
                            uint8_t mac[CSS_MAC_SIZE_BYTES]);

/// Example of how to use the SetVideoAuthRoi host command
/// Calls extern EncryptConfigParams function which encrypts the
/// video authentication configuration parameters using the appropriate
/// AES mode.
///
/// returns void
///
void SetVideoAuthRoiExample(void)
{
    CssVideoAuthConfigParamsT videoAuthConfigParams;
    CssVideoAuthConfigParamsT encryptedParams;
    uint8_t mac[CSS_MAC_SIZE_BYTES] = {0xAA, 0xBB, 0xCC, 0xDD};
    CssErrorT result;

    videoAuthConfigParams.videoAuthMode = (uint16_t)CSS_VIDEO_AUTH_MODE_ROI;
    videoAuthConfigParams.pixelPackMode = (uint16_t)CSS_PIXEL_PACKING_MODE_OPTIMIZED;
    videoAuthConfigParams.pixelPackValue = 0;
    videoAuthConfigParams.roi.firstRow = 8U;
    videoAuthConfigParams.roi.lastRow = 1200U;
    videoAuthConfigParams.roi.rowSkip = 16U;
    videoAuthConfigParams.roi.firstColumn = 8U;
    videoAuthConfigParams.roi.lastColumn = 2400U;
    videoAuthConfigParams.roi.columnSkip = 16U;
    videoAuthConfigParams.frameCounter = 0x00010024ULL;

    // Encrypt the config parameters (external)
    //EncryptConfigParams(&videoAuthConfigParams, &encryptedParams, mac);
    encryptedParams.frameCounter = videoAuthConfigParams.frameCounter;

    result = CssSetVideoAuthRoi((uint8_t* const)&encryptedParams, mac);
    printf("Result is 0x%02x\n", (uint8_t)result);
}

}

static bool is_verbose = true;

// Mocks
extern "C"
{
    // Writes a 16-bit value to the specified register
    void I2cWriteReg16(CssRegisterT const reg, uint16_t const value)
    {
        mock().actualCall("I2cWriteReg16")
            .withParameter("reg", reg)
            .withParameter("value", value);

        if (is_verbose) printf("Writing I2C register 0x%04x with 0x%04x\n",
            reg, value);
    }

    // Reads a 16-bit value from the specified register
    uint16_t I2cReadReg16(CssRegisterT const reg)
    {
        mock().actualCall("I2cReadReg16")
            .withParameter("reg", reg);

        if (is_verbose) printf("Reading 0x%04x from I2C register 0x%04x\n",
                (uint16_t)mock().intReturnValue(), reg);

        return (uint16_t)mock().intReturnValue();
    }
}

TEST_GROUP(CssSharedMemGroup)
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

TEST_GROUP(CssSharedMemMultiByteGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        // Reset page register to 0
        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PAGE)
            .withParameter("value", 0);
        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0)
            .andReturnValue(0);

        (void)CssReadSharedMemUint16(0);
    }
};

TEST_GROUP(CssHostCommandsGroup)
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

TEST_GROUP(CssHostCommandsPageGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        // Reset page register to 0
        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PAGE)
            .withParameter("value", 0);
        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0)
            .andReturnValue(0);

        (void)CssReadSharedMemUint16(0);
    }
};
#if 1
TEST(CssSharedMemGroup, TestWriteSharedMemUint16)
{
    uint16_t const word_offset = 0x1;
    uint16_t const val = 0x1234;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .withParameter("value", val);

    CssWriteSharedMemUint16(word_offset, val);
}

TEST(CssSharedMemGroup, TestWriteSharedMemUint32)
{
    uint16_t const word_offset = 0x2;
    uint32_t const val = 0x12345678;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .withParameter("value", val & 0xFFFF);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 1) * 2))
        .withParameter("value", (val >> 16));

    CssWriteSharedMemUint32(word_offset, val);
}

TEST(CssSharedMemGroup, TestWriteSharedMemUint64)
{
    uint16_t const word_offset = 0x3;
    uint64_t const val = 0x123456789abcdef0;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .withParameter("value", (uint16_t)(val & 0xFFFF));
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 1) * 2))
        .withParameter("value", (uint16_t)((val >> 16) & 0xFFFF));
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 2) * 2))
        .withParameter("value", (uint16_t)((val >> 32) & 0xFFFF));
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 3) * 2))
        .withParameter("value", (uint16_t)((val >> 48) & 0xFFFF));

    CssWriteSharedMemUint64(word_offset, val);
}

TEST(CssSharedMemGroup, TestReadSharedMemUint16)
{
    uint16_t const word_offset = 0x4;
    uint16_t const val = 0x1234;
    uint16_t ret_val;

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .andReturnValue(val);

    ret_val = CssReadSharedMemUint16(word_offset);
    CHECK_EQUAL(val, ret_val);
}

TEST(CssSharedMemGroup, TestReadSharedMemUint32)
{
    uint16_t const word_offset = 0x7;
    uint32_t const val = 0x12345678;
    uint32_t ret_val;

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .andReturnValue((uint16_t)(val & 0xFFFF));
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 1) * 2))
        .andReturnValue((uint16_t)(val >> 16));

    ret_val = CssReadSharedMemUint32(word_offset);
    UNSIGNED_LONGS_EQUAL(val, ret_val);
}

TEST(CssSharedMemGroup, TestReadSharedMemUint64)
{
    uint16_t const word_offset = 0x0;
    uint64_t const val = 0x123456789abcdef0;
    uint64_t ret_val;

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + (word_offset * 2))
        .andReturnValue((uint16_t)(val & 0xFFFF));
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 1) * 2))
        .andReturnValue((uint16_t)((val >> 16) & 0xFFFF));
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 2) * 2))
        .andReturnValue((uint16_t)((val >> 32) & 0xFFFF));
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + ((word_offset + 3) * 2))
        .andReturnValue((uint16_t)((val >> 48) & 0xFFFF));

    ret_val = CssReadSharedMemUint64(word_offset);
    UNSIGNED_LONGLONGS_EQUAL(val, ret_val);
}

TEST(CssSharedMemMultiByteGroup, TestWriteSharedMemMultiByteSmall)
{
    uint16_t const word_offset = CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    uint8_t multi_byte_array[] = {0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U};

    size_t current_word = 0;
    size_t current_byte = 0;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 1);

    for (; current_byte < sizeof(multi_byte_array);
           current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .withParameter("value", (uint16_t)val);
    }

    CssWriteSharedMemMultiByte(word_offset, multi_byte_array, sizeof(multi_byte_array) / 2);

}

TEST(CssSharedMemMultiByteGroup, TestReadSharedMemMultiByteSmall)
{
    uint16_t const word_offset = CSS_SHARED_MEMORY_PAGE_SIZE_WORDS * 2;
    uint8_t multi_byte_array[] = {0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U};
    uint8_t input_array[8];

    size_t current_word = 0;
    size_t current_byte = 0;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 2);

    for (; current_byte < sizeof(multi_byte_array);
           current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .andReturnValue((uint16_t)val);
    }

    CssReadSharedMemMultiByte(word_offset, input_array, sizeof(multi_byte_array) / 2);

    for (current_byte = 0; current_byte < sizeof(multi_byte_array); current_byte++)
    {
        CHECK_EQUAL(multi_byte_array[current_byte], input_array[current_byte]);
    }

}

TEST(CssSharedMemMultiByteGroup, TestWriteSharedMemMultiByteSmallPageBoundary)
{
    uint16_t const word_offset = (CSS_SHARED_MEMORY_PAGE_SIZE_WORDS * 2) - 2;
    uint8_t multi_byte_array[] = {0xA1U, 0xA2U, 0xA3U, 0xA4U, 0xB5U, 0xB6U, 0xB7U, 0xB8U};

    size_t current_word = word_offset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    size_t current_byte = 0;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 1);

    for (   ;
            current_byte < (sizeof(multi_byte_array) / 2);
            current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 2);

    for (   current_word = 0;
            current_byte < sizeof(multi_byte_array);
            current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .withParameter("value", (uint16_t)val);
    }

    CssWriteSharedMemMultiByte(word_offset, multi_byte_array, sizeof(multi_byte_array) / 2);

}

TEST(CssSharedMemMultiByteGroup, TestReadSharedMemMultiByteSmallPageBoundary)
{
    uint16_t const word_offset = (CSS_SHARED_MEMORY_PAGE_SIZE_WORDS * 3) - 2;
    uint8_t multi_byte_array[] = {0xC1U, 0xC2U, 0xC3U, 0xC4U, 0xD5U, 0xD6U, 0xD7U, 0xD8U};
    uint8_t input_array[8];

    size_t current_word = word_offset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    size_t current_byte = 0;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 2);

    for (   ;
            current_byte < sizeof(multi_byte_array) / 2;
            current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .andReturnValue((uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 3);

    for (   current_word = 0;
            current_byte < sizeof(multi_byte_array);
            current_word += 1, current_byte += 2)
    {
        uint32_t val = multi_byte_array[current_byte + 1];
        val <<= 8;
        val |= multi_byte_array[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .andReturnValue((uint16_t)val);
    }

    CssReadSharedMemMultiByte(word_offset, input_array, sizeof(multi_byte_array) / 2);

    for (current_byte = 0; current_byte < sizeof(multi_byte_array); current_byte++)
    {
        CHECK_EQUAL(multi_byte_array[current_byte], input_array[current_byte]);
    }

}

TEST(CssHostCommandsGroup, TestHostCommandTimeout)
{
    CssErrorT result;
    CssGetStatusResponseT getStatusResponse;
    uint32_t const timeout = 10U;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_STATUS);

    for (size_t i = 0; i < timeout; i++)
    {
        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_COMMAND)
            .andReturnValue((uint16_t)HOST_COMMAND_CODE_GET_STATUS);
    }

    CssSetHostCommandTimeout(timeout);

    result = CssGetStatus(&getStatusResponse);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);

    LONGS_EQUAL(ERROR_TIMEDOUT, result);
}

TEST(CssHostCommandsGroup, TestGetStatus)
{
    CssErrorT result;
    CssGetStatusResponseT getStatusResponse;
    CssGetStatusResponseT mockResponse = {
        .currentPhase = CSS_SYSTEM_PHASE_DEBUG,
        .currentLcs = CSS_SYSTEM_LCS_FP,
        .romVersion = 917,
        .patchVersion = 0,
        .reserved = 0
    };

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_STATUS);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .andReturnValue((uint16_t)mockResponse.currentPhase);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + 2)
        .andReturnValue((uint16_t)mockResponse.currentLcs);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + 4)
        .andReturnValue((uint16_t)mockResponse.romVersion);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + 6)
        .andReturnValue((uint16_t)mockResponse.patchVersion);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + 8)
        .andReturnValue((uint16_t)mockResponse.reserved);

    result = CssGetStatus(&getStatusResponse);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);

    LONGS_EQUAL(ERROR_SUCCESS, result);
    CHECK_EQUAL(mockResponse.currentPhase, getStatusResponse.currentPhase);
    CHECK_EQUAL(mockResponse.currentLcs, getStatusResponse.currentLcs);
    CHECK_EQUAL(mockResponse.romVersion, getStatusResponse.romVersion);
    CHECK_EQUAL(mockResponse.patchVersion, getStatusResponse.patchVersion);
    CHECK_EQUAL(mockResponse.reserved, getStatusResponse.reserved);
}

TEST(CssHostCommandsGroup, TestApplyDebugEntitlement)
{
    uint32_t clock_speed_hz = 0x12345678UL;
    uint8_t dummy_blob[224] = {0x88, 0x99, 0xAA, 0xBB};
    CssErrorT result;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .withParameter("value", (uint16_t)clock_speed_hz);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 2U)
        .withParameter("value", (uint16_t)(clock_speed_hz >> 16U));

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 4U)
        .withParameter("value", (uint16_t)(sizeof(dummy_blob) / 2));
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 6U)
        .withParameter("value", (uint16_t)0U);

    size_t current_byte = 0;
    size_t current_word = 0;

    for (   ;
            current_byte < sizeof(dummy_blob);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = dummy_blob[current_byte + 1U];
        val <<= 8U;
        val |= dummy_blob[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + 8U + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    result = CssApplyDebugEntitlement(clock_speed_hz, dummy_blob, sizeof(dummy_blob) / 2);
    LONGS_EQUAL(ERROR_SUCCESS, result);
}

TEST(CssHostCommandsGroup, TestLoadPatchChunk)
{
    uint8_t dummy_chunk[540] = {0xAA, 0xBB, 0xCC, 0xDD};
    CssErrorT result;

    size_t current_byte = 0;
    size_t current_word = 0;

    for (   ;
            current_byte < sizeof(dummy_chunk);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = dummy_chunk[current_byte + 1U];
        val <<= 8U;
        val |= dummy_chunk[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_LOAD_PATCH_CHUNK);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    result = CssLoadPatchChunk(dummy_chunk, sizeof(dummy_chunk) / 2);
    LONGS_EQUAL(ERROR_SUCCESS, result);
}

TEST(CssHostCommandsGroup, TestGetSensorId)
{
    CssErrorT result;
    CssSensorIdT sensor_id;
    CssSensorIdT mock_id = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_SENSOR_ID);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    size_t current_byte = 0;
    size_t current_word = 0;

    for (   ;
            current_byte < sizeof(mock_id);
            current_byte += 2, current_word += 1)
    {
        uint32_t val = mock_id[current_byte + 1];
        val <<= 8;
        val |= mock_id[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2))
            .andReturnValue((uint16_t)val);
    }

    result = CssGetSensorId(sensor_id);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);

    LONGS_EQUAL(ERROR_SUCCESS, result);

    for (size_t i=0; i<sizeof(CssSensorIdT); i++)
    {
        if (is_verbose) printf("SensorId[%d] = 0x%02x\n", i, sensor_id[i]);
        CHECK_EQUAL(mock_id[i], sensor_id[i]);
    }
}

TEST(CssHostCommandsGroup, TestGetSensorIdFails)
{
    CssErrorT result;
    CssSensorIdT sensor_id;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_SENSOR_ID);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);

    result = CssGetSensorId(sensor_id);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);

    LONGS_EQUAL(ERROR_SYSERR, result);
}

TEST(CssHostCommandsPageGroup, TestGetCertificates)
{
    CssErrorT result;
    CssRsa2048PubCertT sensor_cert;
    uint32_t sensor_cert_size_words;
    CssRsa2048PubCertT vendor_cert;
    uint32_t vendor_cert_size_words;

    CssCertificatePurposeT sensor_cert_purpose = CSS_CERTIFICATE_PURPOSE_SENSOR;
    CssCertificateAuthIdT sensor_cert_auth_id = CSS_CERTIFICATE_AUTH_ID_VENDOR_A;
    CssCertificateVersionT sensor_cert_version = 0U;
    CssCertificateNumberT sensor_cert_number = 1U;
    CssRsa2048PubCertT mock_sensor_cert;

    CssCertificatePurposeT vendor_cert_purpose = CSS_CERTIFICATE_PURPOSE_VENDOR;
    CssCertificateAuthIdT vendor_cert_auth_id = CSS_CERTIFICATE_AUTH_ID_VENDOR_A;
    CssCertificateVersionT vendor_cert_version = 1U;
    CssCertificateNumberT vendor_cert_number = 2U;
    CssRsa2048PubCertT mock_vendor_cert;

    mock_sensor_cert.header.typeIdentifier = 0x4232U;
    mock_sensor_cert.header.signatureOffsetLongWords = 2U + (CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES / 2U);
    mock_sensor_cert.header.certificateFlags =  (((uint32_t)sensor_cert_purpose & 0x3U) << 0U) |
                                                (((uint32_t)sensor_cert_auth_id & 0x3U) << 2U) |
                                                ((sensor_cert_version & 0x3U) << 4U) |
                                                ((sensor_cert_number  & 0x3FFFFFFUL) << 6U);

    memset(mock_sensor_cert.publicKey, 0x55, sizeof(mock_sensor_cert.publicKey));
    memset(mock_sensor_cert.signature, 0xAA, sizeof(mock_sensor_cert.signature));

    mock_vendor_cert.header.typeIdentifier = 0x4232U;
    mock_vendor_cert.header.signatureOffsetLongWords = 2U + (CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES / 2U);
    mock_vendor_cert.header.certificateFlags =  (((uint32_t)vendor_cert_purpose & 0x3U) << 0U) |
                                                (((uint32_t)vendor_cert_auth_id & 0x3U) << 2U) |
                                                ((vendor_cert_version & 0x3U) << 4U) |
                                                ((vendor_cert_number  & 0x3FFFFFFUL) << 6U);

    memset(mock_vendor_cert.publicKey, 0x99, sizeof(mock_vendor_cert.publicKey));
    memset(mock_vendor_cert.signature, 0x66, sizeof(mock_vendor_cert.signature));

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .withParameter("value", (uint16_t)sensor_cert_auth_id);

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_CERTIFICATES);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .andReturnValue((uint16_t)(sizeof(sensor_cert) / 2U));

    size_t current_byte = 0;
    size_t current_word = 0;
    uint8_t* mock_sensor_cert_multi_byte = (uint8_t*)&mock_sensor_cert;
    uint8_t* mock_vendor_cert_multi_byte = (uint8_t*)&mock_vendor_cert;

    for (   ;
            current_byte < sizeof(sensor_cert);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_sensor_cert_multi_byte[current_byte + 1U];
        val <<= 8U;
        val |= mock_sensor_cert_multi_byte[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + 2U + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_PARAMS_0 + 2U + (current_word * 2))
        .andReturnValue((uint16_t)(sizeof(vendor_cert) / 2));

    size_t const vendor_cert_start_word_offset = CSS_PARAMS_0 + 4U + (current_word * 2U);
    size_t const bytes_in_page = (CSS_SHARED_MEMORY_PAGE_SIZE_WORDS - (2U + current_word)) * 2U;

    for (   current_byte = 0, current_word = 0;
            current_byte < bytes_in_page;
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_vendor_cert_multi_byte[current_byte + 1U];
        val <<= 8U;
        val |= mock_vendor_cert_multi_byte[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", vendor_cert_start_word_offset + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PAGE)
        .withParameter("value", 1U);

    for (   current_word = 0;
            current_byte < sizeof(vendor_cert);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_vendor_cert_multi_byte[current_byte + 1U];
        val <<= 8U;
        val |= mock_vendor_cert_multi_byte[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    result = CssGetCertificates(sensor_cert_auth_id,
                                (uint8_t* const)&sensor_cert,
                                &sensor_cert_size_words,
                                (uint8_t* const)&vendor_cert,
                                &vendor_cert_size_words);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    LONGS_EQUAL(sizeof(sensor_cert) / 2U, sensor_cert_size_words);
    LONGS_EQUAL(mock_sensor_cert.header.typeIdentifier, sensor_cert.header.typeIdentifier);
    LONGS_EQUAL(mock_sensor_cert.header.signatureOffsetLongWords, sensor_cert.header.signatureOffsetLongWords);
    LONGS_EQUAL(mock_sensor_cert.header.certificateFlags, sensor_cert.header.certificateFlags);

    LONGS_EQUAL(sizeof(vendor_cert) / 2U, vendor_cert_size_words);
    LONGS_EQUAL(mock_vendor_cert.header.typeIdentifier, vendor_cert.header.typeIdentifier);
    LONGS_EQUAL(mock_vendor_cert.header.signatureOffsetLongWords, vendor_cert.header.signatureOffsetLongWords);
    LONGS_EQUAL(mock_vendor_cert.header.certificateFlags, vendor_cert.header.certificateFlags);
}

TEST(CssHostCommandsGroup, TestGetCertificatesFails)
{
    CssErrorT result;
    CssRsa2048PubCertT sensor_cert;
    uint32_t sensor_cert_size_words;
    CssRsa2048PubCertT vendor_cert;
    uint32_t vendor_cert_size_words;

    CssCertificateAuthIdT sensor_cert_auth_id = CSS_CERTIFICATE_AUTH_ID_VENDOR_A;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .withParameter("value", (uint16_t)sensor_cert_auth_id);

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_GET_CERTIFICATES);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);

    result = CssGetCertificates(sensor_cert_auth_id,
                                (uint8_t* const)&sensor_cert,
                                &sensor_cert_size_words,
                                (uint8_t* const)&vendor_cert,
                                &vendor_cert_size_words);
    if (is_verbose) printf("Result is 0x%02x\n", (uint8_t)result);
    LONGS_EQUAL(ERROR_SYSERR, result);

}

TEST(CssHostCommandsGroup, TestSetSessionKeys)
{
    CssSymmetricAuthCipherModeT const video_auth_cipher_mode = CSS_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    CssSymmetricKeyTypeT const video_auth_key_type = CSS_SYMMETRIC_KEY_AES_256;
    CssSymmetricAECipherModeT const control_channel_cipher_mode = CSS_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    CssSymmetricKeyTypeT const control_channel_key_type = CSS_SYMMETRIC_KEY_AES_256;
    uint8_t const host_salt[CSS_SALT_SIZE_BYTES] = {0xA0, 0xA1, 0xA2, 0xA3};
    CssAsymmetricCipherT const session_params_cipher = CSS_ASYMMETRIC_CIPHER_RSAES_OAEP;
    uint16_t const encrypted_params_size_words = CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES / 2U;
    uint8_t const encrypted_params[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES] = {0xB0, 0xB1, 0xB2, 0xB3};
    uint8_t sensor_salt[CSS_SALT_SIZE_BYTES];
    uint8_t video_auth_gmac_iv[CSS_IV_SIZE_BYTES];
    uint8_t control_channel_nonce[CSS_NONCE_SIZE_BYTES];

    uint8_t mock_sensor_salt[CSS_SALT_SIZE_BYTES] = {0x10, 0x11, 0x12, 0x13};
    uint8_t mock_video_auth_gmac_iv[CSS_IV_SIZE_BYTES] = {0x20, 0x21, 0x22, 0x23};
    uint8_t mock_control_channel_nonce[CSS_NONCE_SIZE_BYTES] = {0x30, 0x31, 0x32, 0x33};

    CssErrorT result;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .withParameter("value", (uint16_t)video_auth_cipher_mode);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 2U)
        .withParameter("value", (uint16_t)video_auth_key_type);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 4U)
        .withParameter("value", (uint16_t)control_channel_cipher_mode);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 6U)
        .withParameter("value", (uint16_t)control_channel_key_type);

    size_t current_byte = 0;
    size_t current_word = 4U;

    for (   ;
            current_byte < sizeof(host_salt);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = host_salt[current_byte + 1U];
        val <<= 8U;
        val |= host_salt[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
        .withParameter("value", (uint16_t)session_params_cipher);
    current_word += 1U;
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
        .withParameter("value", (uint16_t)(sizeof(CssSessionParametersT) / 2U));
    current_word += 1U;

    for (   current_byte = 0;
            current_byte < sizeof(encrypted_params);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = encrypted_params[current_byte + 1U];
        val <<= 8U;
        val |= encrypted_params[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_SET_SESSION_KEYS);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    for (   current_byte = 0, current_word = 0;
            current_byte < sizeof(mock_sensor_salt);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_sensor_salt[current_byte + 1U];
        val <<= 8U;
        val |= mock_sensor_salt[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    for (   current_byte = 0;
            current_byte < sizeof(mock_video_auth_gmac_iv);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_video_auth_gmac_iv[current_byte + 1U];
        val <<= 8U;
        val |= mock_video_auth_gmac_iv[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    for (   current_byte = 0;
            current_byte < sizeof(mock_control_channel_nonce);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mock_control_channel_nonce[current_byte + 1U];
        val <<= 8U;
        val |= mock_control_channel_nonce[current_byte];

        mock().expectOneCall("I2cReadReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .andReturnValue((uint16_t)val);
    }

    result = CssSetSessionKeys( video_auth_cipher_mode,
                                video_auth_key_type,
                                control_channel_cipher_mode,
                                control_channel_key_type,
                                host_salt,
                                session_params_cipher,
                                encrypted_params_size_words,
                                (uint8_t* const)encrypted_params,
                                sensor_salt,
                                video_auth_gmac_iv,
                                control_channel_nonce);
    LONGS_EQUAL(ERROR_SUCCESS, result);

    for(size_t i = 0; i < sizeof(sensor_salt); i++)
    {
        CHECK_EQUAL(mock_sensor_salt[i], sensor_salt[i]);
    }
    for(size_t i = 0; i < sizeof(video_auth_gmac_iv); i++)
    {
        CHECK_EQUAL(mock_video_auth_gmac_iv[i], video_auth_gmac_iv[i]);
    }
    for(size_t i = 0; i < sizeof(control_channel_nonce); i++)
    {
        CHECK_EQUAL(mock_control_channel_nonce[i], control_channel_nonce[i]);
    }
}

TEST(CssHostCommandsGroup, TestSetSessionKeysFails)
{
    CssSymmetricAuthCipherModeT const video_auth_cipher_mode = CSS_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC;
    CssSymmetricKeyTypeT const video_auth_key_type = CSS_SYMMETRIC_KEY_AES_256;
    CssSymmetricAECipherModeT const control_channel_cipher_mode = CSS_SYMMETRIC_AE_CIPHER_MODE_AES_GCM;
    CssSymmetricKeyTypeT const control_channel_key_type = CSS_SYMMETRIC_KEY_AES_256;
    uint8_t const host_salt[CSS_SALT_SIZE_BYTES] = {0xA0, 0xA1, 0xA2, 0xA3};
    CssAsymmetricCipherT const session_params_cipher = CSS_ASYMMETRIC_CIPHER_RSAES_OAEP;
    uint16_t const encrypted_params_size_words = CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES / 2U;
    uint8_t const encrypted_params[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES] = {0xB0, 0xB1, 0xB2, 0xB3};
    uint8_t sensor_salt[CSS_SALT_SIZE_BYTES];
    uint8_t video_auth_gmac_iv[CSS_IV_SIZE_BYTES];
    uint8_t control_channel_nonce[CSS_NONCE_SIZE_BYTES];

    CssErrorT result;

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0)
        .withParameter("value", (uint16_t)video_auth_cipher_mode);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 2U)
        .withParameter("value", (uint16_t)video_auth_key_type);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 4U)
        .withParameter("value", (uint16_t)control_channel_cipher_mode);
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + 6U)
        .withParameter("value", (uint16_t)control_channel_key_type);

    size_t current_byte = 0;
    size_t current_word = 4U;

    for (   ;
            current_byte < sizeof(host_salt);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = host_salt[current_byte + 1U];
        val <<= 8U;
        val |= host_salt[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
        .withParameter("value", (uint16_t)session_params_cipher);
    current_word += 1U;
    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
        .withParameter("value", (uint16_t)(sizeof(CssSessionParametersT) / 2U));
    current_word += 1U;

    for (   current_byte = 0;
            current_byte < sizeof(encrypted_params);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = encrypted_params[current_byte + 1U];
        val <<= 8U;
        val |= encrypted_params[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_SET_SESSION_KEYS);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SYSERR);

    result = CssSetSessionKeys( video_auth_cipher_mode,
                                video_auth_key_type,
                                control_channel_cipher_mode,
                                control_channel_key_type,
                                host_salt,
                                session_params_cipher,
                                encrypted_params_size_words,
                                (uint8_t* const)encrypted_params,
                                sensor_salt,
                                video_auth_gmac_iv,
                                control_channel_nonce);
    LONGS_EQUAL(ERROR_SYSERR, result);
}

TEST(CssHostCommandsGroup, TestSetVideoAuthRoi)
{
    uint8_t encrypted_params[sizeof(CssVideoAuthConfigParamsT)] = {0x00, 0x01, 0x02, 0x03};
    uint8_t mac[CSS_MAC_SIZE_BYTES] = {0xFC, 0xFD, 0xFE, 0xFF};
    CssErrorT result;

    size_t current_byte = 0;
    size_t current_word = 0;

    for (   ;
            current_byte < sizeof(encrypted_params);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = encrypted_params[current_byte + 1U];
        val <<= 8U;
        val |= encrypted_params[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    for (   current_byte = 0;
            current_byte < sizeof(mac);
            current_byte += 2U, current_word += 1U)
    {
        uint32_t val = mac[current_byte + 1U];
        val <<= 8U;
        val |= mac[current_byte];

        mock().expectOneCall("I2cWriteReg16")
            .withParameter("reg", CSS_PARAMS_0 + (current_word * 2U))
            .withParameter("value", (uint16_t)val);
    }

    mock().expectOneCall("I2cWriteReg16")
        .withParameter("reg", CSS_COMMAND)
        .withParameter("value", HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI);

    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);
    mock().expectOneCall("I2cReadReg16")
        .withParameter("reg", CSS_COMMAND)
        .andReturnValue((uint16_t)ERROR_SUCCESS);

    result = CssSetVideoAuthRoi(encrypted_params, mac);
    LONGS_EQUAL(ERROR_SUCCESS, result);
}
#endif
#if 0
TEST(CssHostCommandsGroup, TestDemo)
{
    mock().ignoreOtherCalls();
    GetStatusExample();
    ApplyDebugEntitlementExample((char*)"..\\..\\test\\DummyBlob.bin");
    LoadPatchExample((char*)"..\\..\\test\\DummyPatchp12345r678", 3U);
    GetSensorIdExample();
    GetCertificatesExample();

    {
        uint8_t const hostSalt[CSS_SALT_SIZE_BYTES] = {0xA1, 0xB2, 0xC3, 0xD4};
        uint8_t const encryptedParams[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES] = {0xE1, 0xF2, 0x03, 0x14};
        size_t const encryptedParamsSizeWords = sizeof(encryptedParams) / 2U;
        uint8_t sensorSalt[CSS_SALT_SIZE_BYTES];
        uint8_t videoAuthGmacIv[CSS_IV_SIZE_BYTES];
        uint8_t controlChannelNonce[CSS_NONCE_SIZE_BYTES];

        (void)SetSessionKeysExample(hostSalt,
                                    (uint8_t* const)encryptedParams,
                                    encryptedParamsSizeWords,
                                    sensorSalt,
                                    videoAuthGmacIv,
                                    controlChannelNonce);
    }

    SetVideoAuthRoiExample();
}
#endif
