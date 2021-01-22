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
 @brief This file contains all the CryptoCell Management APIs, their enums and definitions.

 The following terms, used throughout this module, are defined in
 <em>Arm Architecture Reference Manual Armv8</em>:
 <ul><li>Privileged and unprivileged modes.</li>
 <li>Secure and Non-secure modes.</li></ul>
 */

/*!
 @defgroup cc_management CryptoCell management APIs
 @brief Contains CryptoCell Management APIs. See mbedtls_cc_mng.h.

 @{
 @ingroup cryptocell_api
 @}
 */

#ifndef _MBEDTLS_CC_MNG_H
#define _MBEDTLS_CC_MNG_H

/* *********************** Includes ***************************** */
#include "cc_pal_types_plat.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* *********************** Defines ***************************** */
/* LCS. */
/*! Chip manufacturer (CM LCS). */
#define CC_MNG_LCS_CM       0x0
/*! Device manufacturer (DM LCS). */
#define CC_MNG_LCS_DM       0x1
/*! Security enabled (Secure LCS).*/
#define CC_MNG_LCS_SEC_ENABLED  0x5
/*! RMA (RMA LCS). */
#define CC_MNG_LCS_RMA      0x7

/* *********************** Macros ***************************** */


/* *********************** Enums ***************************** */
/*! RMA statuses. */
typedef enum  {
    CC_MNG_NON_RMA              = 0,         /*!< Non-RMA: bit [30] = 0, bit [31] = 0. */
    CC_MNG_PENDING_RMA          = 1,         /*!< Pending RMA: bit [30] = 1, bit [31] = 0. */
    CC_MNG_ILLEGAL_STATE        = 2,         /*!< Illegal state: bit [30] = 0, bit [31] = 1. */
    CC_MNG_RMA                  = 3,         /*!< RMA: bit [30] = 1, bit [31] = 1. */
    CC_MNG_END_OF_RMA_STATUS    = 0x7FFFFFFF /*!< Reserved. */
}mbedtls_mng_rmastatus;

/*! AES HW key types. */
typedef enum  {
    CC_MNG_HUK_KEY         = 0,         /*!< Device root key (HUK). */
    CC_MNG_RTL_KEY         = 1,         /*!< Platform key (Krtl). */
    CC_MNG_PROV_KEY        = 2,         /*!< ICV provisioning key (Kcp). */
    CC_MNG_CE_KEY          = 3,         /*!< OEM code-encryption key (Kce). */
    CC_MNG_ICV_PROV_KEY    = 4,         /*!< OEM provisioning key (Kpicv). */
    CC_MNG_ICV_CE_KEY      = 5,         /*!< ICV code-encryption key (Kceicv). */
    CC_MNG_TOTAL_HW_KEYS   = 6,         /*!< Total number of HW Keys. */
    CC_MNG_END_OF_KEY_TYPE = 0x7FFFFFFF /*!< Reserved. */
}mbedtls_mng_keytype;

/*! APBC Only - parts IDs. */
typedef enum  {
    CC_MNG_APBC_SEC_ID      = 0,         /*!< Secure accesses. */
    CC_MNG_APBC_PRIV_ID     = 1,         /*!< Privileged accesses. */
    CC_MNG_APBC_INST_ID     = 2,         /*!< Instruction accesses. */
    CC_MNG_APBC_TOTAL_ID    = 3,         /*!< Total. */
    CC_MNG_APBC_END_OF_ID   = 0x7FFFFFFF /*!< Reserved. */
}mbedtls_mng_apbc_parts;

/*! APBC Part configuration. */
typedef enum  {
    CC_MNG_APBC_NO_CHANGE           = 0,         /*!< Use it as an input when there is no need to change bits.
                                                      Modify bit = 0. */
    CC_MNG_APBC_ALLOW_0_ALLOWLOCK_0 = 1,         /*!< Use it as an input when you need to set the 'Allow' bit to '0' and leave this part as unlocked.
                                                      Modify bit = 1, Allow bit = 0, Allow Lock bit = 0. */
    CC_MNG_APBC_ALLOW_0_ALLOWLOCK_1 = 2,         /*!< Use it as an input when you need to set the 'Allow' bit to '0' and lock this part.
                                                      Modify bit = 1, Allow bit = 0, Allow Lock bit = 1. */
    CC_MNG_APBC_ALLOW_1_ALLOWLOCK_0 = 3,         /*!< Use it as an input when you need to set the 'Allow' bit to '1' and leave this part as unlocked.
                                                      Modify bit = 1, Allow bit = 1, Allow Lock bit = 0. */
    CC_MNG_APBC_ALLOW_1_ALLOWLOCK_1 = 4,         /*!< Use it as an input when you need to set the 'Allow' bit to '1' and lock this part.
                                                      Modify bit = 1, Allow bit = 1, Allow Lock bit = 1. */
    CC_MNG_APBC_TOTAL_PARTS_CONFIG  = 5,         /*!< Total. */
    CC_MNG_APBC_END_OF_PARTS_CONFIG = 0x7FFFFFFF /*!< Reserved. */
}mbedtls_mng_apbc_parts_config;

/************************ Typedefs  ****************************/

/*! A uint8_t representation for the APBC parts in the AO_APB_FILTERING register. */
typedef union mbedtls_mng_apbc_part{
    uint8_t apbcPartVal;
    struct {
        uint8_t accessAllow       :   1; /*!< APB-C accepts only 'mbedtls_mng_apbc_parts' accesses. */
        uint8_t accessAllowLock   :   1; /*!< APB-C 'accessAllow' cannot be modified. */
        uint8_t accessModify      :   1; /*!< User decided to modify the upper couple. */
        uint8_t rfu               :   5; /*!< APBC Part Access Bits. */
    }apbcPartBits;
}mbedtls_mng_apbc_part;

