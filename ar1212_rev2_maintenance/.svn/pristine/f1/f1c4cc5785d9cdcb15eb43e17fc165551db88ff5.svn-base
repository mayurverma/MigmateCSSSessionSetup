//***********************************************************************************
/// \file
///
/// Example RSA encryption/decryption functions definition
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

#include <arpa/inet.h>      // for htonl()

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>

#include "../inc/css_host_command.h"

static bool verbose = false;

/// 2048-bit RSA public modulus (default key)
uint8_t const rsa2048Modulus[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES] = {
          0xd7, 0xeb, 0x75, 0x92, 0x4d, 0xf2, 0x23,
    0xcd, 0xf1, 0x6f, 0x0e, 0x49, 0x83, 0xf3, 0xb1,
    0xb5, 0xf8, 0x07, 0x5e, 0x74, 0xbf, 0x59, 0x8a,
    0xfb, 0xcf, 0x46, 0x0b, 0xae, 0x2e, 0xf3, 0x20,
    0xe7, 0xc1, 0x62, 0x82, 0xa2, 0xe4, 0x2f, 0x3c,
    0xe4, 0xb4, 0xd4, 0x1e, 0xfa, 0x6c, 0x91, 0xb0,
    0x68, 0x8a, 0x56, 0x24, 0x9a, 0x06, 0x7b, 0xc1,
    0xba, 0xa8, 0xce, 0x22, 0x9c, 0x7a, 0xb2, 0x94,
    0xfc, 0x3b, 0x90, 0x4f, 0x36, 0x6d, 0x5c, 0x9d,
    0x9b, 0x1b, 0xbc, 0x25, 0x6a, 0x3b, 0x8e, 0xbb,
    0xe5, 0x9b, 0x25, 0xc8, 0x1e, 0x1a, 0x9a, 0xfa,
    0x0b, 0xcc, 0x12, 0x50, 0x17, 0xbd, 0x90, 0x6e,
    0x3b, 0x43, 0xf3, 0xc4, 0x49, 0xad, 0xe5, 0x85,
    0x4e, 0x03, 0x93, 0xc8, 0x16, 0x4b, 0x5a, 0x7d,
    0xc3, 0x33, 0x98, 0xd6, 0xf2, 0x2b, 0x28, 0x1f,
    0xc9, 0xbe, 0xa9, 0x96, 0x20, 0x38, 0xbf, 0x20,
    0xad, 0xe1, 0x4c, 0x82, 0x2f, 0x1e, 0xc4, 0x43,
    0x9f, 0xe0, 0x08, 0xa7, 0x99, 0xe5, 0x31, 0x52,
    0xa3, 0x77, 0x4a, 0x9d, 0x57, 0x17, 0x52, 0x37,
    0x1b, 0x3e, 0xf3, 0x89, 0x03, 0x8c, 0x17, 0x52,
    0x93, 0xed, 0x68, 0x06, 0x8a, 0xea, 0x7b, 0x9a,
    0x07, 0x37, 0x88, 0x38, 0xc5, 0x29, 0x77, 0xd1,
    0x9e, 0xa3, 0x06, 0x83, 0x77, 0xc2, 0x39, 0xfb,
    0x10, 0x26, 0x80, 0xd4, 0xc6, 0x02, 0xc6, 0x74,
    0xd5, 0xcd, 0x2b, 0x19, 0x88, 0x7d, 0xcc, 0xb8,
    0x45, 0xcb, 0x06, 0xfc, 0x3c, 0x26, 0xb2, 0xd8,
    0x7e, 0xcc, 0xd7, 0x18, 0xed, 0x76, 0x96, 0x24,
    0x90, 0x34, 0x4a, 0x4d, 0x4a, 0x25, 0xbf, 0x63,
    0xdb, 0xb1, 0x84, 0x91, 0x6c, 0xcb, 0xe8, 0x7b,
    0x6a, 0x30, 0x6f, 0xbb, 0x2b, 0x28, 0x77, 0x3f,
    0x17, 0x0a, 0x11, 0x3a, 0xa9, 0x9f, 0xef, 0x9c,
    0x18, 0xe9, 0xce, 0xd5, 0x86, 0xfa, 0xea, 0xc8,
    0x8d
};

