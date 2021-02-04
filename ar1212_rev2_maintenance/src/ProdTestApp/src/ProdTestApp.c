//***********************************************************************************
/// \file
///
/// Production Test Application
///
/// \addtogroup prodtestapp
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

#include "ProdTestAppInternal.h"
#include "LifecycleMgr.h"
#include "AR0820.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

/// Runs a blank check on the OTPM, checking that all readable locations are blank and there are no
/// ECC errors, which indicates the device is intact and ready for programming
///
/// \param[in]       command              Command to handle (OTPM_BLANK_CHECK)
/// \param[in, out]  params               Pointer to the command params (in shared memory)
/// \returns
/// \retval          ERROR_SUCCESS        Command completed successfully
/// \retval          ERROR_INVAL          Invalid command code or parameter pointer null
/// \retval          ERROR_SYSERR         Internal error occurred, command could not complete
/// \retval          ERROR_ACCESS         Incorrect lifecycle state, command could not complete
///
ErrorT ProdTestAppOnOtpmBlankCheck(HostCommandCodeT const command, CommandHandlerCommandParamsT params)
{
    ErrorT err = ERROR_SUCCESS;
    ProdTestAppCommandParamsT* cmdParams = (ProdTestAppCommandParamsT*)params;
    LifecycleMgrLifecycleStateT lifeState;

    err = LifecycleMgrGetCurrentLcs(&lifeState);
    if (ERROR_SUCCESS == err)
    {
        if (HOST_COMMAND_CODE_OTPM_BLANK_CHECK != command)
        {
            err = ERROR_INVAL;
        }
        else if (NULL == params)
        {
            err = ERROR_INVAL;
        }
        else if (LIFECYCLE_MGR_LIFECYCLE_STATE_CM != lifeState)
        {
            err = ERROR_ACCESS;
        }
        else
        {
            err = ProdTestAppCalculateError(true, cmdParams);
        }
    }

    return err;
}

/// Runs a verification on the OTPM, checking secure and assets areas for ECC errors only and checking
/// the rest for both ECC and blank data. Used to verify that programming was successful.
///
/// \param[in]       command              Command to handle (OTPM_VERIFY)
/// \param[in, out]  params               Pointer to the command params (in shared memory)
/// \returns
/// \retval          ERROR_SUCCESS        Command completed successfully
/// \retval          ERROR_INVAL          Invalid command code or parameter pointer is null
/// \retval          ERROR_SYSERR         Internal error occurred, command could not complete
/// \retval          ERROR_ACCESS         Incorrect lifecycle state, command could not complete
///
ErrorT ProdTestAppOnOtpmVerify(HostCommandCodeT const command, CommandHandlerCommandParamsT params)
{
    ErrorT err = ERROR_SUCCESS;
    ProdTestAppCommandParamsT* cmdParams = (ProdTestAppCommandParamsT*)params;
    LifecycleMgrLifecycleStateT lifeState;

    err = LifecycleMgrGetCurrentLcs(&lifeState);
    if (ERROR_SUCCESS == err)
    {
        if (HOST_COMMAND_CODE_OTPM_VERIFY != command)
        {
            err = ERROR_INVAL;
        }
        else if (NULL == params)
        {
            err = ERROR_INVAL;
        }
        else if (LIFECYCLE_MGR_LIFECYCLE_STATE_CM == lifeState)
        {
            err = ERROR_ACCESS;
        }
        else
        {
            err = ProdTestAppCalculateError(false, cmdParams);
        }
    }

    return err;
}

// @} endgroup prodtestapp