/*! Input to the mbedtls_mng_apbc_config_set() function. */
typedef union mbedtls_mng_apbcconfig{
    uint32_t apbcConfigVal;                                   /*!< APB-C configuration values. */
    mbedtls_mng_apbc_part apbcPart[CC_MNG_APBC_TOTAL_ID + 1]; /*!< An array of the configuration bits for the:
                                                                   Secure, Privileged and Instruction parts. */
}mbedtls_mng_apbcconfig;


/* ****************************************** Public Functions **************************************** */
/*
Management APIs enable to set, get or obtain device status by reading or writing the
appropriate registers or the OTP.
*/
/* ********************************************************************************************** */
/*!
  @brief This function reads the OTP word of the OEM flags,
         and returns the OEM RMA flag status: TRUE or FALSE.

  The function returns the value only in DM LCS or Secure LCS.
  It validates the device RoT configuration, and returns the
  value only if both HBK0 and HBK1 are supported.
  Otherwise, it returns FALSE regardless to the OTP status.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_pending_rma_status_get(
                                        uint32_t *rmaStatus    /*!< [out] The RMA status. */
                                      );

/*!
  @brief This function verifies and returns the CryptoCell HW version.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_hw_version_get(
                 uint32_t *partNumber,    /*!< [out] The part number. */
                 uint32_t *revision       /*!< [out] The HW revision. */
);

/*!
  @brief This function sets CryptoCell to Secure mode.

  It is done only while CryptoCell is idle.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_cc_sec_mode_set(
                 CCBool_t isSecAccessMode,           /*!< [in] True: Set CryptoCell to Secure mode, or
                                                               False: Set CryptoCell to non-Secure mode. */
                 CCBool_t isSecModeLock              /*!< [in] True: Lock CryptoCell to current mode, or
                                                               False: Do not lock CryptoCell to current mode.
                                                               Allows calling this function again. */
);

/*!
  @brief This function sets CryptoCell to Privilege mode.

  It is done only while CryptoCell is idle.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_cc_priv_mode_set(
                  CCBool_t isPrivAccessMode,       /*!< [in] True: Set CryptoCell to privileged mode, or
                                                             False: Set CryptoCell to unprivileged mode. */
                  CCBool_t isPrivModeLock          /*!< [in] True: Lock CryptoCell to current mode, or
                                                             False: Do not lock CryptoCell to current mode.
                                                             Allows calling this function again. */
);

/*!
  @brief This function sets the shadow register of one of the
         HW Keys when the device is in CM LCS or DM LCS.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_debug_key_set(mbedtls_mng_keytype keyType,    /*!< [in] The HW-key type. One of the following values: HUK, Kcp, Kce, Kpicv, Kceicv. */
                   uint32_t *pHwKey,                          /*!< [in] A pointer to the HW-key buffer. */
                   size_t keySize                             /*!< [in] The size of the HW key in Bytes. */
);

/*!
  @brief This function retrieves the general configuration from the OTP.
         See <em>Arm TrustZone CryptoCell-312 Software Integrators Manual</em>.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_gen_config_get(
                                uint32_t *pOtpWord /*!< [out] The OTP configuration word. */
                               );

/*!
  @brief This function locks the usage of either Kcp, Kce, or both during runtime,
         in either Secure LCS or RMA LCS.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_oem_key_lock(
               CCBool_t kcpLock,          /*!< [in] The flag for locking Kcp usage. */
               CCBool_t kceLock           /*!< [in] The flag for locking Kce usage. */
);

/*!
  @brief This function sets the CryptoCell APB-C into one of the following modes:
         Secured access mode, Privileged access mode, or Instruction access.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_apbc_config_set(
                mbedtls_mng_apbc_parts_config securePartCfg, /*!< Secured access mode. */
                mbedtls_mng_apbc_parts_config privPartCfg, /*!< Privileged access mode.*/
                mbedtls_mng_apbc_parts_config instPartCfg /*!< Instruction access. */
);
/*!
  @brief This function requests usage of or releases the APB-C.

  @note This function must be called before and after each use of APB-C.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_apbc_access(
                             CCBool_t isApbcAccessUsed  /*!< [in] TRUE: Request usage of APB-C, or FALSE: Free APB-C. */
                           );

/*!
  @brief This function is called once the external PMU decides to power-down CryptoCell.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_suspend(
            uint8_t *pBackupBuffer,   /*!< [in] A pointer to a buffer that can be used for backup. */
            size_t backupSize         /*!< [in] The size of the backup buffer. Must be at least \c CC_MNG_MIN_BACKUP_SIZE_IN_BYTES. */
);

/*!
  @brief This function is called once the external PMU decides to power-up CryptoCell.

  @return CC_OK on success.
  @return A non-zero value from mbedtls_cc_mng_error.h on failure.
 */
int mbedtls_mng_resume(
               uint8_t *pBackupBuffer,    /*!< [in] A pointer to a buffer that can be used for backup. */
               size_t backupSize          /*!< [in] The size of the backup buffer. Must be at least \c CC_MNG_MIN_BACKUP_SIZE_IN_BYTES. */
);
#ifdef __cplusplus
}
#endif

#endif // _MBEDTLS_CC_MNG_H

