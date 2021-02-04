//***********************************************************************************
/// \file
///
/// Asset Manager implementation
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
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "CommandHandler.h"
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

/// Handles entry to an operating phase
/// \param[in] phase    Phase being entered
///
/// \retval ERROR_SUCCESS       Phase entry succesful
/// \retval ERROR_INVAL         Phase entry value is incorrect
///
ErrorT AssetMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    //nothing to do here. Left for potention patching
    (void)phase;

    return ERROR_SUCCESS;
}

/// Handles the ProvisionAsset host command
///
/// \param[in]  commandCode Command to handle (PROVISION_ASSET)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Error status
/// \retval ERROR_SUCCESS       Provision asset succesful
/// \retval ERROR_RANGE         Incorrect asset type
/// \retval ERROR_BADMSG        Invalid asset contents
/// \retval ERROR_INVAL         Invalid parameter
/// \retval ERROR_ACCESS        Permission denied due to current lifecycle state
///
ErrorT AssetMgrOnProvisionAsset(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params)
{
    ErrorT status = ERROR_SUCCESS;
    AssetMgrProvisionAssetRequestParamsT* commandParams = (AssetMgrProvisionAssetRequestParamsT*)params;
    AssetMgrAssetT* assetDecrypted = (AssetMgrAssetT*)commandParams->encAsset;
    CryptoMgrRootOfTrustT rootOfTrust = (CryptoMgrRootOfTrustT)commandParams->rootOfTrust;

    if ((HOST_COMMAND_CODE_PROVISION_ASSET != commandCode) ||
        (NULL == params))
    {
        status = ERROR_INVAL;
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_ON_PROVISION_ASSET_BAD_ARGS, 0U);
    }
    // First step is to check the lifecycle state and ensure the selected provisioning key is valid
    if (ERROR_SUCCESS == status)
    {
        status = AssetMgrCheckCurrentLcsForProvisioning(rootOfTrust);
    }
    // Next step is to authenticate and decrypt the asset
    if (ERROR_SUCCESS == status)
    {
        uint32_t* assetPackageBuffer = (uint32_t*)&commandParams->token;
        status = CryptoMgrAuthenticateAsset(commandParams->assetId, rootOfTrust, assetPackageBuffer);
    }
    // Next step check if decrypted asset size is not over the limits
    if (ERROR_SUCCESS == status)
    {
        uint32_t assetSizeBytes = assetDecrypted->header.assetLengthLongWords * sizeof(uint32_t);
        if ((assetSizeBytes + sizeof(AssetMgrAssetHeaderT)) > commandParams->assetSize)
        {
            status = ERROR_BADMSG;
        }
    }
    // Next step is to check if the asset can be written to OTPM
    if (ERROR_SUCCESS == status)
    {
        AssetMgrAssetTypeT assetType = (AssetMgrAssetTypeT)assetDecrypted->header.assetType;
        AssetMgrRsaPubCertT* pubCert = (AssetMgrRsaPubCertT*)assetDecrypted->payload;
        AssetMgrRsaPrivKeyT* privateKey = (AssetMgrRsaPrivKeyT*)assetDecrypted->payload;
        uint32_t keySize;

        switch (assetType)
        {
            case ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE:
                status = AssetMgrGetPublicKeySize(pubCert, &keySize);
                if (ERROR_SUCCESS == status)
                {
                    uint32_t maxAllowedCount = (ASSET_MGR_RSA_4096_PUBLIC_KEY_SIZE_BYTES == keySize) ?
                                               ASSET_MGR_MAX_PUB_CERT_4096_ASSETS_ALLOWED :
                                               ASSET_MGR_MAX_PUB_CERT_2048_3072_ASSETS_ALLOWED;
                    status = AssetMgrCompareVersionWithOtpm(pubCert, maxAllowedCount);
                }
                break;

            case ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY:
                status = AssetMgrGetPrivateKeySize(privateKey, &keySize);
                if (ERROR_SUCCESS == status)
                {
                    uint32_t maxAllowedCount = (ASSET_MGR_RSA_4096_PRIVATE_EXPONENT_SIZE_BYTES == keySize) ?
                                               ASSET_MGR_MAX_PRIVATE_RSA_4096_ASSETS_ALLOWED :
                                               ASSET_MGR_MAX_PRIVATE_RSA_2048_3072_ASSETS_ALLOWED;
                    status = AssetMgrCompareAssetWithOtpm(assetDecrypted, maxAllowedCount);
                }
                break;

            case ASSET_MGR_ASSET_TYPE_PSK_MASTER_SECRET:
            case ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION:
            case ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION:
                status = AssetMgrCompareAssetWithOtpm(assetDecrypted, ASSET_MGR_MAX_TRNG_OTPM_PSK_ASSETS_ALLOWED);
                break;

            default:
                status = ERROR_RANGE;
                break;
        }
    }
    // Next step is to set the system clock speed
    if (ERROR_SUCCESS == status)
    {
        status = SystemMgrSetClockSpeed(commandParams->clockSpeedHz);
    }
    // Next step is to write the asset in OTPM
    if (ERROR_SUCCESS == status)
    {
        AssetMgrAssetTypeT assetType = (AssetMgrAssetTypeT)assetDecrypted->header.assetType;
        uint32_t assetLengthLongWords = (uint32_t)assetDecrypted->header.assetLengthLongWords;
        status = NvmMgrWriteAsset(assetType, (uint32_t*)assetDecrypted->payload, assetLengthLongWords);
    }

    //clear the params before returning to the host
    (void)memset(params, 0, (size_t)CRYPTO_ICORE_SHARED_RAM_SIZE_BYTES);

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_ON_PROVISION_ASSET, (uint16_t)status);

    return status;
}

