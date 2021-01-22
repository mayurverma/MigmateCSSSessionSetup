//***********************************************************************************
/// \file
///
/// Lifecycle Manager implementation
///
/// \addtogroup lifecyclemgr
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
#include "Common.h"
#include "CommandHandler.h"
#include "OtpmDrv.h"
#include "Diag.h"
#include "DiagCheckpoint.h"
#include "bsv_api.h"
#include "secdebug_api.h"
#include "dx_crys_kernel.h"
#include "SystemMgr.h"
#include "LifecycleMgr.h"
#include "LifecycleMgrInternal.h"

bool LifecycleMgrIsOtpmBad(void);

#if defined(BUILD_TARGET_ARCH_x86)
// To support MinGW32 based unit testing the crypto memory is mapped to an internal buffer as the actual
// memory is outside the address range of a MinGW32 build.
uint8_t cryptoMemory[(DX_HOST_DCU_EN0_REG_OFFSET + 16U)] = {0};
#endif

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
#define LIFE_CYCLE_MGR_INFO_ENABLE_MASK     0x00000010U
//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
/// Check whether OTPM generated a non-recoverable error
///
/// \returns true for OPTM unrecoverable error
bool LifecycleMgrIsOtpmBad(void)
{
    OtpmDrvErrorT otpmDrvError;
    bool error = false;
    ErrorT otpmError;

    otpmError = OtpmDrvGetLastError(&otpmDrvError);
    if (ERROR_SUCCESS == otpmError)
    {
        // Error generated now check if it is recoverable or not
        if (true != otpmDrvError.isCorrectable)
        {
            error = true;
        }
    }

    return error;
}

/// Reports a SW fatal error
///
/// Reports a fatal error, which will result in shutdown of the cybersecurity subsystem.
///
/// \param[in]  context     context code (or zero)
/// \param[in]  infoParam   Optional information parameter for diagnostics (or zero)
/// \returns void
///
void LifecycleMgrSwFatalError(SystemMgrFatalErrorT const context,
    uint32_t const infoParam)
{
    SystemMgrReportCryptoFault(SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR, (uint32_t)context, infoParam);
}

/// Reports a HW fatal error
///
/// Reports a HW fatal error, which will result in shutdown of the cybersecurity subsystem.
///
/// \param[in]  context     context code (or zero)
/// \param[in]  infoParam   Optional information parameter for diagnostics (or zero)
/// \returns void
///
void LifecycleMgrHwFatalError(SystemMgrFatalErrorT const context,
    uint32_t const infoParam)
{
    SystemMgrReportCryptoFault(SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR, (uint32_t)context, infoParam);
}

//
//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------
LifecycleMgrT lifecycleMgr;

/// Dumps Life cycle Manager state in response to a fatal error
///
/// \param[in] dumpBuffer       Pointer to buffer to contain the dump
/// \param[in] sizeLongWords    Size of the dump buffer
/// \returns void
///
void LifecycleMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords)
{
    uint32_t sizeBytes = sizeLongWords * (sizeof(uint32_t));

    if (NULL != dumpBuffer)
    {
        uint32_t dumpSizeBytes = sizeof(lifecycleMgr);

        if (dumpSizeBytes > sizeBytes)
        {
            dumpSizeBytes = sizeBytes;
        }

        (void)memcpy((void*)dumpBuffer, (void*)&lifecycleMgr, dumpSizeBytes);
    }
}

/// Initializes internal control structure
///
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failure
ErrorT LifecycleMgrInit(void)
{
    ErrorT error = ERROR_SUCCESS;

    (void)memset(&lifecycleMgr, 0, sizeof(lifecycleMgr));
    error = DiagRegisterFatalErrorDumpHandler(LifecycleMgrOnDiagFatalErrorDump);

    return error;
}

