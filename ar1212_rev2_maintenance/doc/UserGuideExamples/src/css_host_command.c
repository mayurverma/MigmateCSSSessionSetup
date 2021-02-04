//***********************************************************************************
/// \file
///
/// CSS Host Command function definitions
///
//***********************************************************************************
// Copyright 2019 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//***********************************************************************************

#include "i2c_driver.h"
#include "css_shared_mem.h"
#include "css_host_command.h"

/// Local timeout (poll count)
static uint32_t cssHostCommandTimeout = 100U;

/// Sets the timeout (poll loop count) for host commands
///
/// \param[in]  pollCount   number of polling loops before timeout
///
/// returns void
///
void CssSetHostCommandTimeout(uint32_t const pollCount)
{
    cssHostCommandTimeout = pollCount;
}

/// Issues the specified host command and waits for a response
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[in] commandCode  Host command code to issue
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout
/// \retval Other           Command failed
///
CssErrorT CssIssueHostCommand(CssHostCommandCodeT const commandCode)
{
    int pollCount = cssHostCommandTimeout;
    CssErrorT responseCode = ERROR_TIMEDOUT;

    I2cWriteReg16(CSS_COMMAND, commandCode);

    while (0 != pollCount)
    {
        uint16_t result = I2cReadReg16(CSS_COMMAND);

        if (0 == (result & CSS_COMMAND_DOORBELL__MASK))
        {
            // Workaround for IMAGINGPRJ-3583, re-read to retrieve the
            // command result code
            result = I2cReadReg16(CSS_COMMAND);
            responseCode = result & CSS_COMMAND_RESULT__MASK;
            break;
        }

        pollCount -= 1;
    }

    return responseCode;
}

/// Issues the GetStatus host command and returns the response parameters
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[out] response    Buffer to contain GetStatus response parameters
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssGetStatus(CssGetStatusResponseT* const response)
{
    CssErrorT responseCode = CssIssueHostCommand(HOST_COMMAND_CODE_GET_STATUS);
    if (ERROR_SUCCESS == responseCode)
    {
        response->currentPhase = (CssSystemPhaseT)CssReadSharedMemUint16(0U);
        response->currentLcs = (CssSystemLcsT)CssReadSharedMemUint16(1U);
        response->romVersion = CssReadSharedMemUint16(2U);
        response->patchVersion = CssReadSharedMemUint16(3U);
        response->reserved = CssReadSharedMemUint16(4U);
    }

    return responseCode;
}

/// Issues the GetSensorId host command and returns the sensor's unique identifier
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[out] sensorId    Buffer to contain sensor's unique identifier
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_IO        Failed to retrieve unique identifier
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssGetSensorId(CssSensorIdT sensorId)
{
    CssErrorT responseCode = CssIssueHostCommand(HOST_COMMAND_CODE_GET_SENSOR_ID);
    if (ERROR_SUCCESS == responseCode)
    {
        CssReadSharedMemMultiByte(0U, sensorId, CSS_SENSOR_ID_SIZE_BYTES / 2);
    }

    return responseCode;
}

/// Issues the ApplyDebugEntitlement host command
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[in] clockSpeedHz             CSS clock speed in Hertz
/// \param[in] entitlementBlob          Pointer to buffer containing the debug entitlement BLOB
/// \param[in] entitlementBlobSizeWords Size of the BLOB in (16-bit) words
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully and entitlement granted
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_BADMSG    BLOB failed authentication, or re-issued after previous rejection
/// \retval ERROR_IO        Failed to grant entitlement
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssApplyDebugEntitlement( uint32_t const clockSpeedHz,
                                    uint8_t* const entitlementBlob,
                                    uint32_t const entitlementBlobSizeWords)
{
    CssWriteSharedMemUint32(0U, clockSpeedHz);
    CssWriteSharedMemUint32(2U, entitlementBlobSizeWords);
    CssWriteSharedMemMultiByte(4U, entitlementBlob, entitlementBlobSizeWords);
    return CssIssueHostCommand(HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT);
}

/// Issues the LoadPatchChunk host command
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[in] patchChunk           Pointer to buffer containing the patch chunk
/// \param[in] patchChunkSizeWords  Size of the patch chunk in (16-bit) words
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully and chunk loaded
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_INVAL     Invalid chunk identifier
/// \retval ERROR_RANGE     Invalid chunk parameters
/// \retval ERROR_NOENT     Last chunk has invalid loader address
/// \retval ERROR_BADMSG    Chunk failed to authenticate
/// \retval ERROR_NOEXEC    Loader could not execute
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssLoadPatchChunk(uint8_t* const patchChunk, uint32_t const patchChunkSizeWords)
{
    CssWriteSharedMemMultiByte(0U, patchChunk, patchChunkSizeWords);
    return CssIssueHostCommand(HOST_COMMAND_CODE_LOAD_PATCH_CHUNK);
}

