//***********************************************************************************
/// \file
///
/// Session Manager internal implementation - video authentication support
///
/// \addtogroup sessionmgr
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

#include "Common.h"
#include "Diag.h"
#include "MacDrv.h"

#include "SessionMgrInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Configures the MAC driver
///
/// \param[in] config       the desired configuration
/// \param[in] sessionState the current session state
///
/// \returns Error status
/// \retval ERROR_SUCCESS   MAC driver configured successfully
/// \retval ERROR_INVAL     Invalid configuration parameters
///
ErrorT SessionMgrConfigMacDriver(SessionMgrVideoAuthConfigParamsT* const config,
    SessionMgrSessionStateT* const sessionState)
{
    ErrorT status = ERROR_SUCCESS;
    MacDrvConfigT macConfig;
    MacDrvVideoAuthModeT videoAuthMode = (MacDrvVideoAuthModeT)config->videoAuthMode;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_DRIVER);

    switch (videoAuthMode)
    {
        case MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME:
        case MAC_DRV_VIDEO_AUTH_MODE_ROI:
            macConfig.videoAuthmode = videoAuthMode;
            break;

        default:
            status = ERROR_INVAL;
            break;
    }

    if (ERROR_SUCCESS == status)
    {
        MacDrvPixelPackingModeT pixelPackingMode = (MacDrvPixelPackingModeT)config->pixelPackMode;

        switch (pixelPackingMode)
        {
            case MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED:
            case MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED:
                macConfig.pixelPackingMode = pixelPackingMode;
                break;

            default:
                status = ERROR_INVAL;
                break;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        uint32_t videoAuthKeySizeBytes;

        // Get the video authentication key size (ignore the result because videoAuthKeyType has been validated)
        (void)CryptoMgrGetSymmetricKeySizeBytes(sessionState->sessionParams.videoAuthKeyType,
            &videoAuthKeySizeBytes);

        // Set the session key and IV
        // - we must reset the IV if GMAC is not being used
        status = MacDrvSetSessionKey(sessionState->sessionKeys.videoAuthKey,
                videoAuthKeySizeBytes);

        if (ERROR_SUCCESS == status)
        {
            if (CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC ==
                sessionState->sessionParams.videoAuthCipherMode)
            {
                macConfig.cmacNotGmac = true;
                MacDrvResetIv();       // All four IV registers set to zero for CMAC mode
            }
            else
            {
                macConfig.cmacNotGmac = false;
                status = MacDrvSetGmacIv(sessionState->sessionParams.videoAuthGmacIv,
                        sizeof(sessionState->sessionParams.videoAuthGmacIv));
            }
        }
    }

    if (ERROR_SUCCESS == status)
    {
        macConfig.pixelPackingValue = config->pixelPackValue;
        macConfig.frameCount = (uint32_t)(config->frameCounter & 0xFFFFFFFFUL);
        status = MacDrvSetConfig(&macConfig);
    }

    return status;
}

/// Configures the video authentication ROI
///
/// \param[in] config       the desired configuration
///
/// \returns Error status
/// \retval ERROR_SUCCESS   ROI configured successfully
/// \retval ERROR_AGAIN     Driver not configured
/// \retval ERROR_NOENT     Config is NULL
/// \retval ERROR_NOEXEC    ROI not supported for GMAC authentication
///
ErrorT SessionMgrConfigRoi(SessionMgrVideoAuthConfigParamsT* const config)
{
    MacDrvRoiConfigT roiConfig;
    ErrorT status;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_CONFIG_MAC_ROI);

    roiConfig.firstRow = config->firstRow;
    roiConfig.lastRow = config->lastRow;
    roiConfig.rowSkip = config->rowSkip;

    roiConfig.firstCol = config->firstColumn;
    roiConfig.lastCol = config->lastColumn;
    roiConfig.colSkip = config->columnSkip;

    roiConfig.frameCount = (uint32_t)(config->frameCounter & 0xFFFFFFFFUL);

    status = MacDrvSetRoiConfig(&roiConfig);

    return status;
}

/// Configures the video authentication subsystem (VCore)
///
/// \param[in] config   the desired configuration
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Command was successful, video authentication is active
/// \retval ERROR_INVAL     Invalid command parameters
/// \retval ERROR_NOEXEC    Attempting to reconfigure in GMAC mode
///
ErrorT SessionMgrConfigVideoAuth(SessionMgrVideoAuthConfigParamsT* const config)
{
    SessionMgrSessionStateT* sessionState = &sessionMgrState.sessionState;
    ErrorT status;

    // First re-enable the VCore clocks before attempting to access registers
    status = SystemDrvEnableClock(SYSTEM_DRV_CSS_CLOCK_VCORE, true);

    if ((ERROR_SUCCESS == status) &&
        (!sessionMgrState.videoAuthState.isActive))
    {
        status = SessionMgrConfigMacDriver(config, sessionState);
    }

    if ((ERROR_SUCCESS == status) &&
        sessionMgrState.videoAuthState.isActive &&
        (CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC ==
         sessionState->sessionParams.videoAuthCipherMode))
    {
        // Attempting to reconfigure in GMAC mode - not supported
        status = ERROR_NOEXEC;
    }

    if ((ERROR_SUCCESS == status) &&
        (CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC ==
         sessionState->sessionParams.videoAuthCipherMode))
    {
        // (Re)configure the ROI for CMAC mode
        // - this is supported even if video authentication is already active
        status = SessionMgrConfigRoi(config);
    }

    if ((ERROR_SUCCESS == status) && !sessionMgrState.videoAuthState.isActive)
    {
        // Set up MAC hardware to start authentication.
        status = MacDrvStartAuthentication();
        if (ERROR_SUCCESS == status)
        {
            // Video authentication is now active
            sessionMgrState.videoAuthState.isActive = true;
        }
    }

    if (ERROR_SUCCESS != status)
    {
        // Disable the VCore clocks if anything went wrong.
        (void)SystemDrvEnableClock(SYSTEM_DRV_CSS_CLOCK_VCORE, false);
    }

    return status;
}

/// @} endgroup sessionmgr

