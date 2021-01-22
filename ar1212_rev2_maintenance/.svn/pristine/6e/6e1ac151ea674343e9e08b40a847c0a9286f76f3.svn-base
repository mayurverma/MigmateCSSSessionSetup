//******************************************************************************
/// \file
/// \defgroup init Initialisation Support
/// Processor MPU initialisation support.
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************

/*******************************************************************************
 * The AR0820 memory map is as follows:
 *
 *                Cortex-M0+ Memory Map             =>                 AR0820
 *
 *            -----------------------------                -----------------------------
 * 0xFFFFFFFF |                           |    0xFFFFFFFF  |                           |
 *            |       System Level        |                |       System Level        |
 *            |                           |                |                           | <= NVIC, WDG, SYSTICK etc
 * 0xE0000000 -----------------------------    0xE0000000  -----------------------------
 * 0xDFFFFFFF |                           |    0xDFFFFFFF  |                           |
 *            |     External Device       |                |          UNUSED           |
 *            |                           |                |                           |
 * 0xA0000000 -----------------------------    0xA0000000  -----------------------------
 * 0x9FFFFFFF |                           |    0x9FFFFFFF  |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |       External RAM        |                |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 * 0x60000000 -----------------------------    0x60000000  -----------------------------
 * 0x5FFFFFFF |                           |    0x5FFFFFFF  |                           |
 *            |                           |                |                           |
 *            |                           |                |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |    0x40020000  -----------------------------
 *            |        Peripherals        |    0x4001FFFF  |                           |
 *            |                           |                |                           | <= MCU SysCtl
 *            |                           |    0x4001F000  -----------------------------
 *            |                           |    0x4001EFFF  |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |    0x40010000  -----------------------------
 *            |                           |    0x4000FFFF  |                           |
 *            |                           |                |                           | <= APB peripherals
 * 0x40000000 -----------------------------    0x40000000  -----------------------------
 * 0x3FFFFFFF |                           |    0x3FFFFFFF  |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |    0x3FFF2000  -----------------------------
 *            |                           |    0x3FFF1FFF  |   ICORE SHARED-RAM (4 KB) |
 *            |                           |                |                           | <= SHARED-RAM
 *            |                           |    0x3FFF1000  -----------------------------
 *            |                           |    0x3FFF0FFF  |       ICORE REGS (4 KB)   |
 *            |                           |                |                           | <= ICORE_REGS
 *            |                           |    0x3FFF0000  -----------------------------
 *            |                           |    0x3FFEFFFF  |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |           SRAM            |                |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |    0x20010000  -----------------------------
 *            |                           |    0x2000FFFF  |                           | <= MAIN STACK
 *            |                           |                |                           |
 *            |                           |                |        SRAM (16 KB)       |
 *            |                           |                |                           | <= HEAP
 *            |                           |                |                           | <= READ-WRITE DATA
 *            |                           |    0x2000C000  -----------------------------
 *            |                           |    0x2000BFFF  |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |      PATCH RAM (48 KB)    |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |                           | <= PATCH_RAM
 * 0x20000000 -----------------------------    0x20000000  -----------------------------
 * 0x1FFFFFFF |                           |    0x1FFFFFFF  |                           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |                           |                |          UNUSED           |
 *            |                           |                |                           |
 *            |                           |                |                           |
 *            |          CODE             |                |                           |
 *            |                           |    0x00040000  -----------------------------
 *            |                           |    0x0003FFFF  |                           |
 *            |                           |                |                           | <= READ-ONLY DATA
 *            |                           |                |        ROM (256 KB)       |
 *            |                           |                |                           | <= CODE
 *            |                           |                |                           | <= VECTORS
 * 0x00000000 ----------------------------     0x00000000  -----------------------------
 *
 *
 * The processor data RAM is allocated into the following regions
 *
 * 0x20010000  -----------------------------
 * 0x2000FFFF  |         4 Kbytes          | <= MAIN STACK
 *             |                           |
 * 0x2000F000  ----------------------------- <= 32-byte guard band
 * 0x2000EFFF  |                           |
 *             |         8 Kbytes          |
 *             |                           |
 *             |                           | <= HEAP
 * 0x2000D000  -----------------------------
 * 0x2000CFFF  |         4 Kbytes          |
 *             |                           | <= READ-WRITE DATA
 * 0x2000C000  -----------------------------
 * 0x2000BFFF  |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |         48 Kbytes         |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           |
 *             |                           | <= PATCH RAM
 * 0x20000000  -----------------------------
 *
 * We divide the AR0820 memory space up into 8 MPU-controlled regions:
 *
 * Region 0: ROM vectors, Code and Read-only Data
 * Region 1: Patch RAM
 * Region 2: Read-Write Data, Heap and Main stack
 * Region 3: Stack-guard region
 * Region 4: Host interface registers and shared-memory
 * Region 5: Peripherals
 * Region 6: Unused
 * Region 7: Unused
 *
 *******************************************************************************/
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "Platform.h"
#include "Init.h"

//------------------------------------------------------------------------------
//                                  Imports
//------------------------------------------------------------------------------
// Pull-in linker-defined symbols
extern uint32_t Image$$MAIN_STACK$$ZI$$Base;
extern uint32_t Image$$MAIN_STACK$$ZI$$Length;
//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

/// Represents an MPU region configuration
typedef struct
{
    uint32_t RBAR;  ///< Region Base Address Register
    uint32_t RASR;  ///< Region Attribute and Size Register
} MpuRegionConfigType;

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

#define KBYTE                       0x400UL

//
// The MPU is only used for the ASIC target, where we know the exact sizes of the
// memory regions. The prototype targets may use different memory maps we are
// tricky to map to MPU regions.
//

#define MPU_PATCH_REGION_SIZE_BYTES (48U * KBYTE)   ///< Patch region size in bytes
#define MPU_DATA_REGION_SIZE_BYTES  (4U * KBYTE)    ///< Data region size in bytes
#define MPU_HEAP_REGION_SIZE_BYTES  (8U * KBYTE)    ///< Heap region size in bytes
#define MPU_STACK_REGION_SIZE_BYTES (4U * KBYTE)    ///< Stack region size in bytes
#define MPU_STACK_GUARD_SIZE_BYTES  (32U)           ///< Stack guard region size in bytes
#define MPU_ICORE_REGION_SIZE_BYTES (8U * KBYTE)    ///< ICore registers and shared-memory region size in bytes

//
// Note we have to use hard-coded regions to be able to generate constants for the
// MpuRegionConfig table
//

#define MPU_CODE_REGION_ADDR        CRYPTO_CCORE_ROM_BASE           ///< Code runs from ROM
#define MPU_PATCH_REGION_ADDR       CRYPTO_CCORE_SRAM_BASE          ///< Patch region is at the base of data RAM
#define MPU_ICORE_REGION_ADDR       CRYPTO_ICORE_BASE               ///< Icore registers and shared-memory
#define MPU_PERIPHERAL_REGION_ADDR  CRYPTO_CCORE_PERIPHERALS_BASE   ///< Base of the peripheral region

///< Data region is immediately above the patch region
#define MPU_DATA_REGION_ADDR        (MPU_PATCH_REGION_ADDR + MPU_PATCH_REGION_SIZE_BYTES)

///< Stack is above the heap region
#define MPU_STACK_REGION_ADDR       (MPU_DATA_REGION_ADDR + MPU_DATA_REGION_SIZE_BYTES + MPU_HEAP_REGION_SIZE_BYTES)

// MPU Region identifiers
#define MPU_REGION_0                0UL
#define MPU_REGION_1                1UL
#define MPU_REGION_2                2UL
#define MPU_REGION_3                3UL
#define MPU_REGION_4                4UL
#define MPU_REGION_5                5UL
#define MPU_REGION_6                6UL
#define MPU_REGION_7                7UL

