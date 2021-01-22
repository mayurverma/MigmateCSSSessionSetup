//***********************************************************************************
/// \file
///
/// Example AES-GCM encryption support function definition
///
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
//***********************************************************************************

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include "../inc/css_host_command.h"

static bool verbose = false;

/// 128-bit AES key
uint8_t const controlChannelKey[CSS_AES_128_KEY_SIZE_BYTES] = {
    0xd7, 0xeb, 0x75, 0x92, 0x4d, 0xf2, 0x23, 0xcd,
    0xe1, 0xbb, 0xaa, 0xcc, 0xdf, 0x5c, 0x9e, 0x21
};

/// 96-bit GCM IV
uint8_t const controlChannelNonce[CSS_NONCE_SIZE_BYTES] = {
    0xa5, 0xd2, 0xb5, 0xd0, 0xc8, 0xa4, 0x9e, 0x3d,
    0x09, 0x19, 0x25, 0xa5
};

/// Prints details of the last OpenSSL error
///
/// \param[in] msg  Accompanying message
///
/// returns void
///
void printLastError(char *msg)
{
    static char err[128];
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n", msg, err);
}

/// Encrypts supplied plaintext using AES-GCM
///
/// \param[in] pKey                 128-bit AES key
/// \param[in] pIv                  GCM initialisation vector
/// \param[in] ivSizeBytes          Size of the initialisation vector
/// \param[in] plaintext            Plaintext to encrypt
/// \param[in] plaintextSizeBytes   Size of the plaintext
/// \param[in] aad                  Additional authenticated data
/// \param[in] aadSizeBytes         Size of the additional authenticated data
/// \param[out] ciphertext          Buffer for encrypted ciphertext
/// \param[out] tag                 Buffer for (16-byte) authentication tag
///
/// \returns Size of the encrypted ciphertext on success, -1 on failure
///
int Aes128GcmEncrypt(   uint8_t* const pKey,
                        uint8_t* const pIv,
                        size_t const ivSizeBytes,
                        uint8_t* const plaintext,
                        size_t const plaintextSizeBytes,
                        uint8_t* const aad,
                        size_t const aadSizeBytes,
                        uint8_t* const ciphertext,
                        uint8_t* const tag)
{
    uint8_t* ciphertextBlock;
    uint8_t* plaintextBlock;
    int ciphertextSizeBytes = 0;
    int encryptSizeBytes;
    int res;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    if (0 >= res)
    {
        printLastError("EVP_EncryptInit_ex failed");
        return -1;
    }

    // Set the IV length
    res = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, ivSizeBytes, NULL);
    if (0 >= res)
    {
        printLastError("EVP_CIPHER_CTX_ctrl failed");
        return -1;
    }

    // Set the key and IV
    res = EVP_EncryptInit_ex(ctx, NULL, NULL, pKey, pIv);
    if (0 >= res)
    {
        printLastError("EVP_EncryptInit_ex(pKey, pIv) failed");
        return -1;
    }

    // Add the additional authenticated data to the digest
    res = EVP_EncryptUpdate(ctx, NULL, &encryptSizeBytes, aad, aadSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_EncryptUpdate(aad) failed");
        return -1;
    }

    // Encrypt each 16-byte block
    size_t numBlocks = plaintextSizeBytes / CSS_AES_BLOCK_SIZE_BYTES;
    size_t remainder = plaintextSizeBytes % CSS_AES_BLOCK_SIZE_BYTES;

    for (int i = 0; i < numBlocks; i++)
    {
        ciphertextBlock = &ciphertext[i * CSS_AES_BLOCK_SIZE_BYTES];
        plaintextBlock = &plaintext[i * CSS_AES_BLOCK_SIZE_BYTES];

        res = EVP_EncryptUpdate(ctx,
                                ciphertextBlock,
                                &encryptSizeBytes,
                                plaintextBlock,
                                CSS_AES_BLOCK_SIZE_BYTES);
        if (0 >= res)
        {
            printLastError("EVP_EncryptUpdate() failed");
            return -1;
        }
        if (verbose) printf("Block %d: encrypted %d bytes\n", i, encryptSizeBytes);

        ciphertextSizeBytes += CSS_AES_BLOCK_SIZE_BYTES;
    }

    // Encrypt the remainder (if any)
    if (0 != remainder)
    {
        ciphertextBlock = &ciphertext[numBlocks * CSS_AES_BLOCK_SIZE_BYTES];
        plaintextBlock = &plaintext[numBlocks * CSS_AES_BLOCK_SIZE_BYTES];

        res = EVP_EncryptUpdate(ctx,
                                ciphertextBlock,
                                &encryptSizeBytes,
                                plaintextBlock,
                                remainder);
        if (0 >= res)
        {
            printLastError("EVP_EncryptUpdate() failed");
            return -1;
        }
        if (verbose) printf("Remainder: encrypted %d bytes\n", encryptSizeBytes);

        ciphertextSizeBytes += remainder;
    }

    // Finalise the encryption
    ciphertextBlock = &ciphertext[(numBlocks * CSS_AES_BLOCK_SIZE_BYTES) + remainder];

    res = EVP_EncryptFinal_ex(ctx, ciphertextBlock, &encryptSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_EncryptFinal_ex failed");
        return -1;
    }
    if (verbose) printf("Finalized the encryption (%d bytes)\n", encryptSizeBytes);

    if (verbose)
    {
        printf("Ciphertext: (size %d bytes)\n", ciphertextSizeBytes);
        BIO_dump_fp(stdout, ciphertext, ciphertextSizeBytes);
    }

    // Retrieve the tag
    res = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, CSS_MAC_SIZE_BYTES, tag);
    if (0 >= res)
    {
        printLastError("EVP_CIPHER_CTX_ctrl failed");
        return -1;
    }

    if (verbose)
    {
        printf("Tag\n");
        BIO_dump_fp(stdout, tag, CSS_MAC_SIZE_BYTES);
    }

    return ciphertextSizeBytes;
}

