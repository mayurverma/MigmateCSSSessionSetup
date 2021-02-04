#if !defined(SRC__INIT__INC__INIT_H)
#define      SRC__INIT__INC__INIT_H
//******************************************************************************
/// \file
///
/// AR0820 initialisation support for Cortex-M0+ core. Provides MPU initialisation
/// and support routines.
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

#include <stdbool.h>

#include "Common.h"
#include "AR0820.h"

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------
/// Represents stack related information
typedef struct
{
    uint32_t* base;                            ///< Base Address of stack
    uint32_t sizeLongWords;                    ///< Size of system stack
    uint32_t stackGuardRegionSizeLongWords;    ///< Size of guard region at base of stack
} InitMpuStackConfigType;

/// Function pointer for the Vector Table
typedef void (* InitVectorTableHandlerT)(void);

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

// Initialises the processor MPU
extern void InitMpu(void);

// Enables or disables the patch region
extern void InitMpuEnablePatchRegion(bool const enable);

// Returns information associated with stack i.e. base, length and guard
extern ErrorT InitMpuGetStackInformation(InitMpuStackConfigType* stackConfig);

// Relocate the default ROM Vector Table into RAM
extern void InitRelocateVectorTable(void);

// Replace an exception or interrupt handler in the RAM vector table
extern ErrorT InitInstallNewIrqHandler(IRQn_Type const irq, InitVectorTableHandlerT const newIrqHandler);

/// @} endgroup init
#endif  // !defined(SRC__INIT__INC__INIT_H)

