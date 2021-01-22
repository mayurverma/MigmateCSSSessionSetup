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
 @brief This file contains the definitions and APIs for power-management implementation.

       This is a placeholder for platform-specific power management implementation.
       The module should be updated whether CryptoCell is active or not,
       to notify the external PMU when it might be powered down.
 */

/*!
 @defgroup cc_pal_pm CryptoCell PAL power-management APIs
 @brief Contains PAL power-management APIs. See cc_pal_pm.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_PM_H
#define _CC_PAL_PM_H


/*
******** Function pointer definitions **********
*/


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
 @brief This function initiates an atomic counter.
 @return Void.
 */
void CC_PalPowerSaveModeInit(void);

/*!
 @brief This function returns the number of active registered CryptoCell operations.

 @return The value of the atomic counter.
 */
int32_t CC_PalPowerSaveModeStatus(void);

/*!
 @brief This function updates the atomic counter on each call to CryptoCell.

 On each call to CryptoCell, the counter is increased. At the end of each operation
 the counter is decreased.
 Once the counter is zero, an external callback is called.

 @return \c 0 on success.
 @return A non-zero value on failure.
 */
CCError_t CC_PalPowerSaveModeSelect(CCBool isPowerSaveMode /*!< [in] TRUE: CryptoCell is active or FALSE: CryptoCell is idle. */ );


#endif
