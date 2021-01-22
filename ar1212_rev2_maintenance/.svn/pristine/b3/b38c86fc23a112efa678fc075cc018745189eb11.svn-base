//***********************************************************************************
/// \file
///
/// Interrupts unit test
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
#include "AR0820.h"
#include "Interrupts.h"
}

// Mocks
extern "C"
{

int doorbell_counter = 0;
int sof_counter = 0;
int pendsv_counter = 0;
int systick_counter = 0;
IRQn_Type last_handler = Reset_IRQn;
bool verbose = false;

void Doorbell_IrqHandler(void)
{
    if (verbose) printf("Doorbell_IrqHandler %d\n", doorbell_counter);
    doorbell_counter += 1;
    last_handler = Doorbell_IRQn;
    bool isPending;
    // The pending bit should be cleared automatically when the ISR starts
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(false, isPending);
}

void SOF_IrqHandler(void)
{
    if (verbose) printf("SOF_IrqHandler %d\n", sof_counter);
    sof_counter += 1;
    last_handler = SOF_IRQn;
    bool isPending;
    // The pending bit should be cleared automatically when the ISR starts
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_SOF, &isPending));
    CHECK_EQUAL(false, isPending);
}

void PendSV_Handler(void)
{
    if (verbose) printf("PendSV_Handler %d\n", pendsv_counter);
    pendsv_counter += 1;
    last_handler = PendSV_IRQn;
}

void SysTick_Handler(void)
{
    if (verbose) printf("SysTick_Handler %d\n", systick_counter);
    systick_counter += 1;
    last_handler = SysTick_IRQn;
}




void InterruptsSetInterruptPending(InterruptT const interrupt)
{
    CHECK_TRUE(INTERRUPT_MAX >= interrupt)
    __NVIC_SetPendingIRQ((IRQn_Type)interrupt);
}

} // extern "C"
TEST_GROUP(InterruptsGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        InterruptsEnableAll();
        doorbell_counter = 0;
        sof_counter = 0;
        pendsv_counter = 0;
        systick_counter = 0;
        last_handler = Reset_IRQn;
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, false));
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, false));
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsInit());
        if (verbose) printf("Setup Test.................\n");
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, false));
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, false));
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_PENDSV, INTERRUPT_PRIORITY_0));
        CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_SYSTICK, INTERRUPT_PRIORITY_0));
    }
};

TEST(InterruptsGroup, TestEnabledInterrupt)
{
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true));

    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(1, doorbell_counter);

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, true));

    InterruptsSetInterruptPending(INTERRUPT_SOF);
    CHECK_EQUAL(1, sof_counter);

}

TEST(InterruptsGroup, TestDisabledInterrupt)
{
    bool isPending;

    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(0, doorbell_counter);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(true, isPending);

    // Clear interrupt to leave it in a clean state
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsClearInterruptPending(INTERRUPT_DOORBELL));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(false, isPending);
    CHECK_EQUAL(0, doorbell_counter);
}

TEST(InterruptsGroup, TestDisabledAllInterrupt)
{
    bool isPending;

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, true));
    InterruptsDisableAll();
    InterruptsSetInterruptPending(INTERRUPT_SOF);
    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(0, doorbell_counter);
    CHECK_EQUAL(0, sof_counter);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_SOF, &isPending));
    CHECK_EQUAL(true, isPending);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(true, isPending);

    InterruptsEnableAll();

    CHECK_EQUAL(1, doorbell_counter);
    CHECK_EQUAL(1, sof_counter);
}