/// Decrypts supplied ciphertext using AES-GCM
///
/// \param[in] pKey                 128-bit AES key
/// \param[in] pIv                  GCM initialisation vector
/// \param[in] ivSizeBytes          Size of the initialisation vector
/// \param[in] ciphertext           Ciphertext to decrypt
/// \param[in] ciphertextSizeBytes  Size of the ciphertext
/// \param[in] aad                  Additional authenticated data
/// \param[in] aadSizeBytes         Size of the additional authenticated data
/// \param[in] tag                  Buffer for returned tag
/// \param[out] plaintext           Buffer for returned plaintext
///
/// \returns Size of the decrypted plaintext on success, -1 on failure
///
int Aes128GcmDecrypt(   uint8_t* const pKey,
                        uint8_t* const pIv,
                        size_t const ivSizeBytes,
                        uint8_t* const ciphertext,
                        size_t const ciphertextSizeBytes,
                        uint8_t* const aad,
                        size_t const aadSizeBytes,
                        uint8_t* const tag,
                        uint8_t* const plaintext)
{
    uint8_t* ciphertextBlock;
    uint8_t* plaintextBlock;
    int plaintextSizeBytes = 0;
    int decryptSizeBytes;
    int res;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
    if (0 >= res)
    {
        printLastError("EVP_DecryptInit_ex failed");
        return -1;
    }

    // Set the IV length
    res = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, ivSizeBytes, NULL);
    if (0 >= res)
    {
        printLastError("EVP_CIPHER_CTX_ctrl failed");
        return -1;
    }

    // Set the key and IV
    res = EVP_DecryptInit_ex(ctx, NULL, NULL, pKey, pIv);
    if (0 >= res)
    {
        printLastError("EVP_DecryptInit_ex(pKey, pIv) failed");
        return -1;
    }

    // Add the additional authenticated data to the digest
    res = EVP_DecryptUpdate(ctx, NULL, &decryptSizeBytes, aad, aadSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_DecryptUpdate(aad) failed");
        return -1;
    }

    // Decrypt each 16-byte block
    size_t numBlocks = ciphertextSizeBytes / CSS_AES_BLOCK_SIZE_BYTES;
    size_t remainder = ciphertextSizeBytes % CSS_AES_BLOCK_SIZE_BYTES;

    for (int i = 0; i < numBlocks; i++)
    {
        ciphertextBlock = &ciphertext[i * CSS_AES_BLOCK_SIZE_BYTES];
        plaintextBlock = &plaintext[i * CSS_AES_BLOCK_SIZE_BYTES];

        res = EVP_DecryptUpdate(ctx,
                                plaintextBlock,
                                &decryptSizeBytes,
                                ciphertextBlock,
                                CSS_AES_BLOCK_SIZE_BYTES);
        if (0 >= res)
        {
            printLastError("EVP_DecryptUpdate() failed");
            return -1;
        }
        if (verbose) printf("Block %d: decrypted %d bytes\n", i, decryptSizeBytes);

        plaintextSizeBytes += CSS_AES_BLOCK_SIZE_BYTES;
    }

    // Decrypt the remainder (if any)
    if (0 != remainder)
    {
        ciphertextBlock = &ciphertext[numBlocks * CSS_AES_BLOCK_SIZE_BYTES];
        plaintextBlock = &plaintext[numBlocks * CSS_AES_BLOCK_SIZE_BYTES];

        res = EVP_DecryptUpdate(ctx,
                                plaintextBlock,
                                &decryptSizeBytes,
                                ciphertextBlock,
                                remainder);
        if (0 >= res)
        {
            printLastError("EVP_DecryptUpdate() failed");
            return -1;
        }
        if (verbose) printf("Remainder: decrypted %d bytes\n", decryptSizeBytes);

        plaintextSizeBytes += remainder;
    }

    // Set the tag
    res = EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, CSS_MAC_SIZE_BYTES, tag);
    if (0 >= res)
    {
        printLastError("EVP_CIPHER_CTX_ctrl failed");
        return -1;
    }

    // Finalize the decryption (authenticate)
    plaintextBlock = &plaintext[(numBlocks * CSS_AES_BLOCK_SIZE_BYTES) + remainder];

    res = EVP_DecryptFinal_ex(ctx, plaintextBlock, &decryptSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_EncryptFinal_ex failed");
        return -1;
    }

    if (verbose)
    {
        printf("Decrypted %d bytes\n", plaintextSizeBytes);
        BIO_dump_fp(stdout, plaintext, plaintextSizeBytes);
    }

    return plaintextSizeBytes;
}