/// Issues the GetCertificates host command and returns the retrieved certificates
///
/// Returns ERROR_TIMEOUT if the host command does not complete within the
/// current poll count
///
/// \param[in] certificateAuthId    Selects the trust identity (certificate authority)
/// \param[out] sensorCert          Buffer to contain the retrieved SensorCert
/// \param[out] sensorCertSizeWords Size of the retrieved SensorCert in (16-bit) words
/// \param[out] vendorCert          Buffer to contain the retrieved VendorCert
/// \param[out] vendorCertSizeWords Size of the retrieved VendorCert in (16-bit) words
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully and certificates retrieved
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_INVAL     Invalid certificateAuthId parameter
/// \retval ERROR_NOENT     Certificates not found
/// \retval ERROR_IO        I/O error retrieving certificates
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssGetCertificates(   CssCertificateAuthIdT const certificateAuthId,
                                uint8_t* const sensorCert,
                                uint32_t* const sensorCertSizeWords,
                                uint8_t* const vendorCert,
                                uint32_t* const vendorCertSizeWords)
{
    CssWriteSharedMemUint16(0U, certificateAuthId);

    CssErrorT responseCode = CssIssueHostCommand(HOST_COMMAND_CODE_GET_CERTIFICATES);
    if (ERROR_SUCCESS == responseCode)
    {
        *sensorCertSizeWords = CssReadSharedMemUint16(0U);
        CssReadSharedMemMultiByte(1U,
                                  sensorCert,
                                  *sensorCertSizeWords);

        *vendorCertSizeWords = CssReadSharedMemUint16(1U + *sensorCertSizeWords);
        CssReadSharedMemMultiByte(2U + *sensorCertSizeWords,
                                  vendorCert,
                                  *vendorCertSizeWords);
    }

    return responseCode;
}

/// Issues the SetSessionKeys host command and returns the response parameters
///
/// \params[in] videoAuthCipherMode         cipher and mode to use for video authentication
/// \params[in] videoAuthKeyType            key size to use for video authentication
/// \params[in] controlChannelCipherMode    cipher and mode to use for the secure control channel
/// \params[in] controlChannelKeyType       key type to use for video authentication
/// \params[in] hostSalt                    random salt to the HKDF key derivation function
/// \params[in] sessionParamsCipher         cipher used to encrypt the session parameters
/// \params[in] encryptedParamsSizeWords    size of the encrypted parameters in (16-bit) words
/// \params[in] encryptedParams             encrypted session parameters plus padding
/// \params[out] sensorSalt                 random salt to the HKDF key derivation function
/// \params[out] videoAuthGmacIv            IV for video authentication
/// \params[out] controlChannelNonce        IV / Nonce for control channel encryption
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_AGAIN     Issued prior to successful GetCertificates and GetSensorId
/// \retval ERROR_INVAL     SessionParamsCipher is invalid or unsupported
/// \retval ERROR_BADMSG    Failed to authenticate command parameters
/// \retval ERROR_RANGE     Invalid or unsupported parameter
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssSetSessionKeys(CssSymmetricAuthCipherModeT const videoAuthCipherMode,
                            CssSymmetricKeyTypeT const videoAuthKeyType,
                            CssSymmetricAECipherModeT const controlChannelCipherMode,
                            CssSymmetricKeyTypeT const controlChannelKeyType,
                            uint8_t const hostSalt[CSS_SALT_SIZE_BYTES],
                            CssAsymmetricCipherT const sessionParamsCipher,
                            uint32_t const encryptedParamsSizeWords,
                            uint8_t* const encryptedParams,
                            uint8_t sensorSalt[CSS_SALT_SIZE_BYTES],
                            uint8_t videoAuthGmacIv[CSS_IV_SIZE_BYTES],
                            uint8_t controlChannelNonce[CSS_NONCE_SIZE_BYTES])
{
    CssWriteSharedMemUint16(0x00U, videoAuthCipherMode);
    CssWriteSharedMemUint16(0x01U, videoAuthKeyType);
    CssWriteSharedMemUint16(0x02U, controlChannelCipherMode);
    CssWriteSharedMemUint16(0x03U, controlChannelKeyType);
    CssWriteSharedMemMultiByte(0x04U, (uint8_t* const)hostSalt, CSS_SALT_SIZE_BYTES / 2U);
    CssWriteSharedMemUint16(0x0CU, sessionParamsCipher);
    CssWriteSharedMemUint16(0x0DU, sizeof(CssSessionParametersT) / 2U);
    CssWriteSharedMemMultiByte(0x0EU, encryptedParams, encryptedParamsSizeWords);

    CssErrorT responseCode = CssIssueHostCommand(HOST_COMMAND_CODE_SET_SESSION_KEYS);
    if (ERROR_SUCCESS == responseCode)
    {
        CssReadSharedMemMultiByte(0x00U, sensorSalt, CSS_SALT_SIZE_BYTES / 2U);
        CssReadSharedMemMultiByte(0x08U, videoAuthGmacIv, CSS_IV_SIZE_BYTES / 2U);
        CssReadSharedMemMultiByte(0x0EU, controlChannelNonce, CSS_NONCE_SIZE_BYTES / 2U);
    }

    return responseCode;
}

/// Issues the SetVideoAuthRoi host command
///
/// \param[in] encryptedParams  Buffer containing the encrypted CssVideoAuthConfigParamsT
/// \param[in] mac              Buffer containing the MAC for the encrypted params
///
/// \returns Response code
/// \retval ERROR_SUCCESS   Command completed successfully
/// \retval ERROR_ACCESS    Access not permitted
/// \retval ERROR_INVAL     Invalid video authentication mode or pixel packing mode
/// \retval ERROR_BADMSG    Failed to authenticate command parameters
/// \retval ERROR_NOEXEC    Attempt to reconfigure in GMAC video authentication cipher mode
/// \retval ERROR_SYSERR    Firmware detected a fatal error and has shutdown
/// \retval ERROR_TIMEDOUT  Command failed to complete before timeout poll loops
///
CssErrorT CssSetVideoAuthRoi(uint8_t* const encryptedParams, uint8_t* const mac)
{
    CssWriteSharedMemMultiByte(0x00U, encryptedParams, 16U);
    CssWriteSharedMemMultiByte(0x10U, mac, 8U);

    return CssIssueHostCommand(HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI);
}

