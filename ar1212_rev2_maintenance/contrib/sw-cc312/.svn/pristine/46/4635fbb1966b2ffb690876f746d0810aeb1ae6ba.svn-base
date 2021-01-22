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
 @brief This file contains the error definitions of the CryptoCell utility APIs.
 */

/*!
 @defgroup cc_utils_errors Specific errors of the CryptoCell utility module APIs
 @brief Contains utility API error definitions. See cc_util_error.h.

 @{
 @ingroup cc_utils
 @}
 */

#ifndef  _CC_UTIL_ERROR_H
#define  _CC_UTIL_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/***********************/
/* Util return codes   */
/***********************/
/*! Success definition. */
#define CC_UTIL_OK                                      0x00UL
/*! The error base address definition. */
#define CC_UTIL_MODULE_ERROR_BASE                       0x80000000
/*! Illegal key type. */
#define CC_UTIL_INVALID_KEY_TYPE                        (CC_UTIL_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal data-in pointer. */
#define CC_UTIL_DATA_IN_POINTER_INVALID_ERROR           (CC_UTIL_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal data-in size. */
#define CC_UTIL_DATA_IN_SIZE_INVALID_ERROR              (CC_UTIL_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal data-out pointer. */
#define CC_UTIL_DATA_OUT_POINTER_INVALID_ERROR          (CC_UTIL_MODULE_ERROR_BASE + 0x03UL)
/*! Illegal data-out size. */
#define CC_UTIL_DATA_OUT_SIZE_INVALID_ERROR             (CC_UTIL_MODULE_ERROR_BASE + 0x04UL)
/*! Fatal error. */
#define CC_UTIL_FATAL_ERROR                             (CC_UTIL_MODULE_ERROR_BASE + 0x05UL)
/*! Illegal parameters. */
#define CC_UTIL_ILLEGAL_PARAMS_ERROR                    (CC_UTIL_MODULE_ERROR_BASE + 0x06UL)
/*! Invalid address given. */
#define CC_UTIL_BAD_ADDR_ERROR                          (CC_UTIL_MODULE_ERROR_BASE + 0x07UL)
/*! Illegal domain for endorsement key. */
#define CC_UTIL_EK_DOMAIN_INVALID_ERROR                 (CC_UTIL_MODULE_ERROR_BASE + 0x08UL)
/*! HUK is not valid. */
#define CC_UTIL_KDR_INVALID_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x09UL)
/*! LCS is not valid. */
#define CC_UTIL_LCS_INVALID_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x0AUL)
/*! Session key is not valid. */
#define CC_UTIL_SESSION_KEY_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x0BUL)
/*! Illegal user key size. */
#define CC_UTIL_INVALID_USER_KEY_SIZE                   (CC_UTIL_MODULE_ERROR_BASE + 0x0DUL)
/*! Illegal LCS for the required operation. */
#define CC_UTIL_ILLEGAL_LCS_FOR_OPERATION_ERR           (CC_UTIL_MODULE_ERROR_BASE + 0x0EUL)
/*! Invalid PRF type. */
#define CC_UTIL_INVALID_PRF_TYPE                        (CC_UTIL_MODULE_ERROR_BASE + 0x0FUL)
/*! Invalid hash mode. */
#define CC_UTIL_INVALID_HASH_MODE                       (CC_UTIL_MODULE_ERROR_BASE + 0x10UL)
/*! Unsupported hash mode. */
#define CC_UTIL_UNSUPPORTED_HASH_MODE                   (CC_UTIL_MODULE_ERROR_BASE + 0x11UL)
/*! Key is unusable */
#define CC_UTIL_KEY_UNUSABLE_ERROR                       (CC_UTIL_MODULE_ERROR_BASE + 0x12UL)
#ifdef __cplusplus
}
#endif

#endif /*_CC_UTIL_ERROR_H*/
