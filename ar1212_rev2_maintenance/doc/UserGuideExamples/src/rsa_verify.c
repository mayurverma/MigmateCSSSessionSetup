//***********************************************************************************
/// \file
///
/// Example RSA signing/verify functions definition
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

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
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
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00
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

/// Reverses the byte-order of the supplied object
///
/// out and in can point to the same data object
///
/// \param[in] out          Buffer for reversed data object
/// \param[in] in           Data object to be reversed
/// \param[in] sizeBytes    Size of the data object
///
/// \returns void
///
void reverseBytes(uint8_t* const out, uint8_t* const in, size_t const sizeBytes)
{
    int topPos = sizeBytes - 1;

    if (out != in)
    {
        for (int i = 0; i < sizeBytes; i++, topPos--)
        {
            out[i] = in[topPos];
        }
    }
    else
    {
        for (int i = 0; i < sizeBytes / 2U; i++, topPos--)
        {
            uint8_t tmp = in[i];
            in[i] = in[topPos];
            in[topPos] = tmp;
        }
    }
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

/// Calculates the SHA256 digest of the supplied object
///
/// \param[in] objectData           reference to object
/// \param[in] objectDataSizeBytes  size of object
/// \param[out] digest              buffer for returned digest
///
/// \return Digest size on success, -1 of failure
///
int GetSha256Digest(uint8_t* const objectData,
                    size_t const objectDataSizeBytes,
                    uint8_t* const digest)
{
    size_t digestSizeBytes;
    int res;

    EVP_MD const * md = EVP_sha256();
    if (NULL == md)
    {
        printf("Failed to get EVP_sha256 hash func\n");
        return -1;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (NULL == mdctx)
    {
        printLastError("EVP_MD_CTX_new failed");
        return -1;
    }

    res = EVP_DigestInit_ex(mdctx, md, NULL);
    if (0 >= res)
    {
        printLastError("EVP_DigestInit_ex failed");
        return -1;
    }

    res = EVP_DigestUpdate(mdctx, objectData, objectDataSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_DigestUpdate failed");
        return -1;
    }

    res = EVP_DigestFinal_ex(mdctx, digest, (unsigned int*)&digestSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_DigestFinal_ex failed");
        return -1;
    }

    EVP_MD_CTX_free(mdctx);

    return (int)digestSizeBytes;
}

/// Signs the supplied data object using the supplied key
///
/// \param[in] pKey                 EVP_PKEY object representing the signing private key
/// \param[in] dataObject           The data object to be signed
/// \param[in] dataObjectSizeBytes  Size of the data object in bytes
/// \param[out] signature           Returned signature
///
/// \returns Size of the signature on success, -1 on failure
///
int RsaSign(EVP_PKEY* const pKey,
            uint8_t* const dataObject,
            size_t const dataObjectSizeBytes,
            uint8_t** signature)
{
    int res;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pKey, NULL);
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_PKEY_sign_init(ctx);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_sign_init failed");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PSS_PADDING);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_padding failed");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_pss_saltlen(ctx, RSA_PSS_SALTLEN_DIGEST);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_pss_saltlen failed");
        return -1;
    }

    res = EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256());
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_padding failed");
        return -1;
    }

    // Determine the signature buffer length
    size_t signatureSizeBytes;

    res = EVP_PKEY_sign(ctx,
                        NULL,
                        &signatureSizeBytes,
                        (unsigned const char*)dataObject,
                        dataObjectSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_sign failed (buffer length)");
        return -1;
    }

    *signature = OPENSSL_malloc(signatureSizeBytes);
    if (NULL == *signature)
    {
        printf("Failed to alloc buffer for signature\n");
        return -1;
    }

    res = EVP_PKEY_sign(ctx,
                        *signature,
                        &signatureSizeBytes,
                        (unsigned const char*)dataObject,
                        dataObjectSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_sign failed");
        return -1;
    }

    EVP_PKEY_CTX_free(ctx);

    return signatureSizeBytes;
}

