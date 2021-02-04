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
#include <stdlib.h>

#include "AR0820.h"
#include "Common.h"
#include "CommandHandler.h"
#include "Diag.h"
#include "DiagCheckpoint.h"
#include "bsv_api.h"
#include "secdebug_api.h"
#include "bootimagesverifier_def.h"
#include "LifecycleMgr.h"
#include "LifecycleMgrInternal.h"
#include "bsv_error.h"

/// Returns the current lifecycle state
///
/// \param[out]  lcsCurrent             Pointer memory to store current life cycle
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Lifecycle valid
/// \retval ERROR_SYSERR    Error when retrieving lifecycle
ErrorT LifecycleMgrGetCurrentLcs(LifecycleMgrLifecycleStateT* const lcsCurrent)
{
    ErrorT error = ERROR_SUCCESS;
    CCError_t ccStatus = CC_OK;
    uint32_t lifeCycle;

    if (NULL == lcsCurrent)
    {
        error = ERROR_INVAL;
    }
    else
    {
        ccStatus = CC_BsvLcsGet(CRYPTOCELL_CORE_BASE, &lifeCycle);
        if ((CCError_t)CC_OK != ccStatus)
        {
            error = ERROR_SYSERR;
        }
        else
        {
            switch (lifeCycle)
            {
                case CC_BSV_CHIP_MANUFACTURE_LCS:
                    *lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
                    break;

                case CC_BSV_DEVICE_MANUFACTURE_LCS:
                    *lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_DM;
                    break;

                case CC_BSV_SECURE_LCS:
                    *lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
                    break;

                case CC_BSV_RMA_LCS:
                    *lcsCurrent = LIFECYCLE_MGR_LIFECYCLE_STATE_RMA;
                    break;

                default:
                    error = ERROR_SYSERR;
                    break;
            }
        }

        if (ERROR_SYSERR == error)
        {
            // Sets the "fatal error" flag in the NVM manager, to disable the use of any HW Keys or security services.
            (void)CC_BsvFatalErrorSet(CRYPTOCELL_CORE_BASE);
            // With no certificate, this locks the DCU and disables debug
            (void)CC_BsvSecureDebugSet(CRYPTOCELL_CORE_BASE, NULL, 0, NULL, NULL, 0);
            LifecycleMgrSwFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT, 0U);
        }
    }

    return error;
}

/// Handles the ApplyDebugEntitlement host command
///
/// \param[in]  commandCode Command to handle (APPY_DEBUG_ENTITLEMENT)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Application of debug entitlement successful
/// \retval ERROR_SYSERR    Application of debug entitlement failed
/// \retval ERROR_INVAL     Invalid input parameter
/// \retval ERROR_BADMSG    Invalid message
/// \retval ERROR_NOSPC     Failed to allocate workspace
/// \retval ERROR_ACCESS    Command not permitted in this life cycle
///
ErrorT LifecycleMgrOnApplyDebugEntitlement(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT error = ERROR_SYSERR;
    LifecycleMgrApplyDebugEntitlementHeaderT* command;
    LifecycleMgrLifecycleStateT lcsCurrent;
    CCError_t ccStatus = CC_OK;
    uint32_t* workspace;
    uint32_t enableRma = 0U;

    DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY);

    if (HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_COMMAND,
            (uint32_t)commandCode);
        error = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_PARAM, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        command = (LifecycleMgrApplyDebugEntitlementHeaderT*)params;
        // Check state to determine if certificate should be processed
        error = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if (ERROR_SUCCESS == error)
        {
            if ((LIFECYCLE_MGR_LIFECYCLE_STATE_DM == lcsCurrent) ||
                (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == lcsCurrent))
            {
                DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT);
                // INFO: <RW>: It was expected that if a certificate with a higher version was presented
                // CC_BsvSecureDebugSet() would write the new version to OTPM. In reality CC_BsvSecureDebugSet()
                // does not write the the OTPM however to maintain API this function still informs
                // system manager of the clock speed. See AR0820FW-124: Secure Debug doesn't update NV counters
                error = SystemMgrSetClockSpeed(command->clockSpeedHz);
                if (ERROR_SUCCESS == error)
                {
                    workspace =
                        (uint32_t*)calloc(((uint32_t)CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES / sizeof(uint32_t)),
                            sizeof(uint32_t));
                    if (NULL != workspace)
                    {
                        ccStatus = CC_BsvSecureDebugSet(CRYPTOCELL_CORE_BASE,
                                &command->entitlement,
                            (uint32_t)((uint32_t)command->blobSizeWords << (uint32_t)1U),
                                &enableRma,
                                workspace,
                                CC_SB_MIN_DBG_WORKSPACE_SIZE_IN_BYTES);

                        if ((CCError_t)CC_OK == ccStatus)
                        {
                            if (0U != enableRma)
                            {
                                // This function only supports Debug Entitlement and not RMA certificates
                                // Report a fatal error immediately as the DCUs have been locked
                                SystemMgrReportFatalError(
                                    SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_RMA_PRESENTED, 0U);
                                error = ERROR_SYSERR;
                            }
                            else
                            {
                                if (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == lcsCurrent)
                                {
                                    DiagEnableInfo(LifecycleMgrGetInfoEnable());
                                }
                                lifecycleMgr.debugEntitlementApplied = true;
                                error = ERROR_SUCCESS;
                            }
                        }
                        else
                        {
                            error = ERROR_BADMSG;
                        }
                    }
                    else
                    {
                        error = ERROR_NOSPC;
                    }
                    free(workspace);
                }
            }
            else
            {
                error = ERROR_ACCESS;
            }
        }
    }

    if (ERROR_SUCCESS != error)
    {
        DiagSetCheckpoint(DIAG_CHECKPOINT_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR);
    }

    return error;
}

