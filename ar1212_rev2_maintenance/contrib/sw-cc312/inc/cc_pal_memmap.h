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
 @brief This file contains functions for memory mapping.

       \note None of the described functions validate the input parameters, so that the behavior
       of the APIs in case of an illegal parameter is dependent on the behavior of the operating system.
 */

/*!
 @defgroup cc_pal_memmap CryptoCell PAL memory mapping APIs
 @brief Contains memory mapping functions. See cc_pal_memmap.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_MEMMAP_H
#define _CC_PAL_MEMMAP_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "cc_pal_types.h"
#include "cc_address_defs.h"


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
  @brief This function returns the base virtual address that maps the base physical address.

  @return \c 0 on success.
  @return A non-zero value in case of failure.
 */
uint32_t CC_PalMemMap(
        CCDmaAddr_t physicalAddress, /*!< [in] The starting physical address of the I/O range to be mapped. */
        uint32_t mapSize,   /*!< [in] The number of Bytes that were mapped. */
        uint32_t **ppVirtBuffAddr /*!< [out] A pointer to the base virtual address to which the physical pages were mapped. */ );


/*!
  @brief This function unmaps a specified address range that was previously mapped by #CC_PalMemMap.

  @return \c 0 on success.
  @return A non-zero value in case of failure.
 */
uint32_t CC_PalMemUnMap(uint32_t *pVirtBuffAddr, /*!< [in] A pointer to the base virtual address to which the physical pages were mapped. */
                    uint32_t mapSize       /*!< [in] The number of Bytes that were mapped. */);

#ifdef __cplusplus
}
#endif

#endif