/// Verifies the signed data object using the supplied key
///
/// \param[in] pKey                 EVP_PKEY object representing the signing public key
/// \param[in] dataObject           The dataObject to be verified
/// \param[in] dataObjectSizeBytes  Size of the dataObject in bytes
/// \param[in] signature            The signature
/// \param[in] signatureSizeBytes   Size of the signature
///
/// \returns 1 on success, 0 on verify failure, -1 on other failure
///
int RsaVerify(  EVP_PKEY* const pKey,
                uint8_t* const dataObject,
                size_t const dataObjectSizeBytes,
                uint8_t* const signature,
                size_t const signatureSizeBytes)
{
    int res;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pKey, NULL);
    if (NULL == ctx)
    {
        printf("Failed to get ctx\n");
        return -1;
    }

    res = EVP_PKEY_verify_init(ctx);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_verify_init failed\n");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PSS_PADDING);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_padding failed\n");
        return -1;
    }

    res = EVP_PKEY_CTX_set_rsa_pss_saltlen(ctx, RSA_PSS_SALTLEN_DIGEST);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_rsa_pss_saltlen failed");
        return -1;
    }

    res = EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256());
    if (0 >= res)
    {
        printLastError("EVP_PKEY_CTX_set_signature_md failed\n");
        return -1;
    }

    res = EVP_PKEY_verify(  ctx,
                            (const unsigned char*)signature,
                            signatureSizeBytes,
                            (const unsigned char*)dataObject,
                            dataObjectSizeBytes);
    if (0 >= res)
    {
        printLastError("EVP_PKEY_verify failed\n");
        return res;
    }

    EVP_PKEY_CTX_free(ctx);

    return 1;
}

/// Signs a public certificate using the supplied signing key
///
/// \param[in] pKey         Signing key
/// \param[in] certificate  Certificate to be signed
///
/// \returns Signature size on success, -1 on failure
///
int SignCertificate(EVP_PKEY* const pKey, CssRsa2048PubCertT* const certificate)
{
    uint8_t digest[CSS_SHA256_DIGEST_SIZE_BYTES];
    size_t const signedObjectSizeBytes =
        sizeof(certificate->header) + sizeof(certificate->publicKey);
    int digestSizeBytes;
    int signatureSizeBytes;
    uint8_t* signature;
    int res;

    // Get the sha256 digest of the signed part of the certificate
    digestSizeBytes = GetSha256Digest((uint8_t* const)certificate,
        signedObjectSizeBytes, digest);
    if (0 < digestSizeBytes)
    {
        printf("Digest: (size %d)\n", digestSizeBytes);
        if (verbose) BIO_dump_fp(stdout, digest, (size_t)digestSizeBytes);
    }
    else
    {
        return digestSizeBytes;
    }

    // Sign the digest
    signatureSizeBytes = RsaSign(   pKey,
                                    (uint8_t* const)digest,
                                    (size_t)digestSizeBytes,
                                    &signature);
    if (0 < signatureSizeBytes)
    {
        printf("Signature: (size %d)\n", signatureSizeBytes);
        if (verbose) BIO_dump_fp(stdout, signature, (size_t)signatureSizeBytes);

        // reverse the certificate signature
        reverseBytes(   certificate->signature,
                        signature,
                        signatureSizeBytes);

        OPENSSL_free(signature);
    }

    return signatureSizeBytes;
}

/// Verifies a public certificate using the supplied key
///
/// \param[in] pKey         Key to verify signature
/// \param[in] certificate  Certificate to verify
///
/// \returns 1 on success
///
int VerifyCertificate(CssRsa2048PubCertT* const certificate, EVP_PKEY* const pKey)
{
    uint8_t digest[CSS_SHA256_DIGEST_SIZE_BYTES];
    size_t const signedObjectSizeBytes =
        sizeof(certificate->header) + sizeof(certificate->publicKey);
    int digestSizeBytes;
    int res;

    // Get the sha256 digest of the signed part of the certificate
    digestSizeBytes = GetSha256Digest((uint8_t* const)certificate,
        signedObjectSizeBytes, digest);
    if (0 < digestSizeBytes)
    {
        printf("Digest: (size %d)\n", digestSizeBytes);
        if (verbose) BIO_dump_fp(stdout, digest, (size_t)digestSizeBytes);
    }

    // reverse the certificate signature
    reverseBytes(   certificate->signature,
                    certificate->signature,
                    sizeof(certificate->signature));

    res = RsaVerify(pKey,
                    (uint8_t* const)digest,
                    digestSizeBytes,
                    (uint8_t* const)certificate->signature,
                    sizeof(certificate->signature));
    if (0 < res)
    {
        if (verbose) printf("Signature verified\n");
    }

    return res;
}

