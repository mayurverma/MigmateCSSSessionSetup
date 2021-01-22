//***********************************************************************************
/// \file
///
/// MAC driver
///
/// \addtogroup macdrv
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

#include "AR0820.h"
#include "Common.h"
#include "AR0820_memmap.h"
#include "crypto_vcore.h"

#include "MacDrvInternal.h"

#if defined(BUILD_TARGET_ARCH_x86)
#include "MacDrvUnitTest.h"
#endif

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

MacDrvStateT macDrvState =
{
    .vcoreRegs = CRYPTO_VCORE_REGS
};

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the MAC Driver
/// \returns Error status
/// \retval ERROR_SUCCESS   MAC driver initialized successfully
///
ErrorT MacDrvInit(void)
{
    (void)memset(&macDrvState.config, 0, sizeof(macDrvState.config));
    macDrvState.keyLenBytes = 0U;
    macDrvState.isConfigured = false;
    macDrvState.isActive = false;

    // The video authentication subsystem supports an 'autostart' feature.
    // - Reset the subsystem and disable autostart
    MacDrvResetToIdle();
    macDrvState.vcoreRegs->cnfg &= ~CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__MASK;

    return ERROR_SUCCESS;
}

/// Resets the video authentication subsystem
///
/// Resets the video authentication subsystem. The CMAC and GMAC engines
/// are reset so that they return to their idle states. While in idle,
/// the video authentication subsystem clock can be disabled via the
/// System Driver. When the clock is re-enabled the state machines will
/// remain in the idle state.
///
/// \returns void
///
void MacDrvResetToIdle(void)
{
    // Set the reset bit, self-clearing
    macDrvState.vcoreRegs->cnfg |= CRYPTO_VCORE_CNFG_CPU_VCORE_RESET__MASK;

    // Also reset the GMAC and CMAC engines
    macDrvState.vcoreRegs->cnfg |=
        CRYPTO_VCORE_CNFG_CPU_GMAC_RESET__MASK | CRYPTO_VCORE_CNFG_CPU_CMAC_RESET__MASK;
}

/// Set the session key for video authentication
///
/// \param[in] key          Pointer to key
/// \param[in] keyLenBytes  Length of the key in bytes
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Key set successfully
/// \retval ERROR_INVAL     Invalid or unsupported key
/// \retval ERROR_NOENT     No key specified
///
ErrorT MacDrvSetSessionKey(uint8_t* const key,
    uint32_t const keyLenBytes)
{
    ErrorT status = ERROR_SUCCESS;

    if (NULL != key)
    {
        // Check the key length is valid
        switch (keyLenBytes)
        {
            case MAC_DRV_SESSION_KEY_128_BIT_LEN_BYTES:
                break;

            case MAC_DRV_SESSION_KEY_192_BIT_LEN_BYTES:
                break;

            case MAC_DRV_SESSION_KEY_256_BIT_LEN_BYTES:
                break;

            default:
                status = ERROR_INVAL;
                break;
        }

        if (ERROR_SUCCESS == status)
        {
            __IOM uint32_t* currentSessionKeyReg = &macDrvState.vcoreRegs->session_key7;
            uint32_t numLongWords = keyLenBytes / sizeof(uint32_t);
            uint8_t* currentKey = key;  // start at the most-significant byte of the key
            uint32_t keyLongWord;
            uint32_t idx;

            // Clear the (potentially) unused session key registers
            macDrvState.vcoreRegs->session_key0 = 0U;
            macDrvState.vcoreRegs->session_key1 = 0U;
            macDrvState.vcoreRegs->session_key2 = 0U;
            macDrvState.vcoreRegs->session_key3 = 0U;

            // The session key is big-endian; the most-significant byte is the first
            // element in the key byte array. However, the VCore session key registers
            // are most-significant-long-word-aligned little-endian. The least-significant
            // long word is written to the lower key registers, dependent upon the key size:
            //
            // - 16 byte key: LSW is session key reg 4, MSW is session key reg 7
            // - 24 byte key: LSW is session key reg 2, MSW is session key reg 7
            // - 32 byte key: LSW is session key reg 0, MSW is session key reg 7
            //
            // The key must therefore be byte-reversed when written to the VCORE registers.

            for (idx = 0; idx < numLongWords; idx += 1U)
            {
                keyLongWord = (((uint32_t)currentKey[0] << 24U) |
                               ((uint32_t)currentKey[1] << 16U) |
                               ((uint32_t)currentKey[2] << 8U) |
                               ((uint32_t)currentKey[3] << 0U));

                *currentSessionKeyReg = keyLongWord;

                currentSessionKeyReg--;
                currentKey = &currentKey[4];
            }

            macDrvState.keyLenBytes = keyLenBytes;
        }
    }
    else
    {
        status = ERROR_NOENT;
    }

    return status;
}