/// Get pointer to the public key (modulus) within the specified certificate, and its size
///
/// \param[in]      pubCert             The public certificate retrieved using AssetMgrRetrieveCertificate
/// \param[out]     pubKey              Pointer to the contained public key
/// \param[out]     pubKeySizeBytes     Returned size of the public key in bytes
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Public key located and pointers set
/// \retval ERROR_RANGE                 certSizeLongWords is incorrect
/// \retval ERROR_INVAL                 Invalid parameter
///
ErrorT AssetMgrGetPublicKeyReference(AssetMgrRsaPubCertT* const pubCert,
    uint8_t** pubKey,
    uint32_t* const pubKeySizeBytes)
{
    ErrorT status = ERROR_SUCCESS;

    if ((NULL == pubKey) ||
        (NULL == pubCert) ||
        (NULL == pubKeySizeBytes))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_GET_PUBLIC_KEY_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = AssetMgrGetPublicKeySize(pubCert, pubKeySizeBytes);
    }
    if (ERROR_SUCCESS == status)
    {
        *pubKey = pubCert->publicKey;
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_GET_PUBLIC_KEY, (uint16_t)status);

    return status;
}

/// Retrieves the 'latest' public certificate issued by the specified authority from NVM
///
/// The pubCert parameter points to a buffer able to store the worse-case (4096-bit)
/// certificate. On successful return the certSizeLongWords parameter indicates the size
/// of the certificate retrieved from NVM.
///
/// \param[in]      certAuthId          The issuing certificate authority
/// \param[in]      certPurpose         Identifies the purpose of the certificate to retrieve (vendor/sensor)
/// \param[out]     pubCert             Buffer to contain the retrieved certificate
/// \param[in, out] certSizeLongWords   Size of the buffer, on return size of the retrieved certificate
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Certificate retrieved and present in supplied buffer
/// \retval ERROR_NOENT                 Failed to locate specified certificate in NVM
/// \retval ERROR_IO                    IO error when accessing NVM
/// \retval ERROR_RANGE                 Output buffer too small
/// \retval ERROR_INVAL                 Invalid parameter
///
ErrorT AssetMgrRetrieveCertificate(AssetMgrCertificateAuthIdT const certAuthId,
    AssetMgrCertificatePurposeT const certPurpose,
    AssetMgrRsaPubCertT* const pubCert,
    uint32_t* const certSizeLongWords)
{
    ErrorT status = ERROR_SUCCESS;
    bool assetFound = false;
    NvmMgrFindContextT context;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_RSA_PUBLIC_CERTIFICATE;
    NvmMgrAddressT assetAddressFind;
    uint32_t assetLengthLongWordsFind;
    NvmMgrAddressT assetAddress = 0;
    uint32_t assetLengthLongWords = 0;
    AssetMgrCertificatePurposeT certPurposeRead;
    AssetMgrCertificateAuthIdT certAuthIdRead;
    AssetMgrCertificateVersionT certVersion;
    AssetMgrCertificateNumberT certNumber;

    if ((NULL == certSizeLongWords) ||
        (NULL == pubCert))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_CERTIFICATE_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = NvmMgrFindFirst(&context, assetType, &assetAddressFind, &assetLengthLongWordsFind);
    }
    while (ERROR_SUCCESS == status)
    {
        status = AssetMgrGetCertFlagsFromPubCert(assetAddressFind, &certAuthIdRead, &certPurposeRead, &certVersion,
                &certNumber);
        if (ERROR_SUCCESS == status)
        {
            if ((certAuthIdRead == certAuthId) && (certPurposeRead == certPurpose))
            {
                assetAddress = assetAddressFind;
                assetLengthLongWords = assetLengthLongWordsFind;
                assetFound = true;
            }
            status = NvmMgrFindNext(&context, &assetAddressFind, &assetLengthLongWordsFind);
        }
    }

    // Check we got to the end correctly and at least one was found)
    if ((assetFound) && (ERROR_NOENT == status))
    {
        status = ERROR_SUCCESS;
    }

    if (ERROR_SUCCESS == status)
    {
        if (assetLengthLongWords <= *certSizeLongWords)
        {
            status = NvmMgrReadAsset(assetAddress, (uint32_t* const)pubCert, 0U, assetLengthLongWords);
            *certSizeLongWords = assetLengthLongWords;
        }
        else
        {
            status = ERROR_RANGE;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CERTIFICATE, (uint16_t)status);

    return status;
}

