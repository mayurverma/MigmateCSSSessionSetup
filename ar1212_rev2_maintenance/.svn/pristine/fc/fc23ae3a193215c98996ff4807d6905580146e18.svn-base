//***********************************************************************************
/// \file
///
/// Debug Uart Driver default Mocks
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
#include "DebugUartDrv.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_DEBUG_UART_DRV_INIT)
    void DebugUartDrvInit(void)
    {
        mock().actualCall("DebugUartDrvInit");
    }
#endif
#if !defined(MOCK_DISABLE_DEBUG_UART_DRV_PUTC)
    void DebugUartDrvPutc(char txChar)
    {
        mock().actualCall("DebugUartDrvPutc")
            .withParameter("txChar", txChar);
    }
#endif
}
