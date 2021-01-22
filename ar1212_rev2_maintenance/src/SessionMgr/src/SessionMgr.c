//***********************************************************************************
/// \file
///
/// Session Manager API implementation
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

#include <stdlib.h>     // for calloc()/malloc()/free()
#include <string.h>

#include "Common.h"
#include "Diag.h"
#include "SystemMgr.h"

#include "SessionMgrInternal.h"

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

/// Handles entry to an operating phase
///
/// \param[in] phase    Phase being entered
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Phase entered successfully
/// \retval ERROR_INVAL     Failed to disable the VCore clock
/// \retval ERROR_NOSPC     Failed to register fatal error dump handler
///
ErrorT SessionMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT status = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            (void)memset((void*)&sessionMgrState, 0, sizeof(sessionMgrState));

            // Disable the clock to the video authentication subsystem to
            // minimize power consumption. Clock is renabled once a secure session
            // has been established.
            status = SystemDrvEnableClock(SYSTEM_DRV_CSS_CLOCK_VCORE, false);
            if (ERROR_SUCCESS == status)
            {
                status = DiagRegisterFatalErrorDumpHandler(SessionMgrOnDiagFatalErrorDump);
            }
            else
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_DISABLE_VCORE_CLOCK_FAILED,
                    (uint32_t)status);
            }

            DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_INIT);
            break;

        case SYSTEM_MGR_PHASE_SHUTDOWN:
            if (sessionMgrState.videoAuthState.isActive)
            {
                // Reset the video authentication subsystem, and disable its clock
                MacDrvResetToIdle();
                (void)SystemDrvEnableClock(SYSTEM_DRV_CSS_CLOCK_VCORE, false);
                sessionMgrState.videoAuthState.isActive = false;
            }
            DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ENTER_SHUTDOWN);
            break;

        default:
            // Nothing to do here...
            break;
    }

    return status;
}

/// Handles the GetCertificates host command
///
/// Supported in the DM, Secure and RMA lifecycle states
///
/// \param[in]  commandCode Command to handle (GET_CERTIFICATES)
/// \param[in]  params      Pointer to the command params (in shared memory)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Certificates located and present in shared-memory
/// \retval ERROR_INVAL     Invalid certificate authority identifier
/// \retval ERROR_ACCESS    Access not permitted in CM lifecycle state
/// \retval ERROR_NOENT     Certificates not found in NVM
/// \retval ERROR_SYSERR    Unexpected system failure
///
ErrorT SessionMgrOnGetCertificates(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    // Session Manager provides the SessionMgrGetCertificatesResponse structure, but it is not
    // used here as it only represents a partial response. This is because the sensor certificate
    // size is unknown, so the full response object cannot be represented as a structure. Instead
    // we use a uint16_t array mapped over command parameters.
    uint16_t* responseBuffer = (uint16_t*)params;

    LifecycleMgrLifecycleStateT currentLcs;
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES);

    // Defensive checks...
    if (HOST_COMMAND_CODE_GET_CERTIFICATES != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_PARAMS,
            0U);
        status = ERROR_SYSERR;
    }
    else
    {
        // Validate request params
        AssetMgrCertificateAuthIdT certificateAuthId =
            (AssetMgrCertificateAuthIdT)((SessionMgrGetCertificatesRequestT*)params)->certificateAuthId;

        switch (certificateAuthId)
        {
            case ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_A:
            case ASSET_MGR_CERTIFICATE_AUTH_ID_VENDOR_B:
                // Command is not supported in the CM lifecycle state - get our current state and check
                status = LifecycleMgrGetCurrentLcs(&currentLcs);
                if (ERROR_SUCCESS == status)
                {
                    if (LIFECYCLE_MGR_LIFECYCLE_STATE_CM != currentLcs)
                    {
                        // Valid LCS, but have we already issued certificates?
                        if (sessionMgrState.certificateState.certificatesIssued)
                        {
                            DiagPrintf("SessionMgrRetrieveCertificates: certificates already issued\n");
                            status = ERROR_ACCESS;
                        }
                        else
                        {
                            status = SessionMgrRetrieveCertificates(certificateAuthId, responseBuffer);
                            if (ERROR_SUCCESS == status)
                            {
                                // Flag that certificates have been supplied
                                sessionMgrState.certificateState.certificatesIssued = true;
                                sessionMgrState.certificateState.certificateAuthId = certificateAuthId;
                            }
                        }
                    }
                    else
                    {
                        // Command not supported in CM - reject
                        DiagPrintf("SessionMgrOnGetCertificates: in CM - rejecting\n");
                        status = ERROR_ACCESS;
                    }
                }
                break;

            default:
                DiagPrintf("SessionMgrOnGetCertificates: invalid certificate auth ID: %d\n",
                (uint32_t)certificateAuthId);
                status = ERROR_INVAL;
                break;
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_GET_CERTIFICATES_DONE);

    return status;
}