// RASR::REGION_SIZE size masks (0x0 - 0x06 reserved)
#define MPU_REGION_SIZE_256B        (0x07UL << MPU_RASR_SIZE_Pos)   ///< 256 bytes
#define MPU_REGION_SIZE_512B        (0x08UL << MPU_RASR_SIZE_Pos)   ///< 512 bytes
#define MPU_REGION_SIZE_1K          (0x09UL << MPU_RASR_SIZE_Pos)   ///<   1 Kbytes
#define MPU_REGION_SIZE_2K          (0x0AUL << MPU_RASR_SIZE_Pos)   ///<   2 Kbytes
#define MPU_REGION_SIZE_4K          (0x0BUL << MPU_RASR_SIZE_Pos)   ///<   4 Kbytes
#define MPU_REGION_SIZE_8K          (0x0CUL << MPU_RASR_SIZE_Pos)   ///<   8 Kbytes
#define MPU_REGION_SIZE_16K         (0x0DUL << MPU_RASR_SIZE_Pos)   ///<  16 Kbytes
#define MPU_REGION_SIZE_32K         (0x0EUL << MPU_RASR_SIZE_Pos)   ///<  32 Kbytes
#define MPU_REGION_SIZE_64K         (0x0FUL << MPU_RASR_SIZE_Pos)   ///<  64 Kbytes
#define MPU_REGION_SIZE_128K        (0x10UL << MPU_RASR_SIZE_Pos)   ///< 128 Kbytes
#define MPU_REGION_SIZE_256K        (0x11UL << MPU_RASR_SIZE_Pos)   ///< 256 Kbytes
#define MPU_REGION_SIZE_512K        (0x12UL << MPU_RASR_SIZE_Pos)   ///< 512 Kbytes
#define MPU_REGION_SIZE_1M          (0x13UL << MPU_RASR_SIZE_Pos)   ///<   1 Mbytes
#define MPU_REGION_SIZE_2M          (0x14UL << MPU_RASR_SIZE_Pos)   ///<   2 Mbytes
#define MPU_REGION_SIZE_4M          (0x15UL << MPU_RASR_SIZE_Pos)   ///<   4 Mbytes
#define MPU_REGION_SIZE_8M          (0x16UL << MPU_RASR_SIZE_Pos)   ///<   8 Mbytes
#define MPU_REGION_SIZE_16M         (0x17UL << MPU_RASR_SIZE_Pos)   ///<  16 Mbytes
#define MPU_REGION_SIZE_32M         (0x18UL << MPU_RASR_SIZE_Pos)   ///<  32 Mbytes
#define MPU_REGION_SIZE_64M         (0x19UL << MPU_RASR_SIZE_Pos)   ///<  64 Mbytes
#define MPU_REGION_SIZE_128M        (0x1AUL << MPU_RASR_SIZE_Pos)   ///< 128 Mbytes
#define MPU_REGION_SIZE_256M        (0x1BUL << MPU_RASR_SIZE_Pos)   ///< 256 Mbytes
#define MPU_REGION_SIZE_512M        (0x1CUL << MPU_RASR_SIZE_Pos)   ///< 512 Mbytes
#define MPU_REGION_SIZE_1G          (0x1DUL << MPU_RASR_SIZE_Pos)   ///<   1 Gbytes
#define MPU_REGION_SIZE_2G          (0x1EUL << MPU_RASR_SIZE_Pos)   ///<   2 Gbytes
#define MPU_REGION_SIZE_4G          (0x1FUL << MPU_RASR_SIZE_Pos)   ///<   4 Gbytes

