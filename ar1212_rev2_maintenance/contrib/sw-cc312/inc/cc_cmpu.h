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
 @brief This file contains all of the ICV production library APIs, their enums and definitions.
*/

/*!
 @defgroup CMPU CryptoCell ICV production library APIs
 @brief Contains CryptoCell ICV production library APIs.

 See cc_cmpu.h.

 @{
 @ingroup prod
 @}
 */

#ifndef _CMPU_H
#define _CMPU_H

#include "cc_pal_types_plat.h"
#include "cc_prod.h"

/************************ Defines ******************************/

/*! The size of the ICV production library workspace in Bytes, needed by the library for internal use. */
#define CMPU_WORKSPACE_MINIMUM_SIZE  4096

/*! The size of the ICV production library unique buffer in Bytes: Hbk0 or user data. */
#define PROD_UNIQUE_BUFF_SIZE   16
/************************ Enums ********************************/

/*! The unique data type. */
typedef enum {
        CMPU_UNIQUE_IS_HBK0 = 1, /*!< The device uses the unique data as Hbk0. */
        CMPU_UNIQUE_IS_USER_DATA = 2, /*!< The device uses the unique data as a random value.
                                           Hbk0 is not used for the device. */
        CMPU_UNIQUE_RESERVED    = 0x7FFFFFFF, /*!< Reserved. */
} CCCmpuUniqueDataType_t;

/************************ Typedefs  ****************************/


/************************ Structs  ******************************/

/*!
  @brief The device use of the unique buffer.

  If the device uses Hbk0, then the \p hbk0 field is used.
  Otherwise, a random buffer for the \p userData field is used.
 */
typedef union {
        uint8_t hbk0[PROD_UNIQUE_BUFF_SIZE] /*!< The Hbk0 buffer, if used by the device. */;
        uint8_t userData[PROD_UNIQUE_BUFF_SIZE] /*!< Any random value, if Hbk0 is not used by the device. */;
} CCCmpuUniqueBuff_t;


/*! The ICV production library input options. */
typedef struct {
        CCCmpuUniqueDataType_t  uniqueDataType /*!< The unique data type: Hbk0 or a random user-defined data. */;
        CCCmpuUniqueBuff_t      uniqueBuff /*!< The unique data buffer. */;
        CCAssetType_t           kpicvDataType; /*!< The asset type of the Kpicv. Allowed values are: Not used, Plain-asset or Package. */
        CCAssetBuff_t           kpicv /*!< The buffer of the Kpicv, if its type is plain-asset or package. */;
        CCAssetType_t           kceicvDataType /*!< The asset type of the Kceicv. Allowed values are:Not used, Plain-asset or Package. */;
        CCAssetBuff_t           kceicv /*!< The buffer of the Kceicv, if its type is plain asset or package. */;
        uint32_t                icvMinVersion /*!< The minimal SW version of the ICV. Valid only if Hbk0 is used. */;
        uint32_t                icvConfigWord /*!< The ICV configuration word. */;
        uint32_t                icvDcuDefaultLock[PROD_DCU_LOCK_WORD_SIZE] /*!< The default DCU lock bits of the ICV. Valid only if Hbk0 is used. */;
}CCCmpuData_t;



/************************ Functions *****************************/

/*!
 @brief This function burns all ICV assets into the OTP of the device.

 The user must perform a power-on-reset (PoR) to trigger LCS change to DM LCS.

 @return \c CC_OK on success.
 @return A non-zero value from cc_prod_error.h on failure.
*/
CIMPORT_C CCError_t  CCProd_Cmpu(
        unsigned long    ccHwRegBaseAddr,   /*!< [in] The base address of
                                                 CrytoCell HW register. */
        CCCmpuData_t     *pCmpuData,        /*!< [in] A pointer to the ICV
                                                 defines structure. */
        unsigned long    workspaceBaseAddr, /*!< [in] The base address of the
                                                 workspace for internal use. */
        uint32_t         workspaceSize      /*!< [in] The size of the provided
                                                 workspace. Must be at least
                                                 #CMPU_WORKSPACE_MINIMUM_SIZE. */
                                    );


#endif  //_CMPU_H
