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
 @brief This file contains the common definitions of the CryptoCell AES-CCM star APIs.
 */

/*!
 @defgroup cc_aesccm_star_common Common definitions of the CryptoCell AES-CCM star APIs
 @brief Contains the CryptoCell AES-CCM star APIs. See mbedtls_ccm_common.h.

 @{
 @ingroup cc_aesccm_star
 @}
 */

#ifndef _MBEDTLS_CCM_COMMON_H
#define _MBEDTLS_CCM_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! The size of the AES CCM star nonce in Bytes. */
#define MBEDTLS_AESCCM_STAR_NONCE_SIZE_BYTES               13
/*! The size of source address of the AES CCM star in Bytes. */
#define MBEDTLS_AESCCM_STAR_SOURCE_ADDRESS_SIZE_BYTES      8

/*! AES CCM mode: CCM. */
#define MBEDTLS_AESCCM_MODE_CCM             0
/*! AES CCM mode: CCM star. */
#define MBEDTLS_AESCCM_MODE_STAR            1

#ifdef __cplusplus
}
#endif

#endif /* _MBEDTLS_CCM_COMMON_H */
