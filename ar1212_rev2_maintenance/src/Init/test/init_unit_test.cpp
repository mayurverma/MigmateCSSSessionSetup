//***********************************************************************************
/// \file
///
/// Init component unit test
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

#include <stdint.h>

#include "cpputest_support.h"

extern "C"
{
    #include "AR0820.h"
    #include "Init.h"
}

#define UINT8_INIT_VAL  0xabU
#define UINT16_INIT_VAL 0xcdefU
#define UINT32_INIT_VAL 0x12345678UL

#define INT8_INIT_VAL   -16
#define INT16_INIT_VAL  -1024
#define INT32_INIT_VAL  -12345678L

// Globals
extern "C"
{
    uint8_t uint8_uninit;
    uint16_t uint16_uninit;
    uint32_t uint32_uninit;

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
}

TEST_GROUP(InitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(InitGroup, TestGlobals)
{
    LONGS_EQUAL(0, uint8_uninit);
    LONGS_EQUAL(0, uint16_uninit);
    LONGS_EQUAL(0, uint32_uninit);

    LONGS_EQUAL(UINT8_INIT_VAL, uint8_init);
    LONGS_EQUAL(UINT16_INIT_VAL, uint16_init);
    LONGS_EQUAL(UINT32_INIT_VAL, uint32_init);

    LONGS_EQUAL(0, int8_static);
    LONGS_EQUAL(0, int16_static);
    LONGS_EQUAL(0, int32_static);

    LONGS_EQUAL(INT8_INIT_VAL, int8_init);
    LONGS_EQUAL(INT16_INIT_VAL, int16_init);
    LONGS_EQUAL(INT32_INIT_VAL, int32_init);

    LONGS_EQUAL(0x12343210, uint32_array[0]);
    LONGS_EQUAL(0x56787654, uint32_array[1]);
    LONGS_EQUAL(0x9abcba98, uint32_array[2]);
    LONGS_EQUAL(0xdef0fedc, uint32_array[3]);
    LONGS_EQUAL(0xaa5555aa, uint32_array[4]);
    LONGS_EQUAL(0x16611661, uint32_array[5]);
    LONGS_EQUAL(0x27727722, uint32_array[6]);
    LONGS_EQUAL(0x55aaaa55, uint32_array[7]);
}

#define CPUID_IMPLEMENTER   0x41    // ARM
#define CPUID_VARIANT_2     0x2     // Revision 2
#define CPUID_VARIANT_0     0x0     // Revision 0
#define CPUID_CONSTANT_V7M  0xF     // Cortex V7-M architecture
#define CPUID_CONSTANT_V6M  0xC     // Cortex V6-M architecture
#define CPUID_PARTNO_M3     0xC23   // Cortex-M3
#define CPUID_PARTNO_M0P    0xC60   // Cortex-M0+
#define CPUID_REVISION_1    0x1     // Patch 1
#define CPUID_REVISION_0    0x0    // Patch 0

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

TEST(InitGroup, TestCPUID)
{
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

    LONGS_EQUAL(CPUID_EXPECTED_M0P, cpuid);
}

#define EXPECTED_STACK_VALUE 0xfeedface

// Check the stack has been initialized
TEST(InitGroup, TestStack)
{
    uint32_t* stack_base;
    InitMpuStackConfigType stackConfig;
    uint32_t  guard_region_size_long_words;

    (void)InitMpuGetStackInformation(&stackConfig);
    stack_base = stackConfig.base;
    guard_region_size_long_words = stackConfig.stackGuardRegionSizeLongWords;

    // Check the bottom 1024 bytes
    for (int idx = guard_region_size_long_words;
        idx < (guard_region_size_long_words + 256);
        idx++)
    {
        LONGS_EQUAL(EXPECTED_STACK_VALUE, stack_base[idx]);
    }
}

#define EXPECTED_HEAP_VALUE 0xcafebabe

// Check the heap has been initialized
TEST(InitGroup, TestHeap)
{
    uint32_t * heap_base = (uint32_t *)&Image$$HEAP$$Base;

    // Check somewhere in the middle of the heap
    for (int idx = 256; idx < 192; idx++)
    {
        LONGS_EQUAL(EXPECTED_HEAP_VALUE, heap_base[idx]);
    }
}