// RASR::AP field encodings
#define MPU_RASR_AP_NO_ACCESS       (0x0UL << MPU_RASR_AP_Pos)      ///< No access
#define MPU_RASR_AP_PRIV_RW_USER_NA (0x1UL << MPU_RASR_AP_Pos)      ///< Priviledged access only
#define MPU_RASR_AP_PRIV_RW_USER_RO (0x2UL << MPU_RASR_AP_Pos)      ///< User write generates fault
#define MPU_RASR_AP_PRIV_RW_USER_RW (0x3UL << MPU_RASR_AP_Pos)      ///< Full access
#define MPU_RASR_AP_UNPREDICTABLE   (0x4UL << MPU_RASR_AP_Pos)      ///< DON'T USE!
#define MPU_RASR_AP_PRIV_RO_USER_NA (0x5UL << MPU_RASR_AP_Pos)      ///< Priviledged read-only
#define MPU_RASR_AP_PRIV_RO_USER_RO (0x6UL << MPU_RASR_AP_Pos)      ///< Read-only
#define MPU_RASR_AP_READ_ONLY       (0x7UL << MPU_RASR_AP_Pos)      ///< Read-only

// RASR attributes
#define MPU_RASR_SHAREABLE          MPU_RASR_S_Msk                  ///< Region is shareable (with other cores)
#define MPU_RASR_CACHEABLE          MPU_RASR_C_Msk                  ///< Region is cacheable (external to the core)
#define MPU_RASR_BUFFERABLE         MPU_RASR_B_Msk                  ///< Region is buffered (via write buffer)
#define MPU_RASR_EXECUTE_NEVER      MPU_RASR_XN_Msk                 ///< No execution from region

