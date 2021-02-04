#if !defined(SRC__OTPMDRV__INC__OTPMDRV_H)
#define      SRC__OTPMDRV__INC__OTPMDRV_H
//******************************************************************************
/// \file
///
/// OTPM hardware abstraction
///
/// \defgroup otpmdrv OTPM Driver
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
#include "AssetMgr.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents an OTPM address (relative to the base of OTPM)
typedef uint32_t OtpmDrvAddressT;

/// Represents an OTPM error
typedef struct
{
    uint16_t errorAddress;  ///< Address where error occurred (relative to base of OTPM)
    bool isCorrectable;     ///< Flags if error was correctable (true) or uncorrectable (false)
} OtpmDrvErrorT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initialize the OTPM Driver
extern ErrorT OtpmDrvInit(void);

// Configures the OTPM driver
extern ErrorT OtpmDrvConfigure(const AssetMgrOtpmConfigT* const config, uint32_t clockRateHz);

// Reads long-words from OTPM
extern ErrorT OtpmDrvRead(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords);

// Writes long-words to OTPM
extern ErrorT OtpmDrvWrite(OtpmDrvAddressT const address,
    uint32_t* const buffer,
    uint32_t numLongWords);

// Returns the last hardware error, and clears error flags to prevent false positives
extern ErrorT OtpmDrvGetLastError(OtpmDrvErrorT* const error);

// Indicates if a hardware fault has occurred
extern bool OtpmDrvIsHardwareFault(void);

// Returns size of OTPM array in long words
extern uint32_t OtpmDrvGetArraySizeLongWords(void);

// Returns address of start of user space
extern OtpmDrvAddressT OtpmDrvGetUserSpaceAddress(void);

/// @} endgroup otpmdrv
#endif  // !defined(SRC__OTPMDRV__INC__OTPMDRV_H)

