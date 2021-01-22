//***********************************************************************************
/// \file
///
/// Session Manager internal implementation - certificate retrieval
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
//                                  Locals
//------------------------------------------------------------------------------

/// Retrieves the Sensor certificate from NVM
///
/// \param[in]  certificateAuthId           Identifies the certificate authority that generated the certificate
/// \param[in]  worseCaseCertSizeLongWords  Worse-case size of a public certificate
/// \param[out] sensorCertSizeWords         Returned size of the sensor certificate (plus header)
/// \param[out] responseBuffer              Buffer to contain retrieved certificate
///
/// \return Error status
/// \retval ERROR_SUCCESS   Sensor certificate retrieved and stored in shared-memory
/// \retval ERROR_NOSPC     Failed to allocate buffer for certificate
/// \retval ERROR_NOENT     Failed to locate sensor certificate in NVM
///
ErrorT SessionMgrRetrieveSensorCertificate(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint32_t const worseCaseCertSizeLongWords,
    uint32_t* const sensorCertSizeWords,
    uint16_t* const responseBuffer)
{
    AssetMgrRsaPubCertT* sensorCert = NULL;
    ErrorT status;

    // Allocate a buffer to contain the worse-case certificate size
    sensorCert = (AssetMgrRsaPubCertT*)malloc(worseCaseCertSizeLongWords * sizeof(uint32_t));
    if (NULL == sensorCert)
    {
        DiagPrintf("SessionMgrRetrieveSensorCertificate: failed to alloc sensorCert buffer\n");
        status = ERROR_NOSPC;
    }
    else
    {
        uint32_t certSizeLongWords = worseCaseCertSizeLongWords;

        status = AssetMgrRetrieveCertificate(certificateAuthId,
                ASSET_MGR_CERTIFICATE_PURPOSE_SENSOR,
                sensorCert,
                &certSizeLongWords);

        if (ERROR_SUCCESS == status)
        {
            uint8_t* publicKey;

            /// Session Manager provides the SessionMgrGetCertificatesResponse structure, but it is not
            /// used here as it only represents a partial response. This is because the sensor certificate
            /// size is unknown, so the full response object cannot be represented as a structure. Instead
            /// we use a uint16_t array mapped over command parameters.

            // Copy the retrieved certificate into the response buffer
            // - responseBuffer[0] = uint16_t sensorCertificateSizeWords
            responseBuffer[0] = (uint16_t)(certSizeLongWords * sizeof(uint16_t));

            // - responseBuffer[1..] = uint16_t sensorCertificate
            (void)memcpy((void*)&responseBuffer[1],
                (void*)sensorCert,
                certSizeLongWords * sizeof(uint32_t));

            // Indicate how many words of the response buffer have been written, so that the
            // Vendor certificate, if present, can be written
            *sensorCertSizeWords = (certSizeLongWords * sizeof(uint16_t)) + 1U;

            // Free the certificate buffer
            // - we do this to prevent the subsequent public key buffer fragmenting the heap
            free(sensorCert);
            sensorCert = NULL;

            // Retrieve a pointer to the public key (modulus) from the certificate, and its size
            status = AssetMgrGetPublicKeyReference((AssetMgrRsaPubCertT*)&responseBuffer[1],
                    &publicKey,
                    &sessionMgrState.certificateState.publicKeySizeBytes);

            if (ERROR_SUCCESS != status)
            {
                DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SESSION_MGR_NO_PUBLIC_KEY, (uint16_t)status);
                DiagPrintf("SessionMgrRetrieveSensorCertificate: failed to retrieve public key (%d)\n",
                    status);
            }
            else
            {
                // Allocate a buffer to contain the public key
                sessionMgrState.certificateState.publicKey = (uint8_t*)malloc(
                        sessionMgrState.certificateState.publicKeySizeBytes);
                if (NULL == sessionMgrState.certificateState.publicKey)
                {
                    DiagPrintf("SessionMgrRetrieveSensorCertificate: failed to alloc buffer for public key\n");
                    status = ERROR_NOSPC;
                }
                else
                {
                    // Copy the public key into the allocated buffer
                    (void)memcpy(sessionMgrState.certificateState.publicKey,
                        publicKey,
                        sessionMgrState.certificateState.publicKeySizeBytes);
                }
            }
        }
        else
        {
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SESSION_MGR_NO_SENSOR_CERTIFICATE, (uint16_t)status);
            DiagPrintf("SessionMgrRetrieveSensorCertificate: failed to retrieve Sensor certificate (%d)\n",
                (uint32_t)certificateAuthId);
        }
    }

    if (NULL != sensorCert)
    {
        free(sensorCert);
    }

    return status;
}

