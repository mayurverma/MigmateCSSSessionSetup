/**************************************************************************************
* Copyright (c) 2016-2019, Arm Limited (or its affiliates). All rights reserved       *
*                                                                                     *
* This file and the related binary are licensed under the following license:          *
*                                                                                     *
* ARM Object Code and Header Files License, v1.0 Redistribution.                      *
*                                                                                     *
* Redistribution and use of object code, header files, and documentation, without     *
* modification, are permitted provided that the following conditions are met:         *
*                                                                                     *
* 1) Redistributions must reproduce the above copyright notice and the                *
*    following disclaimer in the documentation and/or other materials                 *
*    provided with the distribution.                                                  *
*                                                                                     *
* 2) Unless to the extent explicitly permitted by law, no reverse                     *
*    engineering, decompilation, or disassembly of is permitted.                      *
*                                                                                     *
* 3) Redistribution and use is permitted solely for the purpose of                    *
*    developing or executing applications that are targeted for use                   *
*    on an ARM-based product.                                                         *
*                                                                                     *
* DISCLAIMER. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND                  *
* CONTRIBUTORS "AS IS." ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT             *
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT,        *
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          *
* COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED            *
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR              *
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF              *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING                *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                  *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        *
**************************************************************************************/

/*!
 @file
 @brief This file contains the CryptoCell utility key-derivation function APIs.

 The key-derivation function is defined as specified in the
 <em>KDF in Counter Mode</em> section in <em>NIST Special Publication
 800-108: Recommendation for Key Derivation Using Pseudorandom Functions</em>.
 */

/*!
 @defgroup cc_utils_key_derivation CryptoCell utility key-derivation APIs
 @brief Contains the CryptoCell utility key-derivation function API. See mbedtls_cc_util_key_derivation.h.

 @{
 @ingroup cc_utils
 @}
 */

#ifndef  _MBEDTLS_CC_UTIL_KEY_DERIVATION_H
#define  _MBEDTLS_CC_UTIL_KEY_DERIVATION_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "mbedtls_cc_util_defs.h"
#include "mbedtls_cc_util_key_derivation_defs.h"
#include "cc_hash_defs.h"

/******************************************************************************
*                           DEFINITIONS
******************************************************************************/

/*! Input key derivation type. */
typedef enum  {
    CC_UTIL_USER_KEY = 0, /*!< The user key.*/
    CC_UTIL_ROOT_KEY = 1, /*!< The device root key (HUK).*/
    CC_UTIL_TOTAL_KEYS = 2, /*!< Total number of keys.*/
    CC_UTIL_END_OF_KEY_TYPE = 0x7FFFFFFF /*!< Reserved.*/
}mbedtls_util_keytype_t;

/*! Pseudo-random function type for key derivation. */
typedef enum {
    CC_UTIL_PRF_CMAC = 0, /*!< CMAC function.*/
    CC_UTIL_PRF_HMAC = 1, /*!< HMAC function.*/
    CC_UTIL_TOTAL_PRFS = 2, /*!< Total number of pseudo-random functions.*/
    CC_UTIL_END_OF_PRF_TYPE = 0x7FFFFFFF /*!< Reserved.*/
}mbedtls_util_prftype_t;


/*!
  @brief  This function performs key derivation using AES-CMAC.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation Using
  Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key Ki.
  AES-CMAC is used as the pseudo-random function (PRF).

  @note   The user must define the label and context for each use-case well, when using this API.

  @return \c CC_UTIL_OK on success.
  @return A non-zero value from cc_util_error.h on failure.
 */

/*  A key-derivation function can iterates n times until l bits of keying material are generated.
        For each of the iterations of the PRF, i=1 to n, do:
        result(0) = 0;
        K(i) = PRF (Ki, [i] || Label || 0x00 || Context || length);
        results(i) = result(i-1) || K(i);

        concisely, result(i) = K(i) || k(i-1) || .... || k(0)*/
CCUtilError_t mbedtls_util_key_derivation(
    mbedtls_util_keytype_t        keyType,       /*!< [in] The key type that is used as an input to a key-derivation function.
                                                           Can be one of the following: \p CC_UTIL_USER_KEY or \p CC_UTIL_ROOT_KEY. */
    mbedtls_util_keydata        *pUserKey,       /*!< [in] A pointer to the key buffer of the user, in case of \p CC_UTIL_USER_KEY. */
    mbedtls_util_prftype_t        prfType,       /*!< [in] The PRF type that is used as an input to a key-derivation function.
                                                           Can be one of the following: \p CC_UTIL_PRF_CMAC or \p CC_UTIL_PRF_HMAC. */
    CCHashOperationMode_t       hashMode,        /*!< [in] One of the supported hash modes, as defined in \p CCHashOperationMode_t. */
    const uint8_t               *pLabel,         /*!< [in] A string that identifies the purpose for the derived keying material.*/
    size_t                      labelSize,       /*!< [in] The label size should be in range of 1 to 64 Bytes in length. */
    const uint8_t               *pContextData,   /*!< [in] A binary string containing the information related to the derived keying material. */
    size_t                      contextSize,     /*!< [in] The context size should be in range of 1 to 64 Bytes in length. */
    uint8_t                     *pDerivedKey,    /*!< [out] Keying material output. Must be at least the size of \p derivedKeySize. */
    size_t                      derivedKeySize   /*!< [in] The size of the derived keying material in Bytes, up to 4080 Bytes. */
    );


/*!
  @brief  This function performs key derivation using CMAC.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation
  Using Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key Ki.
  In this macro, AES-CMAC is used as the pseudo-random function (PRF).

  @return \c CC_UTIL_OK on success.
  @return A non-zero value from cc_util_error.h on failure.
 */
#define mbedtls_util_key_derivation_cmac(keyType, pUserKey, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize) \
    mbedtls_util_key_derivation(keyType, pUserKey, CC_UTIL_PRF_CMAC, CC_HASH_OperationModeLast, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize)


/*!
  @brief  This function performs key derivation using HMAC.

  It is defined as specified in the <em>KDF in Counter Mode</em> section in
  <em>NIST Special Publication 800-108: Recommendation for Key Derivation
  Using Pseudorandom Functions</em>.

  The derivation is based on length l, label L, context C, and derivation key Ki.
  In this macro, HMAC is used as the pseudo-random function (PRF).

 @return \c CC_UTIL_OK on success.
 @return A non-zero value from cc_util_error.h on failure.
 */
#define mbedtls_util_key_derivation_hmac(keyType, pUserKey, hashMode, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize) \
    mbedtls_util_key_derivation(keyType, pUserKey, CC_UTIL_PRF_HMAC, hashMode, pLabel, labelSize, pContextData, contextSize, pDerivedKey, derivedKeySize)


#ifdef __cplusplus
}
#endif

#endif /*_MBEDTLS_CC_UTIL_KEY_DERIVATION_H*/

