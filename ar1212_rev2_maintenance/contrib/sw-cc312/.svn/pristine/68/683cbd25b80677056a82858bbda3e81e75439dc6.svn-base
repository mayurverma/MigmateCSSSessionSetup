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


#ifndef __CC_HAL_H__
#define __CC_HAL_H__

/*!
@file
@brief This file contains HAL definitions and APIs.
*/

#include <stdint.h>
#include "cc_hal_plat.h"
#include "cc_pal_types_plat.h"

/*! HAL return code definitions. */
typedef enum {
    CC_HAL_OK = 0,
    CC_HAL_ENODEV,        /* Device not opened or does not exist */
    CC_HAL_EINTERNAL,     /* Internal driver error (check system log) */
    CC_HAL_MAPFAILED,
    CC_HAL_ENOTSUP,       /* Unsupported function/option */
    CC_HAL_ENOPERM,       /* Not enough permissions for request */
    CC_HAL_EINVAL,        /* Invalid parameters */
    CC_HAL_ENORSC,        /* No resources available (e.g., memory) */
    CC_HAL_RESERVE32B = 0x7FFFFFFFL
} CCHalRetCode_t;

/*!
 * @brief This function is used to map ARM TrustZone CryptoCell TEE registers to Host virtual address space.
    It is called by ::CC_LibInit, and returns a non-zero value in case of failure.
        The existing implementation supports Linux environment. In case virtual addressing is not used, the function can be minimized to contain only the
    following line, and return OK:
        gCcRegBase = (uint32_t)DX_BASE_CC;
  @return CC_HAL_OK on success.
  @return A non-zero value in case of failure.
*/
int CC_HalInit(void);


/*!
 * @brief This function is used to wait for the IRR interrupt signal.
 *
 * @return CCError_t - return CC_OK upon success
 */
CCError_t CC_HalWaitInterrupt(uint32_t data /*!< [in] The interrupt bits to wait upon. */ );

/*!
 * @brief This function is used to wait for the IRR interrupt signal.
 * The existing implementation performs a "busy wait" on the IRR.
 *
 * @return CCError_t - return CC_OK upon success
 */
CCError_t CC_HalWaitInterruptRND(uint32_t data);

/*!
 * @brief This function clears the DSCRPTR_COMPLETION bit in the ICR signal.
 */
void CC_HalClearInterrupt(uint32_t data);

/*!
 * @brief This function is called by CC_LibInit and is used for initializing the ARM TrustZone CryptoCell TEE cache settings registers.
          The existing implementation sets the registers to their default values in HCCC cache coherency mode
      (ARCACHE = 0x2, AWCACHE = 0x7, AWCACHE_LAST = 0x7).
          These values should be changed by the customer in case the customer's platform requires different HCCC values, or in case SCCC is needed
      (the values for SCCC are ARCACHE = 0x3, AWCACHE = 0x3, AWCACHE_LAST = 0x3).

 * @return void
 */
void CC_HalInitHWCacheParams(void);

/*!
 * @brief This function is used to unmap ARM TrustZone CryptoCell TEE registers' virtual address.
      It is called by CC_LibFini, and returns a non-zero value in case of failure.
      In case virtual addressing is not used, the function can be minimized to be an empty function returning OK.
   @return CC_HAL_OK on success.
   @return A non-zero value in case of failure.
 */
int CC_HalTerminate(void);

/*!
 * @brief This function is used to clear the interrupt vector.

 * @return void.
 */
void CC_HalClearInterruptBit(uint32_t data /*!< [in] The interrupt bits to clear. */);

/*!
 * @brief This function is used to mask IRR interrupts.

 * @return void.
 */
void CC_HalMaskInterrupt(uint32_t data /*!< [in] The interrupt bits to mask. */);


#endif

