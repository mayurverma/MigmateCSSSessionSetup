//***********************************************************************************
/// \file
///
/// OTPM driver
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

#include <stddef.h>
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

#define NUM_OTPM_CONFIG_WRITES  11U

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the OTPM Driver
/// \returns Error status
/// \retval ERROR_SUCCESS   OTPM driver initialized successfully
///
ErrorT OtpmDrvInit(void)
{
    (void)memset(&otpmDrv, 0, sizeof(otpmDrv));

    return ERROR_SUCCESS;
}

/// Configures the OTPM driver
///
/// \param[in] config       Desired configuration, if NULL use default configuration
/// \param[in] clockRateHz  Core clock rate in Hz
///
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   OTPM configured successfully
/// \retval ERROR_RANGE     Clock frequency out of acceptable range
/// \retval ERROR_IO        OTPM register not written successfully
///
ErrorT OtpmDrvConfigure(const AssetMgrOtpmConfigT* const config, uint32_t clockRateHz)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t clocks;
    uint32_t clocks1;
    OtpmRegisterWriteT regWrite[NUM_OTPM_CONFIG_WRITES];
    // If NULL pointer passed in for 'config' use default configuration 'otpmDrvDefaultConfig'
    AssetMgrOtpmConfigT const* const configuration = (NULL == config) ? &otpmDrvDefaultConfig : config;

    if (OPTM_DRV_CLOCK_RATE_MIN_HZ > clockRateHz)
    {
        error = ERROR_RANGE;
    }
    else if (OPTM_DRV_CLOCK_RATE_MAX_HZ < clockRateHz)
    {
        error = ERROR_RANGE;
    }
    else
    {
        regWrite[0].data = (((uint32_t)(configuration->mr_pgm_soak) << 16)) + (uint32_t)configuration->mr_pgm;
        regWrite[0].address = CRYPTO_OTPM_CORE_MR;

        regWrite[1].data = (((uint32_t)(configuration->mra_pgm) << 16)) + (uint32_t)configuration->mr_read;
        regWrite[1].address = CRYPTO_OTPM_CORE_MR_MRA;

        regWrite[2].data = (((uint32_t)(configuration->mra_read) << 16)) + (uint32_t)configuration->mra_prg_soak;
        regWrite[2].address = CRYPTO_OTPM_CORE_MRA;

        regWrite[3].data = (((uint32_t)(configuration->mrb_prg_soak) << 16)) + (uint32_t)configuration->mrb_pgm;
        regWrite[3].address = CRYPTO_OTPM_CORE_MRB;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->tds_ns);
        regWrite[4].data = (((uint32_t)(clocks) << 16)) + (uint32_t)configuration->mrb_read;
        regWrite[4].address = CRYPTO_OTPM_CORE_MRB_TDS;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->tpas_ns);
        clocks1 = OtpmDrvClockCycles(clockRateHz, configuration->tpp_ns);
        regWrite[5].data = (((uint32_t)(clocks1) << 16)) + (uint32_t)(clocks & 0xFFFFU);
        regWrite[5].address = CRYPTO_OTPM_CORE_PROG_TIME;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->tpr_ns);
        clocks1 = OtpmDrvClockCycles(clockRateHz, configuration->tpsr_ns);
        regWrite[6].data = (((uint32_t)(clocks1) << 16)) + (uint32_t)(clocks & 0xFFFFU);
        regWrite[6].address = CRYPTO_OTPM_CORE_RECOVERY;

        regWrite[7].data = OtpmDrvClockCycles(clockRateHz, configuration->tspp_ns);
        regWrite[7].address = CRYPTO_OTPM_CORE_SOAK_CYCLES;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->tvpps_ns);
        regWrite[8].data = (((uint32_t)(configuration->soak_count_max) << 16)) + (uint32_t)(clocks & 0xFFFFU);
        regWrite[8].address = CRYPTO_OTPM_CORE_PROGRAM;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->twr_ns);
        clocks1 = OtpmDrvClockCycles(clockRateHz, configuration->twp_ns);
        regWrite[9].data = (((uint32_t)(clocks1) << 16)) + (uint32_t)(clocks & 0xFFFFU);
        regWrite[9].address = CRYPTO_OTPM_CORE_WRITE_TIME;

        clocks = OtpmDrvClockCycles(clockRateHz, configuration->tras_ns);
        clocks1 = OtpmDrvClockCycles(clockRateHz, configuration->trr_ns);
        regWrite[10].data = (((uint32_t)(clocks1) << 16)) + (uint32_t)(clocks & 0xFFFFU);
        regWrite[10].address = CRYPTO_OTPM_CORE_READ_TIME;

        error = OtpmDrvRegisterWrite(regWrite, NUM_OTPM_CONFIG_WRITES);
        if (ERROR_SUCCESS == error)
        {
            otpmDrv.isConfigured = true;
        }
    }

    return error;
}

