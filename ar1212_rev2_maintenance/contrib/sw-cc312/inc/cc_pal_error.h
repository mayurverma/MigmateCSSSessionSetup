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
 @brief This file contains the error definitions of the platform-dependent PAL APIs.
 */

/*!
 @defgroup cc_pal_error Specific errors of the CryptoCell PAL APIs
 @brief Contains platform-dependent PAL-API error definitions. See cc_pal_error.h.

 @{
 @ingroup cc_pal
 @}
 */

#ifndef _CC_PAL_ERROR_H
#define _CC_PAL_ERROR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*! The PAL error base.*/
#define CC_PAL_BASE_ERROR                0x0F000000

/* Memory error returns */
/*! Buffer 1 is greater than buffer 2 error.*/
#define CC_PAL_MEM_BUF1_GREATER          CC_PAL_BASE_ERROR + 0x01UL
/*! Buffer 2 is greater than buffer 1 error.*/
#define CC_PAL_MEM_BUF2_GREATER          CC_PAL_BASE_ERROR + 0x02UL

/* Semaphore error returns */
/*! Semaphore creation failed.*/
#define CC_PAL_SEM_CREATE_FAILED         CC_PAL_BASE_ERROR + 0x03UL
/*! Semaphore deletion failed.*/
#define CC_PAL_SEM_DELETE_FAILED         CC_PAL_BASE_ERROR + 0x04UL
/*! Semaphore reached timeout.*/
#define CC_PAL_SEM_WAIT_TIMEOUT          CC_PAL_BASE_ERROR + 0x05UL
/*! Semaphore wait failed.*/
#define CC_PAL_SEM_WAIT_FAILED           CC_PAL_BASE_ERROR + 0x06UL
/*! Semaphore release failed.*/
#define CC_PAL_SEM_RELEASE_FAILED        CC_PAL_BASE_ERROR + 0x07UL
/*! Illegal PAL address.*/
#define CC_PAL_ILLEGAL_ADDRESS       CC_PAL_BASE_ERROR + 0x08UL

#ifdef __cplusplus
}
#endif

#endif

