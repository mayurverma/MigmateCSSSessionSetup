//***********************************************************************************
/// \file
///
/// Example HKDF function definition
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
#include <string.h>
#include <errno.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/kdf.h>

#include "../inc/css_host_command.h"

static bool verbose = true;

/// Size of the HKDF Info context
#define CSS_HKDF_INFO_CONTEXT_SIZE_BYTES    8U

/// Represents the HKDF 'salt' (512 bits)
typedef struct
{
    /// Host-generated salt
    uint8_t hostSalt[CSS_SALT_SIZE_BYTES];

    /// Sensor-generated salt
    uint8_t sensorSalt[CSS_SALT_SIZE_BYTES];
} CssHkdfSaltT;

/// Represents the 'Info' object for HKDF
typedef struct
{
    /// Context (sensor name)
    uint8_t context[CSS_HKDF_INFO_CONTEXT_SIZE_BYTES];

    /// ROM version
    uint16_t romVersion;

    /// Patch version
    uint16_t patchVersion;

    /// Unique sensor identifier
    CssSensorIdT sensorId;
} CssHkdfInfoT;

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

/// Derives keys using the HMAC Key Derivation Function
///
/// Uses the default EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND mode.
///
/// \param[in] key                      HMAC key
/// \param[in] keySizeBytes             Size of key in bytes
/// \param[in] salt                     HKDF salt
/// \param[in] saltSizeBytes            Size of salt in bytes
/// \param[in] info                     HKDF info
/// \param[in] infoSizeBytes            Info size in bytes
/// \param[in] derivedKeySizeBytes      Desired size of the derived keys
/// \param[out] derivedKeys             Buffer to contain the derived keys
///
/// \returns Size of the derived keys in bytes on success, -1 on failure
///
int Hkdf(   uint8_t* const key,
            size_t const keySizeBytes,
            uint8_t* const salt,
            size_t const saltSizeBytes,
            uint8_t* const info,
            size_t const infoSizeBytes,
            size_t const derivedKeysSizeBytes,
            uint8_t* const derivedKeys)
{
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

    int result = EVP_PKEY_derive_init(ctx);
    if (0 >= result)
    {
        printLastError("EVP_PKEY_derive_init failed");
        return -1;
    }

    // Select SHA-256 hash/digest
    result = EVP_PKEY_CTX_set_hkdf_md(ctx, EVP_sha256());
    if (0 >= result)
    {
        printLastError("EVP_PKEY_CTX_set_hkdf_md failed");
        return -1;
    }

    result = EVP_PKEY_CTX_set1_hkdf_salt(ctx, salt, saltSizeBytes);
    if (0 >= result)
    {
        printLastError("EVP_PKEY_CTX_set1_hkdf_salt failed");
        return -1;
    }

    result = EVP_PKEY_CTX_set1_hkdf_key(ctx, key, keySizeBytes);
    if (0 >= result)
    {
        printLastError("EVP_PKEY_CTX_set1_hkdf_key failed");
        return -1;
    }

    result = EVP_PKEY_CTX_add1_hkdf_info(ctx, info, infoSizeBytes);
    if (0 >= result)
    {
        printLastError("EVP_PKEY_CTX_add1_hkdf_info failed");
        return -1;
    }

    size_t derivedMaterialSizeBytes = derivedKeysSizeBytes;

    result = EVP_PKEY_derive(   ctx,
                                derivedKeys,
                                &derivedMaterialSizeBytes);
    if (0 >= result)
    {
        printLastError("EVP_PKEY_derive failed");
        return -1;
    }

    if (verbose)
    {
        printf("Derived material is: (%d bytes)\n",
            derivedMaterialSizeBytes);
        BIO_dump_fp(stdout, derivedKeys, derivedMaterialSizeBytes);
    }

    EVP_PKEY_CTX_free(ctx);

    return derivedMaterialSizeBytes;
}