/// Reads long-words from OTPM
///
/// \param[in] address      Address in OTPM to read (relative to OTPM base)
/// \param[out] buffer      Buffer to fill
/// \param[in] numLongWords Number of long-words to read
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   OTPM read successfully
/// \retval ERROR_INVAL     NULL buffer pointer or numLongWords is 0
/// \retval ERROR_RANGE     Parameter address (offset) is outside array size
///
ErrorT OtpmDrvRead(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t idx;

    if (NULL == buffer)
    {
        error = ERROR_INVAL;
    }
    else if (0U == numLongWords)
    {
        error = ERROR_INVAL;
    }
    else if (OtpmDrvGetArraySizeLongWords() <= (address + numLongWords - 1U))
    {
        error = ERROR_RANGE;
    }
    else
    {
        for (idx = 0; idx < numLongWords; idx += 1U)
        {
            buffer[idx] = OTPM_BASE[address + idx];
        }
    }

    return error;
}

/// Writes long-words to OTPM
///
/// \param[in] address      Address in OTPM to write (relative to OTPM base)
/// \param[in] buffer      Buffer to write
/// \param[in] numLongWords Number of long-words to write
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   OTPM written successfully
/// \retval ERROR_INVAL     NULL buffer pointer or numLongWords is 0
/// \retval ERROR_RANGE     Parameter address (offset) is outside array size
/// \retval ERROR_ACCESS    OTPM has not been configured so writes are not allowed
/// \retval ERROR_IO        OTPM location not blank.
///
ErrorT OtpmDrvWrite(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t idx;
    uint32_t data;

    if (true != otpmDrv.isConfigured)
    {
        error = ERROR_ACCESS;
    }
    else if (NULL == buffer)
    {
        error = ERROR_INVAL;
    }
    else if (0U == numLongWords)
    {
        error = ERROR_INVAL;
    }
    else if (OtpmDrvGetArraySizeLongWords() <= (address + numLongWords - 1U))
    {
        error = ERROR_RANGE;
    }
    else
    {
        for (idx = 0; idx < numLongWords; idx += 1U)
        {
            data = OTPM_BASE[address + idx];
            if (0U == data)
            {
                DiagSetCheckpointWithInfoUnprotected(DIAG_CHECKPOINT_OTPM_DRV_WRITE, (uint16_t)idx);
                OTPM_BASE[address + idx] = buffer[idx];
            }
            else
            {
                DiagSetCheckpointWithInfoUnprotected(DIAG_CHECKPOINT_OTPM_DRV_WRITE_ERROR, (uint16_t)idx);
                error = ERROR_IO;
                break;
            }
        }
    }

    return error;
}

/// Returns the last hardware error, and clears error flags to prevent false
/// positives.
///
/// \param[out] error       Pointer to returned error object
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Error object returned successfully
/// \retval ERROR_NOENT     No error signalled
/// \retval ERROR_INVAL     Invalid error object
/// \retval ERROR_IO        OTPM register not written successfully
///
ErrorT OtpmDrvGetLastError(OtpmDrvErrorT* const error)
{
    ErrorT result = ERROR_SUCCESS;
    ErrorT writeResult;
    uint32_t eccFlags;
    OtpmRegisterWriteT regWrite;

    eccFlags = OTPM_REGS->core_ecc_flags;

    if (0U != (eccFlags &
               (CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__MASK | CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK)))
    {
        if (NULL != error)
        {
            error->errorAddress = (uint16_t)OTPM_REGS->core_read_error;
            // As both flags can be set, error is only correctable if DED is not set
            error->isCorrectable = (0U == (eccFlags & CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK));
        }
        else
        {
            result = ERROR_INVAL;
        }

        // Clear flags even if value is not read
        regWrite.data = 0U;
        regWrite.address = CRYPTO_OTPM_CORE_ECC_FLAGS;
        writeResult = OtpmDrvRegisterWrite(&regWrite, 1U);
        if (ERROR_SUCCESS == result)
        {
            result = writeResult;
        }
    }
    else
    {
        result = ERROR_NOENT;
    }

    return result;
}

/// Indicates if a hardware fault has occurred
///
/// \returns Status of hardware fault
/// \retval true            Indicates a hardware non-correctable ECC fault occurred
/// \retval false           No fault signalled
///
bool OtpmDrvIsHardwareFault(void)
{
    uint32_t eccFlags;

    eccFlags = OTPM_REGS->core_ecc_flags;

    return (0U != (eccFlags & CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK));
}

/// Returns size of OTPM array in long words
///
/// \returns Size of OTPM array
///
uint32_t OtpmDrvGetArraySizeLongWords(void)
{
    return (CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA) / sizeof(uint32_t);
}

/// Returns address of start of user space
///
/// The user space start after secure space reserved for crypto core, this function returns
/// address (offset) into OTPM space which defines start of user area
///
/// \returns Address of user space
///
OtpmDrvAddressT OtpmDrvGetUserSpaceAddress(void)
{
    return OTPM_DRV_USER_BASE_LONG_WORDS;
}

/// @} endgroup otpmdrv

