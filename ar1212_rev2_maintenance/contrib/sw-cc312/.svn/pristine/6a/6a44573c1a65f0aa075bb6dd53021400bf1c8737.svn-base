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
 * @file
 * @brief This file contains macro definitions for accessing ARM TrustZone CryptoCell register space.
 */

#ifndef _CC_REGS_H_
#define _CC_REGS_H_

#include "cc_bitops.h"

#if !defined(CC_REE) && !defined(CC_IOT) && !defined(CC_SB_SUPPORT_IOT)
#include "dx_nvm.h"
#endif

/* Register Offset macro */
#define CC_REG_OFFSET(unit_name, reg_name)               \
    (DX_BASE_ ## unit_name + DX_ ## reg_name ## _REG_OFFSET)

#define CC_REG_BIT_SHIFT(reg_name, field_name)               \
    (DX_ ## reg_name ## _ ## field_name ## _BIT_SHIFT)

/* Register Offset macros (from registers base address in host) */
#if defined(CC_REE) || defined(CC_TEE) || defined(CC_IOT) || defined(CC_SB_SUPPORT_IOT)

#include "dx_reg_base_host.h"

/* Read-Modify-Write a field of a register */
#define MODIFY_REGISTER_FLD(unitName, regName, fldName, fldVal)         \
do {                                            \
    uint32_t regVal;                            \
    regVal = READ_REGISTER(CC_REG_ADDR(unitName, regName));       \
    CC_REG_FLD_SET(unitName, regName, fldName, regVal, fldVal); \
    WRITE_REGISTER(CC_REG_ADDR(unitName, regName), regVal);       \
} while (0)

#else
#error Execution domain is not CC_REE/CC_TEE/CC_IOT
#endif

/* Registers address macros for ENV registers (development FPGA only) */
#ifdef DX_BASE_ENV_REGS

/* This offset should be added to mapping address of DX_BASE_ENV_REGS */
#define CC_ENV_REG_OFFSET(reg_name) (DX_ENV_ ## reg_name ## _REG_OFFSET)

#endif /*DX_BASE_ENV_REGS*/

/*! Bit fields get */
#define CC_REG_FLD_GET(unit_name, reg_name, fld_name, reg_val)        \
    (DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20 ?          \
    reg_val /*!< \internal Optimization for 32b fields */ :               \
    BITFIELD_GET(reg_val, DX_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT, \
             DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE))

/*! Bit fields access */
#define CC_REG_FLD_GET2(unit_name, reg_name, fld_name, reg_val)       \
    (CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20 ?          \
    reg_val /*!< \internal Optimization for 32b fields */ :               \
    BITFIELD_GET(reg_val, CC_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT, \
             CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE))

/*! Bit fields set */
#define CC_REG_FLD_SET(                                               \
    unit_name, reg_name, fld_name, reg_shadow_var, new_fld_val)      \
do {                                                                     \
    if (DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20)       \
        reg_shadow_var = new_fld_val; /*!< \internal Optimization for 32b fields */\
    else                                                             \
        BITFIELD_SET(reg_shadow_var,                             \
            DX_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT,  \
            DX_ ## reg_name ## _ ## fld_name ## _BIT_SIZE,   \
            new_fld_val);                                    \
} while (0)

/*! Bit fields set */
#define CC_REG_FLD_SET2(                                               \
    unit_name, reg_name, fld_name, reg_shadow_var, new_fld_val)      \
do {                                                                     \
    if (CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE == 0x20)       \
        reg_shadow_var = new_fld_val; /*!< \internal Optimization for 32b fields */\
    else                                                             \
        BITFIELD_SET(reg_shadow_var,                             \
            CC_ ## reg_name ## _ ## fld_name ## _BIT_SHIFT,  \
            CC_ ## reg_name ## _ ## fld_name ## _BIT_SIZE,   \
            new_fld_val);                                    \
} while (0)

/* Usage example:
   uint32_t reg_shadow = READ_REGISTER(CC_REG_ADDR(CRY_KERNEL,AES_CONTROL));
   CC_REG_FLD_SET(CRY_KERNEL,AES_CONTROL,NK_KEY0,reg_shadow, 3);
   CC_REG_FLD_SET(CRY_KERNEL,AES_CONTROL,NK_KEY1,reg_shadow, 1);
   WRITE_REGISTER(CC_REG_ADDR(CRY_KERNEL,AES_CONTROL), reg_shadow);
 */

#endif /*_CC_REGS_H_*/
