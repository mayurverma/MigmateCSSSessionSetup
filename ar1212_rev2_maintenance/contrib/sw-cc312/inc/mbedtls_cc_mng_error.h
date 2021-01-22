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
 @brief This file contains the error definitions of the CryptoCell management APIs.
 */

/*!
 @defgroup cc_management_error Specific errors of the CryptoCell Management APIs
 @brief Contains the CryptoCell management-API error definitions. See mbedtls_cc_mng_error.h.

 @{
 @ingroup cc_management
 @}
*/

#ifndef _MBEDTLS_CC_MNG_ERROR_H
#define _MBEDTLS_CC_MNG_ERROR_H

#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* CryptoCell Management module errors. CC_MNG_MODULE_ERROR_BASE = 0x00F02900 */

/*! Illegal input parameter. */
#define CC_MNG_ILLEGAL_INPUT_PARAM_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x00UL)
/*! Illegal operation. */
#define CC_MNG_ILLEGAL_OPERATION_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal Peripheral ID. */
#define CC_MNG_ILLEGAL_PIDR_ERR             (CC_MNG_MODULE_ERROR_BASE + 0x02UL)
/*! Illegal Component ID. */
#define CC_MNG_ILLEGAL_CIDR_ERR             (CC_MNG_MODULE_ERROR_BASE + 0x03UL)
/*! APB Secure is locked. */
#define CC_MNG_APB_SECURE_IS_LOCKED_ERR         (CC_MNG_MODULE_ERROR_BASE + 0x04UL)
/*! APB Privilege is locked. */
#define CC_MNG_APB_PRIVILEGE_IS_LOCKED_ERR  (CC_MNG_MODULE_ERROR_BASE + 0x05UL)
/*! APBC Secure is locked. */
#define CC_MNG_APBC_SECURE_IS_LOCKED_ERR    (CC_MNG_MODULE_ERROR_BASE + 0x06UL)
/*! APBC Privilege is locked. */
#define CC_MNG_APBC_PRIVILEGE_IS_LOCKED_ERR (CC_MNG_MODULE_ERROR_BASE + 0x07UL)
/*! APBC Instruction is locked. */
#define CC_MNG_APBC_INSTRUCTION_IS_LOCKED_ERR   (CC_MNG_MODULE_ERROR_BASE + 0x08UL)
/*! Invalid Key type. */
#define CC_MNG_INVALID_KEY_TYPE_ERROR       (CC_MNG_MODULE_ERROR_BASE + 0x09UL)
/*! Illegal size of HUK. */
#define CC_MNG_ILLEGAL_HUK_SIZE_ERR     (CC_MNG_MODULE_ERROR_BASE + 0x0AUL)
/*! Illegal size for any HW key other than HUK. */
#define CC_MNG_ILLEGAL_HW_KEY_SIZE_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x0BUL)
/*! HW key is locked. */
#define CC_MNG_HW_KEY_IS_LOCKED_ERR         (CC_MNG_MODULE_ERROR_BASE + 0x0CUL)
/*! Kcp is locked. */
#define CC_MNG_KCP_IS_LOCKED_ERR            (CC_MNG_MODULE_ERROR_BASE + 0x0DUL)
/*! Kce is locked. */
#define CC_MNG_KCE_IS_LOCKED_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x0EUL)
/*! RMA Illegal state. */
#define CC_MNG_RMA_ILLEGAL_STATE_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x0FUL)
/*! Error returned from AO_APB_FILTERING write operation. */
#define CC_MNG_AO_APB_WRITE_FAILED_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x10UL)
/*! APBC access failure. */
#define CC_MNG_APBC_ACCESS_FAILED_ERR       (CC_MNG_MODULE_ERROR_BASE + 0x11UL)
/*! APBC already-off failure. */
#define CC_MNG_APBC_ACCESS_ALREADY_OFF_ERR  (CC_MNG_MODULE_ERROR_BASE + 0x12UL)
/*! APBC access is on failure. */
#define CC_MNG_APBC_ACCESS_IS_ON_ERR        (CC_MNG_MODULE_ERROR_BASE + 0x13UL)
/*! PM SUSPEND/RESUME failure. */
#define CC_MNG_PM_SUSPEND_RESUME_FAILED_ERR (CC_MNG_MODULE_ERROR_BASE + 0x14UL)
/*! SW version failure. */
#define CC_MNG_ILLEGAL_SW_VERSION_ERR       (CC_MNG_MODULE_ERROR_BASE + 0x15UL)
/*! Hash Public Key NA. */
#define CC_MNG_HASH_NOT_PROGRAMMED_ERR      (CC_MNG_MODULE_ERROR_BASE + 0x16UL)
/*! Illegal hash boot key zero count in the OTP error. */
#define CC_MNG_HBK_ZERO_COUNT_ERR           (CC_MNG_MODULE_ERROR_BASE + 0x17UL)

/************************ Enums ********************************/

/************************ Typedefs  ****************************/

/************************ Structs  *****************************/

/************************ Public Variables *********************/

/************************ Public Functions *********************/

#ifdef __cplusplus
}
#endif

#endif // _MBEDTLS_CC_MNG_ERROR_H

