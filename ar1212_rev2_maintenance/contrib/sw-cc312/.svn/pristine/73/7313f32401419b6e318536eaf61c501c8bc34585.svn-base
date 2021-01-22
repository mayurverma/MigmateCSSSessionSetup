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
 @brief This file contains all of the CryptoCell library basic APIs, their enums and definitions.
 */

/*!
 @defgroup cc_top Basic CryptoCell library definitions
 @brief Contains basic CryptoCell library definitions.

 @{
 @ingroup cryptocell_api
 @}
 */

/*!
 @defgroup cc_lib Basic CryptoCell library APIs
 @brief Contains basic CryptoCell library APIs.

 This module lists the basic CryptoCell library APIs and definitions.

 @{
 @ingroup cc_top
 @}
 */

#ifndef __CC_LIB_H__
#define __CC_LIB_H__

#include "cc_pal_types.h"
#include "cc_rnd_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*! Definitions for error returns from ::CC_LibInit or ::CC_LibFini functions. */
typedef enum {
        CC_LIB_RET_OK = 0,          /*!< Success definition.*/
        CC_LIB_RET_EINVAL_CTX_PTR,  /*!< Illegal context pointer.*/
        CC_LIB_RET_EINVAL_WORK_BUF_PTR, /*!< Illegal work-buffer pointer.*/
        CC_LIB_RET_HAL,         /*!< Error returned from the HAL layer.*/
        CC_LIB_RET_PAL,         /*!< Error returned from the PAL layer.*/
        CC_LIB_RET_RND_INST_ERR,    /*!< RND instantiation failed.*/
        CC_LIB_RET_EINVAL_PIDR,     /*!< Invalid peripheral ID. */
        CC_LIB_RET_EINVAL_CIDR,     /*!< Invalid component ID. */
        CC_LIB_AO_WRITE_FAILED_ERR, /*!< Error returned from AO write operation. */
        CC_LIB_RESERVE32B = 0x7FFFFFFFL /*!< Reserved.*/
} CClibRetCode_t;


/*! Internal definition for the product register. */
#define DX_VERSION_PRODUCT_BIT_SHIFT    0x18UL
/*! Internal definition for the product register size. */
#define DX_VERSION_PRODUCT_BIT_SIZE     0x8UL



/*!
 @brief This function performs global initialization of the CryptoCell runtime library.

 It must be called once per CryptoCell cold-boot cycle.
 Among other initializations, this function initializes the CTR-DRBG context
 (including the TRNG seeding). An initialized DRBG context is required for calling
 DRBG APIs, as well as asymmetric-cryptography key-generation and signatures.
 The primary context returned by this function can be used as a single global
 context for all DRBG needs. Alternatively, other contexts may be initialized
 and used with a more limited scope, for specific applications or specific threads.

 @note The Mutexes, if used, are initialized by this API. Therefore, unlike the other
       APIs in the library, this API is not thread-safe.

 @return \c CC_LIB_RET_OK on success.
 @return A non-zero value on failure.
 */
CClibRetCode_t CC_LibInit(
                       CCRndContext_t *rndContext_ptr,  /*!< [in/out] A pointer to the RND context buffer allocated by the user.
                                                                      The context is used to maintain the RND state as well as
                                                                       pointers to a function used for random vector generation.
                                                                       This context must be saved and provided as parameter to
                                                                      any API that uses the RND module.*/
                       CCRndWorkBuff_t  *rndWorkBuff_ptr /*!< [in] Scratchpad for the work of the RND module. */
                       );

/*!
 @brief This function finalizes library operations.

 It frees the associated resources (mutexes) and call HAL and PAL terminate functions.
 The function also cleans the DRBG context.

 @return \c CC_LIB_RET_OK on success.
 @return A non-zero value on failure.
*/
CClibRetCode_t CC_LibFini(
                            CCRndContext_t *rndContext_ptr /*!< [in/out] A pointer to the RND context
                                                                buffer that was initialized in #CC_LibInit.*/
                         );

#ifdef __cplusplus
}
#endif

#endif /*__CC_LIB_H__*/


