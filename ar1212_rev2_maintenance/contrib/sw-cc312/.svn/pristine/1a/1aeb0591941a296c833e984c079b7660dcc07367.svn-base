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
 @defgroup cc_aes CryptoCell AES APIs
 @brief Contains all CryptoCell AES APIs.

 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @file
 @brief This file contains all CryptoCell AES APIs that are currently not supported by Mbed TLS.
 */

/*!
 @defgroup cc_aes_crypt_add CryptoCell-specific AES APIs

 @{
 @ingroup cc_aes

 @brief Contains all CryptoCell AES APIs currently not supported by Mbed TLS. See mbedtls_cc_aes_crypt_additional.h.
 @}
 */

#ifndef  _MBEDTLS_CC_AES_CRYPT_ADDITIONAL_H
#define  _MBEDTLS_CC_AES_CRYPT_ADDITIONAL_H

#include "mbedtls/aes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
  @brief   This function encrypts or decrypts AES-OFB buffer.

  @note Due to the nature of OFB, you must use the same key
  schedule for both encryption and decryption, so that a context is
  initialized with mbedtls_aes_setkey_enc() for both
  #MBEDTLS_AES_ENCRYPT and #MBEDTLS_AES_DECRYPT.

  @return         0 on success.
 */
int mbedtls_aes_crypt_ofb(
                       mbedtls_aes_context *ctx, /*!< The AES context. */
                       size_t length, /*!< The length of the data. */
                       size_t *nc_off, /*!< Not supported. Set to 0. */
                       unsigned char nonce_counter[16], /*!< The 128-bit nonce and counter. */
                       unsigned char stream_block[16], /*!< Not supported. */
                       const unsigned char *input, /*!< The input data stream. */
                       unsigned char *output /*!< The output data stream. */
                       );

#ifdef __cplusplus
}
#endif

#endif /*_MBEDTLS_CC_AES_CRYPT_ADDITIONAL_H*/