/// 2048-bit RSA private exponent (default key)
uint8_t const rsa2048Exponent[CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES] = {
    0x78, 0x0d, 0x84, 0x2e, 0x97, 0xc6, 0x35, 0x60,
    0x31, 0x88, 0x75, 0x22, 0x2d, 0x0a, 0xd7, 0x59,
    0x7d, 0x11, 0xe3, 0xd3, 0xf7, 0xfc, 0xba, 0x89,
    0x2e, 0x03, 0x1d, 0x45, 0x78, 0xf7, 0x57, 0xe9,
    0x64, 0x1d, 0xf8, 0xd2, 0xc7, 0x1a, 0xf8, 0xb9,
    0x95, 0x4d, 0x87, 0xf7, 0xe7, 0x5b, 0xce, 0x01,
    0x22, 0x41, 0xa9, 0x52, 0x36, 0x9e, 0xa2, 0xe5,
    0xbd, 0x2b, 0xf8, 0x14, 0x97, 0x28, 0x2a, 0x2c,
    0x60, 0x95, 0xe9, 0x65, 0x32, 0xc3, 0x57, 0x22,
    0x41, 0xf6, 0x87, 0xbb, 0x1c, 0x68, 0x4d, 0x52,
    0xe9, 0xc3, 0xc5, 0x13, 0xef, 0xe9, 0x11, 0x74,
    0x73, 0xc0, 0x0e, 0xe6, 0x54, 0x51, 0x34, 0xe9,
    0x20, 0xec, 0x2d, 0xcc, 0x65, 0xb3, 0x03, 0x3e,
    0x90, 0x40, 0x9f, 0xf4, 0xbd, 0x6b, 0x10, 0x17,
    0xf3, 0xdd, 0x5c, 0x4e, 0x03, 0xee, 0xe8, 0x55,
    0xcc, 0xfc, 0x0f, 0xeb, 0xd4, 0x81, 0x78, 0x92,
    0x0f, 0xde, 0xf2, 0xbc, 0x4c, 0x41, 0x0d, 0xdc,
    0xfa, 0xb2, 0x39, 0xde, 0xe9, 0x9e, 0x46, 0xf4,
    0x8c, 0x82, 0x4f, 0x91, 0xc7, 0x25, 0x1b, 0x9b,
    0x02, 0x6f, 0x01, 0xb4, 0xa1, 0xec, 0x2e, 0xea,
    0xb5, 0x64, 0xd8, 0x5a, 0x1e, 0x3b, 0x0b, 0xad,
    0x5f, 0x02, 0x74, 0x16, 0xa5, 0x9c, 0xed, 0x14,
    0x48, 0xb5, 0xad, 0x62, 0xbc, 0x89, 0xcc, 0x13,
    0x3d, 0x33, 0x7e, 0x4c, 0x74, 0x45, 0xc4, 0x73,
    0x40, 0xe8, 0x86, 0x52, 0x91, 0xf7, 0x3a, 0xba,
    0x95, 0x7e, 0x34, 0x5d, 0x99, 0xaa, 0xfe, 0xb6,
    0x3f, 0x23, 0x03, 0x51, 0x40, 0x14, 0xb3, 0xc8,
    0x1f, 0x3d, 0xeb, 0x43, 0x2c, 0x39, 0xb8, 0x0f,
    0x8f, 0xce, 0xe4, 0xa6, 0x88, 0xd3, 0xd3, 0x0c,
    0x04, 0x97, 0x8a, 0x5d, 0x2d, 0xde, 0xca, 0x78,
    0xaf, 0xe0, 0x1d, 0x6b, 0x25, 0xe4, 0x77, 0x1f,
    0xe1, 0xbb, 0xaa, 0xcc, 0xdf, 0x5c, 0x9e, 0x21
};

