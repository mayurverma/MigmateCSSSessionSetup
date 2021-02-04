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

#ifndef _BOOT_IMAGES_VERIFIER_DEF_H
#define _BOOT_IMAGES_VERIFIER_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*! @file
@brief This file contains definitions used for the Secure Boot and Secure Debug APIs.
     */

#include "cc_pal_types.h"

/*! Defines maximum number of SW images per content certificate. */
#define CC_SB_MAX_NUM_OF_IMAGES 16

#ifdef CC_SB_X509_CERT_SUPPORTED
    /*! Defines certificate maximal size in bytes.*/
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0xB10)
#else
    /*! Defines certificate maximal size in bytes. */
#define CC_SB_MAX_CERT_SIZE_IN_BYTES    (0x700)
#endif
    /*! Defines certificate maximal size in words.*/
#define CC_SB_MAX_CERT_SIZE_IN_WORDS    (CC_SB_MAX_CERT_SIZE_IN_BYTES/CC_32BIT_WORD_SIZE)

    /*! Defines Secure Debug maximal workspace size in bytes. This workspace is used to store the
     * RSA parameters (such as the modulus and signature).*/
#define CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES   (0x350)

    /*! Defines minimal workspace size.
The Secure Boot APIs use a temporary workspace for processing the data that is read from the flash,
prior to loading the SW modules to their designated memory addresses. This workspace must be large enough to
accommodate the size of the certificates, and twice the size of the data that is read from flash in each processing round.
The definition of CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES is comprised of CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES
and additional space for the certificate itself, which resides in the workspace at the same time the
SW images data is processed.\n
It is assumed that the optimal size of the data to read in each processing round is 4KB, based on the standard flash memory page size.
Therefore, the size of the double buffer (CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES) is defined by default as 8KB in the project configuration file.
This can be changed to accommodate the optimal value in different environments.
CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES is defined by the Boot Services makefile as equal to CC_CONFIG_SB_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES.

\note When writing code that uses the Secure Boot APIs, and includes the bootimagesverifier_def.h file,
the value of CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES must be defined by the partner's makefile to be exactly the same value as was
used when compiling the SBROM library, and CC_SB_X509_CERT_SUPPORTED must be defined in the Makefile (according to CC_CONFIG_SB_X509_CERT_SUPPORTED definition). \par
\note The size of CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES must be a multiple of the hash SHA256 block size (64 bytes).
     */


#define CC_SB_MIN_WORKSPACE_SIZE_IN_BYTES   (CC_SB_MAX_CERT_SIZE_IN_BYTES + CC_MAX(CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES, CC_DOUBLE_BUFFER_MAX_SIZE_IN_BYTES))


#ifdef __cplusplus
}
#endif

#endif


