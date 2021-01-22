#if !defined(SRC__MACDRV__INC__MACDRV_H)
#define      SRC__MACDRV__INC__MACDRV_H
//******************************************************************************
/// \file
///
/// GMAC/CMAC AES engine hardware abstraction
///
/// \defgroup macdrv MAC Driver
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

/// Represents a video authentication mode
typedef enum
{
    /// Every pixel and every row in the full frame will be authenticated. All embedded data and embedded stats will be authenticated.
    MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME = 0x00U,

    ///< Every pixel and every row in the specified ROI will be authenticated. A subset of the embedded data will be authenticated.
    MAC_DRV_VIDEO_AUTH_MODE_ROI = 0x01U,
} MacDrvVideoAuthModeT;

/// Represents a pixel packing mode
typedef enum
{
    ///< Pixels will be tightly-packed with minimal padding
    MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED = 0x00U,

    ///< Pixels will be loosely-packed
    MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED = 0x01U,
} MacDrvPixelPackingModeT;

/// Represents the MAC driver configuration
typedef struct
{
    bool cmacNotGmac;                           ///< Set true to enable CMAC, false to enable GMAC
    MacDrvVideoAuthModeT videoAuthmode;         ///< Video authentication mode
    MacDrvPixelPackingModeT pixelPackingMode;   ///< Pixel packing mode
    uint16_t pixelPackingValue;                 ///< Padding value for packing
    uint32_t frameCount;                        ///< Frame count for frame-sync start
} MacDrvConfigT;

/// Represents the ROI configuration
typedef struct
{
    uint16_t firstRow;          ///< ROI row start point
    uint16_t lastRow;           ///< ROI row end point
    uint16_t rowSkip;           ///< ROI row skip (rows)

    uint16_t firstCol;          ///< ROI column start point
    uint16_t lastCol;           ///< ROI column end point
    uint16_t colSkip;           ///< ROI column skip (pixels)

    uint32_t frameCount;        ///< Frame count for frame-sync update
} MacDrvRoiConfigT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Initialise the MAC Driver
extern ErrorT MacDrvInit(void);

// Set the session key for video authentication
extern ErrorT MacDrvSetSessionKey(uint8_t* const key,
    uint32_t const keyLenBytes);

// Set the Initialization Vector for GMAC authentication
extern ErrorT MacDrvSetGmacIv(uint8_t* const iv,
    uint32_t const ivLenBytes);

// Set all IV registers to zero
extern void MacDrvResetIv(void);

// Set the MAC driver configuration (one-time-set)
extern ErrorT MacDrvSetConfig(MacDrvConfigT* const config);

// Set the ROI configuration (many-time-set)
extern ErrorT MacDrvSetRoiConfig(MacDrvRoiConfigT* const config);

// Starts an internal self-test
extern ErrorT MacDrvStartSelfTest(void);

// Resets the video authentication subsystem
extern void MacDrvResetToIdle(void);

// Starts video authentication
extern ErrorT MacDrvStartAuthentication(void);

// Gets the Vcore register aescmac_status
extern uint32_t MacDrvGetCmacStatus(void);

/// @} endgroup macdrv
#endif  // !defined(SRC__MACDRV__INC__MACDRV_H)

