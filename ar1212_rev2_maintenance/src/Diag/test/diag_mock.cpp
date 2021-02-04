//***********************************************************************************
/// \file
///
/// Diagnostics default Mocks
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
#include "Diag.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_DIAG_SET_CHECKPOINT)
    void DiagSetCheckpoint(DiagCheckpointT const checkpoint)
    {
        mock().actualCall("DiagSetCheckpoint")
            .withParameter("checkpoint", checkpoint);

        printf("CHECKPOINT: 0x%04x (%d)\n", checkpoint, checkpoint);
    }
#endif
#if !defined(MOCK_DISABLE_DIAG_SET_CHECKPOINT_WITH_INFO)
    void DiagSetCheckpointWithInfo(DiagCheckpointT const checkpoint, uint16_t info)
    {
        mock().actualCall("DiagSetCheckpointWithInfo")
            .withParameter("checkpoint", checkpoint)
            .withParameter("info", info);

        printf("INFO CHECKPOINT: 0x%04x (%d):0x%04x\n", checkpoint, checkpoint, info);
    }
#endif
#if !defined(MOCK_DISABLE_DIAG_SET_CHECKPOINT_WITH_INFO_UNPROTECTED)
    void DiagSetCheckpointWithInfoUnprotected(DiagCheckpointT const checkpoint, uint16_t info)
    {
        mock().actualCall("DiagSetCheckpointWithInfoUnprotected")
            .withParameter("checkpoint", checkpoint)
            .withParameter("info", info);

        printf("INFO CHECKPOINT (UPROTECTED): 0x%04x (%d):0x%04x\n", checkpoint, checkpoint, info);
    }
#endif
#if !defined(MOCK_DISABLE_DIAG_ON_FATAL_ERROR)
    void DiagOnFatalError(void)
    {
        mock().actualCall("DiagOnFatalError");
    }
#endif
#if !defined(MOCK_DISABLE_DIAG_REGISTER_FATAL_ERROR_DUMP_HANDLER)
    ErrorT DiagRegisterFatalErrorDumpHandler(DiagFatalErrorDumpHandlerT handler)
    {
        mock().actualCall("DiagRegisterFatalErrorDumpHandler")
            .withFunctionPointerParameter("handler", (void (*)(void))handler);
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_DIAG_ENABLE_INFO)
    void  DiagEnableInfo(bool const enable)
    {
        mock().actualCall("DiagEnableInfo")
            .withParameter("enable", enable);
    }
#endif
}