TEST(InterruptsGroup, TestPrioritiesInterrupt)
{
    bool isPending;

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_DOORBELL, INTERRUPT_PRIORITY_1));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_SOF, INTERRUPT_PRIORITY_2));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, true));

    InterruptsDisableAll();
    InterruptsSetInterruptPending(INTERRUPT_SOF);
    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(0, doorbell_counter);
    CHECK_EQUAL(0, sof_counter);

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(true, isPending);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_SOF, &isPending));
    CHECK_EQUAL(true, isPending);

    //Enable now all at the same time and check sof_handler is last executed
    InterruptsEnableAll();

    CHECK_EQUAL(1, doorbell_counter);
    CHECK_EQUAL(1, sof_counter);
    CHECK_EQUAL(SOF_IRQn, last_handler);

    //Now test swapping priorities The interrupt needs to be disabled first accoring to the documentation in the M0+ guide
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_DOORBELL, INTERRUPT_PRIORITY_2));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_SOF, INTERRUPT_PRIORITY_1));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, true));

    InterruptsDisableAll();
    InterruptsSetInterruptPending(INTERRUPT_SOF);
    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(1, doorbell_counter);
    CHECK_EQUAL(1, sof_counter);

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(true, isPending);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_SOF, &isPending));
    CHECK_EQUAL(true, isPending);

    //Enable now all at the same time and check sof_handler is last executed
    InterruptsEnableAll();

    CHECK_EQUAL(2, doorbell_counter);
    CHECK_EQUAL(2, sof_counter);
    CHECK_EQUAL(Doorbell_IRQn, last_handler);

    //Now test with the same priority. The interrupt needs to be disabled first accoring to the documentation in the M0+ guide
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, false));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_DOORBELL, INTERRUPT_PRIORITY_2));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetInterruptPriority(INTERRUPT_SOF, INTERRUPT_PRIORITY_2));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_DOORBELL, true));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsEnableInterrupt(INTERRUPT_SOF, true));

    InterruptsDisableAll();
    InterruptsSetInterruptPending(INTERRUPT_SOF);
    InterruptsSetInterruptPending(INTERRUPT_DOORBELL);
    CHECK_EQUAL(2, doorbell_counter);
    CHECK_EQUAL(2, sof_counter);

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_DOORBELL, &isPending));
    CHECK_EQUAL(true, isPending);
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsIsInterruptPending(INTERRUPT_SOF, &isPending));
    CHECK_EQUAL(true, isPending);

    //Enable now all at the same time and check sof_handler is last executed
    InterruptsEnableAll();

    CHECK_EQUAL(3, doorbell_counter);
    CHECK_EQUAL(3, sof_counter);
    CHECK_EQUAL(SOF_IRQn, last_handler); // This is latest because is the higest exception number

}

void SetPendingSysTick(bool enable)
{
    if (enable)
    {
        SCB->ICSR |= SCB_ICSR_PENDSTSET_Msk;
    }
    else
    {
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    }
}

void SetPendingPendSV(bool enable)
{
    if (enable)
    {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
    else
    {
        SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
    }
}

TEST(InterruptsGroup, TestPrioritiesExceptions)
{
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_PENDSV, INTERRUPT_PRIORITY_1));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_SYSTICK, INTERRUPT_PRIORITY_2));
    InterruptsDisableAll();
    SetPendingSysTick(true);
    SetPendingPendSV(true);
    //Enable now all at the same time and check sof_handler is last executed
    InterruptsEnableAll();

    CHECK_EQUAL(1, pendsv_counter);
    CHECK_EQUAL(1, systick_counter);
    CHECK_EQUAL(SysTick_IRQn, last_handler);

    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_PENDSV, INTERRUPT_PRIORITY_2));
    CHECK_EQUAL(ERROR_SUCCESS, InterruptsSetExceptionPriority(EXCEPTION_SYSTICK, INTERRUPT_PRIORITY_1));
    InterruptsDisableAll();
    SetPendingSysTick(true);
    SetPendingPendSV(true);
    //Enable now all at the same time and check sof_handler is last executed
    InterruptsEnableAll();

    CHECK_EQUAL(2, pendsv_counter);
    CHECK_EQUAL(2, systick_counter);
    CHECK_EQUAL(PendSV_IRQn, last_handler);

}

