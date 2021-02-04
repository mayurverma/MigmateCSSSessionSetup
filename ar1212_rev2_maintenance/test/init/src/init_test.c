//******************************************************************************
/// \file
/// \defgroup inittest Init integration test
/// Integration test for the Init component
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
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "crypto_icore.h"
#include "Init.h"

uint8_t uint8_uninit;
uint16_t uint16_uninit;
uint32_t uint32_uninit;

#define UINT8_INIT_VAL  0xabU
#define UINT16_INIT_VAL 0xcdefU
#define UINT32_INIT_VAL 0x12345678UL

#define INT8_INIT_VAL   -16
#define INT16_INIT_VAL  -1024
#define INT32_INIT_VAL  -12345678L

uint8_t uint8_init = UINT8_INIT_VAL;
uint16_t uint16_init = UINT16_INIT_VAL;
uint32_t uint32_init = UINT32_INIT_VAL;

static int8_t int8_static;
static int16_t int16_static;
static int32_t int32_static;

int8_t int8_init = INT8_INIT_VAL;
int16_t int16_init = INT16_INIT_VAL;
int32_t int32_init = INT32_INIT_VAL;

uint32_t uint32_array[] =
{
    0x12343210, 0x56787654, 0x9abcba98, 0xdef0fedc,
    0xaa5555aa, 0x16611661, 0x27727722, 0x55aaaa55
};

// Pull-in linker-defined symbols
extern unsigned int Image$$CODE$$Base;
extern unsigned int Image$$PATCH$$Base;
extern unsigned int Image$$DATA$$Base;
extern unsigned int Image$$HEAP$$Base;
extern unsigned int Image$$MAIN_STACK$$Base;
extern unsigned int Image$$MAIN_STACK$$Limit;
extern unsigned int Image$$ICORE_REGISTERS$$Base;
extern unsigned int Image$$SHARED_MEMORY$$Base;

#define CHECKPOINT_CHECK_CPUID			0x10
#define CHECKPOINT_CHECK_GLOBALS    	0x11
#define CHECKPOINT_CHECK_STACK          0x12
#define CHECKPOINT_CHECK_HEAP           0x13
#define CHECKPOINT_CHECK_MALLOC    		0x14
#define CHECKPOINT_CHECK_CALLOC    		0x15
#define CHECKPOINT_CHECK_HARDFAULT    	0x16
#define CHECKPOINT_CHECK_MPU    		0x17
#define CHECKPOINT_CHECK_VTOR           0x18

#define CHECKPOINT_TEST_PASS			0xAAAA

// Set the CHECKPOINT and INFO registers
void set_checkpoint_and_info(uint16_t const checkpoint, uint16_t const info)
{
	CRYPTO_ICORE_REGS->checkpoint = checkpoint;
	CRYPTO_ICORE_REGS->info = info;
}

ErrorT set_cryptoerror(uint16_t const test_id, uint16_t const context)
{
    CRYPTO_ICORE_REGS->cryptoerror = ((uint32_t)test_id << 10) | (context & 0x3FF);
    return ERROR_SYSERR;
}

// Checks the globals have been initialized as expected
int check_globals(void)
{
    if (0 != uint8_uninit) return 1;
    if (0 != uint16_uninit) return 2;
    if (0 != uint32_uninit) return 3;

    if (UINT8_INIT_VAL != uint8_init) return 4;
    if (UINT16_INIT_VAL != uint16_init) return 5;
    if (UINT32_INIT_VAL != uint32_init) return 6;

    if (0 != int8_static) return 7;
    if (0 != int16_static) return 8;
    if (0 != int32_static) return 9;

    if (INT8_INIT_VAL != int8_init) return 10;
    if (INT16_INIT_VAL != int16_init) return 11;
    if (INT32_INIT_VAL != int32_init) return 12;

    if (0x12343210 != uint32_array[0]) return 13;
    if (0x56787654 != uint32_array[1]) return 14;
    if (0x9abcba98 != uint32_array[2]) return 15;
    if (0xdef0fedc != uint32_array[3]) return 16;
    if (0xaa5555aa != uint32_array[4]) return 17;
    if (0x16611661 != uint32_array[5]) return 18;
    if (0x27727722 != uint32_array[6]) return 19;
    if (0x55aaaa55 != uint32_array[7]) return 20;

    return 0;
}

