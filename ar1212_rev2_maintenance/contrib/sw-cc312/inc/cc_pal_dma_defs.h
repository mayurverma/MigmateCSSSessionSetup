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
@brief This file contains the platform-dependent DMA definitions.
@defgroup ssi_pal_dma_defs CryptoCell PAL DMA specific definitions
@{
@ingroup ssi_pal
*/

#ifndef _CC_PAL_DMA_DEFS_H
#define _CC_PAL_DMA_DEFS_H


#ifdef __cplusplus
extern "C"
{
#endif

/*! Definition for DMA buffer handle.*/
typedef void *CC_PalDmaBufferHandle;

/*! DMA directions configuration. */
typedef enum {
    CC_PAL_DMA_DIR_NONE = 0, /*!< No direction. */
    CC_PAL_DMA_DIR_TO_DEVICE = 1,   /*!< The original buffer is the input to the operation. It should be copied or mapped to the temporary buffer prior to activating the HW on it. */
    CC_PAL_DMA_DIR_FROM_DEVICE = 2, /*!< The temporary buffer holds the output of the HW. This API should copy or map it to the original output buffer.*/
    CC_PAL_DMA_DIR_BI_DIRECTION = 3, /*!< The result is written over the original data at the same address. Should be treated as \p CC_PAL_DMA_DIR_TO_DEVICE and \p CC_PAL_DMA_DIR_FROM_DEVICE.*/
    CC_PAL_DMA_DIR_MAX,      /*!< Maximal DMA direction options. */
    CC_PAL_DMA_DIR_RESERVE32 = 0x7FFFFFFF  /*!< Reserved.*/
}CCPalDmaBufferDirection_t;


#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif


