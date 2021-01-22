//***********************************************************************************
/// \file
///
/// Asset Manager internal implementation
///
/// \addtogroup assetmgr
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

#include <stdlib.h>
#include <stddef.h>

#include "AR0820.h"
#include "Common.h"
#include "AssetMgr.h"
#include "AssetMgrInternal.h"

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

/// Retrieves the certificate purpose identifier (from the given certificate)
///
/// \param[in] certificateFlags          The certificate flags from a public certificate
///
/// \returns Certificate purpose identifier
///
AssetMgrCertificatePurposeT AssetMgrGetCertificatePurpose(uint32_t certificateFlags)
{
    uint32_t result = ((certificateFlags & ASSET_MGR_CERT_FLAGS_PURPOSE__MASK) >>
                       ASSET_MGR_CERT_FLAGS_PURPOSE__OFFSET);

    return (AssetMgrCertificatePurposeT)result;
}

/// Retrieves the certificate authority identifier (from the given certificate)
///
/// \param[in] certificateFlags          The certificate flags from a public certificate
///
/// \returns Authority ID
///
AssetMgrCertificateAuthIdT AssetMgrGetCertificateAuthId(uint32_t certificateFlags)
{
    uint32_t result = ((certificateFlags & ASSET_MGR_CERT_FLAGS_AUTH_ID__MASK) >>
                       ASSET_MGR_CERT_FLAGS_AUTH_ID__OFFSET);

    return (AssetMgrCertificateAuthIdT)result;
}

/// Retrieves the certificate version (from the given certificate)
///
/// \param[in] certificateFlags          The certificate flags from a public certificate
///
/// \returns Certificate version
///
AssetMgrCertificateVersionT AssetMgrGetCertificateVersion(uint32_t certificateFlags)
{
    uint32_t result = ((certificateFlags & ASSET_MGR_CERT_FLAGS_VERSION__MASK) >>
                       ASSET_MGR_CERT_FLAGS_VERSION__OFFSET);

    return (AssetMgrCertificateVersionT)result;
}

/// Retrieves the certificate number (from the given certificate)
///
/// \param[in] certificateFlags          The certificate flags from a public certificate
///
/// \returns Certificat number
///
AssetMgrCertificateNumberT AssetMgrGetCertificateNumber(uint32_t certificateFlags)
{
    uint32_t result = ((certificateFlags & ASSET_MGR_CERT_FLAGS_NUMBER__MASK) >>
                       ASSET_MGR_CERT_FLAGS_NUMBER__OFFSET);

    return (AssetMgrCertificateNumberT)result;
}

/// Get the flags fields from a Certificate in NVM
///
/// \param[in] assetAddress   Address of asset in NVM
/// \param[out] certAuthId    Certificate Authority
/// \param[out] certPurpose   Certificate Purpose
/// \param[out] certVersion   Certificate Version
/// \param[out] certNumber    Certificate Number
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset read successfully into buffer
/// \retval ERROR_RANGE         Wrong type asset in NVM
/// \retval ERROR_IO            IO error accessing NVM
///
ErrorT AssetMgrGetCertFlagsFromPubCert(NvmMgrAddressT const assetAddress,
    AssetMgrCertificateAuthIdT* certAuthId,
    AssetMgrCertificatePurposeT* certPurpose,
    AssetMgrCertificateVersionT* certVersion,
    AssetMgrCertificateNumberT* certNumber)
{
    ErrorT status;
    AssetMgrRsaPubCertHeaderT header;
    uint32_t headerSizeLongWords = sizeof(AssetMgrRsaPubCertHeaderT) / sizeof(uint32_t);

    status = NvmMgrReadAsset(assetAddress, (uint32_t*)&header, 0U, headerSizeLongWords);

    if (ERROR_SUCCESS == status)
    {
        AssetMgrCertificateTypeT typeIdentifier = (AssetMgrCertificateTypeT)header.typeIdentifier;
        if ((typeIdentifier != ASSET_MGR_CERTIFICATE_TYPE_2048_BIT) &&
            (typeIdentifier != ASSET_MGR_CERTIFICATE_TYPE_3072_BIT) &&
            (typeIdentifier != ASSET_MGR_CERTIFICATE_TYPE_4096_BIT))
        {
            // Certificate type not supported in OTPM
            status = ERROR_RANGE;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        *certPurpose = AssetMgrGetCertificatePurpose(header.certificateFlags);
        *certAuthId = AssetMgrGetCertificateAuthId(header.certificateFlags);
        *certVersion = AssetMgrGetCertificateVersion(header.certificateFlags);
        *certNumber = AssetMgrGetCertificateNumber(header.certificateFlags);
    }

    return status;
}

/// Get the Certificate Authority ID from a OTPM private key
///
/// \param[in] assetAddress   Address of asset in NVM
/// \param[out] certAuthId    Certificate Authority
///
/// \returns Status of the request
/// \retval ERROR_SUCCESS       Asset read successfully into buffer
/// \retval ERROR_IO            IO error accessing NVM
///
ErrorT AssetMgrGetCertAuthIdFromPrivateKey(NvmMgrAddressT const assetAddress,
    AssetMgrCertificateAuthIdT* certAuthId)
{
    AssetMgrRsaPrivKeyHeaderT header;
    ErrorT status;

    status =
        NvmMgrReadAsset(assetAddress, (uint32_t*)&header, 0U, sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t));

    if (ERROR_SUCCESS == status)
    {
        *certAuthId = (AssetMgrCertificateAuthIdT)header.certificateAuthId;
    }

    return status;
}

