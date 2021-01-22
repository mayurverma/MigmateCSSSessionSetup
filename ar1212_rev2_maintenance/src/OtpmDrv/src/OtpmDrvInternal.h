#if !defined(SRC__OTPMDRV__SRC__OTPMDRVINTERNAL_H)
#define      SRC__OTPMDRV__SRC__OTPMDRVINTERNAL_H
//******************************************************************************
/// \file
///
/// Provides OtpmDrv internal header file
///
/// \addtogroup otpmdrv
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
#include "OtpmDrv.h"

#if defined(BUILD_TARGET_ARCH_x86)
#include "OtpmDrvUnitTest.h"
#endif

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------
#define OTPM_DRV_USER_BASE_LONG_WORDS 44U        ///< Base of user space in OPTM
#define OPTM_DRV_CLOCK_RATE_MIN_HZ  6000000UL    ///< Minimum usable clock frequency for OTPM
#define OPTM_DRV_CLOCK_RATE_MAX_HZ  156000000UL  ///< Maximum usable clock frequency for OTPM
//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// OTPM Driver control/status structure
typedef struct
{
    bool isConfigured;                          ///< Otpm sub-system configured
} OtpmDrvT;

/// OTPM Driver register write description
typedef struct
{
    uint32_t address;  ///< Register address
    uint32_t data;     ///< Register data to be written
} OtpmRegisterWriteT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
/// OTPM Driver control/status data structure
extern OtpmDrvT otpmDrv;

// Default configuration for OTPM registers
extern const AssetMgrOtpmConfigT otpmDrvDefaultConfig;

// Based on clock rate and elapse time returns elapse time in clock cycles
extern uint32_t OtpmDrvClockCycles(uint32_t clockInputHz, uint32_t durationNs);

// Writes long-words to OTPM registers (not array)
extern ErrorT OtpmDrvRegisterWrite(OtpmRegisterWriteT const regWrite[], uint32_t const numWrites);

// Returns size of OTPM register array in long words
extern uint32_t OtpmDrvGetRegisterBankSizeLongWords(void);

/// @} endgroup otpmdrv
#endif  // !defined(SRC__OTPMDRV__SRC__OTPMDRVINTERNAL_H)

