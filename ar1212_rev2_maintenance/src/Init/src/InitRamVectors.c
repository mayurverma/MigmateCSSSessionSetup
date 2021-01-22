//******************************************************************************
/// \file
///
/// Exception/Interrupt vector table access for patches.
///
/// \defgroup init Initialisation Support
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
#include <string.h>
#include "Init.h"

//------------------------------------------------------------------------------
//                                  Imports
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

/// Number of entries in the Vector Table: MSP + 15 exceptions + 32 interrupts
#define INIT_VECTOR_TABLE_ENTRIES           (16 + 32)
/// Offset value used to convert IRQ number to offset on the Vector Table
#define INIT_IRQ_TO_VECTOR_TABLE_OFFSET     16

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
extern InitVectorTableHandlerT Image$$VECTORS$$Base[INIT_VECTOR_TABLE_ENTRIES];

/// Allocate the RAM vector table
/// - note MUST be aligned to a 256-byte boundary
__align(256) InitVectorTableHandlerT ramVectorTable[INIT_VECTOR_TABLE_ENTRIES];

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Relocate the default ROM Vector Table into RAM
///
/// Copy the contents of the default ROM vector table to RAM
/// and switch the VTOR register to point to the new location
///
/// \returns void
///
void InitRelocateVectorTable(void)
{
    InitVectorTableHandlerT* romVectorTable = Image$$VECTORS$$Base;

    // Copy the existing vector table into the new vector table
    (void)memcpy((void*)ramVectorTable, (const void*)romVectorTable, sizeof(ramVectorTable));

    // Update the core VTOR reg - a data-memory barrier is required to
    // ensure that any subsequent exceptions/interrupts use the new
    // vector table
    __DMB();
    SCB->VTOR = (uint32_t)ramVectorTable;
    __DMB();
}

/// Fix one IRQ entry to a new function
///
/// This function requires InitRelocateVectorTable to be run before
///
/// \param[in] irq                 Number for the IRQ signal to be replaced
/// \param[in] newIrqHandler       Address of the new ISR function
///
/// \returns Error status
/// \retval ERROR_SUCCESS   Executed correctly
/// \retval ERROR_SYSERR    Internal fault (incorrect parameters)
///
ErrorT InitInstallNewIrqHandler(IRQn_Type const irq, InitVectorTableHandlerT const newIrqHandler)
{
    ErrorT error = ERROR_SUCCESS;
    int32_t indexTable = ((int32_t)irq + INIT_IRQ_TO_VECTOR_TABLE_OFFSET);

    if ((INIT_VECTOR_TABLE_ENTRIES > indexTable) && (NULL != newIrqHandler))
    {
        // Replace the irq handler
        ramVectorTable[indexTable] = newIrqHandler;
    }
    else
    {
        error = ERROR_SYSERR;
    }

    return error;
}

/// @} endgroup init