/// Derives the Video Authentication and Control Channel keys from the master secret
///
/// Assumes a 16-byte master secret
///
/// \param[in] masterSecret         Reference to the (16-byte) master secret
/// \param[in] hostSalt             Reference to the host-supplied salt
/// \param[in] sensorSalt           Reference to the sensor-supplied salt
/// \param[in] sensorId             Reference to the sensor's unique ID
/// \param[in] romVersion           ROM version identifier
/// \param[in] patchVersion         Patch version identifier
/// \param[in] videoAuthKey         Buffer for video authentication key
/// \param[in] controlChannelKey    Buffer for control channel key
///
/// \returns Size of the derived keys in bytes on success, -1 on failure
///
int DeriveSessionKeys(  uint8_t* const masterSecret,
                        uint8_t* const hostSalt,
                        uint8_t* const sensorSalt,
                        CssSensorIdT const sensorId,
                        uint16_t const romVersion,
                        uint16_t const patchVersion,
                        uint8_t* const videoAuthKey,
                        uint8_t* const controlChannelKey)
{
    // context is "AR0820R2"
    uint8_t const context[8] = {
        0x41U, 0x52U, 0x30U, 0x38U, 0x32U, 0x30U, 0x52U, 0x32U};

    uint8_t derivedKeys[CSS_AES_128_KEY_SIZE_BYTES * 2U];

    CssHkdfSaltT salt;
    CssHkdfInfoT info;

    // Initialise the salt
    memcpy(salt.hostSalt, hostSalt, sizeof(salt.hostSalt));
    memcpy(salt.sensorSalt, sensorSalt, sizeof(salt.sensorSalt));

    if (verbose)
    {
        printf("Salt:\n");
        BIO_dump_fp(stdout, (const char*)&salt, sizeof(salt));
    }

    // Initialise the info
    memcpy(info.context, context, sizeof(info.context));
    info.romVersion = romVersion;
    info.patchVersion = patchVersion;
    memcpy(info.sensorId, sensorId, sizeof(info.sensorId));

    if (verbose)
    {
        printf("Info\n");
        BIO_dump_fp(stdout, (const char*)&info, sizeof(info));
    }

    int derivedKeySizeBytes = Hkdf( masterSecret,
                                    16U,
                                    (uint8_t* const)&salt,
                                    sizeof(salt),
                                    (uint8_t* const)&info,
                                    sizeof(info),
                                    sizeof(derivedKeys),
                                    derivedKeys);
    if (0 < derivedKeySizeBytes)
    {
        if (verbose)
        {
            printf("Derived keys: (%d bytes)\n", derivedKeySizeBytes);
            BIO_dump_fp(stdout, derivedKeys, derivedKeySizeBytes);
        }

        memcpy( videoAuthKey,
                derivedKeys,
                CSS_AES_128_KEY_SIZE_BYTES);
        memcpy( controlChannelKey,
                &derivedKeys[CSS_AES_128_KEY_SIZE_BYTES],
                CSS_AES_128_KEY_SIZE_BYTES);
    }

    return derivedKeySizeBytes;
}

int main(int argc, void * argv[])
{
    uint8_t const masterSecret[16] = {
        0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33,
        0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x1a, 0x2a
    };

    uint8_t const hostSalt[CSS_SALT_SIZE_BYTES] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18
    };

    uint8_t const sensorSalt[CSS_SALT_SIZE_BYTES] = {
        0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
        0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8
    };

    CssSensorIdT const sensorId = {
        0xa1, 0xc2, 0xf3, 0xa4, 0xc5, 0xe6, 0x17, 0x38,
        0xb1, 0xd2, 0xe3, 0xb4, 0xd5, 0xf6, 0x27, 0x48,
        0xc1, 0xe2, 0xc3, 0xc4, 0xe5, 0x06, 0x37, 0x58,
        0xd1, 0xf2, 0xb3, 0xd4, 0xf5, 0x16, 0x47, 0x68
    };

    uint16_t const romVersion = 0x1234;
    uint16_t const patchVersion = 0xA001;

    uint8_t videoAuthKey[CSS_AES_128_KEY_SIZE_BYTES];
    uint8_t controlChannelKey[CSS_AES_128_KEY_SIZE_BYTES];

    printf("\n======== OpenSSL HKDF example running ========\n");

    int derivedKeysSizeBytes = DeriveSessionKeys(   (uint8_t* const)masterSecret,
                                                    (uint8_t* const)hostSalt,
                                                    (uint8_t* const)sensorSalt,
                                                    sensorId,
                                                    romVersion,
                                                    patchVersion,
                                                    videoAuthKey,
                                                    controlChannelKey);
    if (0 < derivedKeysSizeBytes)
    {
        printf("\nVideo Auth Key:\n");
        BIO_dump_fp(stdout, videoAuthKey, sizeof(videoAuthKey));
        printf("\nControl Channel Key:\n");
        BIO_dump_fp(stdout, controlChannelKey, sizeof(controlChannelKey));
    }
    else
    {
        return derivedKeysSizeBytes;
    }

    return 0;
}
