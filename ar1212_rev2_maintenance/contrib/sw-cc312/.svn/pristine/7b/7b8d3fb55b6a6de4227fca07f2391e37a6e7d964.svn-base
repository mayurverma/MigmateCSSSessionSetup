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
 @brief This file contains all of the CryptoCell SHA-512 truncated APIs, their enums and definitions.
 */

/*!
 @defgroup cc_sha512_t_h CryptoCell SHA-512 truncated APIs
 @brief Contains all CryptoCell SHA-512 truncated APIs. See mbedtls_cc_sha512_t.h

 @{
 @ingroup cc_hash
 @}
 */


#ifndef _MBEDTLS_CC_SHA512_T_H
#define _MBEDTLS_CC_SHA512_T_H

#include <sha512.h>

/*!
   @brief          This function initializes the SHA-512_t context.
 */
void mbedtls_sha512_t_init(
                           mbedtls_sha512_context *ctx /*!< The SHA-512_t context to initialize. */
                          );

/*!
   @brief          This function clears the SHA-512_t context.
 */
void mbedtls_sha512_t_free(
                            mbedtls_sha512_context *ctx /*!< The SHA-512_t context to clear. */
                           );

/*!
   @brief          This function starts a SHA-512_t checksum calculation.
 */
void mbedtls_sha512_t_starts(
                              mbedtls_sha512_context *ctx, /*!< The SHA-512_t context to initialize. */
                              int is224 /*!< Determines which function to use: 0: Use SHA-512/256, or 1: Use SHA-512/224. */
                            );

/*!
   @brief          This function feeds an input buffer into an ongoing
                   SHA-512_t checksum calculation.
 */
void mbedtls_sha512_t_update(
                              mbedtls_sha512_context *ctx, /*!< The SHA-512_t context. */
                              const unsigned char *input, /*!< The buffer holding the input data. */
                              size_t ilen /*!< The length of the input data. */
                              );

/*!
   @brief          This function finishes the SHA-512_t operation, and writes
                   the result to the output buffer.

                   <ul><li>For SHA512/224, the output buffer will include
                   the 28 leftmost Bytes of the SHA-512 digest.</li>
                   <li>For SHA512/256, the output buffer will include
                   the 32 leftmost bytes of the SHA-512 digest.</li></ul>
 */
void mbedtls_sha512_t_finish(
                              mbedtls_sha512_context *ctx, /*!< The SHA-512_t context. */
                              unsigned char output[32], /*!< The SHA-512/256 or SHA-512/224 checksum result. */
                              int is224 /*!< Determines which function to use: 0: Use SHA-512/256, or 1: Use SHA-512/224. */
                              );

/*!
   @brief          This function calculates the SHA-512 checksum of a buffer.

                   The function allocates the context, performs the
                   calculation, and frees the context.

                   The SHA-512 result is calculated as
                   output = SHA-512(input buffer).

 */
void mbedtls_sha512_t(
                       const unsigned char *input, /*!< The buffer holding the input data. */
                       size_t ilen, /*!< The length of the input data. */
                       unsigned char output[32], /*!< The SHA-512/256 or SHA-512/224 checksum result. */
                       int is224 /*!< Determines which function to use: 0: Use SHA-512/256, or 1: Use SHA-512/224. */
                      );

#endif
