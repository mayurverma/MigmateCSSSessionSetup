//***********************************************************************************
/// \file
///
/// Interrupts default Mocks
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
#include "Interrupts.h"
}

// Mocks
extern "C"
{
    ErrorT InterruptsInit(void)
    {
        mock().actualCall("InterruptsInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    void InterruptsEnableAll(void)
    {
        mock().actualCall("InterruptsEnableAll");
    }

    void InterruptsDisableAll(void)
    {
        mock().actualCall("InterruptsDisableAll");
    }

    ErrorT InterruptsEnableInterrupt(InterruptT const interrupt, bool const enable)
    {
        mock().actualCall("InterruptsEnableInterrupt")
            .withParameter("interrupt", interrupt)
            .withParameter("enable", enable);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT InterruptsIsInterruptPending(InterruptT const interrupt, bool* isPending)
    {
        mock().actualCall("InterruptsIsInterruptPending")
            .withParameter("interrupt", interrupt)
            .withOutputParameter("isPending", isPending);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT InterruptsClearInterruptPending(InterruptT const interrupt)
    {
        mock().actualCall("InterruptsClearInterruptPending")
            .withParameter("interrupt", interrupt);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT InterruptsSetInterruptPriority(  InterruptT const interrupt,
                                            InterruptPriorityT const priority)
    {
        mock().actualCall("InterruptsSetInterruptPriority")
            .withParameter("interrupt", interrupt)
            .withParameter("priority", priority);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }

    ErrorT InterruptsSetExceptionPriority(  ExceptionT const exception,
                                            InterruptPriorityT const priority)
    {
        mock().actualCall("InterruptsSetExceptionPriority")
            .withParameter("exception", exception)
            .withParameter("priority", priority);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
}