/// Default master secret
uint8_t const fixedSecret[] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50
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

/// Returns a (RSA public) EVP_PKEY object generated from the RSA modulus
///
/// \param[in] rsaModulus           The RSA modulus (as byte array)
/// \param[in] rsaModulusSizeBits   Size of the modulus in bits
///
/// \returns EVP_PKEY object on success, NULL on error
///
EVP_PKEY* GetPublicRsaKey(  int8_t const * const rsaModulus,
                            size_t const rsaModulusSizeBits)
{
    // Use a fixed public exponent
    uint32_t const rsaPubExp = htonl(65537);
    int res;

    // Convert binary key modulus into a BIGNUM
    BIGNUM* bnPubModulus = BN_bin2bn((const unsigned char*)rsaModulus,
                                     rsaModulusSizeBits / 8U,
                                     NULL);
    if (NULL == bnPubModulus)
    {
        printLastError("BN_bin2bn failed for bnPubModulus");
        return NULL;
    }

    // Convert binary pub key exponent into a BIGNUM
    BIGNUM* bnPubExponent = BN_bin2bn((const unsigned char*)&rsaPubExp,
                                      sizeof(rsaPubExp),
                                      NULL);
    if (NULL == bnPubExponent)
    {
        printLastError("BN_bin2bn failed for bnPubExponent");
        return NULL;
    }

    // Create the public key object
    RSA* rsaPubKey = RSA_new();
    if (NULL == rsaPubKey)
    {
        printLastError("RSA_new failed");
        return NULL;
    }

    // Import the public modulus and exponent
    res = RSA_set0_key(rsaPubKey, bnPubModulus, bnPubExponent, NULL);
    if (0 == res)
    {
        printLastError("RSA_set0_key failed");
        return NULL;
    }

    // Convert the RSA key to a PKEY object
    EVP_PKEY* pKey = EVP_PKEY_new();
    if (NULL == pKey)
    {
        printLastError("EVP_PKEY_new failed");
        return NULL;
    }

    res = EVP_PKEY_assign_RSA(pKey, rsaPubKey);
    if (0 == res)
    {
        printLastError("EVP_PKEY_assign_RSA failed");
        return NULL;
    }

    if (verbose)
    {
        BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
        EVP_PKEY_print_public(outbio, pKey, 0, NULL);
    }

    return pKey;
}

/// Returns a (RSA private) EVP_PKEY object generated from the RSA modulus and exponent
///
/// \param[in] rsaModulus           The RSA modulus (as byte array)
/// \param[in] rsaModulusSizeBits   Size of the modulus in bits
/// \param[in] rsaExponent          The RSA private exponent (as byte array)
///
/// \returns EVP_PKEY object on success, NULL on error
///
EVP_PKEY* GetPrivateRsaKey( uint8_t const * const rsaModulus,
                            size_t const rsaModulusSizeBits,
                            uint8_t const * const rsaExponent)
{
    uint32_t const rsaPubExp = htonl(65537);
    int res;

    // Convert binary key modulus into a BIGNUM
    BIGNUM* bnPubModulus = BN_bin2bn((const unsigned char*)rsaModulus,
                                     rsaModulusSizeBits / 8U,
                                     NULL);
    if (NULL == bnPubModulus)
    {
        printLastError("BN_bin2bn failed for bnPubModulus");
        return NULL;
    }

    // Convert binary key exponent into a BIGNUM
    BIGNUM* bnExponent = BN_bin2bn( (const unsigned char*)rsaExponent,
                                    rsaModulusSizeBits / 8U,
                                    NULL);
    if (NULL == bnExponent)
    {
        printLastError("BN_bin2bn failed for bnExponent");
        return NULL;
    }

    // Convert binary pub key exponent into a BIGNUM
    BIGNUM* bnPubExponent = BN_bin2bn((const unsigned char*)&rsaPubExp,
                                      sizeof(rsaPubExp),
                                      NULL);
    if (NULL == bnPubExponent)
    {
        printLastError("BN_bin2bn failed for bnPubExponent");
        return NULL;
    }

    // Create the private key object
    RSA* rsaKey = RSA_new();
    if (NULL == rsaKey)
    {
        printLastError("RSA_new failed");
        return NULL;
    }

    // Import the public modulus and exponent
    res = RSA_set0_key(rsaKey, bnPubModulus, bnPubExponent, bnExponent);
    if (0 == res)
    {
        printLastError("RSA_set0_key failed");
        return NULL;
    }

    // Convert the RSA key to a PKEY object
    EVP_PKEY* pKey = EVP_PKEY_new();
    if (NULL == pKey)
    {
        printLastError("EVP_PKEY_new failed");
        return NULL;
    }

    res = EVP_PKEY_assign_RSA(pKey, rsaKey);
    if (0 == res)
    {
        printLastError("EVP_PKEY_assign_RSA failed");
        return NULL;
    }

    if (verbose)
    {
        BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
        EVP_PKEY_print_private(outbio, pKey, 0, NULL);
    }

    return pKey;
}

