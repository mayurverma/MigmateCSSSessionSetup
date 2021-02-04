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

#ifndef _CC_PAL_MEMALLOC_INT_H
#define _CC_PAL_MEMALLOC_INT_H


#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
/**
* @brief File Description:
*        This file contains wrappers for memory operations APIs.
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
 * @brief This function purpose is to allocate a memory buffer according to aSize.
 *
 *
 * @return The function returns a pointer to allocated buffer or NULL if allocation failed.
 */
void* CC_PalMemMallocPlat(size_t  aSize /*!< [in] Number of bytes to allocate. */);

/*!
 * @brief This function purpose is to reallocate a memory buffer according to aNewSize.
 *        The content of the old buffer is moved to the new location.
 *
 * @return The function returns a pointer to the newly allocated buffer or NULL if allocation failed.
 */
void* CC_PalMemReallocPlat(  void* aBuffer,     /*!< [in] Pointer to allocated buffer. */
                             size_t  aNewSize   /*!< [in] Number of bytes to reallocate. */);

/*!
 * @brief This function purpose is to free allocated buffer.
 *
 *
 * @return void.
 */
void CC_PalMemFreePlat(void* aBuffer /*!< [in] Pointer to allocated buffer.*/);


#ifdef __cplusplus
}
#endif

#endif


