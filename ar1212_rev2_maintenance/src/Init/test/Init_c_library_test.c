//***********************************************************************************
/// \file
/// Tests support for the C/C++ run time libraries
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "AR0820.h"
#include "crypto_icore.h"
#include "Init.h"

#define CHECKPOINT_CHECK_MALLOC    		0x10
#define CHECKPOINT_CHECK_CALLOC    		0x20
#define CHECKPOINT_TEST_PASS			0xAAAA

// Set the CHECKPOINT and INFO registers
void set_checkpoint_and_info(uint16_t const checkpoint, uint16_t const info)
{
	CRYPTO_ICORE_REGS->checkpoint = checkpoint;
	CRYPTO_ICORE_REGS->info = info;
}

// Checks malloc/free
int check_malloc(void)
{
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

// Pull-in linker-defined symbols
extern unsigned int Image$$CODE$$Base;
extern unsigned int Image$$PATCH$$Base;
extern unsigned int Image$$DATA$$Base;
extern unsigned int Image$$HEAP$$Base;
extern unsigned int Image$$MAIN_STACK$$Base;
extern unsigned int Image$$MAIN_STACK$$Limit;
extern unsigned int Image$$ICORE_REGISTERS$$Base;
extern unsigned int Image$$SHARED_MEMORY$$Base;

// Checks calloc against the available heap
int check_calloc(void)
{
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

int main(void)
{
    int ret;

    printf("Init_c_library test running\n");
    set_checkpoint_and_info(0, 0);

	set_checkpoint_and_info(CHECKPOINT_CHECK_MALLOC, 0);
    ret = check_malloc();
    if (0 != ret)
    {
        printf("check_malloc failed with %d\n", ret);
    	set_checkpoint_and_info(0xFF00 | CHECKPOINT_CHECK_MALLOC, ret);
        return ret;
    }

	set_checkpoint_and_info(CHECKPOINT_CHECK_CALLOC, 0);
	ret = check_calloc();
    if (0 != ret)
    {
        printf("check_calloc failed with %d\n", ret);
    	set_checkpoint_and_info(0xFF00 | CHECKPOINT_CHECK_CALLOC, ret);
        return ret;
    }

    printf("!!!!!!!!!!!!!!!!!!!\n");
    printf("!!! Test passed !!!\n");
    printf("!!!!!!!!!!!!!!!!!!!\n");
	set_checkpoint_and_info(CHECKPOINT_TEST_PASS, 0);
    return 0;
}