/// Generates a new RSA keypair of specified bit-size
///
/// \param[in] keySizeBits  Key size in bits
///
/// \returns EVP_PKEY object on success, or NULL
///
EVP_PKEY* GenerateEvpKey(size_t const keySizeBits)
{
    EVP_PKEY_CTX *ctx;
    EVP_PKEY* pKey = NULL;
    int res;

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (NULL == ctx)
    {
        printLastError("EVP_PKEY_CTX_new_id failed\n");
        return NULL;
    }

    res = EVP_PKEY_keygen_init(ctx);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_keygen_init failed\n");
        return NULL;
    }

    res = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, keySizeBits);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_keygen_bits failed\n");
        return NULL;
    }

    res = EVP_PKEY_keygen(ctx, &pKey);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_keygen failed\n");
        return NULL;
    }

    if (verbose)
    {
        BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
        EVP_PKEY_print_private(outbio, pKey, 0, NULL);
    }

    return pKey;
}

/// Write an RSA public key to the specified file in PEM format
///
/// \param[in] pKey         public key to write
/// \param[in] fileName     file to write
///
/// \returns 0 on success
///
int WritePublicKeyToFile(EVP_PKEY* const pKey, char* const fileName)
{
    printf("Write PEM file: %s\n", fileName);

    FILE* pem_fd = fopen(fileName, "wb");
    if (NULL == pem_fd)
    {
        printf("Failed to open %s\n", fileName);
        return -1;
    }

    int res = PEM_write_PUBKEY(pem_fd, pKey);
    if (0 == res)
    {
        printLastError("PEM_write_PUBKEY failed");
        return -1;
    }

    fclose(pem_fd);
    return 0;
}

/// Writes an RSA private key tp the specified file in PEM format
///
/// \param[in] pKey         private key to write
/// \param[in] fileName     file to write
///
/// \returns EVP_PKEY object on success, NULL on failure
///
int WritePrivateKeyToFile(EVP_PKEY* const pKey, char* const fileName)
{
    printf("Writing PEM file: %s\n", fileName);

    FILE* pem_fd = fopen(fileName, "wb");
    if (NULL == pem_fd)
    {
        printf("Failed to open %s\n", fileName);
        return -1;
    }

    int res = PEM_write_PKCS8PrivateKey(pem_fd,
                                        pKey,
                                        NULL,
                                        NULL,
                                        0,
                                        NULL,
                                        NULL);
    if (0 == res)
    {
        printLastError("PEM_write_PKCS8PrivateKey failed");
        return -1;
    }

    fclose(pem_fd);
    return 0;
}

