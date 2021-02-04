//******************************************************************************
/// \file
/// \defgroup hardfault HardFault handler integration test
/// Integration test for the Init component HardFault handler
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
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "AR0820.h"
#include "Diag.h"
#include "crypto_otpm.h"
#include "Init.h"
#include "OtpmDrv.h"
#include "SystemDrv.h"
#include "Interrupts.h"

// Stubs to reduce linkage scope
void DiagSetCheckpointWithInfo(DiagCheckpointT const checkpoint, uint16_t info)
{
    (void)checkpoint;
    (void)info;
}

void DiagSetCheckpointWithInfoUnprotected(DiagCheckpointT const checkpoint, uint16_t info)
{
    (void)checkpoint;
    (void)info;
}

bool hasCC312AbortOccurred = false;

// Stub to reduce linkage scope
bool SystemMgrHasCC312AbortOccurred(void)
{
    return hasCC312AbortOccurred;
}

void DiagSetCheckpoint(DiagCheckpointT const checkpoint)
{
    SystemDrvSetCheckpoint((uint16_t)checkpoint);
}

#if TEST_CASE == 5
#define DX_NVM_IS_IDLE_REG_OFFSET 		0x1F10UL
#define CRYPTO_CCORE_PERIPHERALS_BASE   0x40000000UL

volatile uint32_t loops = 0;

// Waits for CC312 to initialise and allow access to the OTPM registers
void wait_for_cc312_initialization(void)
{
    volatile uint32_t* nvm_is_idle = (volatile uint32_t*)(DX_NVM_IS_IDLE_REG_OFFSET + CRYPTO_CCORE_PERIPHERALS_BASE);
    uint32_t reg = 0;
    printf("!!! Waiting for CC312...");

    do
    {
		// poll NVM register to assure that the NVM boot is finished (and LCS and the keys are valid)
		reg = *nvm_is_idle;
		printf("(%x = %x)", DX_NVM_IS_IDLE_REG_OFFSET + CRYPTO_CCORE_PERIPHERALS_BASE, reg);
    }
    while (0 == reg);
}
#endif

int hardfault_test(void)
{
#if TEST_CASE == 1 || TEST_CASE == 2 || TEST_CASE == 3 || TEST_CASE == 4
    void (*badfunc)(void) = (void (*)(void))0x5678;
#endif
    printf("HardFault integration test running\n");

    // There are few HardFault causes handled by the HardFault Handler. Each
    // results in termination of the firmware, so we can only trigger one
    // each run.
    //
    // 1. Processor RAM ECC DED
    // 2. Processor ROM ECC DED
    // 3. Invalid Thumb state
    // 4. Invalid processor state
    // 5. OTPM ECC DED
    // 6. Unknown
    //
    // 7. Additionally we test for the 'unexpected' interrupt case which
    // also results in termination of the firmware
    //
    // 8. We also test for the 'CC312 software abort' case which triggers
    // a PendSV exception.
    // 9. Stack Overflow (MPU interrupt)

#ifndef TEST_CASE
#define TEST_CASE 1
#endif

#if TEST_CASE == 1
    // Case 1: Processor RAM ECC
    // - force the CyberRamDed_IRQn
    printf("TEST_CASE = 1, expected CRYPTOERROR = 0x3c00\n");
    __NVIC_SetPendingIRQ(CyberRamDed_IRQn);
    badfunc();
#endif

#if TEST_CASE == 2
    // Case 2: Processor ROM ECC DED
    // - force the CyberRomDed_IRQn
    printf("TEST_CASE = 2, expected CRYPTOERROR = 0x3800\n");
    __NVIC_SetPendingIRQ(CyberRomDed_IRQn);
    badfunc();
#endif

#if TEST_CASE == 3
    // Case 3: Invalid Thumb state
    // - generate a HardFault by calling a function without the Thumb bit set
    printf("TEST_CASE = 3, expected CRYPTOERROR = 0x2c02\n");
    hasCC312AbortOccurred = false;
    badfunc();
#endif

#if TEST_CASE == 4
    // Case 4: Invalid processor state
    // - switch to the process stack, then force a HardFault
    printf("TEST_CASE = 4, expected CRYPTOERROR = 0x2c01\n");
    #error TODO
#endif

#if TEST_CASE == 5
    // Case 5: OTPM ECC DED
    // - Generate a HardFault by forcing ECC hard fault, then reading OTPM
    printf("TEST_CASE = 5, expected CRYPTOERROR = 0x202c\n");
    {
        uint32_t value;

        // Wait for the CC312 to initialise before accessing OTPM
        wait_for_cc312_initialization();

        // IMAGINGPRJ-3501: Writes to OTPM control register are not working
        // - workaround CC312 read-modify-write behaviour by writing current
        //   register contents back to clear the register, then write the new
        //   value
        value = OTPM_REGS->core_ecc_flags;
        OTPM_REGS->core_ecc_flags = value;
        OTPM_REGS->core_ecc_flags = CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK;
        value = OTPM_REGS->core_ecc_flags;
        printf("core_ecc_flags = 0x%08x\n", value);

        // Read from the OTPM 'user' area
        (void)OtpmDrvRead(0x2C, &value, 1);
    }
#endif

#if TEST_CASE == 6
    // Case 6: Unknown
    // - Force a HardFault via an unaligned access
    printf("TEST_CASE = 6, expected CRYPTOERROR = 0x3000\n");
    {
        uint16_t* data_ptr = (uint16_t*)0x1233;
        printf("data_ptr[0] = 0x%08x\n", data_ptr[0]);
    }
#endif

#if TEST_CASE == 7
    printf("TEST_CASE = 7, expected CRYPTOERROR = 0x341f\n");
    // Case 7: Generate an unexpected interrupt
    {
        InterruptsEnableInterrupt(INTERRUPT_RESERVED_31, true);
        __NVIC_SetPendingIRQ(Reserved31_IRQn);
    }
#endif

#if TEST_CASE == 8
    // Case 8: CC312 software abort
    // - set the aborted flag and trigger PendSV
    printf("TEST_CASE = 8, expected CRYPTOERROR = 0x1000\n");
    hasCC312AbortOccurred = true;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
#endif

#if TEST_CASE == 9
    // Case 9: Generate stack overflow
    printf("TEST_CASE = 9, expected CRYPTOERROR = 0x4000 when accessing bottom of the stack (0x2000F000)\n");
    uint32_t dummy_array[1024];
    for(uint32_t i = 0; i < 1024; i += 1)
    {
        printf("--- Accessing Memory address %x\n", &dummy_array[i]);
        dummy_array[i] = i;
    }

#endif

    return 0;
}

/// @} endgroup hardfault

