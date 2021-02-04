/****************************************************************************
 * The confidential and proprietary information contained in this file may    *
 * only be used by a person authorised under and to the extent permitted      *
 * by a subsisting licensing agreement from Arm Limited (or its affiliates).  *
 *   (C) COPYRIGHT [2001-2018] Arm Limited (or its affiliates).                    *
 *       ALL RIGHTS RESERVED                                                  *
 * This entire notice must be reproduced on all copies of this file           *
 * and copies of this file may only be made by a person if such person is     *
 * permitted to do so under the terms of a subsisting license agreement       *
 * from Arm Limited (or its affiliates).                                      *
*****************************************************************************/

#ifndef _SECURE_BOOT_DEFS_H
#define _SECURE_BOOT_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! @file
@brief This file contains basic type definitions for the secure boot.
*/

#include "cc_crypto_boot_defs.h"
#include "cc_sec_defs.h"
#include "secureboot_basetypes.h"
#include "secureboot_gen_defs.h"


/* General definitions */
/***********************/

/*! Structure used as input/output to the Secure Boot verification API. */
typedef struct{
    uint32_t otpVersion;                /*!< NV counter saved in OTP. */
    CCSbPubKeyIndexType_t keyIndex;         /*!< Enumeration defining the key hash to retrieve: 128-bit HBK0, 128-bit HBK1, or 256-bit HBK. */
    uint32_t activeMinSwVersionVal;         /*!< Holds the SW version value for the certificate-chain.  */
    CCHashResult_t pubKeyHash;          /*!< in/out-
                                <ul><li> In: Hash of the public key (N||Np), to compare to the public key stored in the certificate.</li>
                                    <li> Out: Hash of the public key (N||Np) stored in the certificate, to be used for verification
                                      of the public key of the next certificate in the chain. </li></ul>*/
    uint32_t initDataFlag;              /*!< Initialization indication. Internal flag. */
    uint32_t isIcvCertOnly;                 /*!< Internal flag indicating only ICV certificate chain is valid. */
}CCSbCertInfo_t;



/*! Defines the SW image certificate's data size. */
#define SW_REC_SIGNED_DATA_SIZE_IN_BYTES            44  // HS(8W) + load_adddr(1) + maxSize(1) + isCodeEncUsed(1)
/*! Defines the SW image additional data size. */
#define SW_REC_NONE_SIGNED_DATA_SIZE_IN_BYTES       8   // storage_addr(1) + ActualSize(1)

/*! Indication whether or not to load the SW image to memory. */
#define CC_SW_COMP_NO_MEM_LOAD_INDICATION       0xFFFFFFFFUL


#ifdef __cplusplus
}
#endif

#endif


