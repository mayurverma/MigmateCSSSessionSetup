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

#ifndef __DX_RNG_H__
#define __DX_RNG_H__

// --------------------------------------
// BLOCK: RNG
// --------------------------------------
#define DX_RNG_IMR_REG_OFFSET   0x0100UL
#define DX_RNG_IMR_EHR_VALID_INT_MASK_BIT_SHIFT     0x0UL
#define DX_RNG_IMR_EHR_VALID_INT_MASK_BIT_SIZE  0x1UL
#define DX_RNG_IMR_AUTOCORR_ERR_INT_MASK_BIT_SHIFT  0x1UL
#define DX_RNG_IMR_AUTOCORR_ERR_INT_MASK_BIT_SIZE   0x1UL
#define DX_RNG_IMR_CRNGT_ERR_INT_MASK_BIT_SHIFT     0x2UL
#define DX_RNG_IMR_CRNGT_ERR_INT_MASK_BIT_SIZE  0x1UL
#define DX_RNG_IMR_VN_ERR_INT_MASK_BIT_SHIFT    0x3UL
#define DX_RNG_IMR_VN_ERR_INT_MASK_BIT_SIZE     0x1UL
#define DX_RNG_IMR_WATCHDOG_INT_MASK_BIT_SHIFT  0x4UL
#define DX_RNG_IMR_WATCHDOG_INT_MASK_BIT_SIZE   0x1UL
#define DX_RNG_IMR_RNG_DMA_DONE_INT_BIT_SHIFT   0x5UL
#define DX_RNG_IMR_RNG_DMA_DONE_INT_BIT_SIZE    0x1UL
#define DX_RNG_ISR_REG_OFFSET   0x0104UL
#define DX_RNG_ISR_EHR_VALID_BIT_SHIFT  0x0UL
#define DX_RNG_ISR_EHR_VALID_BIT_SIZE   0x1UL
#define DX_RNG_ISR_AUTOCORR_ERR_BIT_SHIFT   0x1UL
#define DX_RNG_ISR_AUTOCORR_ERR_BIT_SIZE    0x1UL
#define DX_RNG_ISR_CRNGT_ERR_BIT_SHIFT  0x2UL
#define DX_RNG_ISR_CRNGT_ERR_BIT_SIZE   0x1UL
#define DX_RNG_ISR_VN_ERR_BIT_SHIFT     0x3UL
#define DX_RNG_ISR_VN_ERR_BIT_SIZE  0x1UL
#define DX_RNG_ISR_RNG_DMA_DONE_BIT_SHIFT   0x5UL
#define DX_RNG_ISR_RNG_DMA_DONE_BIT_SIZE    0x1UL
#define DX_RNG_ISR_RESEEDING_DONE_BIT_SHIFT     0x10UL
#define DX_RNG_ISR_RESEEDING_DONE_BIT_SIZE  0x1UL
#define DX_RNG_ISR_INSTANTIATION_DONE_BIT_SHIFT     0x11UL
#define DX_RNG_ISR_INSTANTIATION_DONE_BIT_SIZE  0x1UL
#define DX_RNG_ISR_FINAL_UPDATE_DONE_BIT_SHIFT  0x12UL
#define DX_RNG_ISR_FINAL_UPDATE_DONE_BIT_SIZE   0x1UL
#define DX_RNG_ISR_OUTPUT_READY_BIT_SHIFT   0x13UL
#define DX_RNG_ISR_OUTPUT_READY_BIT_SIZE    0x1UL
#define DX_RNG_ISR_RESEED_CNTR_FULL_BIT_SHIFT   0x14UL
#define DX_RNG_ISR_RESEED_CNTR_FULL_BIT_SIZE    0x1UL
#define DX_RNG_ISR_RESEED_CNTR_TOP_40_BIT_SHIFT     0x15UL
#define DX_RNG_ISR_RESEED_CNTR_TOP_40_BIT_SIZE  0x1UL
#define DX_RNG_ISR_PRNG_CRNGT_ERR_BIT_SHIFT     0x16UL
#define DX_RNG_ISR_PRNG_CRNGT_ERR_BIT_SIZE  0x1UL
#define DX_RNG_ISR_REQ_SIZE_BIT_SHIFT   0x17UL
#define DX_RNG_ISR_REQ_SIZE_BIT_SIZE    0x1UL
#define DX_RNG_ISR_KAT_ERR_BIT_SHIFT    0x18UL
#define DX_RNG_ISR_KAT_ERR_BIT_SIZE     0x1UL
#define DX_RNG_ISR_WHICH_KAT_ERR_BIT_SHIFT  0x19UL
#define DX_RNG_ISR_WHICH_KAT_ERR_BIT_SIZE   0x2UL
#define DX_RNG_ICR_REG_OFFSET   0x0108UL
#define DX_RNG_ICR_EHR_VALID_BIT_SHIFT  0x0UL
#define DX_RNG_ICR_EHR_VALID_BIT_SIZE   0x1UL
#define DX_RNG_ICR_AUTOCORR_ERR_BIT_SHIFT   0x1UL
#define DX_RNG_ICR_AUTOCORR_ERR_BIT_SIZE    0x1UL
#define DX_RNG_ICR_CRNGT_ERR_BIT_SHIFT  0x2UL
#define DX_RNG_ICR_CRNGT_ERR_BIT_SIZE   0x1UL
#define DX_RNG_ICR_VN_ERR_BIT_SHIFT     0x3UL
#define DX_RNG_ICR_VN_ERR_BIT_SIZE  0x1UL
#define DX_RNG_ICR_RNG_WATCHDOG_BIT_SHIFT   0x4UL
#define DX_RNG_ICR_RNG_WATCHDOG_BIT_SIZE    0x1UL
#define DX_RNG_ICR_RNG_DMA_DONE_BIT_SHIFT   0x5UL
#define DX_RNG_ICR_RNG_DMA_DONE_BIT_SIZE    0x1UL
#define DX_RNG_ICR_RESEEDING_DONE_BIT_SHIFT     0x10UL
#define DX_RNG_ICR_RESEEDING_DONE_BIT_SIZE  0x1UL
#define DX_RNG_ICR_INSTANTIATION_DONE_BIT_SHIFT     0x11UL
#define DX_RNG_ICR_INSTANTIATION_DONE_BIT_SIZE  0x1UL
#define DX_RNG_ICR_FINAL_UPDATE_DONE_BIT_SHIFT  0x12UL
#define DX_RNG_ICR_FINAL_UPDATE_DONE_BIT_SIZE   0x1UL
#define DX_RNG_ICR_OUTPUT_READY_BIT_SHIFT   0x13UL
#define DX_RNG_ICR_OUTPUT_READY_BIT_SIZE    0x1UL
#define DX_RNG_ICR_RESEED_CNTR_FULL_BIT_SHIFT   0x14UL
#define DX_RNG_ICR_RESEED_CNTR_FULL_BIT_SIZE    0x1UL
#define DX_RNG_ICR_RESEED_CNTR_TOP_40_BIT_SHIFT     0x15UL
#define DX_RNG_ICR_RESEED_CNTR_TOP_40_BIT_SIZE  0x1UL
#define DX_RNG_ICR_PRNG_CRNGT_ERR_BIT_SHIFT     0x16UL
#define DX_RNG_ICR_PRNG_CRNGT_ERR_BIT_SIZE  0x1UL
#define DX_RNG_ICR_REQ_SIZE_BIT_SHIFT   0x17UL
#define DX_RNG_ICR_REQ_SIZE_BIT_SIZE    0x1UL
#define DX_RNG_ICR_KAT_ERR_BIT_SHIFT    0x18UL
#define DX_RNG_ICR_KAT_ERR_BIT_SIZE     0x1UL
#define DX_RNG_ICR_WHICH_KAT_ERR_BIT_SHIFT  0x19UL
#define DX_RNG_ICR_WHICH_KAT_ERR_BIT_SIZE   0x2UL
#define DX_TRNG_CONFIG_REG_OFFSET   0x010CUL
#define DX_TRNG_CONFIG_RND_SRC_SEL_BIT_SHIFT    0x0UL
#define DX_TRNG_CONFIG_RND_SRC_SEL_BIT_SIZE     0x2UL
#define DX_TRNG_CONFIG_SOP_SEL_BIT_SHIFT    0x2UL
#define DX_TRNG_CONFIG_SOP_SEL_BIT_SIZE     0x1UL
#define DX_TRNG_VALID_REG_OFFSET    0x0110UL
#define DX_TRNG_VALID_VALUE_BIT_SHIFT   0x0UL
#define DX_TRNG_VALID_VALUE_BIT_SIZE    0x1UL
#define DX_EHR_DATA_0_REG_OFFSET    0x0114UL
#define DX_EHR_DATA_0_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_0_VALUE_BIT_SIZE    0x20UL
#define DX_EHR_DATA_1_REG_OFFSET    0x0118UL
#define DX_EHR_DATA_1_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_1_VALUE_BIT_SIZE    0x20UL
#define DX_EHR_DATA_2_REG_OFFSET    0x011CUL
#define DX_EHR_DATA_2_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_2_VALUE_BIT_SIZE    0x20UL
#define DX_EHR_DATA_3_REG_OFFSET    0x0120UL
#define DX_EHR_DATA_3_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_3_VALUE_BIT_SIZE    0x20UL
#define DX_EHR_DATA_4_REG_OFFSET    0x0124UL
#define DX_EHR_DATA_4_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_4_VALUE_BIT_SIZE    0x20UL
#define DX_EHR_DATA_5_REG_OFFSET    0x0128UL
#define DX_EHR_DATA_5_VALUE_BIT_SHIFT   0x0UL
#define DX_EHR_DATA_5_VALUE_BIT_SIZE    0x20UL
#define DX_RND_SOURCE_ENABLE_REG_OFFSET     0x012CUL
#define DX_RND_SOURCE_ENABLE_VALUE_BIT_SHIFT    0x0UL
#define DX_RND_SOURCE_ENABLE_VALUE_BIT_SIZE     0x1UL
#define DX_SAMPLE_CNT1_REG_OFFSET   0x0130UL
#define DX_SAMPLE_CNT1_VALUE_BIT_SHIFT  0x0UL
#define DX_SAMPLE_CNT1_VALUE_BIT_SIZE   0x20UL
#define DX_AUTOCORR_STATISTIC_REG_OFFSET    0x0134UL
#define DX_AUTOCORR_STATISTIC_AUTOCORR_TRYS_BIT_SHIFT   0x0UL
#define DX_AUTOCORR_STATISTIC_AUTOCORR_TRYS_BIT_SIZE    0xEUL
#define DX_AUTOCORR_STATISTIC_AUTOCORR_FAILS_BIT_SHIFT  0xEUL
#define DX_AUTOCORR_STATISTIC_AUTOCORR_FAILS_BIT_SIZE   0x8UL
#define DX_TRNG_DEBUG_CONTROL_REG_OFFSET    0x0138UL
#define DX_TRNG_DEBUG_CONTROL_VNC_BYPASS_BIT_SHIFT  0x1UL
#define DX_TRNG_DEBUG_CONTROL_VNC_BYPASS_BIT_SIZE   0x1UL
#define DX_TRNG_DEBUG_CONTROL_TRNG_CRNGT_BYPASS_BIT_SHIFT   0x2UL
#define DX_TRNG_DEBUG_CONTROL_TRNG_CRNGT_BYPASS_BIT_SIZE    0x1UL
#define DX_TRNG_DEBUG_CONTROL_AUTO_CORRELATE_BYPASS_BIT_SHIFT   0x3UL
#define DX_TRNG_DEBUG_CONTROL_AUTO_CORRELATE_BYPASS_BIT_SIZE    0x1UL
#define DX_RNG_SW_RESET_REG_OFFSET  0x0140UL
#define DX_RNG_SW_RESET_VALUE_BIT_SHIFT     0x0UL
#define DX_RNG_SW_RESET_VALUE_BIT_SIZE  0x1UL
#define DX_RNG_DEBUG_EN_INPUT_REG_OFFSET    0x01B4UL
#define DX_RNG_DEBUG_EN_INPUT_VALUE_BIT_SHIFT   0x0UL
#define DX_RNG_DEBUG_EN_INPUT_VALUE_BIT_SIZE    0x1UL
#define DX_RNG_BUSY_REG_OFFSET  0x01B8UL
#define DX_RNG_BUSY_RNG_BUSY_BIT_SHIFT  0x0UL
#define DX_RNG_BUSY_RNG_BUSY_BIT_SIZE   0x1UL
#define DX_RNG_BUSY_TRNG_BUSY_BIT_SHIFT     0x1UL
#define DX_RNG_BUSY_TRNG_BUSY_BIT_SIZE  0x1UL
#define DX_RNG_BUSY_PRNG_BUSY_BIT_SHIFT     0x2UL
#define DX_RNG_BUSY_PRNG_BUSY_BIT_SIZE  0x1UL
#define DX_RST_BITS_COUNTER_REG_OFFSET  0x01BCUL
#define DX_RST_BITS_COUNTER_VALUE_BIT_SHIFT     0x0UL
#define DX_RST_BITS_COUNTER_VALUE_BIT_SIZE  0x1UL
#define DX_RNG_VERSION_REG_OFFSET   0x01C0UL
#define DX_RNG_VERSION_EHR_WIDTH_192_BIT_SHIFT  0x0UL
#define DX_RNG_VERSION_EHR_WIDTH_192_BIT_SIZE   0x1UL
#define DX_RNG_VERSION_CRNGT_EXISTS_BIT_SHIFT   0x1UL
#define DX_RNG_VERSION_CRNGT_EXISTS_BIT_SIZE    0x1UL
#define DX_RNG_VERSION_AUTOCORR_EXISTS_BIT_SHIFT    0x2UL
#define DX_RNG_VERSION_AUTOCORR_EXISTS_BIT_SIZE     0x1UL
#define DX_RNG_VERSION_TRNG_TESTS_BYPASS_EN_BIT_SHIFT   0x3UL
#define DX_RNG_VERSION_TRNG_TESTS_BYPASS_EN_BIT_SIZE    0x1UL
#define DX_RNG_VERSION_PRNG_EXISTS_BIT_SHIFT    0x4UL
#define DX_RNG_VERSION_PRNG_EXISTS_BIT_SIZE     0x1UL
#define DX_RNG_VERSION_KAT_EXISTS_BIT_SHIFT     0x5UL
#define DX_RNG_VERSION_KAT_EXISTS_BIT_SIZE  0x1UL
#define DX_RNG_VERSION_RESEEDING_EXISTS_BIT_SHIFT   0x6UL
#define DX_RNG_VERSION_RESEEDING_EXISTS_BIT_SIZE    0x1UL
#define DX_RNG_VERSION_RNG_USE_5_SBOXES_BIT_SHIFT   0x7UL
#define DX_RNG_VERSION_RNG_USE_5_SBOXES_BIT_SIZE    0x1UL
#define DX_RNG_CLK_ENABLE_REG_OFFSET    0x01C4UL
#define DX_RNG_CLK_ENABLE_VALUE_BIT_SHIFT   0x0UL
#define DX_RNG_CLK_ENABLE_VALUE_BIT_SIZE    0x1UL
#define DX_RNG_DMA_ENABLE_REG_OFFSET    0x01C8UL
#define DX_RNG_DMA_ENABLE_VALUE_BIT_SHIFT   0x0UL
#define DX_RNG_DMA_ENABLE_VALUE_BIT_SIZE    0x1UL
#define DX_RNG_DMA_SRC_MASK_REG_OFFSET  0x01CCUL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL0_BIT_SHIFT   0x0UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL0_BIT_SIZE    0x1UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL1_BIT_SHIFT   0x1UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL1_BIT_SIZE    0x1UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL2_BIT_SHIFT   0x2UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL2_BIT_SIZE    0x1UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL3_BIT_SHIFT   0x3UL
#define DX_RNG_DMA_SRC_MASK_EN_SRC_SEL3_BIT_SIZE    0x1UL
#define DX_RNG_DMA_SRAM_ADDR_REG_OFFSET     0x01D0UL
#define DX_RNG_DMA_SRAM_ADDR_VALUE_BIT_SHIFT    0x0UL
#define DX_RNG_DMA_SRAM_ADDR_VALUE_BIT_SIZE     0xBUL
#define DX_RNG_DMA_SAMPLES_NUM_REG_OFFSET   0x01D4UL
#define DX_RNG_DMA_SAMPLES_NUM_VALUE_BIT_SHIFT  0x0UL
#define DX_RNG_DMA_SAMPLES_NUM_VALUE_BIT_SIZE   0x8UL
#define DX_RNG_WATCHDOG_VAL_REG_OFFSET  0x01D8UL
#define DX_RNG_WATCHDOG_VAL_VALUE_BIT_SHIFT     0x0UL
#define DX_RNG_WATCHDOG_VAL_VALUE_BIT_SIZE  0x20UL
#define DX_RNG_DMA_STATUS_REG_OFFSET    0x01DCUL
#define DX_RNG_DMA_STATUS_RNG_DMA_BUSY_BIT_SHIFT    0x0UL
#define DX_RNG_DMA_STATUS_RNG_DMA_BUSY_BIT_SIZE     0x1UL
#define DX_RNG_DMA_STATUS_DMA_SRC_SEL_BIT_SHIFT     0x1UL
#define DX_RNG_DMA_STATUS_DMA_SRC_SEL_BIT_SIZE  0x2UL
#define DX_RNG_DMA_STATUS_NUM_OF_SAMPLES_BIT_SHIFT  0x3UL
#define DX_RNG_DMA_STATUS_NUM_OF_SAMPLES_BIT_SIZE   0x8UL

#endif //__DX_RNG_H__