/// Initialize the CC312 Secure Boot library
///
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failure
ErrorT LifecycleMgrCc312Init(void)
{
    ErrorT error = ERROR_SUCCESS;
    CCError_t ccStatus;
    bool otpmBad;
    uint32_t lcsCurrent;

    DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_ENTRY);
    // This function must be the first Arm CryptoCell 312 SBROM library API called. It verifies the HW product
    // and version numbers, and initializes the HW.
    ccStatus = CC_BsvInit(CRYPTOCELL_CORE_BASE);
    // The above function accesses OTPM but assumes there will never be any errors so does not capture any error
    // conditions. On completion we need to check if any error was generated

    // Check for OTPM errors
    otpmBad = LifecycleMgrIsOtpmBad();
    if (true == otpmBad)
    {
        ccStatus = (CCError_t)CC_FAIL;
        LifecycleMgrHwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_OTPM_FAULT, 0U);
    }

    if ((CCError_t)CC_OK == ccStatus)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_GET_LCS);
        // Retrieves the security lifecycle state from the NVM manager. Lower levels will capture OTPM errors
        ccStatus = CC_BsvLcsGetAndInit(CRYPTOCELL_CORE_BASE, &lcsCurrent);
        if ((CCError_t)CC_OK != ccStatus)
        {
            // Sets the "fatal error" flag in the NVM manager, to disable the use of any HW Keys or security services.
            (void)CC_BsvFatalErrorSet(CRYPTOCELL_CORE_BASE);
            // With no certificate, this locks the DCU and disables debug
            (void)CC_BsvSecureDebugSet(CRYPTOCELL_CORE_BASE, NULL, 0U, NULL, NULL, 0U);
            LifecycleMgrSwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT, 0U);
            DiagEnableInfo(false);
        }
    }

    if ((CCError_t)CC_OK == ccStatus)
    {
        // Check life cycle, if secure disable access to info register
        if ((uint32_t)CC_BSV_SECURE_LCS == lcsCurrent)
        {
            DiagEnableInfo(false);
        }
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_GATING);
        // This API enables the core_clk gating mechanism, which is disabled during power-up.
        ccStatus = CC_BsvCoreClkGatingEnable(CRYPTOCELL_CORE_BASE);
        if ((CCError_t)CC_OK != ccStatus)
        {
            LifecycleMgrHwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_CRYPTO_CLOCK_GATING_FAULT, 0U);
        }
    }

    if ((CCError_t)CC_OK == ccStatus)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS);
        // Controls the APB secure filter, this call disables secure mode and secure lock
        ccStatus = CC_BsvSecModeSet(CRYPTOCELL_CORE_BASE, (CCBool_t)CC_FALSE, (CCBool_t)CC_FALSE);
        if ((CCError_t)CC_OK != ccStatus)
        {
            LifecycleMgrHwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_SEC_FILTER_FAULT, 0U);
        }
    }

    if ((CCError_t)CC_OK == ccStatus)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS);
        // Activates the APB privilege filter, this call disables secure mode and secure lock
        ccStatus = CC_BsvPrivModeSet(CRYPTOCELL_CORE_BASE, (CCBool_t)CC_FALSE, (CCBool_t)CC_FALSE);
        if ((CCError_t)CC_OK != ccStatus)
        {
            LifecycleMgrHwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_PRIV_FILTER_FAULT, 0U);
        }
    }

    if ((CCError_t)CC_OK != ccStatus)
    {
        error = ERROR_SYSERR;
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_CC312_INIT_FAIL);
    }

    return error;
}

/// Reads DCU bit controlling access to info register
///
/// \returns state of DCU Info Enable Bit
bool LifecycleMgrGetInfoEnable(void)
{
    volatile uint32_t* dcuEn0 = (volatile uint32_t*)(CRYPTOCELL_CORE_BASE + DX_HOST_DCU_EN0_REG_OFFSET);

    return (0U != (*dcuEn0 & LIFE_CYCLE_MGR_INFO_ENABLE_MASK));
}
/// @} endgroup lifecyclemgr