#define CPUID_IMPLEMENTER   0x41    // ARM
#define CPUID_VARIANT_2     0x2     // Revision 2
#define CPUID_VARIANT_0     0x0     // Revision 0
#define CPUID_CONSTANT_V7M  0xF     // Cortex V7-M architecture
#define CPUID_CONSTANT_V6M  0xC     // Cortex V6-M architecture
#define CPUID_PARTNO_M3     0xC23   // Cortex-M3
#define CPUID_PARTNO_M0P    0xC60   // Cortex-M0+
#define CPUID_REVISION_1    0x1     // Patch 1
#define CPUID_REVISION_0    0x0     // Patch 0

#define CPUID_EXPECTED_M3   ((CPUID_IMPLEMENTER << SCB_CPUID_IMPLEMENTER_Pos)   | \
                            (CPUID_VARIANT_2    << SCB_CPUID_VARIANT_Pos)       | \
                            (CPUID_CONSTANT_V7M << SCB_CPUID_ARCHITECTURE_Pos)  | \
                            (CPUID_PARTNO_M3    << SCB_CPUID_PARTNO_Pos)        | \
                            (CPUID_REVISION_1   << SCB_CPUID_REVISION_Pos))

#define CPUID_EXPECTED_M0P  ((CPUID_IMPLEMENTER << SCB_CPUID_IMPLEMENTER_Pos)   | \
                            (CPUID_VARIANT_0    << SCB_CPUID_VARIANT_Pos)       | \
                            (CPUID_CONSTANT_V6M << SCB_CPUID_ARCHITECTURE_Pos)  | \
                            (CPUID_PARTNO_M0P   << SCB_CPUID_PARTNO_Pos)        | \
                            (CPUID_REVISION_1   << SCB_CPUID_REVISION_Pos))

// Checks CPUID matches the expected value
int check_CPUID(uint32_t expected)
{
	printf("Checking CPUID...\n");

    uint32_t cpuid = SCB->CPUID;

    if (CPUID_EXPECTED_M3 == cpuid)
    {
        printf("Running on Cortex-M3\n");
    }
    else if (CPUID_EXPECTED_M0P == cpuid)
    {
        printf("Running on Cortex-M0+\n");
    }
    else
    {
        printf("Unexpected CPUID: 0x%08lx\n", cpuid);
    }

    if (expected != cpuid)
    {
        return 1;
    }

    return 0;
}

#define EXPECTED_STACK_VALUE 0xfeedface

// Check the stack has been initialized
int check_stack(void)
{
	printf("Check stack initialization...\n");

    uint32_t* stack_base;
    InitMpuStackConfigType stackConfig;
    uint32_t  guard_region_size_long_words;

    (void)InitMpuGetStackInformation(&stackConfig);
    stack_base = stackConfig.base;
    guard_region_size_long_words = stackConfig.stackGuardRegionSizeLongWords;

    // Check the bottom 1024 bytes of the stack
    for (int idx = guard_region_size_long_words;
        idx < (guard_region_size_long_words + 256);
        idx++)
    {
        if (stack_base[idx] != EXPECTED_STACK_VALUE)
        {
            printf("Stack @ 0x%08x not initialized: 0x%08x\n",
                &stack_base[idx], stack_base[idx]);
            return 1;
        }
    }

    return 0;
}

#define EXPECTED_HEAP_VALUE 0xcafebabe

// Check the heap has been initialized
int check_heap(void)
{
	printf("Check heap initialization...\n");

    uint32_t * heap_base = (uint32_t *)&Image$$HEAP$$Base;

    // Check somewhere in the middle of the heap
    for (int idx = 1024; idx < 1280; idx++)
    {
        if (heap_base[idx] != EXPECTED_HEAP_VALUE)
        {
            printf("Heap @ 0x%08x not initialized: 0x%08x\n",
                &heap_base[idx], heap_base[idx]);
            return 1;
        }
    }

    return 0;
}

// Checks malloc/free
int check_malloc(void)
{
	printf("Testing malloc/free...\n");

    uint32_t * mem_region = (uint32_t *)malloc(1024 * sizeof(uint32_t));
    if (NULL == mem_region)
    {
        return 1;
    }

    for (int idx=0; idx<1024; idx++)
    {
        mem_region[idx] = 1024U - idx;
    }

    for (int idx=0; idx<1024; idx++)
    {
        if (mem_region[idx] != 1024U - idx)
        {
            return 2;
        }
    }

    free(mem_region);
    return 0;
}

