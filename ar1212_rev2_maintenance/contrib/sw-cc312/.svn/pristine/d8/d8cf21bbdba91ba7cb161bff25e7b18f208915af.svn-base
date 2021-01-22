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
 @brief This file contains functions for resource management (mutex operations).

       These functions are generally implemented as wrappers to different operating-system calls.

       \note None of the described functions validate the input parameters, so that the behavior
       of the APIs in case of an illegal parameter is dependent on the behavior of the operating system.
 */

/*!
 @defgroup cc_pal_mutex CryptoCell PAL mutex APIs
 @brief Contains resource management functions. See cc_pal_mutex.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_MUTEX_H
#define _CC_PAL_MUTEX_H

#include "cc_pal_mutex_plat.h"
#include "cc_pal_types_plat.h"

#ifdef __cplusplus
extern "C"
{
#endif



/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
  @brief This function creates a mutex.


  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexCreate(CC_PalMutex *pMutexId /*!< [out] A pointer to the handle of the created mutex. */);


/*!
  @brief This function destroys a mutex.


  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexDestroy(CC_PalMutex *pMutexId /*!< [in] A pointer to handle of the mutex to destroy. */);


/*!
  @brief This function waits for a mutex with \p aTimeOut. \p aTimeOut is
         specified in milliseconds. A value of \p aTimeOut=CC_INFINITE means that the function will not return.

  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexLock(CC_PalMutex *pMutexId, /*!< [in] A pointer to handle of the mutex. */
                uint32_t aTimeOut   /*!< [in] The timeout in mSec, or CC_INFINITE. */);


/*!
  @brief This function releases the mutex.

  @return \c 0 on success.
  @return A non-zero value on failure.
 */
CCError_t CC_PalMutexUnlock(CC_PalMutex *pMutexId/*!< [in] A pointer to the handle of the mutex. */);



#ifdef __cplusplus
}
#endif

#endif

