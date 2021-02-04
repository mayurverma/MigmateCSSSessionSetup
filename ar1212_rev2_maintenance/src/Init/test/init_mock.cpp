//***********************************************************************************
/// \file
///
/// Init default Mocks
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
#include "Init.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_INIT_MPU_GET_STACK_INFORMATION)
    // Returns information associated with stack i.e. base, length and guard
    ErrorT InitMpuGetStackInformation(InitMpuStackConfigType* stackConfig)
    {
        mock().actualCall("InitMpuGetStackInformation")
           .withOutputParameter("stackConfig", stackConfig);
        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_INIT_RELOCATE_VECTOR_TABLE)
    void InitRelocateVectorTable(void)
    {
        mock().actualCall("InitRelocateVectorTable");
    }
#endif

#if !defined(MOCK_DISABLE_INIT_INSTALL_NEW_IRQ_HANDLER)
    ErrorT InitInstallNewIrqHandler(IRQn_Type const irq, InitVectorTableHandlerT const newIrqHandler)
    {
        mock().actualCall("InitInstallNewIrqHandler")
            .withParameter("irq", irq)
            .withParameter("newIrqHandler", newIrqHandler);
        return (ErrorT)mock().returnIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

}