/// Handles the SetEphemeralMasterSecret host command
///
/// Supported in CM and DM lifecycle states
///
/// \param[in]  commandCode Command to handle (SET_EPHERMERAL_MASTER_SECRET)
/// \param[in]  params      Pointer to the command params (in shared memory)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Master secret stored successfully
/// \retval ERROR_INVAL     Invalid certificate authority identifier
/// \retval ERROR_ACCESS    Access not permitted in CM lifecycle state
/// \retval ERROR_NOENT     Certificates not found in NVM
/// \retval ERROR_SYSERR    Unexpected system failure
///
ErrorT SessionMgrOnSetEphemeralMasterSecret(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    LifecycleMgrLifecycleStateT currentLcs;
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET);

    // Defensive checks...
    if (HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_PARAMS,
            0U);
        status = ERROR_SYSERR;
    }
    else
    {
        // Command is not supported in the Secure and RMA lifecycle states - get our current state and check
        status = LifecycleMgrGetCurrentLcs(&currentLcs);
        if (ERROR_SUCCESS == status)
        {
            if ((LIFECYCLE_MGR_LIFECYCLE_STATE_CM == currentLcs) ||
                (LIFECYCLE_MGR_LIFECYCLE_STATE_DM == currentLcs))
            {
                if (0U != sessionMgrState.ephemeralMasterSecret.keySizeBytes)
                {
                    // Reject as master secret has already been set
                    DiagPrintf("SessionMgrOnSetEphemeralMasterSecret: already set - rejecting\n");
                    status = ERROR_ALREADY;
                }
            }
            else
            {
                // Command not supported in Secure or RMA - reject
                DiagPrintf("SessionMgrOnSetEphemeralMasterSecret: in Secure/RMA - rejecting\n");
                status = ERROR_ACCESS;
            }
        }
    }

    if (ERROR_SUCCESS == status)
    {
        // Verify request parameters
        SessionMgrSetEphemeralMasterSecretRequestT* request = (SessionMgrSetEphemeralMasterSecretRequestT*)params;
        CryptoMgrSymmetricKeyTypeT keyType = (CryptoMgrSymmetricKeyTypeT)request->masterSecretKeyType;
        uint32_t keySizeBytes;

        status = CryptoMgrGetSymmetricKeySizeBytes(keyType, &keySizeBytes);
        if (ERROR_SUCCESS == status)
        {
            // Valid key type, store the master secret
            sessionMgrState.ephemeralMasterSecret.keySizeBytes = keySizeBytes;
            sessionMgrState.ephemeralMasterSecret.masterSecret.header.masterSecretKeyType =
                request->masterSecretKeyType;
            (void)memcpy(sessionMgrState.ephemeralMasterSecret.masterSecret.masterSecret,
                request->masterSecret,
                keySizeBytes);
        }
        else
        {
            DiagPrintf("SessionMgrOnSetEphemeralMasterSecret: invalid key type (%d) - rejecting\n",
                keyType);
        }
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE);

    return status;
}

