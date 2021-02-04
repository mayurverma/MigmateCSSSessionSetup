#if !defined(SRC__MACDRV__SRC__MACDRVINTERNAL_H)
#define      SRC__MACDRV__SRC__MACDRVINTERNAL_H
//******************************************************************************
/// \file
///
/// xMAC driver internal APIs and types
///
/// \addtogroup macdrv
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

#include "MacDrv.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#define MAC_DRV_SESSION_KEY_128_BIT_LEN_BYTES   16U     ///< 128-bit session key length in bytes
#define MAC_DRV_SESSION_KEY_192_BIT_LEN_BYTES   24U     ///< 192-bit session key length in bytes
#define MAC_DRV_SESSION_KEY_256_BIT_LEN_BYTES   32U     ///< 256-bit session key length in bytes

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents the xMAC driver state
typedef struct
{
    MacDrvConfigT config;                   ///< Driver configuration
    uint32_t keyLenBytes;                   ///< Length of the session key
    bool isConfigured;                      ///< Flags if the driver has been configured
    bool isActive;                          ///< Flags if the video authentication subsystem is active
    CRYPTO_VCORE_REGS_T* const vcoreRegs;   ///< Reference to the VCORE registers
} MacDrvStateT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

extern MacDrvStateT macDrvState;

/// @} endgroup macdrv
#endif  // !defined(SRC__MACDRV__SRC__MACDRVINTERNAL_H)

