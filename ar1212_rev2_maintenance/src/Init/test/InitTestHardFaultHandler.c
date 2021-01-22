//******************************************************************************
/// \file
///
/// Test Hard Fault handler
///
/// \addtogroup init
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************

#include <stdio.h>

#include "Common.h"
#include "AR0820.h"
#include "Diag.h"
#include "SystemMgr.h"

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

extern volatile uint32_t initTestNumHardFaults;
extern volatile bool initTestDataFaultExpected;
extern volatile bool initTestInstructionFaultExpected;
extern volatile bool initTestThumbFaultExpected;

/// Handles a hard fault (invoked by assembler wrapper)
///
/// \param[in] FramePtr 	Points to the stacked exception frame
/// \param[in] LinkReg  	The current LR value
/// \param[in] ControlReg	The current CONTROL value
/// \returns void
///
void InitHardFaultHandler(uint32_t* FramePtr, uint32_t LinkReg, uint32_t ControlReg)
{
    xPSR_Type* xPSR = (xPSR_Type*)&FramePtr[7];

    DiagPrintf("!!! HARDFAULT !!!\n");
    DiagPrintf("R0  = 0x%08lx R1 = 0x%08lx R2 = 0x%08lx R3   = 0x%08lx\n",
        FramePtr[0], FramePtr[1], FramePtr[2], FramePtr[3]);
    DiagPrintf("R12 = 0x%08lx LR = 0x%08lx PC = 0x%08lx xPSR = 0x%08lx\n",
        FramePtr[4], FramePtr[5], FramePtr[6], FramePtr[7]);
    DiagPrintf("currLR = 0x%08lx\n", LinkReg);
    DiagPrintf("Exception# = 0x%08x\n", xPSR->b.ISR);

    if (0 != ControlReg)
    {
        DiagPrintf("Control = 0x%08x\n", ControlReg);
    }

    if (0 == xPSR->b.T)
    {
        DiagPrintf("Thumb bit clear, wrong ARM state\n");
    }

    if (initTestDataFaultExpected || initTestInstructionFaultExpected)
    {
    	initTestNumHardFaults += 1;

    	if (initTestDataFaultExpected)
    	{
        	initTestDataFaultExpected = false;

        	// return to the instruction after the faulting access
        	// - we modify the stacked return address
        	// - FRAMEPTR[6] is the stacked PC which points to the instruction which caused the exception
        	FramePtr[6] = FramePtr[6] + 2;
    	}
    	else
    	{
    		initTestInstructionFaultExpected = false;

    		// return to instruction following the address of the exception
    		// - we modify the stacked return address
    		// - FRAMEPTR[5] is the stacked LR which points the instruction after the BLX (with Thumb bit set)
    		// - FRAMEPTR[6] is the stacked PC which indicates the address which generated the fault
    		FramePtr[6] = FramePtr[5] & ~1;
    	}
    }

    if (initTestThumbFaultExpected)
    {
    	initTestThumbFaultExpected = false;

    	// Set the Thumb bit in the stacked xPSR to allow return to the test
    	xPSR->b.T = 1;
    }

    // return
}

/// @} endgroup init