/// Simple test that signs a certificate and then verifies the signature
///
/// \returns 0
///
int signVerifyTest(void)
{
    printf("\nRSA signature/verification test running\n\n");

    CssRsa2048PubCertT publicCertificate;
    int signatureSizeBytes;
    int res;

    // Initialise the certificate
    publicCertificate.header.typeIdentifier = 0x4332U;
    publicCertificate.header.signatureOffsetLongWords = 0x42U;
    publicCertificate.header.certificateFlags = 0U;
    memcpy(publicCertificate.publicKey, rsa2048Modulus, sizeof(publicCertificate.publicKey));
    memset(publicCertificate.signature, 0, sizeof(publicCertificate.signature));

    BIO_dump_fp(stdout, (const char*)&publicCertificate, 8U + sizeof(publicCertificate.publicKey));

    // Get the private key from the modulus and exponent byte arrays
    EVP_PKEY* pKey = GetPrivateRsaKey(  rsa2048Modulus,
                                        sizeof(rsa2048Modulus) * 8U,
                                        rsa2048Exponent);
    if (NULL == pKey) return -1;

    signatureSizeBytes = SignCertificate(pKey, &publicCertificate);
    if (0 >= signatureSizeBytes)
    {
        printf("Failed to sign certificate: %d\n", signatureSizeBytes);
        return signatureSizeBytes;
    }

    EVP_PKEY_free(pKey);

    // Get the public key from the modulus byte array
    pKey = GetPublicRsaKey( rsa2048Modulus,
                            sizeof(rsa2048Modulus) * 8U);
    if (NULL == pKey) return -1;

    res = VerifyCertificate(&publicCertificate, pKey);
    if (0 < res)
    {
        printf("Signature verified\n");
    }

    EVP_PKEY_free(pKey);
    return 0;
}

/// Reads and verifies the signature of a certificate stored in a file
///
/// \param[in] fileName     file containing the certificate
///
/// \returns 0 on success
///
int readAndVerifyCertificate(char* const fileName)
{
    CssRsa2048PubCertT certificate;
    FILE* certFile;

    printf("\nReading and verifying certificate from file test\n\n");
    printf("Opening %s\n", fileName);

    certFile = fopen(fileName, "rb");
    if (NULL == certFile)
    {
        printf("Failed to open %s\n", fileName);
        return -1;
    }

    int bytesRead = fread(&certificate, 1, sizeof(certificate), certFile);
    if (bytesRead != sizeof(certificate))
    {
        printf("Failed to read certificate: %d\n", bytesRead);
        return -1;
    }

    fclose(certFile);

    printf("Certificate:\n");
    BIO_dump_fp(stdout, (const char*)&certificate, sizeof(certificate));

    // Get the public key from the modulus byte array
    EVP_PKEY* pKey = GetPublicRsaKey(   certificate.publicKey,
                                        sizeof(certificate.publicKey) * 8U);
    if (NULL == pKey) return -1;

    int res = VerifyCertificate(&certificate, pKey);
    if (0 < res)
    {
        printf("Signature verified\n");
    }

    EVP_PKEY_free(pKey);
    return 0;
}

int main(int argc, void * argv[])
{
    printf("\n======== OpenSSL RSA verify test running ========\n");

    signVerifyTest();

    // Read and verify an independent certificate
    return readAndVerifyCertificate("doc/UserGuideExamples/output/rsa_cert.bin");
}
