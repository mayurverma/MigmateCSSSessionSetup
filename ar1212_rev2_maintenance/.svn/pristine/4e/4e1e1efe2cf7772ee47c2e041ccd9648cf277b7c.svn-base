//***********************************************************************************
/// \file
///
/// DEPU App patch internal functions
///
/// \addtogroup DepuApp
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
#include "AR0820.h"
#include "DepuAppInternal.h"
#include "Diag.h"
#include "LifecycleMgr.h"
#include "NvmMgr.h"
#include "NvmMgrInternal.h"
#include "mcu_sysctrl.h"
#include "ccProdOtpPatch.h"
#include "Toolbox.h"
#if defined(BUILD_TARGET_ARCH_x86)
#include "DepuAppUnitTest.h"
#endif

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

/// DEPU Configuration for OTPM registers. Subject to adjust during bring up phase.
AssetMgrOtpmConfigT depuOtpmDrvConfig =
{
    10000U,                 // tvpps_ns;           Charge Pump Warm-up time in nSec
    1U,                     // tpas_ns;            Program Address Setup time in nSec
    200000U,                // tpp_ns;             Program pulse width in nSec
    1U,                     // tpr_ns;             Program recovery time in nSec
    600000U,                // tspp_ns;            Soak pulse width in nSec
    300000U,                // tpsr_ns;            Power Supply Recovery time in nSec
    1U,                     // tds_ns;             Data setup time in nSec
    2U,                     // twr_ns;             Write Recovery time in nSec
    5U,                     // twp_ns;             Write Pulse width in nSec
    4U,                     // tras_ns;            Read address setup time in nSec
    17U,                    // trr_ns;             Read recovery time in nSec
    15U,                    // soak_count_max;     Maximum soak counter (retry)
    0x1024U,                // mr_pgm;             MR value for OTPM programming
    0x08a4U,                // mr_pgm_soak;        MR value for OTPM program soak
    0x0000U,                // mr_read;            MR value for OTPM read
    0x1200U,                // mra_pgm;            MRA value for OTPM programming
    0x1200U,                // mra_prg_soak;       MRA value for OTPM program soak
    0x0210U,                // mra_read;           MRA value for OTPM read
    0x103eU,                // mrb_pgm;            MRB value for OTPM programming
    0x2034U,                // mrb_prg_soak;       MRB value for OTPM program soak
    0x2000U,                // mrb_read;           MRB value for OTPM read
    0x0000U,                // pad
};

/// DEPU Application control/state structure
DepuAppT depuApp;

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// DEPU App function provisions AR0820 with cyber-security
///
/// \param[in,out]  params         Pointer to the command params and return response (in shared memory)
///
/// \returns Error status
/// \retval ERROR_SUCCESS          The whole CMPU/DMPU processes have been completed successfully.
/// \retval ERROR_IO               There are errors in read/write OTPM in CMPU or DMPU stages.
/// \retval ERROR_NODEV            The LCS modes have not been cycled through CM->DM->SECURE correctly.
/// \retval ERROR_RANGE            The blankAddressLongWords value is not the expected value
///
ErrorT DepuAppProgramOtpm(DepuAppProgramOtpmCommandParamsT* const params)
{
    ErrorT error = ERROR_SUCCESS;
    DepuAppProgramOtpmResponseParamsT* const response = (DepuAppProgramOtpmResponseParamsT* const)params;
    uint32_t clockSpeedHz = params->clockSpeedHz;
    uint16_t blockLengthBytes = params->blockLengthBytes;
    uint16_t expectedChecksum = params->checksum;

    // Initialize local response values
    ccProdOtp.errorAddress = 0;
    ccProdOtp.numberSoftErrors = 0;

    // Copy data from shared ram before doing integrity check
    (void)memcpy((void*)&depuApp.depuAppAssets, (void*)&params->depuAppAssets, sizeof(depuApp.depuAppAssets));

    if (blockLengthBytes != sizeof(depuApp.depuAppAssets))
    {
        error = ERROR_INVAL;
    }
    else
    {
        uint16_t checksum;
        uint16_t* dataBlock = (uint16_t*)&depuApp.depuAppAssets;

        checksum = ToolboxCalcChecksum(dataBlock, blockLengthBytes);

        if (checksum != expectedChecksum)
        {
            error = ERROR_INVAL;
        }
    }

    if (ERROR_SUCCESS == error)
    {
        // Next step is to set the system clock speed and set OTPM programming parameters
        error = OtpmDrvConfigure((AssetMgrOtpmConfigT* const)&depuOtpmDrvConfig, clockSpeedHz);
    }

    if (ERROR_SUCCESS == error)
    {
        // Report and clear errors generated during FW initialization
        error = CC_ProdOtpGetLastError();
    }

    if (ERROR_SUCCESS == error)
    {
        error = DepuAppCmpuProcess();
    }

    if (ERROR_SUCCESS == error)
    {
        error = DepuAppDmpuProcess();
    }

    if (ERROR_SUCCESS == error)
    {
        error = DepuAppAssetProvisioning();
    }

    if ((ERROR_SUCCESS == error) && (nvmMgr.blankAddressLongWords != DEPU_APP_EXPECTED_LAST_OTPM_ADDRESS_LW))
    {
        error = ERROR_RANGE;
    }

    response->errorAddress = ccProdOtp.errorAddress;
    response->numberSoftErrors = ccProdOtp.numberSoftErrors;

    return error;
}

