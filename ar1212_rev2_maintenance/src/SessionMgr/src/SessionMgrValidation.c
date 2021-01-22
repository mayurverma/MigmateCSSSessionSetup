//***********************************************************************************
/// \file
///
/// Session Manager internal implementation - validation helpers
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

#include <string.h>     // for memset()/memcpy()

#include "Common.h"
#include "Diag.h"

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

/// Checks if the specified cipher mode is valid
///
/// \param[in] symmetricAuthCipherMode Cipher mode to validate
///
/// \returns Error status
/// \retval ERROR_SUCCESS   cipher mode is valid
/// \retval ERROR_RANGE     mode is out-of-range
///
ErrorT SessionMgrValidateSymmetricAuthCipherMode(CryptoMgrSymmetricAuthCipherModeT const symmetricAuthCipherMode)
{
    ErrorT status = ERROR_SUCCESS;

    switch (symmetricAuthCipherMode)
    {
        case CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_CMAC:
        case CRYPTO_MGR_SYMMETRIC_AUTH_CIPHER_MODE_AES_GMAC:
            break;

        default:
            status = ERROR_RANGE;
            break;
    }

    return status;
}

/// Checks if the specified key type is valid
///
/// \param[in] symmetricKeyType symmetric Key type to validate
///
/// \returns Error status
/// \retval ERROR_SUCCESS   symmetric key type is valid
/// \retval ERROR_RANGE     symmetric key type is out-of-range
///
ErrorT SessionMgrValidateSymmetricKeyType(CryptoMgrSymmetricKeyTypeT const symmetricKeyType)
{
    ErrorT status = ERROR_SUCCESS;

    switch (symmetricKeyType)
    {
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_128:
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_192:
        case CRYPTO_MGR_SYMMETRIC_KEY_AES_256:
            break;

        default:
            status = ERROR_RANGE;
            break;
    }

    return status;
}

/// Checks if the specified AE cipher mode is valid
///
/// \param[in] symmetricAECipherMode AE cipher mode to validate
///
/// \returns Error status
/// \retval ERROR_SUCCESS   AE cipher mode is valid
/// \retval ERROR_RANGE     AE cipher mode is out-of-range
///
ErrorT SessionMgrValidateSymmetricAECipherMode(CryptoMgrSymmetricAECipherModeT const symmetricAECipherMode)
{
    ErrorT status = ERROR_SUCCESS;

    switch (symmetricAECipherMode)
    {
        case CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_CCM:
        case CRYPTO_MGR_SYMMETRIC_AE_CIPHER_MODE_AES_GCM:
            break;

        default:
            status = ERROR_RANGE;
            break;
    }

    return status;
}

/// Initialises the (common) session setup parameters
///
/// \param[in] params   The common session setup parameters
///
/// \return void
///
void SessionMgrInitSessionParams(SessionMgrSessionSetupParamsT* const params)
{
    SessionMgrSessionParamsT* sessionParams = &sessionMgrState.sessionState.sessionParams;

    // Note we copy the params (and cast to the appropriate type) into the session state
    // variables. If the parameters are subsequently deemed invalid they will not be processed.
    sessionParams->videoAuthCipherMode =
        (CryptoMgrSymmetricAuthCipherModeT)params->videoAuthCipherMode;

    sessionParams->videoAuthKeyType =
        (CryptoMgrSymmetricKeyTypeT)params->videoAuthKeyType;

    sessionParams->controlChannelCipherMode =
        (CryptoMgrSymmetricAECipherModeT)params->controlChannelCipherMode;

    sessionParams->controlChannelKeyType =
        (CryptoMgrSymmetricKeyTypeT)params->controlChannelKeyType;

    (void)memcpy(sessionParams->hostSalt,
        params->hostSalt,
        sizeof(sessionParams->hostSalt));
}

/// Validates the host-supplied (common) session parameters
///
/// \param[in] params Session parameters
///
/// \return Error status
/// \retval ERROR_SUCCESS   Parameters are valid
/// \retval ERROR_RANGE     Parameter is out of range
ErrorT SessionMgrValidateSessionParams(SessionMgrSessionParamsT* const params)
{
    ErrorT status = SessionMgrValidateSymmetricAuthCipherMode(params->videoAuthCipherMode);

    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrValidateSymmetricKeyType(params->videoAuthKeyType);
    }

    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrValidateSymmetricAECipherMode(params->controlChannelCipherMode);
    }

    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrValidateSymmetricKeyType(params->controlChannelKeyType);
    }

    return status;
}

/// @} endgroup sessionmgr