/// Retrieves the 'latest' Asset from NVM
///
/// \param[in] assetType                 The asset type
/// \param[in] buffer                    Buffer to contain the retrieved asset
/// \param[in,out] bufferSizeLongWords   Size of the buffer, on return size of the retrieved asset
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Asset retrieved and present in NVM
/// \retval ERROR_NOENT         Failed to locate Asset in NVM
/// \retval ERROR_RANGE         The buffer size is too small
/// \retval ERROR_IO            IO error when accessing NVM
///
ErrorT AssetMgrRetrieveLatestAsset(AssetMgrAssetTypeT const assetType,
    uint32_t* const buffer,
    uint32_t* const bufferSizeLongWords)
{
    ErrorT status = ERROR_NOENT;
    NvmMgrFindContextT context;
    NvmMgrAddressT assetAddressFind;
    uint32_t assetLengthLongWordsFind;
    NvmMgrAddressT assetAddress = 0U;
    uint32_t assetLengthLongWords = 0U;
    uint32_t counterLocal = 0U;

    status = NvmMgrFindFirst(&context, assetType, &assetAddressFind, &assetLengthLongWordsFind);

    while (ERROR_SUCCESS == status)
    {
        assetAddress = assetAddressFind;
        assetLengthLongWords = assetLengthLongWordsFind;

        counterLocal += 1U;
        status = NvmMgrFindNext(&context, &assetAddressFind, &assetLengthLongWordsFind);
    }

    // Check we got to the end correctly and at least one was found)
    if ((0U != counterLocal) && (ERROR_NOENT == status))
    {
        status = ERROR_SUCCESS;
    }

    if (ERROR_SUCCESS == status)
    {
        if (assetLengthLongWords <= *bufferSizeLongWords)
        {
            status = NvmMgrReadAsset(assetAddress, buffer, 0U, assetLengthLongWords);
            *bufferSizeLongWords = assetLengthLongWords;
        }
        else
        {
            status = ERROR_RANGE;
        }
    }

    return status;
}