/// Encrypts the supplied plaintext using the supplied key
///
/// Uses the PCKS1 OAEP padding scheme with SHA-256 digest
///
/// \param[in] pKey                 EVP_PKEY object representing the public key
/// \param[in] plaintext            The plaintext to be encrypted
/// \param[in] plaintextSizeBytes   Size of the plaintext in bytes
/// \param[out] ciphertext          Returned encrypted ciphertext
///
/// \returns Size of the encrypted ciphertext on success, -1 on failure
///
int RsaEncrypt( EVP_PKEY* const pKey,
                uint8_t* const plaintext,
                size_t const plaintextSizeBytes,
                uint8_t** ciphertext)
{
    int res;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pKey, NULL);
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_PKEY_encrypt_init(ctx);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_encrypt_init failed");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_padding failed");
        return -1;
    }

    // use SHA-256 for the message digest algorithm
    res = EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_ctrl_str failed");
        return -1;
    }

    // Determine the encrypted buffer length
    size_t ciphertextSizeBytes;

    res = EVP_PKEY_encrypt( ctx,
                            NULL,
                            &ciphertextSizeBytes,
                            (unsigned const char*)plaintext,
                            plaintextSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_encrypt failed (buffer length)");
        return -1;
    }

    *ciphertext = OPENSSL_malloc(ciphertextSizeBytes);
    if (NULL == *ciphertext)
    {
        printf("Failed to alloc buffer for ciphertext\n");
        return -1;
    }

    res = EVP_PKEY_encrypt( ctx,
                            *ciphertext,
                            &ciphertextSizeBytes,
                            (unsigned const char*)plaintext,
                            plaintextSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_encrypt failed");
        return -1;
    }

    EVP_PKEY_CTX_free(ctx);

    return (int)ciphertextSizeBytes;
}

/// Decrypts the supplied ciphertext using the supplied key
///
/// Uses the PCKS1 OAEP padding scheme with SHA-256 digest
///
/// \param[in] pKey                 EVP_PKEY object representing the private key
/// \param[in] ciphertext           The ciphertext to be decrypted
/// \param[in] ciphertextSizeBytes  Size of the ciphertext in bytes
/// \param[out] plaintext           Returned decrypted plaintext
///
/// \returns Size of the decrypted plaintext on success, -1 on failure
///
int RsaDecrypt( EVP_PKEY* const pKey,
                uint8_t* const ciphertext,
                size_t const ciphertextSizeBytes,
                uint8_t** plaintext)
{
    int res;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pKey, NULL);
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_PKEY_decrypt_init(ctx);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_decrypt_init failed\n");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_padding failed\n");
        return -1;
    }

    // use SHA-256 for the message digest algorithm
    res = EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_ctrl_str failed");
        return -1;
    }

    size_t plaintextSizeBytes;

    res = EVP_PKEY_decrypt( ctx,
                            NULL,
                            &plaintextSizeBytes,
                            (const unsigned char*)ciphertext,
                            ciphertextSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_decrypt failed\n");
        return -1;
    }

    // Allocate buffer for the plaintext
    *plaintext = OPENSSL_malloc(plaintextSizeBytes);
    if (NULL == *plaintext)
    {
        printf("Failed to alloc plaintext buffer\n");
        return -1;
    }

    res = EVP_PKEY_decrypt( ctx,
                            *plaintext,
                            &plaintextSizeBytes,
                            (const unsigned char*)ciphertext,
                            ciphertextSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_decrypt failed\n");
        return -1;
    }

    EVP_PKEY_CTX_free(ctx);

    return (int)plaintextSizeBytes;
}

