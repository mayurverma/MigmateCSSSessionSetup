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
 @defgroup cc_utils CryptoCell utility APIs
 @brief This contains all utility APIs.
 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @file
 @brief This file contains general definitions of the CryptoCell utility APIs.
 */

/*!
 @defgroup cc_utils_defs CryptoCell utility APIs general definitions
 @brief Contains CryptoCell utility APIs general definitions. See mbedtls_cc_util_defs.h.
 @{
 @ingroup cc_utils
 @}
 */

#ifndef  _MBEDTLS_CC_UTIL_DEFS_H
#define  _MBEDTLS_CC_UTIL_DEFS_H



#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types_plat.h"
#include "mbedtls_cc_util_key_derivation_defs.h"


/******************************************************************************
*                           DEFINITIONS
******************************************************************************/
/*! The size of the AES 128-bit key in Bytes. */
#define CC_UTIL_AES_128BIT_SIZE 16
/*! The size of the AES 192-bit key in Bytes. */
#define CC_UTIL_AES_192BIT_SIZE 24
/*! The size of the AES 256-bit key in Bytes. */
#define CC_UTIL_AES_256BIT_SIZE 32
/*****************************************/
/* CMAC derive key definitions*/
/*****************************************/
/*! The minimal size of the data for the CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MIN_DATA_IN_SIZE  CC_UTIL_FIX_DATA_MIN_SIZE_IN_BYTES+2
/*! The maximal size of the data for CMAC derivation operation. */
#define CC_UTIL_CMAC_DERV_MAX_DATA_IN_SIZE  CC_UTIL_MAX_KDF_SIZE_IN_BYTES
/*! The size of the AES CMAC result in Bytes. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES   0x10UL
/*! The size of the AES CMAC result in words. */
#define CC_UTIL_AES_CMAC_RESULT_SIZE_IN_WORDS   (CC_UTIL_AES_CMAC_RESULT_SIZE_IN_BYTES/sizeof(uint32_t))

/*! Util error type. */
typedef uint32_t CCUtilError_t;



/*! Key data. */
typedef struct mbedtls_util_keydata {
    uint8_t*  pKey;     /*!< A pointer to the key. */
    size_t    keySize;  /*!< The size of the key in Bytes. */
}mbedtls_util_keydata;

#ifdef __cplusplus
}
#endif

#endif /*_MBEDTLS_CC_UTIL_DEFS_H*/

