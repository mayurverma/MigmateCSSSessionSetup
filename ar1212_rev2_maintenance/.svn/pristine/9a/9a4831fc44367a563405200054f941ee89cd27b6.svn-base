//***********************************************************************************
/// \file
///
/// Debug Uart driver
///
/// \addtogroup debuguartdrv
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

#include "AR0820.h"
#include "Common.h"
#include "debug_uart.h"
#include "DebugUartDrv.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
#define DEBUG_UART_BAUD_DIV_TEST            (100U)         // Non-asic build, so a non-standard baud-rate is OK

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

/// Initialize the debug UART to 156Mbps, 8, n, 1 (debug non-asic builds)
/// \returns void
///
void DebugUartDrvInit(void)
{
    // set the baud rate
    DEBUG_UART_REGS->bauddiv = DEBUG_UART_BAUD_DIV_TEST;

    // Enable the UART TX. Hi-Speed test mode (baud rate = clk). Note that there is no usable uart on silicon.
    DEBUG_UART_REGS->ctrl = DEBUG_UART_CTRL_TX_EN__MASK | DEBUG_UART_CTRL_HS_TEST_MODE__MASK;
}

/// Transmit a single char from the debug UART
/// Function will block if it has to wait for space in Tx Buffer
/// \param[in] txChar       The 8-bit char to be transmitted
/// \returns void
///
void DebugUartDrvPutc(char txChar)
{
    // wait if the Tx buffer is full
    while ((DEBUG_UART_REGS->state & DEBUG_UART_STATE_TX_BUFF_FULL__MASK) != 0U)
    {
        // Don't need a timeout. The Tx write buffer can only hold one char, so the longest ever wait
        // time would be for 2 chars to transmit. THere is no HW handshaking,
        // so no holdups are possible, assuming DebugUartDrvInit() has already been called....
    }
    // write the char to the HW
    DEBUG_UART_REGS->data = (uint32_t)txChar;
}

/// @} endgroup debuguartdrv