/// Encrypts supplied Video Authentication configuration params using AES-GCM
///
/// \param[in] configParams         Parameters to encrypt
/// \param[out] encryptedParams     Buffer for encrypted ciphertext
/// \param[out] mac                 Buffer for (16-byte) MAC
///
/// \returns Size of the encrypted parameters on success, -1 on failure
///
int EncryptConfigParams(CssVideoAuthConfigParamsT* const configParams,
                        uint8_t* const encryptedParams,
                        uint8_t* const mac)
{
    uint16_t aad = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    int encryptedSizeBytes;

    encryptedSizeBytes = Aes128GcmEncrypt(  (uint8_t* const)controlChannelKey,
                                            (uint8_t* const)controlChannelNonce,
                                            sizeof(controlChannelNonce),
                                            (uint8_t* const)configParams,
                                            sizeof(CssVideoAuthConfigParamsT),
                                            (uint8_t* const)&aad,
                                            sizeof(aad),
                                            encryptedParams,
                                            mac);
    return encryptedSizeBytes;
}

/// Decrypts supplied configuration params using AES-GCM
///
/// \param[in] encryptedParams      Parameters to decrypt
/// \param[in] mac                  Buffer containing (16-byte) MAC
/// \param[out] configParams        Buffer for decrypted parameters
///
/// \returns Size of the decrypted parameters on success, -1 on failure
///
int DecryptConfigParams(uint8_t* const encryptedParams,
                        uint8_t* const mac,
                        CssVideoAuthConfigParamsT* const configParams)
{
    uint16_t aad = HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI;
    int decryptedSizeBytes;

    decryptedSizeBytes = Aes128GcmDecrypt(  (uint8_t* const)controlChannelKey,
                                            (uint8_t* const)controlChannelNonce,
                                            sizeof(controlChannelNonce),
                                            encryptedParams,
                                            sizeof(CssVideoAuthConfigParamsT),
                                            (uint8_t* const)&aad,
                                            sizeof(aad),
                                            mac,
                                            (uint8_t* const)configParams);
    return decryptedSizeBytes;
}

