//***********************************************************************************
/// \file
///
/// Command Driver default Mocks
//
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

#include "cpputest_support.h"

extern "C"
{
#include "Common.h"
#include "CommandDrv.h"
#include "CommandHandler.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_COMMAND_DRV_INIT)
    ErrorT CommandDrvInit(void)
    {
        mock().actualCall("CommandDrvInit");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_DRV_GET_COMMAND)
    uint16_t CommandDrvGetCommand(void)
    {
        mock().actualCall("CommandDrvGetCommand");
        return (uint16_t)mock().returnUnsignedIntValueOrDefault(HOST_COMMAND_CODE_GET_STATUS);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_DRV_GET_COMMAND_PARAMS)
    uint32_t * CommandDrvGetCommandParams(void)
    {
        mock().actualCall("CommandDrvGetCommandParams");
        return (uint32_t *)mock().returnPointerValueOrDefault((uint32_t*)0x3FFF1000);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_DRV_SET_RESPONSE)
    void CommandDrvSetResponse(uint16_t const response)
    {
        mock().actualCall("CommandDrvSetResponse")
            .withParameter("response", response);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_DRV_IS_COMMAND_IN_PROGRESS)
    bool CommandDrvIsCommandInProgress(void)
    {
    	mock().actualCall("CommandDrvIsCommandInProgress");
    	return (bool)mock().returnBoolValueOrDefault(false);
    }
#endif
}