// RAS::SRD (sub-region disable) encodings
#define MPU_RASR_DIS_SUBREGION_0    (0x01UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 0
#define MPU_RASR_DIS_SUBREGION_1    (0x02UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 1
#define MPU_RASR_DIS_SUBREGION_2    (0x04UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 2
#define MPU_RASR_DIS_SUBREGION_3    (0x08UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 3
#define MPU_RASR_DIS_SUBREGION_4    (0x10UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 4
#define MPU_RASR_DIS_SUBREGION_5    (0x20UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 5
#define MPU_RASR_DIS_SUBREGION_6    (0x40UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 6
#define MPU_RASR_DIS_SUBREGION_7    (0x80UL << MPU_RASR_SRD_Pos)    ///< Disable sub-region 7

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//
// MPU configuration constants
//
const MpuRegionConfigType MpuRegionConfig[] =
{
    {
        //
        // Region 0:    256 Kbytes Code + Read-only data region, cacheable
        //
        MPU_CODE_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_0,

        // RASR contents
        MPU_RASR_AP_READ_ONLY | \
        MPU_RASR_CACHEABLE | \
        MPU_REGION_SIZE_256K | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 1:    48 Kbytes patch RAM region, no-exec, disable all access
        //              - sub-region 6 and 7 disabled
        //
        MPU_PATCH_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_1,

        // RASR contents
        MPU_RASR_EXECUTE_NEVER | \
        MPU_RASR_AP_NO_ACCESS | \
        MPU_RASR_CACHEABLE | \
        MPU_RASR_DIS_SUBREGION_6 | \
        MPU_RASR_DIS_SUBREGION_7 | \
        MPU_REGION_SIZE_64K | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 2:    16 Kbytes Data/Heap/Stack region, no-exec, cacheable
        //
        MPU_DATA_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_2,

        // RASR contents
        MPU_RASR_EXECUTE_NEVER | \
        MPU_RASR_AP_PRIV_RW_USER_NA | \
        MPU_RASR_CACHEABLE | \
        MPU_REGION_SIZE_16K | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 3:    256 byte Stack Guard region, disable all access
        //              - sub-region 0 enabled, sub-region 1-7 disabled to provide
        //                a buffer zone between heap and stack
        //
        MPU_STACK_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_3,

        // RASR contents
        MPU_RASR_EXECUTE_NEVER | \
        MPU_RASR_AP_NO_ACCESS | \
        MPU_RASR_CACHEABLE | \
        MPU_RASR_DIS_SUBREGION_1 | \
        MPU_RASR_DIS_SUBREGION_2 | \
        MPU_RASR_DIS_SUBREGION_3 | \
        MPU_RASR_DIS_SUBREGION_4 | \
        MPU_RASR_DIS_SUBREGION_5 | \
        MPU_RASR_DIS_SUBREGION_6 | \
        MPU_RASR_DIS_SUBREGION_7 | \
        MPU_REGION_SIZE_256B | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 4:    8 Kbytes ICore registers and shared-memory region, no-exec, cacheable
        //
        MPU_ICORE_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_4,

        // RASR contents
        MPU_RASR_EXECUTE_NEVER | \
        MPU_RASR_AP_PRIV_RW_USER_NA | \
        MPU_RASR_CACHEABLE | \
        MPU_REGION_SIZE_8K | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 5:    1 Gbyte Peripheral region, bufferable
        //
        MPU_PERIPHERAL_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_5,

        // RASR contents
        MPU_RASR_EXECUTE_NEVER | \
        MPU_RASR_AP_PRIV_RW_USER_NA | \
        MPU_RASR_BUFFERABLE | \
        MPU_REGION_SIZE_1G | \
        MPU_RASR_ENABLE_Msk
    },
    {
        //
        // Region 6:    Reserved, disable all access
        //
        MPU_RBAR_VALID_Msk | MPU_REGION_6,

        // RASR contents
        0U
    },
    {
        //
        // Region 7: Reserved, disable all access
        //
        MPU_RBAR_VALID_Msk | MPU_REGION_7,

        // RASR contents
        0U
    }
};

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialises the Cortex-M0+ MPU
///
/// Disables, configures, then enables the MPU. The MPU is configured to prevent any
/// accesses to non-existent locations.
///
/// The MPU has 8 configurable regions. There are configured for the following accesses:
///
/// | Memory Region |   Access    | MPU Region | Base Addr   |    Size    | End Addr    |  SRD  |
/// |:-------------:|:-----------:|:----------:|:-----------:|:----------:|:-----------:|:-----:|
/// |   Background  |  No access  |     -      | 0x0000 0000 |   4 Gbytes | 0xFFFF FFFF |   -   |
/// |               |             |            |             |            |             |       |
/// |      ROM      |  r/o, exec  |     0      | 0x0000 0000 | 256 Kbytes | 0x0003 FFFF |  0x00 |
/// |               |             |            |             |            |             |       |
/// |     PATCH     |  No access  |     1      | 0x2000 0000 |  64 Kbytes | 0x2000 FFFF |  0xC0 |
/// |               |             |            |             |            |             |       |
/// |DATA HEAP STACK| r/w,no exec |     2      | 0x2000 C000 |  16 Kbytes | 0x2000 FFFF |  0x00 |
/// |               |             |            |             |            |             |       |
/// |  STACK_GUARD  |  No access  |     3      | 0x2000 F000 |  256 bytes | 0x2000 F0FF |  0xFE |
/// |               |             |            |             |            |             |       |
/// |ICORE_REGS_RAM | r/w,no exec |     4      | 0x3FFF 0000 |   8 Kbytes | 0x3FFF 1FFF |  0x00 |
/// |               |             |            |             |            |             |       |
/// |  PERIPHERALS  | r/w,no exec |     5      | 0x4000 0000 |   1 GBytes | 0x7FFF FFFF |  0x00 |
/// |               |             |            |             |            |             |       |
/// |   Reserved    |             |    6,7     |             |            |             |       |
/// |               |             |            |             |            |             |       |
///
/// Region 2 overlays region 1 at 0x2000 C000 - 0x2000 FFFF
///
/// A 32 byte 'buffer' zone is provided between the HEAP region and the MAIN_STACK region
/// to detect stack-overflows. Region 3 overlaps region 2 at 0x2000 F000 - 0x2000 F0FF.
///
/// MPU configuration:
/// - Region 0: 256 Kbytes
/// - Region 1: 64 Kbytes subregion 0-5 enabled, subregion 6-7 disabled
/// - Region 2: 16 Kbytes
/// - Region 3: 256 bytes, subregion 0 enabled, subregions 1-7 disabled
/// - Region 4: 8 Kbytes
/// - Region 5: 1 Gbytes
/// - Region 6, 7: Disabled
///
/// \returns void
///
void InitMpu(void)
{
    // INFO: <PH>: We expect the core to have an MPU - however the ARM FVP M0+ doesn't!
    if (0U != MPU->TYPE)
    {
        uint32_t Idx;

        // Disable the MPU first to ensure no side-effects, and force PRIVDEFENA
        // to zero to prevent privileged access to the background region
        __DMB();
        MPU->CTRL = 0U;

        for (Idx = 0; Idx < (sizeof(MpuRegionConfig) / sizeof((MpuRegionConfig)[0])); Idx++)
        {
            MPU->RBAR = MpuRegionConfig[Idx].RBAR;
            MPU->RASR = MpuRegionConfig[Idx].RASR;
        }

        // Enable the MPU...
        MPU->CTRL = (uint32_t)MPU_CTRL_ENABLE_Msk;

        // ...and wait for all instruction/memory accesses to complete
        __DSB();
        __ISB();
    }
}