/// Encrypts the session parameters using the supplied public key
///
/// \param[in] pKey                         EVP_PKEY object representing the public key
/// \param[in] sessionParameters            The parameters to be encrypted
/// \param[in] sessionParametersSizeBytes   Size of the parameters in bytes
/// \param[out] encryptedParameters         Returned encrypted parameters
///
/// \returns Size of the encrypted parameters on success, -1 on failure
///
int EncryptSessionParameters(   EVP_PKEY* const pKey,
                                CssSessionParametersT* const sessionParameters,
                                size_t sessionParametersSizeBytes,
                                uint8_t** encryptedParameters)
{
    int encryptedParamsSizeBytes = RsaEncrypt(  pKey,
                                                (uint8_t* const)sessionParameters,
                                                sessionParametersSizeBytes,
                                                encryptedParameters);
    if (0 < encryptedParamsSizeBytes)
    {
        printf("Encrypted Params: (size %d)\n", encryptedParamsSizeBytes);
        if (verbose) BIO_dump_fp(stdout, *encryptedParameters,
            (size_t)encryptedParamsSizeBytes);
    }

    return encryptedParamsSizeBytes;
}

/// Decrypts the encrypted session parameters using the supplied private key
///
/// \param[in] pKey                         EVP_PKEY object representing the private key
/// \param[in] encryptedParameters          The parameters to be decrypted
/// \param[in] encryptedParametersSizeBytes Size of the encrypted parameters in bytes
/// \param[out] sessionParameters           Returned decrypted session parameters
///
/// \returns Size of the decrypted session parameters on success, -1 on failure
///
int DecryptSessionParameters(EVP_PKEY* const pKey,
                                uint8_t* const encryptedParameters,
                                size_t const encryptedParametersSizeBytes,
                                CssSessionParametersT** sessionParameters)
{
    int sessionParametersSizeBytes = RsaDecrypt(pKey,
                                                encryptedParameters,
                                                encryptedParametersSizeBytes,
                                                (uint8_t**)sessionParameters);
    if (0 < sessionParametersSizeBytes)
    {
        printf("Decrypted params (size %d)\n", sessionParametersSizeBytes);
    }

    return sessionParametersSizeBytes;
}

/// Writes supplied data object to a binary file
///
/// \param[in] dataObject
/// \param[in] dataObjectSizeBytes
/// \param[in] fileName
///
/// \returns 0 on success
///
int writeObjectToFile(  uint8_t* const dataObject,
                        size_t dataObjectSizeBytes,
                        char* const fileName)
{
    printf("Writing encrypted parameters to %s\n", fileName);

    FILE* encFile = fopen(fileName, "w+b");
    if (NULL == encFile)
    {
        printf("Failed to open %s\n", fileName);
        return -1;
    }

    int bytesWritten = fwrite(dataObject, 1, dataObjectSizeBytes, encFile);
    if (bytesWritten != dataObjectSizeBytes)
    {
        printf("Failed to write to file: %d\n", bytesWritten);
        return -1;
    }

    fclose(encFile);
    return 0;
}

/// Encrypts dummy session parameters using a generated RSA key
///
/// \returns 0
///
int generateEncryptedParameters(void)
{
    CssSessionParametersT sessionParameters;
    uint8_t* encryptedParameters;
    int encryptedParamsSizeBytes;
    int res;

    printf("\nGenerating encrypted parameters\n\n");

    // Initialise the session parameters
    sessionParameters.masterSecretKeyType = CSS_SYMMETRIC_KEY_AES_128;
    sessionParameters.pad = 0x1234U;
    memcpy(sessionParameters.masterSecret, fixedSecret, sizeof(sessionParameters.masterSecret));

    printf("Master Secret is:\n");
    BIO_dump_fp(stdout, sessionParameters.masterSecret, sizeof(sessionParameters.masterSecret));

    EVP_PKEY* pKey = GenerateEvpKey(2048U);
    if (NULL == pKey) return -1;
    if (verbose)
    {
        BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
        EVP_PKEY_print_private(outbio, pKey, 0, NULL);
    }

    res = WritePublicKeyToFile(pKey, "doc/UserGuideExamples/output/rsa2048_public.pem");
    if (0 != res) return res;

    res = WritePrivateKeyToFile(pKey, "doc/UserGuideExamples/output/rsa2048_private.pem");
    if (0 != res) return res;

    // Encrypt the session parameters
    encryptedParamsSizeBytes = EncryptSessionParameters(pKey,
                                                        &sessionParameters,
                                                        sizeof(sessionParameters),
                                                        &encryptedParameters);
    if (0 < encryptedParamsSizeBytes)
    {
        printf("Encrypted Params: (size %d)\n", encryptedParamsSizeBytes);
        BIO_dump_fp(stdout, encryptedParameters, (size_t)encryptedParamsSizeBytes);
    }

    EVP_PKEY_free(pKey);

    // Write the encrypted parameters to a file
    res = writeObjectToFile(encryptedParameters,
                            encryptedParamsSizeBytes,
                            "doc/UserGuideExamples/output/encParams.bin");
    if (0 != res) return -1;

    return 0;
}

