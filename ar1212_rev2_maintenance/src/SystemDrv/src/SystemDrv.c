//***********************************************************************************
/// \file
///
/// System driver
///
/// \addtogroup systemdrv
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

#include "AR0820.h"
#include "Common.h"
#include "Diag.h"
#include "crypto_icore.h"
#include "mcu_sysctrl.h"

#include "SystemDrvInternal.h"

#if defined(BUILD_TARGET_ARCH_x86)
#include "SystemDrvUnitTest.h"
#endif

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the System Driver
///
/// \returns Error status
/// \retval ERROR_SUCCESS   System driver initialized successfully
///
ErrorT SystemDrvInit(void)
{
    return ERROR_SUCCESS;
}

/// Retrieves the Checkpoint register
///
/// \returns Current checkpoint value
///
uint16_t SystemDrvGetCheckpoint(void)
{
    return (uint16_t)(CRYPTO_ICORE_REGS->checkpoint);
}

/// Sets the Checkpoint register
///
/// \param[in] checkpoint   Checkpoint to set
///
/// \returns void
///
void SystemDrvSetCheckpoint(uint16_t const checkpoint)
{
    DiagPrintf("SystemDrv: CHECKPOINT: 0x%04x\n",
        checkpoint);

    CRYPTO_ICORE_REGS->checkpoint = (uint32_t)checkpoint;
}

/// Retrieves the Pausepoint register
///
/// \returns contents of the Pausepoint register field
///
uint16_t SystemDrvGetPausepoint(void)
{
    return (uint16_t)((CRYPTO_ICORE_REGS->pausepoint & CRYPTO_ICORE_PAUSEPOINT_PAUSEPOINT__MASK) >>
                      CRYPTO_ICORE_PAUSEPOINT_PAUSEPOINT__SHIFT);
}

/// Sets the Pausepoint register 'Paused' flag
///
/// \returns void
///
void SystemDrvSetPaused(void)
{
    // The Pausepoint field of the Pausepoint register is read-only to the firmware
    // - we can therefore set the Paused bit by writing to the whole register
    CRYPTO_ICORE_REGS->pausepoint = CRYPTO_ICORE_PAUSEPOINT_PAUSED__MASK;
}

/// Retrieves the Pausepoint register 'Paused' flag
///
/// \returns true if the Paused flag is set
///
bool SystemDrvIsPaused(void)
{
    return (0U != (CRYPTO_ICORE_REGS->pausepoint & CRYPTO_ICORE_PAUSEPOINT_PAUSED__MASK));
}

/// Retrieves the boot options flags
///
/// \returns contents of the Boot Options register
///
uint16_t SystemDrvGetBootOptionFlags(void)
{
    return (uint16_t)CRYPTO_ICORE_REGS->bootoptions;
}

/// Sets the Info register
///
/// \param[in] info     Information value to set
///
/// \returns void
///
void SystemDrvSetInfo(uint16_t const info)
{
    DiagPrintf("SystemDrv: INFO: 0x%04x\n",
        info);

    CRYPTO_ICORE_REGS->info = (uint32_t)info;
}

/// Set the Crypto Error register.
///
/// \param[in] errorId      Error identifier
/// \param[in] context      Optional error context
///
/// \returns void
///
void SystemDrvSetCryptoError(uint32_t const errorId, uint32_t const context)
{
    DiagPrintf("!!! FATAL ERROR !!! errorID: 0x%02x context: 0x%02x\n",
        (uint8_t)(errorId & 0x3F), (uint16_t)(context & 0x3FF));

    CRYPTO_ICORE_REGS->cryptoerror = (uint32_t)
                                     ((errorId << CRYPTO_ICORE_CRYPTOERROR_FAULT_CODE__SHIFT) &
                                      CRYPTO_ICORE_CRYPTOERROR_FAULT_CODE__MASK) | \
                                     ((context << CRYPTO_ICORE_CRYPTOERROR_FAULT_CONTEXT__SHIFT) &
                                      CRYPTO_ICORE_CRYPTOERROR_FAULT_CONTEXT__MASK);
}

/// Enable or disable one of the CSS clocks
///
/// \param[in] cssClock     Clock to enable/disable
/// \param[in] enable       True to enable/false to disable
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Clock enabled/disabled
/// \retval ERROR_INVAL     Invalid clock
///
ErrorT SystemDrvEnableClock(SystemDrvCssClockT const cssClock, bool const enable)
{
    ErrorT status = ERROR_SUCCESS;
    uint32_t val;

    switch (cssClock)
    {
        case SYSTEM_DRV_CSS_CLOCK_VCORE:
            val = MCU_SYSCTRL_REGS->reset_control;
            val &= ~MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK;
            val |= (enable ? MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__MASK : 0U);
            MCU_SYSCTRL_REGS->reset_control = val;
            break;

        default:
            status = ERROR_INVAL;
            break;
    }

    return status;
}

/// Enable or disable the CPU wait option (after watchdog reset)
/// Note hardware defaults to CPU wait disabled.
///
/// \param[in]  enable      True to enable/false to disable
///
/// \returns void
///
void SystemDrvEnableCpuWait(bool const enable)
{
    uint32_t val = MCU_SYSCTRL_REGS->reset_control;

    val &= ~MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK;
    val |= (enable ? MCU_SYSCTRL_RESET_CONTROL_CPU_WAIT__MASK : 0U);
    MCU_SYSCTRL_REGS->reset_control = val;
}

/// Enable or disable the ability to reset the I2C Write CRC
/// Note hardware defaults to reset enabled
///
/// \param[in]  enable      True to enable reset/false to disable reset
///
/// \returns void
///
void SystemDrvEnableI2cWriteCrcReset(bool const enable)
{
    uint32_t val = MCU_SYSCTRL_REGS->reset_control;

    val &= ~MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK;
    val |= (enable ? 0U : MCU_SYSCTRL_RESET_CONTROL_I2C_WRITE_CRC_NO_RESET__MASK);
    MCU_SYSCTRL_REGS->reset_control = val;
}

/// Get the lock status of the PLL
///
/// \returns true if PLL is locked
///
bool SystemDrvIsPllLocked(void)
{
    uint32_t val = MCU_SYSCTRL_REGS->reset_control;

    return (0U != (val & MCU_SYSCTRL_RESET_CONTROL_PLL_LOCK_STATUS__MASK));
}

/// @} endgroup systemdrv