/// Enables or disables access to the Patch RAM region
///
/// \param[in] enable   Flag to enable or disable access
/// \returns void
///
void InitMpuEnablePatchRegion(bool const enable)
{
    // Disable the MPU before attempting to reconfigure it
    // - we also clear PRIVDEFENA to disable the background region
    __DMB();
    MPU->CTRL &= ~(MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk);

    MPU->RBAR = MPU_PATCH_REGION_ADDR | MPU_RBAR_VALID_Msk | MPU_REGION_1;

    if (enable)
    {
        // Allow privileged read/write and execute
        // - disable sub-regions 6 and 7
        MPU->RASR = MPU_RASR_AP_PRIV_RW_USER_NA | \
                    MPU_RASR_CACHEABLE | \
                    MPU_RASR_DIS_SUBREGION_6 | \
                    MPU_RASR_DIS_SUBREGION_7 | \
                    MPU_REGION_SIZE_64K | \
                    MPU_RASR_ENABLE_Msk;
    }
    else
    {
        // Disable read/write access, no execute
        // - disable sub-regions 6 and 7
        MPU->RASR = MPU_RASR_EXECUTE_NEVER | \
                    MPU_RASR_AP_NO_ACCESS | \
                    MPU_RASR_CACHEABLE | \
                    MPU_RASR_DIS_SUBREGION_6 | \
                    MPU_RASR_DIS_SUBREGION_7 | \
                    MPU_REGION_SIZE_64K | \
                    MPU_RASR_ENABLE_Msk;
    }

    // Re-enable the MPU...
    MPU->CTRL |= (uint32_t)MPU_CTRL_ENABLE_Msk;

    // ...and wait for all instruction/memory accesses to complete
    __DSB();
    __ISB();
}

/// Returns information associated with stack i.e. base, length and guard
///
/// param[out]   Pointer to memory to populate with info of type MpuStackConfigType
/// \returns Command success/failure
/// \retval  ERROR_SUCCESS     Command completed successfully
/// \retval  ERROR_INVAL       NULL pointer passed as input
///
ErrorT InitMpuGetStackInformation(InitMpuStackConfigType* stackConfig)
{
    ErrorT error = ERROR_SUCCESS;

    if (NULL == stackConfig)
    {
        error = ERROR_INVAL;
    }
    else
    {
        stackConfig->base = (uint32_t*)((uint32_t)&Image$$MAIN_STACK$$ZI$$Base);
        stackConfig->sizeLongWords = ((uint32_t)&Image$$MAIN_STACK$$ZI$$Length) / sizeof(uint32_t);
        stackConfig->stackGuardRegionSizeLongWords = (uint32_t)(MPU_STACK_GUARD_SIZE_BYTES / sizeof(uint32_t));
    }

    return error;
}

/// @} endgroup init