/// Set the Initialization Vector to zero
///
/// \returns void
///
void MacDrvResetIv(void)
{
    // clear all four IV registers
    macDrvState.vcoreRegs->iv0 = 0U;
    macDrvState.vcoreRegs->iv1 = 0U;
    macDrvState.vcoreRegs->iv2 = 0U;
    macDrvState.vcoreRegs->iv3 = 0U;
}

/// Set the Initialization Vector for GMAC authentication
///
/// \param[in] iv           Pointer to the initialization vector
/// \param[in] ivLenBytes   Length of the IV in bytes
/// \returns Error status
/// \retval ERROR_SUCCESS   IV set successfully
/// \retval ERROR_INVAL     Invalid IV length
/// \retval ERROR_NOENT     No IV specified
///
ErrorT MacDrvSetGmacIv(uint8_t* const iv,
    uint32_t const ivLenBytes)
{
    ErrorT status = ERROR_SUCCESS;

    if (NULL == iv)
    {
        status = ERROR_NOENT;
    }
    else if (12U != ivLenBytes)
    {
        status = ERROR_INVAL;
    }
    else
    {
        __IOM uint32_t* currentIvReg = &macDrvState.vcoreRegs->iv3;
        uint8_t* currentIv = iv;  // start at the most-significant byte of the IV
        uint32_t ivLongWord;
        uint32_t idx;

        // AR0820FW-280: MacDrvSetGmacIv must set the least-significant bit of the IV for GMAC mode
        macDrvState.vcoreRegs->iv0 = 0x00000001U;

        // The GMAC IV is in big-endian format; the most-significant byte is element zero in the
        // supplied iv array. However, the VCore IV registers are most-significant-long-word-aligned
        // little-endian. The least-significant long word is written to the IV1 register. The IV must
        // therefore be byte-reversed when written to the VCORE registers.

        for (idx = 0; idx < 3U; idx += 1U)
        {
            ivLongWord = (((uint32_t)currentIv[0] << 24U) |
                          ((uint32_t)currentIv[1] << 16U) |
                          ((uint32_t)currentIv[2] << 8U) |
                          ((uint32_t)currentIv[3] << 0U));

            *currentIvReg = ivLongWord;

            currentIvReg--;
            currentIv = &currentIv[4];
        }
    }

    return status;
}