// Checks calloc against the available heap
int check_calloc(void)
{
	printf("Testing calloc/free...\n");

    uint32_t size;
    const uint32_t heap_base = (uint32_t) &Image$$HEAP$$Base;
    const uint32_t heap_limit = (uint32_t) &Image$$MAIN_STACK$$Base;
    const uint32_t heap_size = heap_limit - heap_base;
    uint32_t * mem_region;
    uint32_t * mem_region_2;

    printf("heap_base %lx\n", heap_base);
    printf("heap_size %lx\n", heap_size);

    size = heap_size / sizeof(uint32_t);

    // Check calloc with size too big
    mem_region = (uint32_t *)calloc(size, sizeof(uint32_t));
    if (NULL != mem_region)
    {
        return 1;
    }
    // Check calloc with the full size
    size -= 36;
    mem_region = (uint32_t *)calloc(size, sizeof(uint32_t));
    if (NULL == mem_region)
    {
        return 2;
    }
    if (((uint32_t)mem_region < heap_base) || ((uint32_t)mem_region > (heap_base + heap_size)))
    {
        return -3;
    }
    for (uint32_t idx=0; idx < size; idx++)
    {
        if (mem_region[idx] != 0)
        {
            return 4;
        }
    }
    free(mem_region);

    // try again to allocate memory
    mem_region = (uint32_t *)calloc(size/2, sizeof(uint32_t));
    if (NULL == mem_region)
    {
        return 5;
    }
    mem_region_2 = (uint32_t *)calloc(size/2, sizeof(uint32_t));
    if (NULL == mem_region_2)
    {
        return 6;
    }
    if (((uint32_t)mem_region_2 < heap_base) || ((uint32_t)mem_region_2 > (heap_base + heap_size)))
    {
        return 7;
    }

    free(mem_region);
    free(mem_region_2);

    return 0;
}

// Variables to control the test-mode hard-fault handler
volatile uint32_t initTestNumHardFaults;
volatile bool initTestDataFaultExpected;
volatile bool initTestInstructionFaultExpected;
volatile bool initTestThumbFaultExpected;
volatile uint32_t initTestVal;

// Tests the HardFault handler
int test_hardfault_handler(void)
{
	void (*badFunc)(void) = NULL;
	uint16_t * badFuncMem;

	printf("Testing HardFault handler...\n");

    // Generate a ARM Thumb state fault
    initTestNumHardFaults = 0;
    initTestDataFaultExpected = false;
    initTestInstructionFaultExpected = true;
    initTestThumbFaultExpected = true;

    void (*func_ptr)(void);
    uint32_t * func_addr = (uint32_t *)0xF204;
    func_ptr = (void (*)(void))func_addr;
    func_ptr();

    if (1 != initTestNumHardFaults)
    {
    	printf("Bad thumb-state function call did not generate a HardFault\n");
    	return 1;
    }

    // Generate an unaligned exception
    initTestDataFaultExpected = true;
    uint16_t * data_ptr = (uint16_t *)0x1233;
    printf("data_ptr[0] = 0x%08x\n", data_ptr[0]);

    if (2 != initTestNumHardFaults)
    {
    	printf("Unaligned access did not generate a HardFault\n");
    	return 2;
    }

    // Generate an illegal instruction exception
    initTestInstructionFaultExpected = true;
  	badFuncMem = (uint16_t *)malloc(4);
    if (NULL == badFuncMem)
    {
    	printf("Failed to malloc badFuncT\n");
    	return 3;
    }

    badFuncMem[0] = 0x2000;	// MOV R0, #0
    badFuncMem[1] = 0xBB00;	// Illegal instruction
    badFuncMem[2] = 0x4770;	// BX LR

    // Note: MUST set LSB as M0+ only operates in Thumb mode
    badFunc = (void (*)(void))((uint32_t)badFuncMem | 1);
    badFunc();

    free(badFuncMem);

    if (3 != initTestNumHardFaults)
    {
    	printf("Illegal instruction did not generate a HardFault\n");
    	return 4;
    }

    printf("HardFault handler test passed\n");
    return 0;
}