/// Handles the SetPskSessionKeys host command
///
/// Supported in all lifecycle states
/// Only called in the Configure phase
///
/// \param[in]  commandCode Command to handle (SET_PSK_SESSION_KEY)
/// \param[in]  params      Pointer to the command params (in shared memory)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Master secret stored successfully
/// \retval ERROR_AGAIN     GetSensorId command not already called
/// \retval ERROR_IO        MasterSecret asset in NVM is corrupt
/// \retval ERROR_SYSERR    Unexpected system failure
///
ErrorT SessionMgrOnSetPskSessionKeys(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY);

    // Defensive checks...
    if (HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_PARAMS,
            0U);
        status = ERROR_SYSERR;
    }
    else
    {
        // Host must retrieve the sensor ID before attempting to start a session
        // - check it did, and reject if not
        bool getSensorIdCalled;

        status = CryptoMgrGetSensorIdCalled(&getSensorIdCalled);
        if (ERROR_SUCCESS == status)
        {
            if (!getSensorIdCalled)
            {
                DiagPrintf("SessionMgrOnSetPskSessionKeys: sensor ID not retrieved - rejecting\n");
                status = ERROR_AGAIN;
            }
        }
    }

    if ((ERROR_SUCCESS == status) && sessionMgrState.sessionState.sessionInitiated)
    {
        DiagPrintf("SessionMgrOnSetPskSessionKeys: rejecting second initiation attempt\n");
        status = ERROR_ACCESS;
    }

    if (ERROR_SUCCESS == status)
    {
        SessionMgrSetPskSessionKeysRequestT* request = (SessionMgrSetPskSessionKeysRequestT*)params;
        SessionMgrSetSessionKeysResponseT* response = (SessionMgrSetSessionKeysResponseT*)params;

        status = SessionMgrInitiatePskSession(request, response);
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE);

    return status;
}

/// Handles the SetSessionKeys host command
///
/// Supported in the DM, Secure and RMA lifecycle states
/// Only called in the Configure phase
///
/// \param[in]  commandCode Command to handle (SET_SESSION_KEY)
/// \param[in]  params      Pointer to the command params (in shared memory)
///
/// \retval ERROR_SUCCESS   Master secret stored successfully
/// \retval ERROR_AGAIN     GetSensorId command not already called
/// \retval ERROR_IO        MasterSecret asset in NVM is corrupt
/// \retval ERROR_SYSERR    Unexpected system failure
///
ErrorT SessionMgrOnSetSessionKeys(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY);

    // Defensive checks...
    if (HOST_COMMAND_CODE_SET_SESSION_KEYS != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_PARAMS,
            0U);
        status = ERROR_SYSERR;
    }
    else
    {
        // Command is not supported in the CM lifecycle state - get our current state and check
        status = LifecycleMgrGetCurrentLcs(&currentLcs);
        if ((ERROR_SUCCESS == status) &&
            (LIFECYCLE_MGR_LIFECYCLE_STATE_CM == currentLcs))
        {
            DiagPrintf("SessionMgrOnSetSessionKeys: in CM - rejecting\n");
            status = ERROR_ACCESS;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        // Host must retrieve the sensor ID before attempting to start a session
        // - check it did, and reject if not
        bool getSensorIdCalled;

        status = CryptoMgrGetSensorIdCalled(&getSensorIdCalled);
        if ((ERROR_SUCCESS == status) && !getSensorIdCalled)
        {
            DiagPrintf("SessionMgrOnSetPskSessionKeys: sensor ID not retrieved - rejecting\n");
            status = ERROR_AGAIN;
        }
    }

    if ((ERROR_SUCCESS == status) && !sessionMgrState.certificateState.certificatesIssued)
    {
        // Host must retrieve the sensor (and optionally the vendor) certificate before
        // attempting to start a session.
        DiagPrintf("SessionMgrOnSetPskSessionKeys: certificates not retrieved - rejecting\n");
        status = ERROR_AGAIN;
    }

    if ((ERROR_SUCCESS == status) && sessionMgrState.sessionState.sessionInitiated)
    {
        DiagPrintf("SessionMgrOnSetPskSessionKeys: rejecting second initiation attempt\n");
        status = ERROR_ACCESS;
    }

    if (ERROR_SUCCESS == status)
    {
        SessionMgrSetSessionKeysRequestT* request = (SessionMgrSetSessionKeysRequestT*)params;
        SessionMgrSetSessionKeysResponseT* response = (SessionMgrSetSessionKeysResponseT*)params;

        status = SessionMgrInitiateRsaSession(currentLcs, request, response);
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_SESSION_KEY_DONE);

    return status;
}

