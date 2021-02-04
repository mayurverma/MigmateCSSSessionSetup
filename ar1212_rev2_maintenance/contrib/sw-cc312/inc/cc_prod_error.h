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
@brief This file contains the error definitions of the CryptoCell production-library APIs.
*/

/*!
 @defgroup prod_errors Specific errors of the CryptoCell production-library APIs
 @brief Contains the CryptoCell production-library-API error definitions. See cc_prod_error.h.

 @{
 @ingroup prod
 @}
 */

#ifndef _PROD_ERROR_H
#define _PROD_ERROR_H


#include "cc_error.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/* The error base address of the production library module of CryptoCell - 0x00F02A00 */
/*! Library initialization failure. */
#define CC_PROD_INIT_ERR                        (CC_PROD_MODULE_ERROR_BASE + 0x01UL)
/*! Illegal parameter. */
#define CC_PROD_INVALID_PARAM_ERR               (CC_PROD_MODULE_ERROR_BASE + 0x02UL)
/*! Invalid number of zeroes calculated. */
#define CC_PROD_ILLEGAL_ZERO_COUNT_ERR          (CC_PROD_MODULE_ERROR_BASE + 0x03UL)
/*! LCS is invalid for the operation. */
#define CC_PROD_ILLEGAL_LCS_ERR                 (CC_PROD_MODULE_ERROR_BASE + 0x04UL)
/*! Invalid asset-package fields. */
#define CC_PROD_ASSET_PKG_PARAM_ERR             (CC_PROD_MODULE_ERROR_BASE + 0x05UL)
/*! Failed to validate the asset package. */
#define CC_PROD_ASSET_PKG_VERIFY_ERR            (CC_PROD_MODULE_ERROR_BASE + 0x06UL)
/*! HAL Fatal error occured. */
#define CC_PROD_HAL_FATAL_ERR                   (CC_PROD_MODULE_ERROR_BASE + 0x07UL)

#endif
