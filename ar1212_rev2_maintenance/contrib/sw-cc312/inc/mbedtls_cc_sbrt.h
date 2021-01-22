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
 @brief This file contains CryptoCell Secure Boot certificate-chain processing APIs.
 */

/*!
 @defgroup cc_sbrt CryptoCell Secure Boot certificate-chain-processing APIs.
 @brief Contains CryptoCell Secure Boot certificate-chain-processing APIs. See mbedtls_cc_sbrt.h.

 @{
 @ingroup cc_sb
 @}
 */

#ifndef  _MBEDTLS_CC_SBRT_H
#define  _MBEDTLS_CC_SBRT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "secureboot_defs.h"
#include "secureboot_gen_defs.h"
/*!
  @brief This function initializes the Secure Boot certificate-chain processing.

  It initializes the internal data fields of the certificate package.

  @note This function must be the first API called when processing a Secure Boot certificate chain.

  @return \c CC_OK on success.
  @return A non-zero value from sbrom_bsv_error.h on failure.
 */

CCError_t mbedtls_sb_cert_chain_cerification_init(
    CCSbCertInfo_t *certPkgInfo     /*!< [in/out] A pointer to the information
                                                  about the certificate package. */
    );

/*!
  @brief This function verifies a single certificate package containing
  either a key or content certificate.

  It verifies the following:
  <ul><li>The public key, as saved in the certificate, against its hash, found
  in either the OTP memory (HBK) or in \p certPkgInfo.</li>
  <li>The RSA signature of the certificate.</li>
  <li>The SW version in the certificate is higher than or equal to the minimal
  SW version, as recorded on the device and passed in \p certPkgInfo.</li>
  <li>For content certificates: Each SW module against its hash in the certificate.</li></ul>

  @note The certificates may reside in the memory or in the flash. flashReadFunc()
        must be implemented accordingly.
  @note Certificates and images must both be placed either in the memory, or in the flash.

  @return \c CC_OK on success.
  @return A non-zero value from bsv_error.h on failure.
 */
CCError_t mbedtls_sb_cert_verify_single(
    CCSbFlashReadFunc flashReadFunc,    /*!< [in] A pointer to the flash-read function. */
        void *userContext,              /*!< [in] An additional pointer for flashRead() usage. May be NULL. */
        CCAddr_t certStoreAddress,      /*!< [in] The address where the certificate is located. This address is provided to \p flashReadFunc. */
        CCSbCertInfo_t *pCertPkgInfo,   /*!< [in/out] A pointer to the certificate-package information. */
        uint32_t *pHeader,              /*!< [in/out] A pointer to a buffer used for extracting the X.509 TBS Headers. \note Must be NULL for proprietary certificates. */
        uint32_t  headerSize,           /*!< [in] The size of \p pHeader in Bytes. \note Must be 0 for proprietary certificates. */
        uint32_t *pWorkspace,           /*!< [in] Buffer for the internal use of the function. */
        uint32_t workspaceSize          /*!< [in] The size of the workspace in Bytes. \note Must be at least #CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES. */
    );

/*!
  @brief This function changes the storage address of a specific SW image in the content certificate.

  @note The certificate must be loaded to the RAM before calling this function.
  @note The function does not verify the certificate before the address change.

  @return \c CC_OK on success.
  @return A non-zero value from bsv_error.h on failure.
 */
CCError_t mbedtls_sb_sw_image_store_address_change(
    uint32_t *pCert,            /*!< [in] The certificate address, after it has been loaded to memory. */
    uint32_t maxCertSizeWords,  /*!< [in] The certificate boundaries, that is, the maximal memory size allocated for the certificate in words . */
    CCAddr_t address,           /*!< [in] The new storage address to change to. */
    uint32_t indexOfAddress     /*!< [in] The index of the SW image in the content certificate, starting from 0. */
    );

#ifdef __cplusplus
}

#endif /*_MBEDTLS_SBRT_H*/

#endif