/// DEPU App Decrypt an asset in a buffer
///
/// \param[in]  asset            Pointer to the encrypted asset Package
/// \param[in]  assetDecrypted   Pointer to the decrypted asset Package
/// \param[in]  assetSizeBytes   Size of the asset Package
///
/// \returns Error status
/// \retval ERROR_SUCCESS          The decryption has been completed successfully.
/// \retval ERROR_BADMSG           There is something wrong in the asset contents
/// \retval ERROR_SYSERR           Any other errors return from CC312 lib function calls.
///
ErrorT DepuAppDecryptAsset(DepuAppAssetT* const asset, DepuAppAssetT* const assetDecrypted,
    uint32_t const assetSizeBytes)
{
    ErrorT error = ERROR_SUCCESS;
    CryptoMgrRootOfTrustT rootOfTrust;

    (void)memcpy((void* const)assetDecrypted, (void* const)asset, assetSizeBytes);

    // rootOfTrust value is range tested inside CryptoMgrAuthenticateAsset function
    rootOfTrust = (CryptoMgrRootOfTrustT)assetDecrypted->rootOfTrust;

    // Next step is to authenticate and decrypt the asset
    error = CryptoMgrAuthenticateAsset(assetDecrypted->assetId, rootOfTrust, &assetDecrypted->token);

    return error;
}

/// DEPU App CMPU stage processing.
///
/// \returns Error status
/// \retval ERROR_SUCCESS             The CMPU processing has been completed successfully.
/// \retval ERROR_IO                  There are errors in read/write OTPM in CMPU stage.
/// \retval ERROR_NODEV               The LCS mode have not been changed from CM to DM at the end of CMPU processing.
/// \retval ERROR_SYSERR              Any other errors return from CC312 lib function calls.
///
ErrorT DepuAppCmpuProcess(void)
{
    ErrorT error = ERROR_SUCCESS;
    CCCmpuData_t cmpuData;
    CCError_t ccError = CC_OK;

    // populate cmpuData with data
    cmpuData.uniqueDataType = CMPU_UNIQUE_IS_HBK0;
    (void)memcpy((void*)cmpuData.uniqueBuff.hbk0, (void*)depuApp.depuAppAssets.hbk0,
        sizeof(depuApp.depuAppAssets.hbk0));
    cmpuData.kpicvDataType = ASSET_PKG_KEY;
    (void)memcpy((void*)cmpuData.kpicv.pkgAsset, (void*)depuApp.depuAppAssets.kpicv,
        sizeof(depuApp.depuAppAssets.kpicv));
    cmpuData.kceicvDataType = ASSET_NO_KEY;
    cmpuData.icvMinVersion = 0U;
    cmpuData.icvConfigWord = 0U;    // We are not going to use this word.
    (void)memcpy(cmpuData.icvDcuDefaultLock, depuApp.icvDcuDefaultLock, sizeof(depuApp.icvDcuDefaultLock));

    ccError = CCProd_Cmpu(CRYPTOCELL_CORE_BASE, &cmpuData, (uint32_t)depuApp.workSpaceBuffer,
            CMPU_WORKSPACE_MINIMUM_SIZE);
    if ((CCError_t)CC_OK != ccError)
    {
        DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_CMPU_ERROR, (uint16_t)ccError);
        if (CC_PROD_XXPU_MODULE_ERROR_BASE == (ccError & CC_PROD_ERROR_BASE_MASK))
        {
            error = ccProdOtp.lastError;
        }
        else
        {
            error = ERROR_SYSERR;
        }
    }

    if (ERROR_SUCCESS == error)
    {
        // Reset CC312 HW to cycle LCS to DM
        DepuAppCc312HardReset();
        error = LifecycleMgrCc312Init();
    }
    if (ERROR_SUCCESS == error)
    {
        LifecycleMgrLifecycleStateT lcsCurrent;
        error = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if ((ERROR_SUCCESS == error) && (LIFECYCLE_MGR_LIFECYCLE_STATE_DM != lcsCurrent))
        {
            error = ERROR_NODEV;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_CMPU_PROVISIONING, (uint16_t)error);

    return error;
}