/// Retrieves the 'latest' private key issued by the specified authority from NVM
///
/// The privKey parameter points to a buffer able to store the worse-case (4096-bit)
/// key. On successful return the privKeySizeLongWords parameter indicates the size
/// of the key retrieved from NVM.
///
/// \param[in]      certAuthId              The issuing certificate authority
/// \param[out]     privKey                 Buffer to contain the retrieved key (exponent)
/// \param[in, out] privKeySizeLongWords    Size of the buffer, on return size of the retrieved key
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Private Key retrieved and present in supplied buffer
/// \retval ERROR_NOENT                 Failed to locate specified key in NVM
/// \retval ERROR_IO                    IO error when accessing NVM
/// \retval ERROR_INVAL                 Invalid parameter
///
ErrorT AssetMgrRetrievePrivateKey(AssetMgrCertificateAuthIdT const certAuthId,
    uint8_t* const privKey,
    uint32_t* const privKeySizeLongWords)
{
    ErrorT status = ERROR_SUCCESS;
    bool assetFound = false;
    NvmMgrFindContextT context;
    AssetMgrAssetTypeT const assetType = ASSET_MGR_ASSET_TYPE_RSA_PRIVATE_KEY;
    NvmMgrAddressT assetAddressFind;
    uint32_t assetLengthLongWordsFind;
    NvmMgrAddressT assetAddress = 0;
    uint32_t assetLengthLongWords = 0;
    AssetMgrCertificateAuthIdT certAuthIdRead;

    if ((NULL == privKey) ||
        (NULL == privKeySizeLongWords))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_PRIVATE_KEY_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = NvmMgrFindFirst(&context, assetType, &assetAddressFind, &assetLengthLongWordsFind);
    }
    while (ERROR_SUCCESS == status)
    {
        status = AssetMgrGetCertAuthIdFromPrivateKey(assetAddressFind, &certAuthIdRead);
        if (ERROR_SUCCESS == status)
        {
            if (certAuthIdRead == certAuthId)
            {
                assetAddress = assetAddressFind;
                assetLengthLongWords = assetLengthLongWordsFind;
                assetFound = true;
            }
            status = NvmMgrFindNext(&context, &assetAddressFind, &assetLengthLongWordsFind);
        }
    }
    // Check we got to the end correctly and at least one was found)
    if ((assetFound) && (ERROR_NOENT == status))
    {
        status = ERROR_SUCCESS;
    }
    if (ERROR_SUCCESS == status)
    {
        uint32_t headerSize = sizeof(AssetMgrRsaPrivKeyHeaderT) / sizeof(uint32_t);

        if (assetLengthLongWords <= (*privKeySizeLongWords + headerSize))
        {
            *privKeySizeLongWords = assetLengthLongWords - headerSize;
            status = NvmMgrReadAsset(assetAddress,
                (uint32_t* const)privKey,
                    headerSize,
                    *privKeySizeLongWords);
        }
        else
        {
            status = ERROR_RANGE;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PRIVATE_KEY, (uint16_t)status);

    return status;
}