// Tests the MPU regions
int test_MPU(void)
{
	uint32_t codeBase = (uint32_t)&Image$$CODE$$Base;
	uint32_t codeEnd = 0x00040000UL;
	uint32_t patchBase = (uint32_t)&Image$$PATCH$$Base;
	uint32_t dataBase = (uint32_t)&Image$$DATA$$Base;
	uint32_t stackBase = (uint32_t)&Image$$MAIN_STACK$$Base;
	uint32_t stackEnd = (uint32_t)&Image$$MAIN_STACK$$Limit;
	uint32_t icoreBase = (uint32_t)&Image$$ICORE_REGISTERS$$Base;
	uint32_t shrMemBase = (uint32_t)&Image$$SHARED_MEMORY$$Base;
	uint32_t vCoreBase = (uint32_t)CRYPTO_VCORE_BASE;
	uint32_t peripheralEnd = 0x60000000UL;

	uint32_t * dataPtr;

	// Note MUST set the LSB as the M0 only operates in Thumb mode
	void (*funcPtr)(void) = (void (*)(void))(codeEnd | 1);

    printf("MPU Type: 0x%08lx\n", MPU->TYPE);

    //
    // Region 0: ROM
    //
    printf("Testing ROM protection...\n");

    // Attempt to write to ROM
    initTestNumHardFaults = 0;
    initTestDataFaultExpected = true;
    initTestInstructionFaultExpected = false;
    initTestThumbFaultExpected = false;

    dataPtr = (uint32_t *)codeBase;
    *dataPtr = 0x12345678UL; // Access should cause a HardFault

    if (1 != initTestNumHardFaults)
    {
    	printf("ROM write access did not generate a HardFault\n");
    	return 1;
    }

    // Attempt to read ROM (should not generate a fault)
    initTestDataFaultExpected = false;

    dataPtr = (uint32_t *)codeBase;
    printf("Initial SP is 0x%08x\n", *dataPtr);

    // Attempt to read past end of ROM
    initTestDataFaultExpected = true;
    initTestVal = 0xc0deface;

    dataPtr = (uint32_t *)codeEnd;
    initTestVal = *dataPtr;

    if (2 != initTestNumHardFaults)
    {
    	printf("ROM read access did not generate a HardFault\n");
    	return 2;
    }

    // Attempt to execute past end of ROM
    initTestInstructionFaultExpected = true;
    funcPtr();

    if (3 != initTestNumHardFaults)
    {
    	printf("ROM oob execution access did not generate a HardFault\n");
    	return 3;
    }

    //
    // Region 1: Patch RAM
    //
    printf("Testing Patch RAM protection...\n");

    // Attempt to execute from patch RAM
    initTestInstructionFaultExpected = true;
    funcPtr = (void (*)(void))(patchBase | 1);
    funcPtr();

    if (4 != initTestNumHardFaults)
    {
    	printf("Patch execution access did not generate a HardFault\n");
    	return 4;
    }

    // Attempt to read from the patch RAM
    initTestDataFaultExpected = true;
    dataPtr = (uint32_t *)(patchBase + 0x1000);
    initTestVal = *dataPtr;

    if (5 != initTestNumHardFaults)
    {
    	printf("Patch read access did not generate a HardFault\n");
    	return 5;
    }

    //
    // Region 2: Data RAM
    //
    printf("Testing Data RAM and stack protection...\n");

    // Attempt to execute from data RAM
    initTestInstructionFaultExpected = true;
    funcPtr = (void (*)(void))(dataBase | 1);
    funcPtr();

    if (6 != initTestNumHardFaults)
    {
    	printf("Data execution access did not generate a HardFault\n");
    	return 6;
    }

    // Attempt to read the top of the stack
    initTestDataFaultExpected = false;
    dataPtr = (uint32_t *)(stackEnd - 4);
    initTestVal = *dataPtr;
    printf("Stack top @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to read beyond the end of the stack
    initTestDataFaultExpected = true;
    dataPtr = (uint32_t *)stackEnd;
    initTestVal = *dataPtr;

    if (7 != initTestNumHardFaults)
    {
    	printf("Stack limit read access did not generate a HardFault\n");
    	return 7;
    }

    //
    // Region 3: Stack guard
    //
    printf("Testing stack guard...\n");

    // Attempt to read just above the stack guard
    dataPtr = (uint32_t *)(stackBase + 32UL);
    initTestVal = *dataPtr;
    printf("Stack base @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to read within the stack guard
    initTestDataFaultExpected = true;
    dataPtr = (uint32_t *)(stackBase + 28UL);
    initTestVal = *dataPtr;

    if (8 != initTestNumHardFaults)
    {
    	printf("Stack guard read access did not generate a HardFault\n");
    	return 8;
    }

    //
    // Region 4: ICore registers and shared-memory
    //
    printf("Testing ICore registers and shared-memory protection...\n");

    // Attempt to read the Icore registers
    initTestDataFaultExpected = false;
    dataPtr = (uint32_t *)(icoreBase);
    initTestVal = *dataPtr;
    printf("ICore reg @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to execute from ICore shared-memory
    initTestInstructionFaultExpected = true;
    funcPtr = (void (*)(void))(shrMemBase | 1);
    funcPtr();

    if (9 != initTestNumHardFaults)
    {
    	printf("Shared-memory execution access did not generate a HardFault\n");
    	return 9;
    }

    // Attempt to write/read/write/read from the shared-memory
    initTestDataFaultExpected = false;
    dataPtr = (uint32_t *)(shrMemBase + 0x400);
    *dataPtr = 0xAA55CCEEUL;
    initTestVal = *dataPtr;
    if (initTestVal != 0xAA55CCEEUL)
    {
    	printf("write/read1 of shared-memory failed: 0x%08x\n", initTestVal);
    	return 10;
    }
    printf("Shared-memory @ %p = 0x%08x\n", dataPtr, initTestVal);

    *dataPtr = 0xCAFEC0DEUL;
    initTestVal = *dataPtr;
    if (initTestVal != 0xCAFEC0DEUL)
    {
    	printf("write/read2 of shared-memory failed: 0x%08x\n", initTestVal);
    	return 11;
    }
    printf("Shared-memory @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to read beyond the end of the shared-memory
    initTestDataFaultExpected = true;
    dataPtr = (uint32_t *)(shrMemBase + 0x1000);
    initTestVal = *dataPtr;

    if (10 != initTestNumHardFaults)
    {
    	printf("Shared-memory limit read access did not generate a HardFault\n");
    	return 12;
    }

    //
    // Region 5: Peripherals
    //
    printf("Testing Peripherals protection...\n");

    // Attempt to execute from the Peripherals region (Vcore registers)
    initTestInstructionFaultExpected = true;
    funcPtr = (void (*)(void))(vCoreBase | 1);
    funcPtr();

    if (11 != initTestNumHardFaults)
    {
    	printf("Peripherals execution access did not generate a HardFault\n");
    	return 13;
    }

    // Attempt to read from the peripheral region
    initTestDataFaultExpected = false;
    dataPtr = (uint32_t *)(vCoreBase);
    initTestVal = *dataPtr;
    printf("VCore reg @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to read beyond the end of the peripherals
    initTestDataFaultExpected = true;
    dataPtr = (uint32_t *)peripheralEnd;
    initTestVal = *dataPtr;

    if (12 != initTestNumHardFaults)
    {
    	printf("Peripheral limit read access did not generate a HardFault\n");
    	return 14;
    }

    //
    // Enable the patch region and check it can be accessed
    //
    printf("Enabling patch region and test protection...\n");

    InitMpuEnablePatchRegion(true);

    // Attempt to read from the patch RAM
    initTestDataFaultExpected = false;
    dataPtr = (uint32_t *)(patchBase + 0x1000);
    initTestVal = *dataPtr;

    printf("Patch RAM @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Attempt to write to the patch RAM
    *dataPtr = 0xBEEFF00DUL;
    initTestVal = *dataPtr;

    printf("Patch RAM @ %p = 0x%08x\n", dataPtr, initTestVal);

    // Disable the patch region and try to read again
    InitMpuEnablePatchRegion(false);

    initTestDataFaultExpected = true;
    initTestVal = *dataPtr;

    if (13 != initTestNumHardFaults)
    {
    	printf("Patch read access did not generate a HardFault\n");
    	return 15;
    }

    printf("MPU test passed\n");
    return 0;
}

uint32_t vtor_original_called = 0;
uint32_t vtor_new_called = 0;

// Dummy interrupt 31 handler
void Reserved31_IrqHandler(void)
{
    printf("Original was called\n");
    vtor_original_called += 1;
}

// Replacement interrupt 31 handler
void New31_IrqHandler(void)
{
    printf("New was called\n");
    vtor_new_called += 1;
}

// Tests that the VTOR register can be used to reposition the vector table
int test_vtor(void)
{
    printf("VTOR test running\n");

    // Trigger the ROM-based handler
    __NVIC_EnableIRQ(Reserved31_IRQn);
    __NVIC_SetPendingIRQ(Reserved31_IRQn);

    if (0 == vtor_original_called)
    {
        printf("ROM Irq31 handler was not called\n");
        return 1;
    }

    // Replace the vector table - use the RAM table
    printf("Updating VTOR...\n");

    InitRelocateVectorTable();
    InitInstallNewIrqHandler(Reserved31_IRQn, (InitVectorTableHandlerT)New31_IrqHandler);

    // Trigger the RAM-based handler
    __NVIC_SetPendingIRQ(Reserved31_IRQn);

    if (1 != vtor_original_called)
    {
        printf("Failed! ROM Irq31 handler was called\n");
        return 2;
    }

    if (0 == vtor_new_called)
    {
        printf("Failed! RAM Irq31 handler was not called\n");
        return 3;
    }

    // Check installing a incorrect IRQ
    ErrorT error = InitInstallNewIrqHandler((IRQn_Type)(32), (InitVectorTableHandlerT)New31_IrqHandler);
    if (ERROR_SUCCESS == error)
    {
        printf("Failed! Irq31+1 was accepted as valid\n");
        return 4;
    }

    // Check installing a incorrect Handler
    error = InitInstallNewIrqHandler(Reserved31_IRQn, (InitVectorTableHandlerT)NULL);
    if (ERROR_SUCCESS == error)
    {
        printf("Failed! NULL handler was accepted as valid\n");
        return 5;
    }

    printf("VTOR test passed\n");

    // Re-enable the MPU
    MPU->CTRL = 1;
    __DSB();

    return 0;
}

int init_test(void)
{
    printf("Init integration test running\n");
    set_checkpoint_and_info(0, 0);

	set_checkpoint_and_info(CHECKPOINT_CHECK_CPUID, 0);
    int ret = check_CPUID(CPUID_EXPECTED_M0P);
    if (0 != ret)
    {
        printf("check_CPUID failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_CPUID, ret);
    }

    set_checkpoint_and_info(CHECKPOINT_CHECK_GLOBALS, 0);
    ret = check_globals();
    if (0 != ret)
    {
        printf("check_globals failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_GLOBALS, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_STACK, 0);
    ret = check_stack();
    if (0 != ret)
    {
        printf("check_stack failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_STACK, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_HEAP, 0);
    ret = check_heap();
    if (0 != ret)
    {
        printf("check_heap failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_HEAP, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_MALLOC, 0);
    ret = check_malloc();
    if (0 != ret)
    {
        printf("check_malloc failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_MALLOC, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_CALLOC, 0);
	ret = check_calloc();
    if (0 != ret)
    {
        printf("check_calloc failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_CALLOC, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_HARDFAULT, 0);
    ret = test_hardfault_handler();
    if (0 != ret)
    {
        printf("test_hardfault_handler failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_HARDFAULT, ret);
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_MPU, 0);
    ret = test_MPU();
    if (0 != ret)
    {
        printf("test_MPU failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_MPU, ret);
    }

    set_checkpoint_and_info(CHECKPOINT_CHECK_VTOR, 0);
    ret = test_vtor();
    if (0 != ret)
    {
        printf("test_vtor failed with %d\n", ret);
    	return set_cryptoerror(CHECKPOINT_CHECK_VTOR, ret);
    }

    printf("!!!!!!!!!!!!!!!!!!!\n");
    printf("!!! Test passed !!!\n");
    printf("!!!!!!!!!!!!!!!!!!!\n");
	set_checkpoint_and_info(CHECKPOINT_TEST_PASS, 0);
    return 0;
}

/// @} endgroup inittest

