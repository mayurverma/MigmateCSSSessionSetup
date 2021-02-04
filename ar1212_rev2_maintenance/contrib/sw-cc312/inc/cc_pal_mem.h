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
 @brief This file contains functions for memory operations.

 The functions are generally implemented as wrappers to different operating-system calls.

 \note None of the described functions validate the input parameters, so that the behavior
       of the APIs in case of an illegal parameter is dependent on the behavior of the operating system.
 */

/*!
 @defgroup cc_pal_mem CryptoCell PAL memory operations
 @brief Contains memory-operation functions. See cc_pal_mem.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_MEM_H
#define _CC_PAL_MEM_H


#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"
#include "cc_pal_mem_plat.h"
#include "cc_pal_malloc_plat.h"
#include <stdlib.h>
#include <string.h>

    /*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

    /**** ----- Memory Operations APIs ----- ****/

/*!
  @brief This function compares between two given buffers, according to the given size.

  @return The return values are according to operating-system return values.
 */
#define CC_PalMemCmp(aTarget, aSource, aSize) CC_PalMemCmpPlat(aTarget, aSource, aSize)

/*!
  @brief This function copies \p aSize Bytes from the source buffer to the destination buffer.

  @return void.
 */
#define CC_PalMemCopy(aDestination, aSource, aSize) CC_PalMemCopyPlat(aDestination, aSource, aSize)

/*!
  @brief This function moves \p aSize Bytes from the source buffer to the destination buffer.
  This function supports overlapped buffers.

  @return void.
 */
#define CC_PalMemMove(aDestination, aSource, aSize) CC_PalMemMovePlat(aDestination, aSource, aSize)


/*!
  @brief This function sets \p aSize Bytes of \p aChar in the given buffer.

  @return void.
 */
#define CC_PalMemSet(aTarget, aChar, aSize) CC_PalMemSetPlat(aTarget, aChar, aSize)


/*!
  @brief This function sets \p aSize Bytes in the given buffer with zeroes.

  @return void.
 */
#define CC_PalMemSetZero(aTarget, aSize) CC_PalMemSetZeroPlat(aTarget, aSize)


/*!
  @brief This function allocates a memory buffer according to \p aSize.

  @return A pointer to the allocated buffer on success.
  @return NULL on failure.
 */
#define CC_PalMemMalloc(aSize) CC_PalMemMallocPlat(aSize)

/*!
  @brief This function reallocates a memory buffer according to \p aNewSize.
         The contents of the old buffer is moved to the new location.

  @return A pointer to the newly-allocated buffer on success.
  @return NULL on failure.
 */
#define CC_PalMemRealloc(aBuffer, aNewSize) CC_PalMemReallocPlat(aBuffer, aNewSize)

/*!
  @brief This function frees a previously-allocated buffer.


  @return void.
 */
#define CC_PalMemFree(aBuffer) CC_PalMemFreePlat(aBuffer)

#ifdef __cplusplus
}
#endif

#endif