/// Retrieves the Vendor certificate from NVM
///
/// \param[in]  certificateAuthId           Identifies the certificate authority that generated the certificate
/// \param[in]  worseCaseCertSizeLongWords  Worse-case size of a public certificate
/// \param[in]  sensorCertSizeWords         Size of the sensor certificate in shared-memory
/// \param[out] responseBuffer              Buffer to contain retrieved certificate
///
/// \return Error status
/// \retval ERROR_SUCCESS   Vendor certificate retrieved and stored in shared-memory
/// \retval ERROR_NOSPC     Failed to allocate buffer for certificate
/// \retval ERROR_NOENT     Failed to locate vendor certificate in NVM
///
ErrorT SessionMgrRetrieveVendorCertificate(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint32_t const worseCaseCertSizeLongWords,
    uint32_t const sensorCertSizeWords,
    uint16_t* const responseBuffer)
{
    AssetMgrRsaPubCertT* vendorCert = NULL;
    ErrorT status;

    // Allocate a buffer to contain the Vendor certificate
    vendorCert = (AssetMgrRsaPubCertT*)malloc(worseCaseCertSizeLongWords * sizeof(uint32_t));
    if (NULL == vendorCert)
    {
        DiagPrintf("SessionMgrRetrieveVendorCertificate: failed to alloc vendorCert buffer2\n");
        status = ERROR_NOSPC;
    }
    else
    {
        // Retrieve the Vendor certificate (if present)
        uint32_t certSizeLongWords = worseCaseCertSizeLongWords;

        status = AssetMgrRetrieveCertificate(certificateAuthId,
                ASSET_MGR_CERTIFICATE_PURPOSE_VENDOR,
                vendorCert,
                &certSizeLongWords);

        if (ERROR_SUCCESS == status)
        {
            // Copy the retrieved certificate into the response buffer
            responseBuffer[sensorCertSizeWords] = (uint16_t)(certSizeLongWords * sizeof(uint16_t));
            (void)memcpy(&responseBuffer[sensorCertSizeWords + 1U],
                vendorCert,
                certSizeLongWords * sizeof(uint32_t));
        }
        else
        {
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_SESSION_MGR_NO_VENDOR_CERTIFICATE, (uint16_t)status);
            DiagPrintf("SessionMgrRetrieveCertificates: failed to retrieve Vendor certificate (%d)\n",
                (uint32_t)certificateAuthId);
        }
    }

    if (NULL != vendorCert)
    {
        free(vendorCert);
    }

    return status;
}

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Retrieves the Sensor and (optional) Vendor certificate from Asset Manager
///
/// Session Manager provides the SessionMgrGetCertificatesResponse structure, but it is not
/// used here as it only represents a partial response. This is because the sensor certificate
/// size is unknown, so the full response object cannot be represented as a structure. Instead
/// we use a uint16_t array mapped over command parameters.
///
/// \param[in]  certificateAuthId   Identifies the certificate authority that generated the certificate
/// \param[out] responseBuffer      Buffer to contain retrieved certificates
///
/// \return Error status
/// \retval ERROR_SUCCESS   Sensor and (optional) Vendor certificate retrieved and stored in shared-memory
/// \retval ERROR_NOSPC     Failed to allocate buffer for certificate
/// \retval ERROR_NOENT     Failed to locate sensor certificate in NVM
///
ErrorT SessionMgrRetrieveCertificates(AssetMgrCertificateAuthIdT const certificateAuthId,
    uint16_t* const responseBuffer)
{
    // Asset Manager guarantees that all certificates are a long-word multiple
    uint32_t const worseCaseCertSizeLongWords = sizeof(AssetMgrRsa4096PubCertT) / sizeof(uint32_t);
    uint32_t sensorCertSizeWords = 0U;
    ErrorT status;

    // Attempt to retrieve the sensor certificate
    status = SessionMgrRetrieveSensorCertificate(certificateAuthId,
            worseCaseCertSizeLongWords, &sensorCertSizeWords, responseBuffer);

    if (ERROR_SUCCESS == status)
    {
        // Attempt to retrieve the (optional) vendor certificate
        status = SessionMgrRetrieveVendorCertificate(certificateAuthId,
                worseCaseCertSizeLongWords, sensorCertSizeWords, responseBuffer);

        if (ERROR_NOENT == status)
        {
            // Vendor certificate not found - this is permitted so suppress the error
            responseBuffer[sensorCertSizeWords] = 0U;
            status = ERROR_SUCCESS;
        }
    }

    return status;
}

/// @} endgroup sessionmgr

