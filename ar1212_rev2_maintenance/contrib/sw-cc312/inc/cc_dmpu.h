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
 @brief This file contains all of the OEM production library APIs, their enums and definitions.
 */

/*!
 @defgroup DMPU CryptoCell OEM production library APIs
 @brief Contains CryptoCell OEM production library APIs.

 See cc_dmpu.h.

 @{
 @ingroup prod
 @}
 */
#ifndef _DMPU_H
#define _DMPU_H

#include "cc_pal_types_plat.h"
#include "cc_prod.h"

/************************ Defines ******************************/

/*! The size of the OEM production library workspace in Bytes, needed by the library for internal use. */
#define DMPU_WORKSPACE_MINIMUM_SIZE  1536

/*! The size of the Hbk1 buffer in words. */
#define DMPU_HBK1_SIZE_IN_WORDS  4

/*! The size of the Hbk buffer in words. */
#define DMPU_HBK_SIZE_IN_WORDS  8

/************************ Enums ********************************/

/*! The type of the unique data. */
typedef enum {
    /*! The device uses Hbk1. */
        DMPU_HBK_TYPE_HBK1 = 1,
    /*! The device uses a full Hbk. */
        DMPU_HBK_TYPE_HBK = 2,
    /*! Reserved. */
        DMPU_HBK_TYPE_RESERVED  = 0x7FFFFFFF,
} CCDmpuHBKType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/

/*!
  The device use of the Hbk buffer.

  If the device uses Hbk0 and Hbk1, then the Hbk1 field is used.
  Otherwise, the Hbk field is used.
 */
typedef union {
        uint8_t hbk1[DMPU_HBK1_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE] /*!< Hbk1 buffer if used by the device. */;
        uint8_t hbk[DMPU_HBK_SIZE_IN_WORDS*CC_PROD_32BIT_WORD_SIZE] /*!< Hbk buffer, that is, the full 256 bits. */;
} CCDmpuHbkBuff_t;



/*! The OEM production library input defines .*/
typedef struct {
        CCDmpuHBKType_t   hbkType /*!< The type of Hbk: Hbk1 with 128 bits, or Hbk with 256 bits. */;
        CCDmpuHbkBuff_t   hbkBuff /*!< The Hbk buffer. */;
        CCAssetType_t     kcpDataType /*!< The Kcp asset type. Allowed values are: Not used, Plain-asset or Package. */;
        CCAssetBuff_t     kcp /*!< The Kcp buffer, if \p kcpDataType is plain asset or package. */;
        CCAssetType_t     kceDataType /*!< The Kce asset type. Allowed values are: Not used, Plain-asset or Package. */;
        CCAssetBuff_t     kce /*!< The Kce buffer, if \p kceDataType is plain asset or package. */;
        uint32_t          oemMinVersion /*!< The minimal SW version of the OEM. */;
        uint32_t          oemDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE] /*!< The default DCU lock bits of the OEM. */;
}CCDmpuData_t;



/************************ Functions *****************************/

/*!
 @brief This function burns all OEM assets into the OTP of the device.

 The user must perform a power-on-reset (PoR) to trigger LCS change to Secure.

 @return \c CC_OK on success.
 @return A non-zero value from cc_prod_error.h on failure.
*/
CIMPORT_C CCError_t  CCProd_Dmpu(
        unsigned long   ccHwRegBaseAddr,   /*!< [in] The base
                                                address of CrytoCell HW registers. */
        CCDmpuData_t    *pDmpuData,        /*!< [in] A pointer to the
                                                defines structure of the OEM. */
        unsigned long   workspaceBaseAddr, /*!< [in] The base
                                                address of the workspace for
                                                internal use. */
        uint32_t        workspaceSize      /*!< [in] The size of
                                                provided workspace. Must be at least
                                                \p DMPU_WORKSPACE_MINIMUM_SIZE. */
) ;


#endif  //_DMPU_H