/// Handles the SetVideoAuthROI host command
///
/// \param[in]  commandCode Command to handle (SET_VIDEO_AUTH_ROI)
/// \param[in]  params      Pointer to the command params (in shared memory)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Command was successful, video authentication is active
/// \retval ERROR_SYSERR    Bad command or parameter
/// \retval ERROR_BADMSG    Failed to decrypt/authenticate command parameters
/// \retval ERROR_INVAL     Invalid command parameters
/// \retval ERROR_NOEXEC    Attempting to reconfigure in GMAC mode
///
ErrorT SessionMgrOnSetVideoAuthROI(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    SessionMgrSetVideoAuthRoiRequestT* request = (SessionMgrSetVideoAuthRoiRequestT*)params;
    LifecycleMgrLifecycleStateT currentLcs = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    SessionMgrVideoAuthConfigParamsT* decryptedParams = NULL;  // local buffer for decrypted params
    ErrorT status = ERROR_SUCCESS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI);

    // Defensive checks...
    if (HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_COMMAND,
            (uint32_t)commandCode);
        status = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_PARAMS,
            0U);
        status = ERROR_SYSERR;
    }
    else
    {
        status = LifecycleMgrGetCurrentLcs(&currentLcs);
        if (ERROR_SUCCESS == status)
        {
            // Configuration params are encrypted - decrypt and authenticate
            decryptedParams = (SessionMgrVideoAuthConfigParamsT*)
                              calloc(sizeof(SessionMgrVideoAuthConfigParamsT), sizeof(uint8_t));

            if (NULL == decryptedParams)
            {
                DiagPrintf("SessionMgrOnSetVideoAuthRoi: failed to alloc decryption buffer\n");
                status = ERROR_NOSPC;
            }
            else
            {
                status = SessionMgrDecryptCommandRequestParams(
                        commandCode,
                    (uint8_t*)request,
                    (uint8_t*)decryptedParams,
                        sizeof(SessionMgrVideoAuthConfigParamsT));
            }
        }
    }

    if (ERROR_SUCCESS == status)
    {
        status = SessionMgrConfigVideoAuth(decryptedParams);
    }

    if ((ERROR_SUCCESS != status) &&
        (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == currentLcs))
    {
        // Force the command to fail with BADMSG to prevent information leaks
        status = ERROR_BADMSG;
    }

    if (NULL != decryptedParams)
    {
        // Zero-out the decrypted params before freeing to prevent information leaks
        (void)memset((void*)decryptedParams, 0, sizeof(SessionMgrVideoAuthConfigParamsT));
        free(decryptedParams);
    }

    DiagSetCheckpoint(DIAG_CHECKPOINT_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE);

    return status;
}

/// Decrypts a host message request parameters
///
/// \param[in] commandCode          The Host command code
/// \param[in] encryptedParams      Pointer to buffer containing encrypted configuration parameters
/// \param[out] decryptedParams     Pointer to buffer containing decrypted configuration parameters
/// \param[in] paramSizeBytes       Size in bytes of encrypted/decrypted params type (excluding mac size)
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Command params decrypted successfully
/// \retval ERROR_ACCESS    Session hasn't been initiated
/// \retval ERROR_INVAL     Invalid key type
/// \retval ERROR_BADMSG    Message is not authentic
/// \retval ERROR_RANGE     CipherMode is not supported
/// \retval ERROR_SYSERR    Bad command or system error
///
ErrorT SessionMgrDecryptCommandRequestParams(HostCommandCodeT const commandCode,
    uint8_t* const encryptedParams,
    uint8_t* const decryptedParams,
    uint32_t const paramSizeBytes)
{
    SessionMgrSessionStateT* sessionState = &sessionMgrState.sessionState;
    uint32_t keySizeBytes;
    uint16_t associatedData = (uint16_t)commandCode;
    uint32_t associatedDataSizeBytes = sizeof(associatedData);
    ErrorT status;

    // Must be in SESSION phase
    if (SYSTEM_MGR_PHASE_SESSION != SystemMgrGetCurrentPhase())
    {
        status = ERROR_ACCESS;
    }
    else
    {
        status = CryptoMgrGetSymmetricKeySizeBytes(sessionState->sessionParams.controlChannelKeyType, &keySizeBytes);
        if (ERROR_SUCCESS == status)
        {
            // Decrypt the session parameters
            status = CryptoMgrAeadDecrypt(sessionState->sessionParams.controlChannelCipherMode,
                    keySizeBytes,
                    sessionState->sessionKeys.controlChannelKey,
                    sessionState->sessionParams.controlChannelNonce,
                    associatedDataSizeBytes,
                (uint8_t*)&associatedData,
                    paramSizeBytes,
                    encryptedParams,
                    decryptedParams);
        }
    }

    return status;
}

/// @} endgroup sessionmgr