/// Set the MAC driver configuration
///
/// \param[in] config       Desired configuration
/// \returns Error status
/// \retval ERROR_SUCCESS   Configuration set successfully
/// \retval ERROR_ALREADY   Already configured
/// \retval ERROR_NOENT     Config is NULL
/// \retval ERROR_INVAL     Invalid configuration
///
ErrorT MacDrvSetConfig(MacDrvConfigT* const config)
{
    ErrorT status = ERROR_SUCCESS;

    // NOTE: clock must be enabled when calling this func!

    if (macDrvState.isConfigured)
    {
        status = ERROR_ALREADY;
    }
    else if (NULL == config)
    {
        status = ERROR_NOENT;
    }
    else
    {
        uint32_t val;

        // Check the configuration is valid
        if (config->cmacNotGmac && (MAC_DRV_VIDEO_AUTH_MODE_ROI != config->videoAuthmode))
        {
            // CMAC authentication but not ROI mode
            status = ERROR_INVAL;
        }
        else if (!config->cmacNotGmac && (MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME != config->videoAuthmode))
        {
            // GMAC authentication but not full-frame mode
            status = ERROR_INVAL;
        }
        else
        {
            uint32_t packingModeMask = (1U << CRYPTO_VCORE_CNFG_PACK_MODE__SIZE) - 1U;
            uint32_t packingMode = (uint32_t)config->pixelPackingMode & packingModeMask;

            // Store the configuration, and flag that we're configured
            (void)memcpy(&macDrvState.config, config, sizeof(macDrvState.config));
            macDrvState.isConfigured = true;

            // Configure the VCore registers
            // - note packing MUST be enabled
            // - note auth-frame-start-en MUST be enabled
            macDrvState.vcoreRegs->strt_frame_ident = config->frameCount;

            val = config->cmacNotGmac ? CRYPTO_VCORE_CNFG_AESCMAC_EN__MASK : 0U;
            val |= packingMode << CRYPTO_VCORE_CNFG_PACK_MODE__SHIFT;
            val |= CRYPTO_VCORE_CNFG_PACK_ENABLE__MASK;
            val |= CRYPTO_VCORE_CNFG_AUTH_FRAME_START_EN__MASK;

            macDrvState.vcoreRegs->cnfg = val;
            macDrvState.vcoreRegs->pack_val_lo = (uint32_t)config->pixelPackingValue;

            if (!config->cmacNotGmac)
            {
                // ROI is not supported for GMAC
                macDrvState.vcoreRegs->roi = 0U;
                macDrvState.vcoreRegs->roi_frame_ident = 0U;
            }
            else
            {
                // ROI configuration is deferred until MacDrvSetRoiConfig
            }
        }
    }

    return status;
}

/// Set the ROI configuration (many-time-set)
///
/// Sets a new ROI authentication mode
///
/// \param[in] config       Desired configuration
/// \returns Error status
/// \retval ERROR_SUCCESS   ROI configuration set successfully
/// \retval ERROR_AGAIN     Driver not configured
/// \retval ERROR_NOENT     Config is NULL
/// \retval ERROR_NOEXEC    ROI not supported for GMAC authentication
///
ErrorT MacDrvSetRoiConfig(MacDrvRoiConfigT* const config)
{
    ErrorT status = ERROR_SUCCESS;

    if (!macDrvState.isConfigured)
    {
        status = ERROR_AGAIN;
    }
    else if (NULL == config)
    {
        status = ERROR_NOENT;
    }
    else if (!macDrvState.config.cmacNotGmac)
    {
        // ROI not supported for GMAC authentication
        status = ERROR_NOEXEC;
    }
    else
    {
        uint32_t fieldMask;
        uint32_t fieldVal;
        uint32_t regVal;

        // Update the ROI configuration registers
        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__SIZE) - 1U;
        fieldVal = (uint32_t)config->firstRow & fieldMask;
        regVal = fieldVal << CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__SHIFT;

        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__SIZE) - 1U;
        fieldVal = (uint32_t)config->lastRow & fieldMask;
        regVal |= fieldVal << CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__SHIFT;

        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__SIZE) - 1U;
        fieldVal = (uint32_t)config->rowSkip & fieldMask;
        regVal |= fieldVal << CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__SHIFT;

        macDrvState.vcoreRegs->roi_row_cnfg = regVal;

        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__SIZE) - 1U;
        fieldVal = (uint32_t)config->firstCol & fieldMask;
        regVal = fieldVal << CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__SHIFT;

        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__SIZE) - 1U;
        fieldVal = (uint32_t)config->lastCol & fieldMask;
        regVal |= fieldVal << CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__SHIFT;

        fieldMask = ((uint32_t)1U << CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__SIZE) - 1U;
        fieldVal = (uint32_t)config->colSkip & fieldMask;
        regVal |= fieldVal << CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__SHIFT;

        macDrvState.vcoreRegs->roi_col_cnfg = regVal;

        if (macDrvState.isActive)
        {
            // Authentication is already active, set the 'match' frame counter and
            // tell the hardware to update when this frame occurs
            macDrvState.vcoreRegs->roi_frame_ident = config->frameCount;
            macDrvState.vcoreRegs->roi = CRYPTO_VCORE_ROI_ROI_MODE_EN__MASK |
                                         CRYPTO_VCORE_ROI_ROI_FC_UPDATE__MASK;
        }
        else
        {
            // Authentication is not active, set the immediate start (which self-clears)
            macDrvState.vcoreRegs->roi_frame_ident = 0U;
            macDrvState.vcoreRegs->roi = CRYPTO_VCORE_ROI_ROI_MODE_EN__MASK |
                                         CRYPTO_VCORE_ROI_ROI_INIT__MASK;
        }
    }

    return status;
}

/// Starts video authentication
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Authentication started
/// \retval ERROR_AGAIN     Driver not configured
/// \retval ERROR_ALREADY   Driver already active
/// \retval ERROR_INVAL     Invalid key length
///
ErrorT MacDrvStartAuthentication(void)
{
    ErrorT status = ERROR_SUCCESS;

    if (!macDrvState.isConfigured)
    {
        status = ERROR_AGAIN;
    }
    else if (macDrvState.isActive)
    {
        status = ERROR_ALREADY;
    }
    else
    {
        uint32_t keySize = 0U;
        uint32_t reg;

        // Set the key size
        switch (macDrvState.keyLenBytes)
        {
            case MAC_DRV_SESSION_KEY_128_BIT_LEN_BYTES:
                keySize = 0U;
                break;

            case MAC_DRV_SESSION_KEY_192_BIT_LEN_BYTES:
                keySize = 1U;
                break;

            case MAC_DRV_SESSION_KEY_256_BIT_LEN_BYTES:
                keySize = 2U;
                break;

            default:
                status = ERROR_INVAL;
                break;
        }

        if (ERROR_SUCCESS == status)
        {
            // Enable the appropriate MAC engine
            // - the unused engine will remain in its idle state
            if (macDrvState.config.cmacNotGmac)
            {
                // Read the AESMAC_CNFG reg, preserving the HW default for the mode and encdec bits
                reg = (uint32_t)(macDrvState.vcoreRegs->aescmac_cnfg &
                                 (CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_MODE__MASK |
                                  CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_ENCDEC__MASK));

                // Add in the keysize bits
                reg |= (keySize << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_KEYSIZE__SHIFT);

                // Enable CMAC authentication mode
                reg |= CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_AUTHMODE__MASK;

                macDrvState.vcoreRegs->aescmac_cnfg = reg;
                macDrvState.vcoreRegs->aescmac_start = CRYPTO_VCORE_AESCMAC_START_AESCMAC_START__MASK;
            }
            else
            {
                // Read the AESHP_CNFG reg, preserving the HW default for the enc/dec bit
                reg = (uint32_t)(macDrvState.vcoreRegs->aeshp_cnfg & CRYPTO_VCORE_AESHP_CNFG_AESHP_ENCDEC__MASK);

                // Add in the keysize bits
                reg |= (keySize << CRYPTO_VCORE_AESHP_CNFG_AESHP_KEYSIZE__SHIFT);

                // Enable GMAC authentication mode
                reg |= CRYPTO_VCORE_AESHP_CNFG_AESHP_AUTHMODE__MASK;

                macDrvState.vcoreRegs->aeshp_cnfg = reg;
                macDrvState.vcoreRegs->aeshp_start = CRYPTO_VCORE_AESHP_START_AESHP_START__MASK;
            }

            macDrvState.isActive = true;
        }
    }

    return status;
}

/// Return the value of the Vcore aescmac_status register
///
/// \returns Vcore CMAC status register
/// \retval CMAC status register value
uint32_t MacDrvGetCmacStatus(void)
{
    return (macDrvState.vcoreRegs->aescmac_status);
}
/// @} endgroup macdrv