/// Handles the AuthorizeRMA host command
///
/// This command is not supported for AR0820 rev 2 due to the need to reprogram OTPM contents
///
/// \param[in]  commandCode Command to handle (AUTHORIZE_RMA)
/// \param[in]  params      Pointer to the command params (in shared memory)
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   RMA authorised
/// \retval ERROR_SYSERR    RMA authorisation failed
///
ErrorT LifecycleMgrOnAuthorizeRMA(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params)
{
    ErrorT error = ERROR_ACCESS;

    (void)commandCode;
    (void)params;

    // This command is not supported for AR0820 rev 2 due to the need to reprogram OTPM contents
    return error;
}

/// Handles entry to an operating phase
/// \param[in] phase    Phase being entered
/// \returns Command success/failure
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failure
ErrorT LifecycleMgrOnPhaseEntry(SystemMgrPhaseT const phase)
{
    ErrorT error = ERROR_SUCCESS;

    switch (phase)
    {
        case SYSTEM_MGR_PHASE_INITIALIZE:
            error = LifecycleMgrInit();
            break;

        case SYSTEM_MGR_PHASE_DEBUG:
            // CSS_FCR20.2  Lifecycle Manager shall initialize the CC312 Secure Boot library on entry to the Debug phase.
            // CSS_FCR20.2.1   Lifecycle Manager shall check for OTPM 'hard' errors after the CC312 Secure Boot library has
            // initialized (SWR40.6). If a 'hard' error occurred, Lifecycle Manager shall report a fatal error.
            error = LifecycleMgrCc312Init();
            if (ERROR_SUCCESS != error)
            {
                SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_INIT_FAILED,
                    (uint32_t)error);
            }
            break;

        case SYSTEM_MGR_PHASE_PATCH:
        case SYSTEM_MGR_PHASE_CONFIGURE:
            // CSS_FCR20.3  Lifecycle Manager applies the default DCU state on entry to the Patch or Configure phases if a
            // debug entitlement was not presented.
            if (true != lifecycleMgr.debugEntitlementApplied)
            {
                CCError_t status;
                // INFO: <RW>: The input parameters are set incorrectly to force the function to jump to lock the DCU's.
                // We ignore the return code if CC_BSV_ILLEGAL_INPUT_PARAM_ERR otherwise generate a Fatal error
                // because that means something has gone wrong when writing to the DCUs.
                status = CC_BsvSecureDebugSet(CRYPTOCELL_CORE_BASE, NULL, 0U, NULL, NULL, 0U);
                if ((uint32_t)CC_BSV_ILLEGAL_INPUT_PARAM_ERR != status)
                {
                    SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_CONFIG_FAILED, status);
                    error = ERROR_SYSERR;
                }
            }
            break;

        case SYSTEM_MGR_PHASE_SHUTDOWN:
            // Assume some fatal error occurred
            // Sets the "fatal error" flag in the NVM manager, to disable the use of any HW Keys or security services.
            (void)CC_BsvFatalErrorSet(CRYPTOCELL_CORE_BASE);
            // With no certificate, this locks the DCU and disables debug
            (void)CC_BsvSecureDebugSet(CRYPTOCELL_CORE_BASE, NULL, 0U, NULL, NULL, 0U);
            break;

        default:
            // Do nothing
            break;
    }

    return error;
}

/// @} endgroup lifecyclemgr

