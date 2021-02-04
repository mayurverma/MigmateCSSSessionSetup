//***********************************************************************************
/// \file
///
/// Interrupts/exceptions support
///
/// \addtogroup platform
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

#include "Common.h"
#include "AR0820.h"
#include "Interrupts.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Initialize the Interrupts support
/// \returns Error status
/// \retval ERROR_SUCCESS   Interrupts initialized successfully
///
ErrorT InterruptsInit(void)
{
    return ERROR_SUCCESS;
}

/// Enable all interrupts
/// \returns void
///
void InterruptsEnableAll(void)
{
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
    __enable_irq();

    // Force a pending interrupt to be taken by flushing the instruction pipeline
    __ISB();
#endif
}

/// Disable all interrupts
/// \returns void
///
void InterruptsDisableAll(void)
{
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
    __disable_irq();
#endif
}

/// Enable or disable specified interrupt
///
/// \param[in]  interrupt   Interrupt to enable/disable
/// \param[in]  enable      Flag to enable or disable
/// \returns    Error status
/// \retval     ERROR_SUCCESS   Interrupt enabled/disabled successfully
/// \retval     ERROR_INVAL     Invalid interrupt
///
ErrorT InterruptsEnableInterrupt(InterruptT const interrupt, bool const enable)
{
    ErrorT status = ERROR_INVAL;

    if (INTERRUPT_MAX >= interrupt)
    {
        if (enable)
        {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
            __NVIC_EnableIRQ((IRQn_Type)interrupt);
#endif
        }
        else
        {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
            __NVIC_DisableIRQ((IRQn_Type)interrupt);
#endif
        }
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Tests if the specified interrupt is pending
///
/// \param[in]  interrupt   Interrupt to test
/// \param[out] isPending   Flag to indicate if pending
/// \returns    Error status
/// \retval     ERROR_SUCCESS   Interrupt tested successfully
/// \retval     ERROR_INVAL     Invalid interrupt
///
ErrorT InterruptsIsInterruptPending(InterruptT const interrupt, bool* isPending)
{
    ErrorT status = ERROR_INVAL;

    if ((INTERRUPT_MAX >= interrupt) && isPending)
    {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
        *isPending = (0U != __NVIC_GetPendingIRQ((IRQn_Type)interrupt));
#endif
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Clear the specified pending interrupt
///
/// \param[in]  interrupt   Interrupt to clear
/// \returns    Error status
/// \retval     ERROR_SUCCESS   Interrupt cleared successfully
/// \retval     ERROR_INVAL     Invalid interrupt
///
ErrorT InterruptsClearInterruptPending(InterruptT const interrupt)
{
    ErrorT status = ERROR_INVAL;

    if (INTERRUPT_MAX >= interrupt)
    {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
        __NVIC_ClearPendingIRQ((IRQn_Type)interrupt);
#endif
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Set priority of specified interrupt
///
/// \param[in]  interrupt   Interrupt to set
/// \param[in]  priority    Priority to set
/// \returns    Error status
/// \retval     ERROR_SUCCESS   Interrupt priority set successfully
/// \retval     ERROR_INVAL     Invalid interrupt
///
ErrorT InterruptsSetInterruptPriority(InterruptT const interrupt,
    InterruptPriorityT const priority)
{
    ErrorT status = ERROR_INVAL;

    if (INTERRUPT_MAX >= interrupt)
    {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
        __NVIC_SetPriority((IRQn_Type)interrupt, (uint32_t)priority);
#else
        (void)priority;
#endif
        status = ERROR_SUCCESS;
    }

    return status;
}

/// Set priority of specified exception
///
/// \param[in]  exception   Exception to set
/// \param[in]  priority    Priority to set
/// \returns    Error status
/// \retval     ERROR_SUCCESS   Exception priority set successfully
/// \retval     ERROR_INVAL     Invalid exception
///
ErrorT InterruptsSetExceptionPriority(ExceptionT const exception, InterruptPriorityT const priority)
{
    ErrorT status = ERROR_INVAL;

    if (EXCEPTION_MAX <= exception)
    {
#if defined(BUILD_TARGET_ARCH_cortex_m0plus)
        __NVIC_SetPriority((IRQn_Type)exception, (uint32_t)priority);
#else
        (void)priority;
#endif
        status = ERROR_SUCCESS;
    }

    return status;
}

// @} endgroup platform