/// Compare Public Certificate versions in OTPM
///
/// This function checks, given public certificate, that the version if newer compared to
/// the certificates stored in OTPM. The version is only tested if the certificates have the
/// same purpose, certificate authority and number. If a newer certificate is found in OTPM
/// the function aborts the search and returns the error code.
///
/// \param[in]  pubCert            Public Certificate
/// \param[in]  maxAllowedCount    Maximum number of assets in OTPM of the requested type.
/// \returns Error status
/// \retval ERROR_SUCCESS       All checks passed
/// \retval ERROR_ALREADY       Certificate version is older than one in OTPM
/// \retval ERROR_SYSERR        Failures from OTPM
///
ErrorT AssetMgrCompareVersionWithOtpm(AssetMgrRsaPubCertT* const pubCert, uint32_t const maxAllowedCount)
{
    ErrorT status = ERROR_SUCCESS;
    NvmMgrFindContextT context;
    NvmMgrAddressT assetAddress = 0U;
    uint32_t assetLengthLongWords = 0U;
    AssetMgrCertificatePurposeT certPurpose = AssetMgrGetCertificatePurpose(pubCert->header.certificateFlags);
    AssetMgrCertificateAuthIdT certAuthId = AssetMgrGetCertificateAuthId(pubCert->header.certificateFlags);
    AssetMgrCertificateVersionT certVersion = AssetMgrGetCertificateVersion(pubCert->header.certificateFlags);
    AssetMgrCertificateNumberT certNumber = AssetMgrGetCertificateNumber(pubCert->header.certificateFlags);
    uint32_t assetCount = 0U;

    status = NvmMgrFindFirst(&context,
            ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE,
            &assetAddress,
            &assetLengthLongWords);

    while (ERROR_SUCCESS == status)
    {
        assetCount += 1U;
        if (maxAllowedCount <= assetCount)
        {
            status = ERROR_NOSPC;
        }
        else
        {
            AssetMgrCertificatePurposeT certPurposeRead;
            AssetMgrCertificateAuthIdT certAuthIdRead;
            AssetMgrCertificateVersionT certVersionRead;
            AssetMgrCertificateNumberT certNumberRead;

            status = AssetMgrGetCertFlagsFromPubCert(assetAddress,
                    &certAuthIdRead,
                    &certPurposeRead,
                    &certVersionRead,
                    &certNumberRead);

            if ((ERROR_SUCCESS == status) &&
                (certPurposeRead == certPurpose) &&
                (certAuthIdRead == certAuthId) &&
                (certNumberRead == certNumber) &&
                (certVersionRead >= certVersion))
            {
                status = ERROR_ALREADY;
            }
            if (ERROR_SUCCESS == status)
            {
                status = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
            }
        }
    }

    if (ERROR_NOENT == status)
    {
        // This function checks that the new certificate has the right version compared
        // to the ones in OTPM. If no certificates are present the version is correct automatically.
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Compare Asset with OTPM contents
///
/// This function checks, given an asset, that the contents are not stored in OTPM already
/// If an identical asset is found in OTPM the function aborts the search and returns the error code.
///
/// This function also checks that the amount of assets of the same type searched during the comparison
/// doesn't reach the maximum given by maxAllowedCount
///
/// \param[in]  asset            Asset contents
/// \param[in]  maxAllowedCount  Maximum number of assets in OTPM of the requested type.
///
/// \returns Error status
/// \retval ERROR_SUCCESS       All checks passed
/// \retval ERROR_ALREADY       Asset already exists in OTPM
/// \retval ERROR_NOSPC         No more assets allowed in OTPM of this type
/// \retval ERROR_IO            IO error when accessing NVM
///
ErrorT AssetMgrCompareAssetWithOtpm(AssetMgrAssetT* const asset, uint32_t const maxAllowedCount)
{
    ErrorT status = ERROR_SUCCESS;
    AssetMgrAssetTypeT assetType = (AssetMgrAssetTypeT)asset->header.assetType;
    uint32_t assetLengthLongWordsHeader = (uint32_t)asset->header.assetLengthLongWords;
    NvmMgrFindContextT context;
    NvmMgrAddressT assetAddress = 0;
    uint32_t assetLengthLongWords = 0;
    uint32_t assetCount = 0;

    status = NvmMgrFindFirst(&context, assetType, &assetAddress, &assetLengthLongWords);

    while (ERROR_SUCCESS == status)
    {
        assetCount += 1U;
        if (maxAllowedCount <= assetCount)
        {
            status = ERROR_NOSPC;
        }
        else
        {
            bool differenceFound = (assetLengthLongWordsHeader != assetLengthLongWords);
            for (uint32_t index = 0U;
                 ((index < assetLengthLongWords) && (!differenceFound) && (ERROR_SUCCESS == status)); index++)
            {
                uint32_t buffer;
                status = NvmMgrReadAsset(assetAddress + index, &buffer, 0U, sizeof(buffer) / sizeof(uint32_t));
                if ((ERROR_SUCCESS == status) && (buffer != asset->payload[index]))
                {
                    differenceFound = true;
                }
            }
            if ((ERROR_SUCCESS == status) && (!differenceFound))
            {
                status = ERROR_ALREADY;
            }
        }
        if (ERROR_SUCCESS == status)
        {
            status = NvmMgrFindNext(&context, &assetAddress, &assetLengthLongWords);
        }
    }

    if (ERROR_NOENT == status)
    {
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Get public key size in bytes from public certificate
///
/// This function returns the key size of a given public certificate
///
/// \param[in]  pubCert     Public Certificate
/// \param[out] keySize     RSA Key Size (number of bytes in the RSA modulus)
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Get Key size succesful
/// \retval ERROR_RANGE         Certificate size incorrect
/// \retval ERROR_BADMSG        Certificate type incorrect
///
ErrorT AssetMgrGetPublicKeySize(AssetMgrRsaPubCertT* const pubCert, uint32_t* keySize)
{
    ErrorT status = ERROR_SUCCESS;
    uint32_t keySizeLocal = (uint32_t)(pubCert->header.signatureOffsetLongWords * sizeof(uint32_t)) -
                            (uint32_t)sizeof(AssetMgrRsaPubCertHeaderT);
    uint32_t keySizeExpected = 0;

    AssetMgrCertificateTypeT typeIdentifier = (AssetMgrCertificateTypeT)pubCert->header.typeIdentifier;

    switch (typeIdentifier)
    {
        case ASSET_MGR_CERTIFICATE_TYPE_2048_BIT:
            keySizeExpected = ASSET_MGR_RSA_2048_PUBLIC_KEY_SIZE_BYTES;
            break;

        case ASSET_MGR_CERTIFICATE_TYPE_3072_BIT:
            keySizeExpected = ASSET_MGR_RSA_3072_PUBLIC_KEY_SIZE_BYTES;
            break;

        case ASSET_MGR_CERTIFICATE_TYPE_4096_BIT:
            keySizeExpected = ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES;
            break;

        case ASSET_MGR_CERTIFICATE_TYPE_RESERVED:
        default:
            status = ERROR_BADMSG;
            break;
    }
    if ((ERROR_SUCCESS == status) && (keySizeExpected != keySizeLocal))
    {
        status = ERROR_RANGE;
    }
    if (ERROR_SUCCESS == status)
    {
        *keySize = keySizeLocal;
    }

    return status;
}

/// Get private key size in bytes from asset
///
/// This function returns the key size of a given private key asset
///
/// \param[in]  privateKeyAsset  Private Key asset
/// \param[out] keySize          RSA Key Size (number of bytes in the RSA exponent)
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Get Key size succesful
/// \retval ERROR_RANGE         Certificate data incorrect
///
ErrorT AssetMgrGetPrivateKeySize(AssetMgrRsaPrivKeyT* const privateKeyAsset, uint32_t* keySize)
{
    ErrorT status = ERROR_SUCCESS;

    AssetMgrSensorAuthPrivKeyIdT keyId = (AssetMgrSensorAuthPrivKeyIdT)(privateKeyAsset->header.authPrivKeyId);

    switch (keyId)
    {
        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_2048:
            *keySize = ASSET_MGR_RSA_2048_PRIVATE_EXPONENT_SIZE_BYTES;
            break;

        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_3072:
            *keySize = ASSET_MGR_RSA_3072_PRIVATE_EXPONENT_SIZE_BYTES;
            break;

        case ASSET_MGR_SENSOR_AUTH_PRIV_KEY_ID_RSA_BASIC_4096:
            *keySize = ASSET_MGR_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES;
            break;

        default:
            status = ERROR_RANGE;
            break;
    }

    return status;
}

/// Checks if the current LCS supports provisioning
///
/// \param[in] rootOfTrust  The root-of-trust key to use for provisioning
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Provisioning is permitted in the current LCS with the specified root-of-trust
/// \retval ERROR_ACCESS    Provisioning is not permitted
///
ErrorT AssetMgrCheckCurrentLcsForProvisioning(CryptoMgrRootOfTrustT const rootOfTrust)
{
    LifecycleMgrLifecycleStateT lcsCurrent;
    ErrorT status;

    status = LifecycleMgrGetCurrentLcs(&lcsCurrent);
    if (ERROR_SUCCESS == status)
    {
        if (LIFECYCLE_MGR_LIFECYCLE_STATE_DM == lcsCurrent)
        {
            if (CRYPTO_MGR_ROOT_OF_TRUST_DM == rootOfTrust)
            {
                // Not permitted to use the DM root-of-trust as it doesn't exist in this LCS
                status = ERROR_ACCESS;
            }
            else
            {
                // Permitted to use the CM root-of-trust in this LCS
            }
        }
        else if (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == lcsCurrent)
        {
            // Permitted to use both CM and DM root-of-trusts in this LCS
        }
        else
        {
            // Cannot provision assets in CM or RMA lifecycle states
            status = ERROR_ACCESS;
        }
    }

    return status;
}

/// @} endgroup assetmgr