/// DEPU App DMPU stage processing.
///
/// \returns Error status
/// \retval ERROR_SUCCESS          The DMPU processing has been completed successfully.
/// \retval ERROR_IO               There are errors in read/write OTPM in DMPU stage.
/// \retval ERROR_NODEV            The LCS mode have not been changed from DM to SECURE at the end of DMPU processing.
/// \retval ERROR_SYSERR           Any other errors return from CC312 lib function calls.
///
ErrorT DepuAppDmpuProcess(void)
{
    ErrorT error = ERROR_SUCCESS;
    CCDmpuData_t dmpuData;
    DepuAppAssetT* const assetKcp = (DepuAppAssetT* const)depuApp.workSpaceBuffer;
    DepuAppAssetT* const asset = (DepuAppAssetT*)depuApp.depuAppAssets.kcp;

    error = DepuAppDecryptAsset(asset, assetKcp, sizeof(depuApp.depuAppAssets.kcp));

    if (ERROR_SUCCESS == error)
    {
        CCError_t ccError = CC_OK;

        dmpuData.hbkType = DMPU_HBK_TYPE_HBK1;
        (void)memcpy((void*)dmpuData.hbkBuff.hbk1, (void*)depuApp.depuAppAssets.hbk1,
            sizeof(depuApp.depuAppAssets.hbk1));
        dmpuData.kcpDataType = ASSET_PLAIN_KEY;
        (void)memcpy((void*)dmpuData.kcp.plainAsset, (void*)assetKcp->encAsset, PROD_ASSET_SIZE);
        dmpuData.kceDataType = ASSET_NO_KEY;
        dmpuData.oemMinVersion = 0U;
        // The model doesn't have the same DCU setup as the real part, so both DMPU/CMPU write the same value
        // to the DCU lock registers so the outcome is the same and it can be tested with the model.
        (void)memcpy(dmpuData.oemDcuDefaultLock, depuApp.icvDcuDefaultLock, sizeof(depuApp.icvDcuDefaultLock));

        ccError = CCProd_Dmpu(CRYPTOCELL_CORE_BASE, &dmpuData, (uint32_t)depuApp.workSpaceBuffer,
                DMPU_WORKSPACE_MINIMUM_SIZE);

        if ((CCError_t)CC_OK != ccError)
        {
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_DMPU_ERROR, (uint16_t)ccError);
            if (CC_PROD_XXPU_MODULE_ERROR_BASE == (ccError & CC_PROD_ERROR_BASE_MASK))
            {
                error = ccProdOtp.lastError;
            }
            else
            {
                error = ERROR_SYSERR;
            }
        }
    }
    if (ERROR_SUCCESS == error)
    {
        // Reset CC312 HW to cycle LCS to SECURE
        DepuAppCc312HardReset();
        error = LifecycleMgrCc312Init();
    }
    if (ERROR_SUCCESS == error)
    {
        LifecycleMgrLifecycleStateT lcsCurrent;
        error = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if ((ERROR_SUCCESS == error) && (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE != lcsCurrent))
        {
            error = ERROR_NODEV;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_DMPU_PROVISIONING, (uint16_t)error);

    return error;
}