/// Retrieves the 'latest' CC312 TRNG characterization data from NVM
///
/// \param[out] CC312TrngChar   Buffer to contain the retrieved characterization data
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Characterization data retrieved and present in supplied buffer
/// \retval ERROR_NOENT         Failed to locate characterization data in NVM
/// \retval ERROR_IO            IO error when accessing NVM
/// \retval ERROR_RANGE         NVM asset size incorrect
/// \retval ERROR_INVAL         Invalid parameter
///
ErrorT AssetMgrRetrieveCC312TrngChar(AssetMgrCC312TrngCharT* CC312TrngChar)
{
    ErrorT status = ERROR_SUCCESS;
    uint32_t assetLengthLongWords = sizeof(AssetMgrCC312TrngCharT) / sizeof(uint32_t);

    if (NULL == CC312TrngChar)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_TRNG_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = AssetMgrRetrieveLatestAsset(ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION, (uint32_t*)CC312TrngChar,
                &assetLengthLongWords);
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR, (uint16_t)status);

    return status;
}

/// Retrieves the 'latest' PSK Master Secret from NVM
///
/// \param[out] masterSecret                     Buffer to contain the retrieved master secret
/// \param[in, out] masterSecretSizeLongWords    Size of the buffer, on return size of the retrieved key
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Master Secret retrieved and present in supplied buffer
/// \retval ERROR_NOENT         Failed to locate Master Secret in NVM
/// \retval ERROR_IO            IO error when accessing NVM
/// \retval ERROR_INVAL         Invalid parameter
///
ErrorT AssetMgrRetrievePskMasterSecret(AssetMgrPskMasterSecretT* masterSecret,
    uint32_t* const masterSecretSizeLongWords)
{
    ErrorT status = ERROR_SUCCESS;

    if ((NULL == masterSecret) ||
        (NULL == masterSecretSizeLongWords))
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_PSK_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = AssetMgrRetrieveLatestAsset(ASSET_MGR_ASSET_TYPE_PSK_MASTER_SECRET,
            (uint32_t*)masterSecret,
                masterSecretSizeLongWords);
    }
    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET, (uint16_t)status);

    return status;
}

/// Retrieves the 'latest' OTPM configuration data from NVM
///
/// \param[out] OTPMConfig      Buffer to contain the retrieved configuration data
///
/// \returns Error status
/// \retval ERROR_SUCCESS       Configuration data retrieved and present in supplied buffer
/// \retval ERROR_NOENT         Failed to locate configuration data in NVM
/// \retval ERROR_IO            IO error when accessing NVM
/// \retval ERROR_INVAL         Invalid parameter
///
ErrorT AssetMgrRetrieveOtpmConfig(AssetMgrOtpmConfigT* OTPMConfig)
{
    ErrorT status = ERROR_SUCCESS;
    uint32_t assetLengthLongWords = sizeof(AssetMgrOtpmConfigT) / sizeof(uint32_t);

    if (NULL == OTPMConfig)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_OTPM_CONFIG_BAD_ARGS, 0U);
        status = ERROR_INVAL;
    }
    if (ERROR_SUCCESS == status)
    {
        status = AssetMgrRetrieveLatestAsset(ASSET_MGR_ASSET_TYPE_OTPM_CONFIGURATION, (uint32_t*)OTPMConfig,
                &assetLengthLongWords);
    }
    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_ASSET_MGR_RETRIEVE_OTPM_CONFIG, (uint16_t)status);

    return status;
}

/// @} endgroup assetmgr

