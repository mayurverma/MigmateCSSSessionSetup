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

#ifndef __DX_NVM_H__
#define __DX_NVM_H__

// --------------------------------------
// BLOCK: NVM
// --------------------------------------
#define DX_AIB_FUSE_PROG_COMPLETED_REG_OFFSET   0x1F04UL
#define DX_AIB_FUSE_PROG_COMPLETED_VALUE_BIT_SHIFT  0x0UL
#define DX_AIB_FUSE_PROG_COMPLETED_VALUE_BIT_SIZE   0x1UL
#define DX_NVM_DEBUG_STATUS_REG_OFFSET  0x1F08UL
#define DX_NVM_DEBUG_STATUS_VALUE_BIT_SHIFT     0x1UL
#define DX_NVM_DEBUG_STATUS_VALUE_BIT_SIZE  0x3UL
#define DX_LCS_IS_VALID_REG_OFFSET  0x1F0CUL
#define DX_LCS_IS_VALID_VALUE_BIT_SHIFT     0x0UL
#define DX_LCS_IS_VALID_VALUE_BIT_SIZE  0x1UL
#define DX_NVM_IS_IDLE_REG_OFFSET   0x1F10UL
#define DX_NVM_IS_IDLE_VALUE_BIT_SHIFT  0x0UL
#define DX_NVM_IS_IDLE_VALUE_BIT_SIZE   0x1UL
#define DX_LCS_REG_REG_OFFSET   0x1F14UL
#define DX_LCS_REG_LCS_REG_BIT_SHIFT    0x0UL
#define DX_LCS_REG_LCS_REG_BIT_SIZE     0x3UL
#define DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SHIFT     0x8UL
#define DX_LCS_REG_ERROR_KDR_ZERO_CNT_BIT_SIZE  0x1UL
#define DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SHIFT    0x9UL
#define DX_LCS_REG_ERROR_PROV_ZERO_CNT_BIT_SIZE     0x1UL
#define DX_LCS_REG_ERROR_KCE_ZERO_CNT_BIT_SHIFT     0xAUL
#define DX_LCS_REG_ERROR_KCE_ZERO_CNT_BIT_SIZE  0x1UL
#define DX_LCS_REG_ERROR_KPICV_ZERO_CNT_BIT_SHIFT   0xBUL
#define DX_LCS_REG_ERROR_KPICV_ZERO_CNT_BIT_SIZE    0x1UL
#define DX_LCS_REG_ERROR_KCEICV_ZERO_CNT_BIT_SHIFT  0xCUL
#define DX_LCS_REG_ERROR_KCEICV_ZERO_CNT_BIT_SIZE   0x1UL
#define DX_HOST_SHADOW_KDR_REG_REG_OFFSET   0x1F18UL
#define DX_HOST_SHADOW_KDR_REG_VALUE_BIT_SHIFT  0x0UL
#define DX_HOST_SHADOW_KDR_REG_VALUE_BIT_SIZE   0x1UL
#define DX_HOST_SHADOW_KCP_REG_REG_OFFSET   0x1F1CUL
#define DX_HOST_SHADOW_KCP_REG_VALUE_BIT_SHIFT  0x0UL
#define DX_HOST_SHADOW_KCP_REG_VALUE_BIT_SIZE   0x1UL
#define DX_HOST_SHADOW_KCE_REG_REG_OFFSET   0x1F20UL
#define DX_HOST_SHADOW_KCE_REG_VALUE_BIT_SHIFT  0x0UL
#define DX_HOST_SHADOW_KCE_REG_VALUE_BIT_SIZE   0x1UL
#define DX_HOST_SHADOW_KPICV_REG_REG_OFFSET     0x1F24UL
#define DX_HOST_SHADOW_KPICV_REG_VALUE_BIT_SHIFT    0x0UL
#define DX_HOST_SHADOW_KPICV_REG_VALUE_BIT_SIZE     0x1UL
#define DX_HOST_SHADOW_KCEICV_REG_REG_OFFSET    0x1F28UL
#define DX_HOST_SHADOW_KCEICV_REG_VALUE_BIT_SHIFT   0x0UL
#define DX_HOST_SHADOW_KCEICV_REG_VALUE_BIT_SIZE    0x1UL
#define DX_OTP_ADDR_WIDTH_DEF_REG_OFFSET    0x1F2CUL
#define DX_OTP_ADDR_WIDTH_DEF_VALUE_BIT_SHIFT   0x0UL
#define DX_OTP_ADDR_WIDTH_DEF_VALUE_BIT_SIZE    0x4UL

#endif //__DX_NVM_H__