/// DEPU App Provisioning stage processing.
///
/// \returns Error status
/// \retval ERROR_SUCCESS          The DMPU processing has been completed successfully.
/// \retval ERROR_IO               There are errors in read/write OTPM in DMPU stage.
/// \retval ERROR_SYSERR           Any other errors return from CC312 lib function calls.
///
ErrorT DepuAppAssetProvisioning(void)
{
    ErrorT error = ERROR_SUCCESS;

    // Write Provisioning Identifier (32 bits of information to identify the provisioing tool)
    uint32_t provisioningId = DEPU_APP_PROVISIONING_ID;  // This is added by the build system

    // Burn PROVISIONING LOG asset into the OTPM.
    error = DepuAppWriteAsset((uint16_t)ASSET_MGR_ASSET_TYPE_PROVISIONING_ID,
        (uint32_t*)&provisioningId,
        (sizeof(provisioningId) / sizeof(uint32_t)));

    if (ERROR_SUCCESS == error)
    {
        // Burn 4 TRNG parameters into the OTPM.
        error = DepuAppWriteAsset((uint16_t)ASSET_MGR_ASSET_TYPE_TRNG_CHARACTERIZATION,
            (uint32_t*)&ccPalTrng.cc312PalTrngParameter,
            (sizeof(ccPalTrng.cc312PalTrngParameter) / sizeof(uint32_t)));
    }

    if (ERROR_SUCCESS == error)
    {
        // INFO:<SC> do not write Otpm config due to AR0820FW-271
        // (Provision OTPM Config Asset doesn't write the full asset in OTPM)
    }

    if (ERROR_SUCCESS == error)
    {
        // Write RSA private key
        error = DepuAppOnProvisionAsset((DepuAppAssetT*)depuApp.depuAppAssets.rsaPrivateKey,
                sizeof(depuApp.depuAppAssets.rsaPrivateKey));
    }

    if (ERROR_SUCCESS == error)
    {
        // Write RSA sensor public certificate
        error = DepuAppOnProvisionAsset((DepuAppAssetT*)depuApp.depuAppAssets.rsaSensorCertificate,
                sizeof(depuApp.depuAppAssets.rsaSensorCertificate));
    }

    if (ERROR_SUCCESS == error)
    {
        // Write RSA vendor public certificate
        error = DepuAppOnProvisionAsset((DepuAppAssetT*)depuApp.depuAppAssets.rsaVendorCertificate,
                sizeof(depuApp.depuAppAssets.rsaVendorCertificate));
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_ASSET_PROVISIONING, (uint16_t)error);

    return error;
}

/// DEPU App Provisioning of a single encrypted asset.
///
/// This is equivalent to AssetMgrOnProvisionAsset but needs to be changed to:
///    a) Avoid calling SystemMgrSetClockSpeed (it might reconfigure the otpm config)
///    b) Avoid calling NvmMgrWriteAsset and call DepuAppWriteAsset instead
///    c) Remove checking of existing assets (optimization)
///
/// \param[in]  asset            Pointer to the encrypted asset Package
/// \param[in]  assetSizeBytes   Size of the asset Package
///
/// \returns Error status
/// \retval ERROR_SUCCESS          The DMPU processing has been completed successfully.
/// \retval ERROR_IO               There are errors in read/write OTPM in DMPU stage.
/// \retval ERROR_NODEV            The LCS mode have not been changed from DM to SECURE at the end of DMPU processing.
/// \retval ERROR_SYSERR           Any other errors return from CC312 lib function calls.
///
ErrorT DepuAppOnProvisionAsset(DepuAppAssetT* const asset, const uint32_t assetSizeBytes)
{
    ErrorT error = ERROR_SUCCESS;
    AssetMgrAssetT* payloadDecrypted;

    DepuAppAssetT* const assetDecrypted = (DepuAppAssetT*)depuApp.workSpaceBuffer;

    error = DepuAppDecryptAsset(asset, assetDecrypted, assetSizeBytes);

    payloadDecrypted = (AssetMgrAssetT*)assetDecrypted->encAsset;

    // Next step is to write the asset in OTPM
    if (ERROR_SUCCESS == error)
    {
        uint16_t assetType = payloadDecrypted->header.assetType;
        uint32_t assetLengthLongWords = (uint32_t)payloadDecrypted->header.assetLengthLongWords;
        error = DepuAppWriteAsset(assetType, (uint32_t*)payloadDecrypted->payload, assetLengthLongWords);
    }

    return error;
}

/// A short delay between reset and poll NVM Idle.
///
/// \param[in]      cycles         A number of while loop count.
///
/// \returns void
///
void DepuAppCc312ReadRegisterDelay(int32_t const cycles)
{
    volatile int32_t i = cycles;

    do
    {
        i -= 1;
    }
    while (0 < i);
}

/// Reset CC312 hardware block without reseting the ARM Cortex-M0 subsystem.
///
/// OnSemi hardware design wired reset_control HARD_RESET bit to reset CC312 cell.
///
/// \returns void
///
void DepuAppCc312HardReset(void)
{
    uint32_t RegCtrl = 0;
    MCU_SYSCTRL_REGS_T* sysCtrlRegs = MCU_SYSCTRL_REGS;

    DiagSetCheckpoint(DIAG_CHECKPOINT_DEPU_APP_ON_CC312_HARD_RESET);
    RegCtrl = sysCtrlRegs->reset_control;
    RegCtrl |= MCU_SYSCTRL_RESET_CONTROL_HARD_RESET__MASK;
    sysCtrlRegs->reset_control = RegCtrl;

    DepuAppCc312ReadRegisterDelay(100);
}

/// @} endgroup DepuApp