/// Reads encrypted configuration parameters from specified filename and decrypts
///
/// \param[in] fileName     File containing the encrypted params and MAC
///
/// \returns 0 on success
///
int DecryptConfigParamsFromFile(char* const fileName)
{
    printf("Decrypting config params from file: %s\n", fileName);

    FILE* encryptedFile = fopen(fileName, "rb");
    if (NULL == encryptedFile)
    {
        printf("Failed to open file %s\n", fileName);
        return -1;
    }

    uint8_t encryptedConfig[sizeof(CssVideoAuthConfigParamsT)];
    uint8_t mac[CSS_MAC_SIZE_BYTES];

    int bytesRead = fread(&encryptedConfig, 1, sizeof(encryptedConfig), encryptedFile);
    if (bytesRead != sizeof(encryptedConfig))
    {
        printf("Failed to read encrypted config: %d\n", bytesRead);
        return -1;
    }

    bytesRead = fread(&mac, 1, sizeof(mac), encryptedFile);
    if (bytesRead != sizeof(mac))
    {
        printf("Failed to read mac: %d\n", bytesRead);
        return -1;
    }

    printf("Encrypted config:\n");
    BIO_dump_fp(stdout, (const char*)encryptedConfig, sizeof(encryptedConfig));
    printf("MAC:\n");
    BIO_dump_fp(stdout, (const char*)mac, sizeof(mac));

    CssVideoAuthConfigParamsT configParams;

    int decryptedSizeBytes = DecryptConfigParams(encryptedConfig,
                                                mac,
                                                &configParams);
    if (0 < decryptedSizeBytes)
    {
        printf("Decrypted %d bytes\n", decryptedSizeBytes);
        BIO_dump_fp(stdout, (const char*)&configParams, decryptedSizeBytes);
    }

    printf("videoAuthMode:  0x%04x\n", configParams.videoAuthMode);
    printf("pixelPackMode:  0x%04x\n", configParams.pixelPackMode);
    printf("pixelPackValue: 0x%04x\n", configParams.pixelPackValue);
    printf("firstRow:       0x%04x\n", configParams.roi.firstRow);
    printf("lastRow:        0x%04x\n", configParams.roi.lastRow);
    printf("rowSkip:        0x%04x\n", configParams.roi.rowSkip);
    printf("firstColumn:    0x%04x\n", configParams.roi.firstColumn);
    printf("lastColumn:     0x%04x\n", configParams.roi.lastColumn);
    printf("columnSkip:     0x%04x\n", configParams.roi.columnSkip);
    printf("frameCounter:   0x%08x%08x\n",
        (configParams.frameCounter & 0xFFFFFFFF00000000ULL) >> 32,
        (configParams.frameCounter & 0xFFFFFFFFULL));

    return 0;
}

int GcmEncryptDecryptTest(void)
{
    CssVideoAuthConfigParamsT configParams;
    uint8_t encryptedParams[sizeof(CssVideoAuthConfigParamsT)];
    CssVideoAuthConfigParamsT decryptedParams;
    uint8_t mac[CSS_MAC_SIZE_BYTES];
    int encryptedSizeBytes;
    int decryptedSizeBytes;

    printf("\n\nAES-GCM encrypt/decrypt test running\n");

    // Configure the parameters
    configParams.videoAuthMode = CSS_VIDEO_AUTH_MODE_ROI;
    configParams.pixelPackMode = CSS_PIXEL_PACKING_MODE_OPTIMIZED;
    configParams.pixelPackValue = 0x55U;
    configParams.roi.firstRow = 0x0010U;
    configParams.roi.lastRow = 0x0100U;
    configParams.roi.rowSkip = 0x0008U;
    configParams.roi.firstColumn = 0x0020U;
    configParams.roi.lastColumn = 0x0200U;
    configParams.roi.columnSkip = 0x0010U;
    configParams.frameCounter = 0x1234000100020003ULL;

    printf("Config params\n");
    BIO_dump_fp(stdout, (const char*)&configParams, sizeof(configParams));

    encryptedSizeBytes = EncryptConfigParams(   &configParams,
                                                encryptedParams,
                                                mac);
    if (0 < encryptedSizeBytes)
    {
        printf("Encrypted %d bytes\n", encryptedSizeBytes);
    }

    decryptedSizeBytes = DecryptConfigParams(   encryptedParams,
                                                mac,
                                                &decryptedParams);
    if (0 < decryptedSizeBytes)
    {
        printf("Decrypted %d bytes\n", decryptedSizeBytes);
        BIO_dump_fp(stdout, (const char*)&decryptedParams, decryptedSizeBytes);
    }

    return 0;
}

int main(int argc, void * argv[])
{
    printf("\n======== OpenSSL AES-GCM encrypt/decrypt demo running ========\n");

    GcmEncryptDecryptTest();
    DecryptConfigParamsFromFile("doc/UserGuideExamples/output/aes_gcm_ciphertext.bin");

    return 0;
}
