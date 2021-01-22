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


#ifndef __CC_HAL_PLAT_H__
#define __CC_HAL_PLAT_H__

#include "dx_host.h"
#include "cc_bitops.h"

#include "dx_reg_common.h" /*temporary (missing HW defines)*/


/******************************************************************************
*               DEFINITIONS
******************************************************************************/
#define CC_LARGE_SECRET_KEY_NUM_OF_BYTES 32
#define CC_SMALL_SECRET_KEY_NUM_OF_BYTES 16

/* Peripheral ID registers values */
#define CC_BSV_PID_0_VAL        0x000000C0UL
#define CC_BSV_PID_1_VAL        0x000000B0UL
#define CC_BSV_PID_2_VAL        0x0000000BUL
#define CC_BSV_PID_3_VAL        0x00000000UL
#define CC_BSV_PID_4_VAL        0x00000004UL
#define CC_BSV_PID_SIZE_WORDS       5

/* Component ID registers values */
#define CC_BSV_CID_0_VAL        0x0DUL
#define CC_BSV_CID_1_VAL        0xF0UL
#define CC_BSV_CID_2_VAL        0x05UL
#define CC_BSV_CID_3_VAL        0xB1UL
#define CC_BSV_CID_SIZE_WORDS       4


/******************************************************************************
*                               MACROS
******************************************************************************/
extern unsigned long gCcRegBase;

/******************************************************************************
*                               MACROS
******************************************************************************/
/*get the size of the RKEK from HW */
//(key_size >> DX_NVM_CC_BOOT_LARGE_RKEK_LOCAL_BIT_SHIFT) & DX_NVM_CC_BOOT_LARGE_RKEK_LOCAL_BIT_SIZE
#define GET_ROOT_KEY_SIZE(key_size) \
do{ \
    key_size = CC_HAL_READ_REGISTER(CC_REG_OFFSET(CRY_KERNEL, NVM_CC_BOOT));\
        if (CC_REG_FLD_GET(CRY_KERNEL, NVM_CC_BOOT, LARGE_RKEK_LOCAL, key_size)) \
                key_size = CC_LARGE_SECRET_KEY_NUM_OF_BYTES; \
        else \
                key_size = CC_SMALL_SECRET_KEY_NUM_OF_BYTES; \
}while (0)


/*!
 * Read CryptoCell memory-mapped-IO register.
 *
 * \param regOffset The offset of the ARM CryptoCell register to read
 * \return uint32_t Return the value of the given register
 */
#define CC_HAL_READ_REGISTER(regOffset)                 \
        (*((volatile uint32_t *)(gCcRegBase + (regOffset))))

/*!
 * Write CryptoCell memory-mapped-IO register.
 * \note This macro must be modified to make the operation synchronous, i.e. the write operation must complete,
 *       and the new value must be written to the register before the macro returns. The mechanisms required to
 *       achieve this are architecture-dependent (e.g., the memory barrier in ARM architecture).
 *
 * \param regOffset The offset of the ARM CryptoCell register to write
 * \param val The value to write
 */
#define CC_HAL_WRITE_REGISTER(regOffset, val)       \
        (*((volatile uint32_t *)(gCcRegBase + (regOffset))) = (val))



#endif /*__CC_HAL_PLAT_H__*/

