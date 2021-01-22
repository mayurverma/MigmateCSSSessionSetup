//***********************************************************************************
/// \file
///
/// Internal support finctions
///
/// \addtogroup otpmdrv
/// @{
//***********************************************************************************
// Copyright 2018 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//
//*************************************************************************************
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "Diag.h"
#include "crypto_otpm.h"

#include "OtpmDrv.h"
#include "OtpmDrvInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
/// Size of register space in long words
#define OTPM_DRV_REG_ARRAY_SIZE_LONG_WORDS           32U
//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
// Default configuration for OTPM registers
const AssetMgrOtpmConfigT otpmDrvDefaultConfig =
{
    10000U,                 // tvpps_ns;           Charge Pump Warm-up time in nSec
    1U,                     // tpas_ns;            Program Address Setup time in nSec
    200000U,                // tpp_ns;             Program pulse width in nSec
    1U,                     // tpr_ns;             Program recovery time in nSec
    600000U,                // tspp_ns;            Soak pulse width in nSec
    300000U,                // tpsr_ns;            Power Supply Recovery time in nSec
    1U,                     // tds_ns;             Data setup time in nSec
    2U,                     // twr_ns;             Write Recovery time in nSec
    5U,                     // twp_ns;             Write Pulse width in nSec
    4U,                     // tras_ns;            Read address setup time in nSec
    17U,                    // trr_ns;             Read recovery time in nSec
    15U,                    // soak_count_max;     Maximum soak counter (retry)
    0x1024U,                // mr_pgm;             MR value for OTPM programming
    0x08a4U,                // mr_pgm_soak;        MR value for OTPM program soak
    0x0000U,                // mr_read;            MR value for OTPM read
    0x1200U,                // mra_pgm;            MRA value for OTPM programming
    0x1200U,                // mra_prg_soak;       MRA value for OTPM program soak
    0x0210U,                // mra_read;           MRA value for OTPM read
    0x103eU,                // mrb_pgm;            MRB value for OTPM programming
    0x2034U,                // mrb_prg_soak;       MRB value for OTPM program soak
    0x2000U,                // mrb_read;           MRB value for OTPM read
    0x0000U,                // pad;                Padding so that structure is a multiple of 4-bytes
};

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
OtpmDrvT otpmDrv;

/// Based on clock rate and elapse time returns elapse time in clock cycles
///
/// Input clock in nSec (1e-9), frequency in Hz. Cycles = frequency x time i.e.
/// at 1MHz 1000nSec equates to 1 cycle
///
/// Input frequency is in Hz, time is in nS thus
/// cycles = frequency * time * 1e-9 or  cycles = (frequency * time)/1e9
/// 1e9 divided by 2**9 is maximum number which generates whole number result for 1e9/28**9 i.e 1953125 so
/// cycles = (frequency * time)/((2**9)*(1953125)) or
/// cycles = (frequency * time)/(2**9) * 1/(1953125)
///
/// \param[in] clockInputHz      System clock frequency in Hz
/// \param[in] durationNs        Duration in nSec
/// \returns duration in clock cycles (rounded up)
///
uint32_t OtpmDrvClockCycles(uint32_t clockInputHz, uint32_t durationNs)
{
    uint64_t clocks;

    clocks = (uint64_t)clockInputHz * durationNs;
    // clocks = (frequency * time)/1e9
    // clocks = (frequency * time)/((2**9)*(1953125U))
    clocks >>= 9;
    // Multiply by reciprocal of 1953125U with 32 bits of precision
    clocks *= (0xFFFFFFFFU / 1953125U);
    // Remove fractional part
    clocks >>= 32;
    // Always add one
    clocks += 1U;

    return (uint32_t)clocks;
}

/// Writes long-words to OTPM registers (not array)
///
/// \param[in] regWrite     Array of register write operations
/// \param[in] numWrites    Number of registers to write
///
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   OTPM written successfully
/// \retval ERROR_RANGE     OTPM register address is not valid
/// \retval ERROR_IO        OTPM register not written successfully
///
ErrorT OtpmDrvRegisterWrite(OtpmRegisterWriteT const regWrite[], uint32_t const numWrites)
{
    ErrorT error = ERROR_SUCCESS;
    volatile uint32_t* otpmRegisters = (volatile uint32_t*)CRYPTO_OTPM_REG_BASE;
    OtpmDrvAddressT const indexLimit = OtpmDrvGetRegisterBankSizeLongWords();
    OtpmDrvAddressT writeIndex;
    uint32_t writeData;
    uint32_t readData;
    uint32_t idx;

    for (idx = 0; idx < numWrites; idx += 1U)
    {
        writeIndex = (regWrite[idx].address - CRYPTO_OTPM_REG_BASE) / sizeof(uint32_t);
        writeData = regWrite[idx].data;
        if (indexLimit <= writeIndex)
        {
            error = ERROR_RANGE;
        }
        else
        {
            // AR0820FW-170 : When writing to the OTPM registers, the CC312 hardware
            // assumes that it is writing to an OTPM array i.e. bits can only be set
            // (not cleared) and set bits are never set more than once because this
            // can damage the array in some types of OTPM. Therefore when a write is
            // requested the CC312 reads the current value and determines which bits
            // are not yet set and just writes those ones back to the array.
            //
            // In order to write registers, we need to manipulate the input data
            // based on this understanding. If the current contents are written then
            // the CC312 logic will compute and write it to the register to clear it.
            // Then the desired value can be written to the register as normal.
            readData = otpmRegisters[writeIndex];     // Read data value from register array
            otpmRegisters[writeIndex] = readData;     // Write back value to clear all bits
            otpmRegisters[writeIndex] = writeData;    // Write desired value

            // Check write was successful
            readData = otpmRegisters[writeIndex];
            if (writeData != readData)
            {
                DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_OTPM_DRV_REGISTER_WRITE_ERROR, (uint16_t)idx);
                error = ERROR_IO;
            }
        }

        // Stop on first error
        if (ERROR_SUCCESS != error)
        {
            break;
        }
    }

    return error;
}

/// Returns size of OTPM register array in long words
///
/// \returns Size of OTPM register array
///
uint32_t OtpmDrvGetRegisterBankSizeLongWords(void)
{
    return OTPM_DRV_REG_ARRAY_SIZE_LONG_WORDS;
}

/// @} endgroup otpmdrv

