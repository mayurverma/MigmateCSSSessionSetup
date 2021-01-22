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

#include "mbedtls_cc_util_asset_prov.h"
#include "sha256_alt.h"
#include "rsa.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "ccm_alt.h"
#include "gcm_alt.h"
#include "mbedtls_cc_hkdf.h"
}


// Mocks
extern "C"
{
    CCError_t mbedtls_util_asset_pkg_unpack(
                CCAssetProvKeyType_t        keyType,
                uint32_t                    assetId,
                uint32_t                    *pAssetPackage,
                size_t                      assetPackageLen,
                uint32_t                    *pAssetData,
                size_t                      *pAssetDataLen
                )
    {
        mock().actualCall("mbedtls_util_asset_pkg_unpack")
            .withParameter("keyType", keyType)
            .withParameter("assetId", assetId)
            .withParameter("pAssetPackage", pAssetPackage)
            .withParameter("assetPackageLen", assetPackageLen)
            .withOutputParameter("pAssetData", pAssetData)
            .withOutputParameter("pAssetDataLen", pAssetDataLen);

        return (CCError_t)mock().returnUnsignedIntValueOrDefault((CCError_t)CC_OK);
    }


    int mbedtls_ctr_drbg_random( void *p_rng,
                         unsigned char *output, size_t output_len )
    {
        CHECK_TRUE(NULL != p_rng);
        CHECK_TRUE(NULL != output);
        (void)output_len;
        mock().actualCall("mbedtls_ctr_drbg_random")
            .withParameter("output_len", output_len);
        return (int)mock().returnIntValueOrDefault(0);
    }

    int mbedtls_rsa_import_raw( mbedtls_rsa_context *ctx,
                                unsigned char const *N, size_t N_len,
                                unsigned char const *P, size_t P_len,
                                unsigned char const *Q, size_t Q_len,
                                unsigned char const *D, size_t D_len,
                                unsigned char const *E, size_t E_len )
   {
        CHECK_TRUE(NULL != ctx);
        CHECK_TRUE(NULL != N);
        CHECK_TRUE(NULL != D);
        CHECK_TRUE(NULL != E);
        POINTERS_EQUAL(NULL, P);
        POINTERS_EQUAL(NULL, Q);
        CHECK_EQUAL(3, E_len); // always {01U, 00U, 01U} for AR0820
        CHECK_EQUAL(0, P_len);
        CHECK_EQUAL(0, Q_len);
        mock().actualCall("mbedtls_rsa_import_raw")
            .withParameter("N_len", N_len)
            .withParameter("D_len", D_len);
        return (int)mock().returnIntValueOrDefault(0);
    }

    int mbedtls_rsa_complete( mbedtls_rsa_context *ctx )
    {
        CHECK_TRUE(NULL != ctx);
        mock().actualCall("mbedtls_rsa_complete");
        return (int)mock().returnIntValueOrDefault(0);
    }

    int mbedtls_rsa_pkcs1_decrypt( mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           int mode, size_t *olen,
                           const unsigned char *input,
                           unsigned char *output,
                           size_t output_max_len )
    {
        CHECK_TRUE(NULL != ctx);
        POINTERS_EQUAL(mbedtls_ctr_drbg_random, f_rng);
        CHECK_TRUE(NULL != p_rng);
        CHECK_TRUE(NULL != olen);
        CHECK_TRUE(NULL != input);
        CHECK_TRUE(NULL != output);
        CHECK_EQUAL(MBEDTLS_RSA_PRIVATE, mode);
        mock().actualCall("mbedtls_rsa_pkcs1_decrypt")
            .withParameter("output_max_len", output_max_len)
            .withOutputParameter("olen", olen);
        return (int)mock().returnIntValueOrDefault(0);
    }

    void mbedtls_rsa_init( mbedtls_rsa_context *ctx,
                           int padding,
                           int hash_id)
   {
        CHECK_TRUE(NULL != ctx);
        CHECK_EQUAL(MBEDTLS_RSA_PKCS_V21, padding);
        CHECK_EQUAL(MBEDTLS_MD_SHA256, hash_id);
        mock().actualCall("mbedtls_rsa_init");
    }

    void mbedtls_rsa_free( mbedtls_rsa_context *ctx )
    {
        CHECK_TRUE(NULL != ctx);
        mock().actualCall("mbedtls_rsa_free");
    }

    void mbedtls_cipher_init( mbedtls_cipher_context_t *ctx )
    {
        CHECK_TRUE(NULL != ctx);
        mock().actualCall("mbedtls_cipher_init");
    }
    void mbedtls_cipher_free( mbedtls_cipher_context_t *ctx )
    {
        CHECK_TRUE(NULL != ctx);
        mock().actualCall("mbedtls_cipher_free");
    }

    mbedtls_cipher_info_t cipher_info_default;

    const mbedtls_cipher_info_t *mbedtls_cipher_info_from_values( const mbedtls_cipher_id_t cipher_id,
                                                  int key_bitlen,
                                                  const mbedtls_cipher_mode_t mode )
    {
        CHECK_EQUAL(MBEDTLS_CIPHER_ID_AES, cipher_id);
        mock().actualCall("mbedtls_cipher_info_from_values")
            .withParameter("key_bitlen", key_bitlen)
            .withParameter("mode", mode);

        return (const mbedtls_cipher_info_t *)mock().returnConstPointerValueOrDefault(&cipher_info_default);

    }

    int mbedtls_cipher_setup( mbedtls_cipher_context_t *ctx, const mbedtls_cipher_info_t *cipher_info )
    {
        CHECK_TRUE(NULL != ctx);
        CHECK_TRUE(NULL != cipher_info);
        mock().actualCall("mbedtls_cipher_setup");
        return (int)mock().returnIntValueOrDefault(0);
    }

    int mbedtls_cipher_setkey( mbedtls_cipher_context_t *ctx, const unsigned char *key,
                       int key_bitlen, const mbedtls_operation_t operation )
    {
        CHECK_TRUE(NULL != ctx);
        CHECK_TRUE(NULL != key);
        CHECK_EQUAL(MBEDTLS_DECRYPT, operation);
        mock().actualCall("mbedtls_cipher_setkey")
            .withParameter("key_bitlen", key_bitlen);

        return (int)mock().returnIntValueOrDefault(0);
    }

    int mbedtls_cipher_auth_decrypt( mbedtls_cipher_context_t *ctx,
                             const unsigned char *iv, size_t iv_len,
                             const unsigned char *ad, size_t ad_len,
                             const unsigned char *input, size_t ilen,
                             unsigned char *output, size_t *olen,
                             const unsigned char *tag, size_t tag_len )
    {
        CHECK_TRUE(NULL != ctx);
        CHECK_TRUE(NULL != iv);
        CHECK_TRUE(NULL != ad);
        CHECK_TRUE(NULL != input);
        CHECK_TRUE(NULL != output);
        CHECK_TRUE(NULL != tag);
        CHECK_TRUE(NULL != olen);
        CHECK_EQUAL(12, iv_len);
        CHECK_EQUAL(16, tag_len);
        CHECK_EQUAL(ilen, *olen);

        mock().actualCall("mbedtls_cipher_auth_decrypt")
            .withParameter("ad_len", ad_len)
            .withParameter("ilen", ilen)
            .withOutputParameter("olen", olen);

        return (int)mock().returnIntValueOrDefault(0);
    }

    CCError_t  mbedtls_hkdf_key_derivation(
                mbedtls_hkdf_hashmode_t    HKDFhashMode,
                uint8_t*                   Salt_ptr,
                size_t                     SaltLen,
                uint8_t*                   Ikm_ptr,
                uint32_t                   IkmLen,
                uint8_t*                   Info,
                uint32_t                   InfoLen,
                uint8_t*                   Okm,
                uint32_t                   OkmLen,
                CCBool                     IsStrongKey)
    {
        CHECK_EQUAL(CC_HKDF_HASH_SHA256_mode, HKDFhashMode);
        CHECK_TRUE(NULL != Salt_ptr);
        CHECK_TRUE(NULL != Ikm_ptr);
        CHECK_TRUE(NULL != Info);
        CHECK_TRUE(NULL != Okm);
        CHECK_EQUAL(CC_FALSE, IsStrongKey);
        mock().actualCall("mbedtls_hkdf_key_derivation")
            .withParameter("SaltLen", SaltLen)
            .withParameter("IkmLen", IkmLen)
            .withParameter("InfoLen", InfoLen)
            .withParameter("OkmLen", OkmLen);

        return (CCError_t)mock().returnUnsignedIntValueOrDefault(CC_OK);
    }
}

