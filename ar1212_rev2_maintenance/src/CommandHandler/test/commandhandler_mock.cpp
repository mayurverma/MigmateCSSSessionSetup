//***********************************************************************************
/// \file
///
/// Command Handler default Mocks
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
#include "CommandHandler.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_COMMAND_HANDLER_ON_PHASE_ENTRY)
    ErrorT CommandHandlerOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("CommandHandlerOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_HANDLER_ON_DOORBELL_EVENT)
    ErrorT CommandHandlerOnDoorbellEvent(HostCommandCodeT const commandCode)
    {
        mock().actualCall("CommandHandlerOnDoorbellEvent")
            .withParameter("commandCode", commandCode);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_HANDLER_FATAL_ERROR_STOP)
    void CommandHandlerFatalErrorStop(void)
    {
        mock().actualCall("CommandHandlerFatalErrorStop");
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_HANDLER_IS_HOST_COMMAND_IN_PROGRESS)
    bool CommandHandlerIsHostCommandInProgress(void)
    {
        mock().actualCall("CommandHandlerIsHostCommandInProgress");
        return mock().returnBoolValueOrDefault(true);
    }
#endif
#if !defined(MOCK_DISABLE_COMMAND_HANDLER_ADD_COMMAND)
    ErrorT CommandHandlerAddCommand(CommandHandlerCommandTableEntryT* command)
    {
    	mock().actualCall("CommandHandlerAddCommand")
    			.withPointerParameter("command",command);
    	return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
}
