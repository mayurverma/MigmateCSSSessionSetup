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



#ifndef _CC_PAL_BUFF_ATTR_H
#define _CC_PAL_BUFF_ATTR_H

/*!
@file
@brief This file contains the definitions and APIs to get inout data buffer attributes.
       This is a place holder for platform specific inout data attributes functions implementation
       The module should be updated whether data buffer is secure or non-secure,
       in order to notify the low level driver how to configure the HW accordigly.
@defgroup cc_pal_buff_attr CryptoCell PAL Data Buffer Attributes APIs
@{
@ingroup cc_pal

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"

/******************************************************************************
*               Buffer Information
******************************************************************************/

/*! User buffer attribute (secure / non-secure). */
#define DATA_BUFFER_IS_NONSECURE    1
#define DATA_BUFFER_IS_SECURE       0
/*! Buffer attribute (secure / non-secure) as used by arm_cmse.h . */
#define CMSE_NONSECURE              0
#define CMSE_SECURE                 1

/******************************************************************************
*               Functions
******************************************************************************/

/**
 * @brief This function purpose is to verify the buffer's attributes according to address, size, and type (in/out).
 * The function returns whether the buffer is secure or non-secure.
 * In any case of invalid memory, the function shall return an error (i.e.mixed regions of secured and non-secure memory).
 *
 * @return Zero on success.
 * @return A non-zero value in case of failure.
 */
CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer, /*!< [in] Address of data buffer. */
                                  size_t   buffSize,                /*!< [in] Buffer size in bytes. */
                                  uint8_t  buffType,                /* ! [in] Input for read / output for write */
                                  uint8_t  *pBuffNs                 /*!< [out] HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
                                  );

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif


