#if !defined(SRC__SYSTEMDRV__INC__SYSTEMDRV_H)
#define      SRC__SYSTEMDRV__INC__SYSTEMDRV_H
//******************************************************************************
/// \file
///
/// Provides 'system' driver functions
///
/// \defgroup systemdrv System driver
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
//

#include "Common.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Supported CSS clocks (bit mask)
typedef enum
{
    SYSTEM_DRV_CSS_CLOCK_VCORE = (1U << 0),    ///< VCore clock (CMAC/GMAC)
} SystemDrvCssClockT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initialize the System Driver
extern ErrorT SystemDrvInit(void);

// Retrieves the Checkpoint register
extern uint16_t SystemDrvGetCheckpoint(void);

// Sets the Checkpoint register
extern void SystemDrvSetCheckpoint(uint16_t const checkpoint);

// Retrieves the Pausepoint register
extern uint16_t SystemDrvGetPausepoint(void);

// Sets the Pausepoint register 'Paused' flag
extern void SystemDrvSetPaused(void);

// Retrieves the Pausepoint register 'Paused' flag
extern bool SystemDrvIsPaused(void);

// Retrieves the boot options flags
extern uint16_t SystemDrvGetBootOptionFlags(void);

// Sets the Info register
extern void SystemDrvSetInfo(uint16_t const info);

// Puts the processor to sleep and waits for an interrupt
extern void SystemDrvWaitForInterrupt(void);

// Set the Crypto Error register (and assert SYS_CHECK pin). Terminate firmware.
extern void SystemDrvSetCryptoError(uint32_t const errorId, uint32_t const context);

// Terminate processor execution
extern void SystemDrvTerminate(void);

// Enable or disable one of the CSS clocks
extern ErrorT SystemDrvEnableClock(SystemDrvCssClockT const cssClock, bool const enable);

// Retrieve contents of the INFO register
extern uint16_t SystemDrvGetInfo(void);

// Enable or disable the CPU wait option (after watchdog reset)
extern void SystemDrvEnableCpuWait(bool const enable);

// Enable or disable the ability to reset the I2C Write CRC
extern void SystemDrvEnableI2cWriteCrcReset(bool const enable);

// Get the lock status of the PLL
extern bool SystemDrvIsPllLocked(void);

/// @} endgroup systemdrv
#endif  // !defined(SRC__SYSTEMDRV__INC__SYSTEMDRV_H)

