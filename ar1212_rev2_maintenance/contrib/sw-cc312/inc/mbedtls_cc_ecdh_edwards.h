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

 @brief This file contains the CryptoCell ECDH Edwards curve APIs.
 */

/*!
 @defgroup
 @brief Contains the CryptoCell ECDH Edwards curve APIs. See mbedtls_cc_ecdh_edwards.h.
 @{
 @ingroup ecdh_module
 @}
 */
#ifndef ECDH_EDWARDS_H
#define ECDH_EDWARDS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "mbedtls/ecp.h"

/************************ Defines ******************************/

/************************ Typedefs  ****************************/

/************************ Enums ********************************/

/************************ Structs  ******************************/

/************************ context Structs  ******************************/

/*!
   @brief           This function generates a public key and a TLS
                    ServerKeyExchange payload.

   @note            This function may be used only for curve 25519.

                    This is the first function used by a TLS server for ECDHE
                    ciphersuites.

   @note            This function assumes that the ECP group (\c grp) of the
                    \p ctx context has already been properly set,
                    for example, using mbedtls_ecp_group_load().

   @see             ecp.h

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */

int mbedtls_ecdh_make_params_edwards(
                      mbedtls_ecdh_context *ctx, /*!< The ECDH context. */
                      size_t *olen, /*!< The number of characters written. */
                      unsigned char *buf, /*!< The destination buffer. */
                      size_t blen, /*!< The length of the destination buffer. */
                      int (*f_rng)(void *, unsigned char *, size_t), /*!< The RNG function. */
                      void *p_rng /*!< The RNG context. */
                      );

/**
   @brief           This function parses and processes a TLS ServerKeyExhange
                    payload.

   @note            This function may be used only for curve 25519.

                    This is the first function used by a TLS client for ECDHE
                    ciphersuites.

   @see             ecp.h

   @return          \c 0 on success.
   @return          An \c MBEDTLS_ERR_ECP_XXX error code on failure.
 */
int mbedtls_ecdh_read_params_edwards(
                      mbedtls_ecdh_context *ctx, /*!< The ECDH context. */
                      const unsigned char **buf, /*!< The pointer to the start of the input buffer. */
                      const unsigned char *end /*!< The address for one Byte past the end of the buffer. */
                      );


#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_ECDH_EDWARDS */
