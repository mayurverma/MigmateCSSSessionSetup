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
 @brief This file contains general definitions of the CryptoCell runtime SW APIs.
 */

/*!
 @defgroup cc_general_defs General CryptoCell definitions
 @brief Contains general definitions of the CryptoCell runtime SW APIs.

 See cc_general_defs.h.
 @{
 @ingroup cc_top
 @}
 */
#ifndef _CC_GENERAL_DEFS_H
#define _CC_GENERAL_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_hash_defs.h"

/************************ Defines ******************************/
/*! Hash parameters for HMAC operation. */
typedef struct {
    uint16_t hashResultSize;         /*!< The size of the HMAC hash result. */
    CCHashOperationMode_t hashMode;  /*!< The hash operation mode. */
}HmacHash_t;

/*! The maximal size of the hash string. */
#define CC_HASH_NAME_MAX_SIZE  10

extern const HmacHash_t HmacHashInfo_t[CC_HASH_NumOfModes]; /*!< Hash parameters for HMAC operation. */
extern const uint8_t HmacSupportedHashModes_t[CC_HASH_NumOfModes]; /*!< Supported hash modes. */
extern const char HashAlgMode2mbedtlsString[CC_HASH_NumOfModes][CC_HASH_NAME_MAX_SIZE]; /*!< Hash string names. */


/* general definitions */
/*-------------------------*/
/*! Maximal size of AES HUK in Bytes. */
#define CC_AES_KDR_MAX_SIZE_BYTES   32
/*! Maximal size of AES HUK in words. */
#define CC_AES_KDR_MAX_SIZE_WORDS   (CC_AES_KDR_MAX_SIZE_BYTES/sizeof(uint32_t))


/* Life-cycle states. */

#define CC_LCS_CHIP_MANUFACTURE_LCS     0x0 /*!< The Chip Manufacturer (CM) LCS value. */

#define CC_LCS_SECURE_LCS               0x5 /*!< The Secure LCS value. */

#ifdef __cplusplus
}
#endif

#endif

