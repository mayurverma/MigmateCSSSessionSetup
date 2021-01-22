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
 @brief This file contains all of the enums and definitions that are used for the
        ICV and OEM production libraries.
 */

/*!
 @defgroup prod CryptoCell production-library APIs
 @brief Contains CryptoCell production-library APIs.
 @{
 @ingroup cryptocell_api
 @}
 */

#ifndef _PROD_H
#define _PROD_H

/*!
 @defgroup prod_mem CryptoCell production-library definitions
 @brief Contains CryptoCell production-library definitions. See cc_prod.h.
 @{
 @ingroup prod
 @}
 */
/************************ Defines ******************************/

/*! The definition of the number of Bytes in a word. */
#define CC_PROD_32BIT_WORD_SIZE    sizeof(uint32_t)
/*! The size of the plain-asset in Bytes. */
#define PROD_ASSET_SIZE     16
/*! The size of the asset-package in Bytes. */
#define PROD_ASSET_PKG_SIZE     64
/*! The size of the asset-package in words. */
#define PROD_ASSET_PKG_WORD_SIZE    (PROD_ASSET_PKG_SIZE/CC_PROD_32BIT_WORD_SIZE)
/*! The number of words of the DCU LOCK. */
#define PROD_DCU_LOCK_WORD_SIZE     4

/************************ Enums ********************************/

/*! The type of the provided asset. */
typedef enum {
    /*! The asset is not provided. */
        ASSET_NO_KEY = 0,
    /*! The asset is provided as plain, not in a package. */
        ASSET_PLAIN_KEY = 1,
    /*! The asset is provided as a package. */
        ASSET_PKG_KEY = 2,
    /*! Reserved. */
        ASSET_TYPE_RESERVED     = 0x7FFFFFFF,
} CCAssetType_t;

/************************ Typedefs  ****************************/

/*! Defines the buffer of the plain asset. A 16-Byte array. */
typedef uint8_t CCPlainAsset_t[PROD_ASSET_SIZE];
/*! Defines the buffer of the asset-package. A 64-Byte array. */
typedef uint32_t CCAssetPkg_t[PROD_ASSET_PKG_WORD_SIZE];


/************************ Structs  ******************************/

/*! @brief The asset buffer.

  If the asset is provided as plain asset, the \p plainAsset field is used.
  Otherwise, the \p pkgAsset field is used.
  */
typedef union {
    /*! Plain asset buffer. */
        CCPlainAsset_t     plainAsset;
    /*! Asset-package buffer. */
        CCAssetPkg_t       pkgAsset;
} CCAssetBuff_t;

#endif  //_PROD_H
