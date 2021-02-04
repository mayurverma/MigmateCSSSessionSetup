//******************************************************************************
/// \file
/// \addtogroup init
/// Processor MPU initialisation support - MPU disabled
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

#include <stddef.h>

#include "Common.h"
#include "AR0820.h"
#include "Platform.h"
#include "Init.h"

//------------------------------------------------------------------------------
//                                  Imports
//------------------------------------------------------------------------------

// Pull-in linker-defined symbols
extern uint32_t Image$$MAIN_STACK$$ZI$$Base;
extern uint32_t Image$$MAIN_STACK$$ZI$$Length;

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

/// Initialises the Cortex-M0+ MPU
///
/// Disables the MPU.
///
/// \returns void
///
void InitMpu(void)
{
    // INFO: <PH>: We expect the core to have an MPU - however the ARM FVP M0+ doesn't!
    if (0U != MPU->TYPE)
    {
        // Disable the MPU first to ensure no side-effects, and force PRIVDEFENA
        // to zero to prevent privileged access to the background region
        __DMB();
        MPU->CTRL = 0U;
    }
}

/// Enables or disables access to the Patch RAM region
///
/// \param[in] enable   Flag to enable or disable access
/// \returns void
void InitMpuEnablePatchRegion(bool const enable)
{
    (void)enable;
}

/// Returns information associated with stack i.e. base, length and guard
///
/// param[out]   Pointer to memory to populate with info of type MpuStackConfigType
/// \returns Command success/failure
/// \retval  ERROR_SUCCESS     Command completed successfully
/// \retval  ERROR_INVAL       NULL pointer passed as input
///
ErrorT InitMpuGetStackInformation(InitMpuStackConfigType* stackConfig)
{
    ErrorT error = ERROR_SUCCESS;

    if (NULL == stackConfig)
    {
        error = ERROR_INVAL;
    }
    else
    {
        stackConfig->base = (uint32_t*)((uint32_t)&Image$$MAIN_STACK$$ZI$$Base);
        stackConfig->sizeLongWords = ((uint32_t)&Image$$MAIN_STACK$$ZI$$Length) / sizeof(uint32_t);
        stackConfig->stackGuardRegionSizeLongWords = 0U;
    }

    return error;
}
/// @} endgroup init

