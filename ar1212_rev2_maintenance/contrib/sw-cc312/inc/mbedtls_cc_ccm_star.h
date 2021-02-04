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

 @brief This file contains the CryptoCell AES-CCM star APIs, their enums and definitions.

 This API supports AES-CCM*, as defined in <em>IEEE 802.15.4: IEEE Standard for Local and
 metropolitan area networks— Part 15.4: Low-Rate Wireless Personal Area Networks (LR-WPANs)</em>,
 with the instantiations defined in section B.3.2, and the nonce defined in section 7.3.2.
 */

/*!
 @defgroup cc_aesccm_star CryptoCell AES-CCM star APIs
 @brief Contains the CryptoCell AES-CCM star APIs. See mbedtls_cc_ccm_star.

 @{
 @ingroup cc_aes
 @}
 */

#ifndef _MBEDTLS_AES_CCM_STAR_H
#define _MBEDTLS_AES_CCM_STAR_H


#include "cc_pal_types.h"
#include "cc_error.h"

#include "mbedtls/ccm.h"
#include "mbedtls_ccm_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/************************ Defines ******************************/

/************************ Typedefs  ****************************/

/************************ Enums ********************************/

/************************ Structs  ******************************/

/************************ context Structs  ******************************/

/*!
  @brief           This function initializes the CCM star context.

                   It makes the context ready for mbedtls_ccm_star_setkey() or
                   mbedtls_ccm_star_free().
 */
void mbedtls_ccm_star_init(
                            mbedtls_ccm_context *ctx  /*!< The CCM star context to initialize. */
                           );

/*!
  @brief           This function initializes the CCM star context set in the
                   \p ctx parameter and sets the encryption or decryption key.


  @return          \c 0 on success.
  @return          A cipher-specific error code on failure.
 */
int mbedtls_ccm_star_setkey(
                             mbedtls_ccm_context *ctx,   /*!< The CCM star context to initialize. */
                             mbedtls_cipher_id_t cipher, /*!< The 128-bit block cipher to use. */
                             const unsigned char *key,   /*!< The encryption or decryption key. */
                             unsigned int keybits        /*!< The size of the key in bits. This must be acceptable by the cipher. */
                            );

/*!
  @brief           This function releases and clears the specified CCM star context
                   and underlying cipher sub-context.
 */
void mbedtls_ccm_star_free(
                            mbedtls_ccm_context *ctx /*!< The CCM star context to free. */
                          );
/*!
  @brief           This function encrypts a buffer using CCM star.


  @note            The tag is written to a separate buffer. To concatenate
                   the \p tag with the \p output, as done in <em>RFC-3610:
                   Counter with CBC-MAC (CCM)</em>, use
                   \p tag = \p output + \p length, and make sure that the
                   output buffer is at least \p length + \p tag_len wide.

  @return          \c 0 on success.
 */
int mbedtls_ccm_star_encrypt_and_tag(
                                      mbedtls_ccm_context *ctx,   /*!< The CCM star context to use for encryption. */
                                      size_t length,              /*!< The length of the input data in Bytes. */
                                      const unsigned char *iv,    /*!< Initialization vector (nonce). */
                                      size_t iv_len,              /*!< The length of the IV in Bytes. Must be 13 Bytes. */
                                      const unsigned char *add,   /*!< The additional data field. */
                                      size_t add_len,             /*!< The length of additional data in Bytes.
                                                                       Must be less than 2^16 - 2^8. */
                                      const unsigned char *input, /*!< The buffer holding the input data. */
                                      unsigned char *output,      /*!< The buffer holding the output data.
                                                                       Must be at least \p length Bytes wide. */
                                      unsigned char *tag,         /*!< The buffer holding the tag. */
                                      size_t tag_len              /*!< The length of the tag to generate in Bytes:
                                                                       4, 6, 8, 10, 12, 14 or 16. */
                                      );

/*!
  @brief           This function performs a CCM star authenticated decryption
                   of a buffer.

  @return          \c 0 if successful and authenticated.
  @return          #MBEDTLS_ERR_CCM_AUTH_FAILED if the tag does not match.
 */
int mbedtls_ccm_star_auth_decrypt(
                                   mbedtls_ccm_context *ctx,   /*!< The CCM star context to use for decryption. */
                                   size_t length,              /*!< The length of the input data in Bytes. */
                                   const unsigned char *iv,    /*!< Initialization vector. */
                                   size_t iv_len,              /*!< The length of the IV in Bytes. */
                                   const unsigned char *add,   /*!< The additional data field. */
                                   size_t add_len,             /*!<  The length of additional data in Bytes.*/
                                   const unsigned char *input, /*!< The buffer holding the input data. */
                                   unsigned char *output,      /*!<  The buffer holding the output data.*/
                                   const unsigned char *tag,   /*!< The buffer holding the tag. */
                                   size_t tag_len              /*!< The length of the tag in Bytes. */
                                   );

/*!
  @brief               This function receives the MAC source address, the
                       frame counter, and the MAC size, and returns the
                       required nonce for AES-CCM*, as defined in
                       <em>IEEE 802.15.4: IEEE Standard for Local and
                       metropolitan area networks— Part 15.4: Low-Rate
                       Wireless Personal Area Networks (LR-WPANs)</em>.

  @note                This API should be called before mbedtls_ccm_star_init(),
                       and the generated nonce should be provided to this function.

  @return              \c CC_OK on success.
  @return              A non-zero value on failure, as defined cc_aesccm_error.h.
 */
int mbedtls_ccm_star_nonce_generate(
                                     unsigned char * src_addr, /*!< The MAC address in EUI-64 format. */
                                     uint32_t frame_counter, /*!< The MAC frame counter. */
                                     uint8_t size_of_t, /*!< The size of the AES-CCM* MAC tag in Bytes:
                                                                             4, 6, 8, 10, 12, 14 or 16. */
                                     unsigned char * nonce_buf /*!< The required nonce for AES-CCM*. */
                                    );

 #ifdef __cplusplus
}
#endif

#endif /* _MBEDTLS_AES_CCM_STAR_H */
