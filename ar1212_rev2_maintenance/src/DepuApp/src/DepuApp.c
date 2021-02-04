//***********************************************************************************
/// \file
///
/// Patch for DEPU App, the host command processing and top-level functions.
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
#include "DepuAppInternal.h"
#include "Diag.h"
#include "LifecycleMgr.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

/// Starts DEPU host command to program otpm
///
/// \param[in]      commandCode         Command to handle (DEPU_PROGRAM_OTPM and DEPU_LOAD_ASSETS)
/// \param[in,out]  params              Pointer to the command params and return response (in shared memory)
/// \returns Command error status
/// \retval ERROR_SUCCESS          The whole CMPU/DMPU processes have been completed successfully.
/// \retval ERROR_IO               There are errors in read/write OTPM in CMPU or DMPU stages.
/// \retval ERROR_NODEV            The LCS modes have not been cycled through CM->DM->SECURE correctly.
/// \retval ERROR_INVAL            Invalid parameter
/// \retval ERROR_SYSERR           Host command or parameters are wrong or any other errors return from
///                                CC312 lib function calls.
/// \retval ERROR_RANGE            The blankAddressLongWords (NvmMgr) value is not the expected value
/// \retval ERROR_NOEXEC           The starting LCS is in a wrong mode (not from CM) or have other HW errors.
//
ErrorT  DepuAppOnProgramOtpm(HostCommandCodeT const commandCode, CommandHandlerCommandParamsT params)
{
    ErrorT error = ERROR_SUCCESS;

    if (HOST_COMMAND_CODE_DEPU_PROGRAM_OTPM != commandCode)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_DEPU_APP_BAD_COMMAND,
            (uint32_t)commandCode);
        error = ERROR_SYSERR;
    }
    else if (NULL == params)
    {
        SystemMgrReportFatalError(SYSTEM_MGR_FATAL_ERROR_DEPU_APP_BAD_PARAM, 0U);
        error = ERROR_SYSERR;
    }
    else
    {
        LifecycleMgrLifecycleStateT lcsCurrent;
        error = LifecycleMgrGetCurrentLcs(&lcsCurrent);
        if ((ERROR_SUCCESS == error) && (LIFECYCLE_MGR_LIFECYCLE_STATE_CM != lcsCurrent))
        {
            error = ERROR_NOEXEC;
        }
    }

    if (ERROR_SUCCESS == error)
    {
        DepuAppProgramOtpmCommandParamsT* message = (DepuAppProgramOtpmCommandParamsT*)params;
        error = DepuAppProgramOtpm(message);
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_DEPU_APP_ON_PROGRAM_OTPM, (uint16_t)error);

    return error;
}

/// @} endgroup DepuApp