/// Simple test that encrypts then decrypts dummy session parameters
///
/// \returns 0
///
int encryptDecryptTest(void)
{
    CssSessionParametersT sessionParameters;
    uint8_t* encryptedParameters;
    int encryptedParamsSizeBytes;
    CssSessionParametersT* decryptedParameters;
    int decryptedParametersSizeBytes;
    int res;

    printf("\nRunning Encrypt/Decrypt test\n\n");

    // Initialise the session parameters
    sessionParameters.masterSecretKeyType = CSS_SYMMETRIC_KEY_AES_128;
    sessionParameters.pad = 0x1234U;
    memcpy(sessionParameters.masterSecret, fixedSecret, sizeof(sessionParameters.masterSecret));

    printf("Master Secret is:\n");
    BIO_dump_fp(stdout, sessionParameters.masterSecret, sizeof(sessionParameters.masterSecret));

    // Get the public key from the modulus byte array
    EVP_PKEY* pKey = GetPublicRsaKey(rsa2048Modulus, sizeof(rsa2048Modulus) * 8U);
    if (NULL == pKey) return -1;

    // Encrypt the session parameters
    encryptedParamsSizeBytes = EncryptSessionParameters(pKey,
                                                        &sessionParameters,
                                                        sizeof(sessionParameters),
                                                        &encryptedParameters);
    if (0 < encryptedParamsSizeBytes)
    {
        printf("Encrypted Params: (size %d)\n", encryptedParamsSizeBytes);
        BIO_dump_fp(stdout, encryptedParameters, (size_t)encryptedParamsSizeBytes);
    }

    EVP_PKEY_free(pKey);

    // Get the private key from the modulus and exponent byte arrays
    pKey = GetPrivateRsaKey(rsa2048Modulus, sizeof(rsa2048Modulus) * 8U, rsa2048Exponent);
    if (NULL == pKey) return -1;

    // Decrypt the session parameters
    decryptedParametersSizeBytes = DecryptSessionParameters(pKey,
                                                            encryptedParameters,
                                                            encryptedParamsSizeBytes,
                                                            &decryptedParameters);
    if (0 < decryptedParametersSizeBytes)
    {
        printf("Decrypted params (size %d)\n", decryptedParametersSizeBytes);
        printf("\tKey type: %d\n", decryptedParameters->masterSecretKeyType);
        printf("\tPad: %d\n", decryptedParameters->pad);
        printf("\tMaster Secret is:\n");
        BIO_dump_fp(stdout, decryptedParameters->masterSecret, sizeof(decryptedParameters->masterSecret));
    }

    EVP_PKEY_free(pKey);
    OPENSSL_free(encryptedParameters);
    OPENSSL_free(decryptedParameters);

    return 0;
}

int main(int argc, void * argv[])
{
    printf("\n======== OpenSSL RSA encryption/decryption demo running ========\n");

    generateEncryptedParameters();
    encryptDecryptTest();

    return 0;
}
