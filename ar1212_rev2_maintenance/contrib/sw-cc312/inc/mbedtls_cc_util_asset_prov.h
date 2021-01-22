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
 @file mbedtls_cc_util_asset_prov.h
 @brief This file contains CryptoCell runtime-library ICV and OEM asset-provisioning APIs and definitions.
 */

/*!
 @defgroup cc_util_asset_prov CryptoCell runtime-library asset-provisioning APIs
 @brief Contains CryptoCell runtime-library ICV and OEM asset-provisioning APIs and definitions. See mbedtls_cc_util_asset_prov.h.

 @{
 @ingroup cc_util
 @}
 */

#ifndef  _MBEDTLS_CC_UTIL_ASSET_PROV_H
#define  _MBEDTLS_CC_UTIL_ASSET_PROV_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "cc_pal_types_plat.h"

/*! The maximal size of an asset package. */
#define CC_ASSET_PROV_MAX_ASSET_PKG_SIZE  4144

/*! The type of key used to pack the asset. */
typedef enum {
       ASSET_PROV_KEY_TYPE_KPICV = 1, /*!< ICV: The Kpicv key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_KCP = 2, /*!< OEM: The Kcp key was used to pack the asset. */
       ASSET_PROV_KEY_TYPE_RESERVED    = 0x7FFFFFFF, /*!< Reserved. */
} CCAssetProvKeyType_t;


/*!
 @brief This API securely provisions ICV or OEM assets to devices, using CryptoCell.

 It takes an encrypted and authenticated asset package produced by the ICV or OEM asset-packaging offline utility
 (using AES-CCM with key derived from Kpicv or Kcp respectively, and the asset identifier), authenticates and decrypts it.
 The decrypted asset data is returned to the caller.

 @note  The function is valid in all LCSes. However, an error is returned if the requested key is locked.

 @return \c CC_UTIL_OK on success.
 @return A non-zero value on failure, as defined in cc_util_error.h.
 */
CCError_t mbedtls_util_asset_pkg_unpack(
            CCAssetProvKeyType_t        keyType,         /*!< [in]  The type of key used to pack the asset.*/
            uint32_t                    assetId,         /*!< [in] A 32-bit index identifying the asset, in big-endian order.  */
            uint32_t                    *pAssetPackage,  /*!< [in]  The encrypted and authenticated asset package. */
            size_t                      assetPackageLen, /*!< [in] The length of the asset package. Must not exceed CC_ASSET_PROV_MAX_ASSET_PKG_SIZE. */
            uint32_t                    *pAssetData,     /*!< [out] The buffer for retrieving the decrypted asset data. */
            size_t                      *pAssetDataLen   /*!< [in, out] In: The size of the available asset-data buffer. Maximal size is 4K bytes, or
                                                              Out: A pointer to the actual length of the decrypted asset data. */
            );


#ifdef __cplusplus
}
#endif

#endif /*_MBEDTLS_CC_UTIL_ASSET_PROV_H*/

