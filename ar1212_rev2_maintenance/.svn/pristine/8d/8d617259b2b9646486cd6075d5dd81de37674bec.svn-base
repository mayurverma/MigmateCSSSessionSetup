//***********************************************************************************
/// \file
///
/// Example RSA key generation and key file write/read support functions
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

#include <arpa/inet.h>

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include "../inc/css_host_command.h"

static bool verbose = false;

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

/// Reads an RSA public key from the specified PEM format file
///
/// \param[in] fileName     file to read
///
/// \returns pointer to EVP_PKEY object or NULL
///
EVP_PKEY* ReadPublicKeyFromFile(char* const fileName)
{
    printf("Reading PEM file: %s\n", fileName);

    FILE* pem_fd = fopen(fileName, "rb");
    if (NULL == pem_fd)
    {
        printf("Failed to open %s\n", fileName);
        return NULL;
    }

    EVP_PKEY* pKey = EVP_PKEY_new();
    pKey = PEM_read_PUBKEY(pem_fd, &pKey, NULL, NULL);
    if (NULL == pKey)
    {
        printLastError("Read pubkey failed");
        return NULL;
    }

    fclose(pem_fd);

    if (verbose)
    {
        BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);
        EVP_PKEY_print_public(outbio, pKey, 0, NULL);
    }

    return pKey;
}

/// Reads an RSA private key from the specified PEM format file
///
/// \param[in] fileName     file to read
///
/// \returns EVP_PKEY object on success, NULL on failure
///
EVP_PKEY* ReadPrivateKeyFromFile(char* const fileName)
{
    printf("Reading PEM file: %s\n", fileName);

    FILE* pem_fd = fopen(fileName, "rb");
    if (NULL == pem_fd)
    {
        printf("Failed to open %s\n", fileName);
        return NULL;
    }

    EVP_PKEY* pKey = EVP_PKEY_new();
    pKey = PEM_read_PrivateKey(pem_fd, &pKey, NULL, NULL);
    if (NULL == pKey)
    {
        printLastError("Read PrivateKey failed");
        return NULL;
    }

    fclose(pem_fd);

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

int main(int argc, void * argv[])
{
    BIO* outbio = BIO_new_fp(stdout, BIO_NOCLOSE);

    printf("RSA keygen demo running\n");

    EVP_PKEY* pKey = GenerateEvpKey(CSS_RSA_2048_PUBLIC_KEY_SIZE_BYTES * 8U);
    if (NULL == pKey) return -1;
    printf("New RSA key:\n");
    EVP_PKEY_print_private(outbio, pKey, 0, NULL);

    int res = WritePublicKeyToFile(pKey, "doc/UserGuideExamples/output/rsa2048_public.pem");
    if (0 != res) return res;

    res = WritePrivateKeyToFile(pKey, "doc/UserGuideExamples/output/rsa2048_private.pem");
    if (0 != res) return res;

    EVP_PKEY_free(pKey);

    // Retrieve the public key
    pKey = ReadPublicKeyFromFile("doc/UserGuideExamples/output/rsa2048_public.pem");
    if (0 != res) return -1;
    printf("Read RSA public key:\n");
    EVP_PKEY_print_public(outbio, pKey, 0, NULL);
    EVP_PKEY_free(pKey);

    // Retrieve the private key
    pKey = ReadPrivateKeyFromFile("doc/UserGuideExamples/output/rsa2048_private.pem");
    if (0 != res) return -1;
    printf("Read RSA private key:\n");
    EVP_PKEY_print_private(outbio, pKey, 0, NULL);
    EVP_PKEY_free(pKey);

    return 0;
}
