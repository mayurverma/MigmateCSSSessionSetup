//***********************************************************************************
/// \file
///
/// Command Driver unit test
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
#include "CommandDrv.h"
#include "AR0820.h"
#include "crypto_icore.h"
}


#define COMMANDDRV_UNITTEST_COMMAND_VALUE                 0x85A1U    // doesn't have to be a real command
#define COMMANDDRV_UNITTEST_UNINT_SHARED_RAM_VALUE    0x87654321UL   // any old non-zero number
#define COMMANDDRV_UNITTEST_PARAM_VALUE               0x34567890UL   // any old number
#define COMMANDDRV_UNITTEST_GOOD_RESPONSE_VALUE           0x7654U    // any old different number with b15 (doorbell) cleared
#define COMMANDDRV_UNITTEST_BAD_RESPONSE_VALUE            0xFEDCU    // any old different number with b15 (doorbell) set



TEST_GROUP(CommandDrvInitGroup)
{
    // Optional setup - called prior to each test
    // set the shared-memory contents to NON-zero
    // so that we can test that CommandDrvInit() clears it to zero
    void setup(void)
    {
        uint16_t idx;
        uint32_t* sharedRam_p = (uint32_t*)CRYPTO_ICORE_SHARED_RAM;

        for (idx = 0U; idx < (CRYPTO_ICORE_SHARED_RAM_SIZE_BYTES / sizeof(uint32_t)); idx += 1U)
        {
            *sharedRam_p++ = COMMANDDRV_UNITTEST_UNINT_SHARED_RAM_VALUE;
        }

        // just a quick test to prove we've set it - test the bottom and top locations
        sharedRam_p = (uint32_t*)CRYPTO_ICORE_SHARED_RAM;
        LONGS_EQUAL(COMMANDDRV_UNITTEST_UNINT_SHARED_RAM_VALUE, *sharedRam_p);

        sharedRam_p = (uint32_t*)(CRYPTO_ICORE_SHARED_RAM + CRYPTO_ICORE_SHARED_RAM_SIZE_BYTES - sizeof(uint32_t));
        LONGS_EQUAL(COMMANDDRV_UNITTEST_UNINT_SHARED_RAM_VALUE, *sharedRam_p);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(CommandDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT status = CommandDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST(CommandDrvGroup, TestGetCommand)
{
    uint16_t command;

    // backdoor write a command into the command register (doesn't have to be a real command) and then fetch it
    CRYPTO_ICORE_REGS_T* icoreRegs_p = (CRYPTO_ICORE_REGS_T*)CRYPTO_ICORE_REG_BASE;
    icoreRegs_p->command = (uint16_t)COMMANDDRV_UNITTEST_COMMAND_VALUE;

    command = CommandDrvGetCommand();
    LONGS_EQUAL(COMMANDDRV_UNITTEST_COMMAND_VALUE, command);
}

TEST(CommandDrvGroup, TestGetCommandParams)
{
    uint32_t * commandParams;

    // put some params in the shared ram
    *(uint32_t*)CRYPTO_ICORE_SHARED_RAM = COMMANDDRV_UNITTEST_PARAM_VALUE;

    commandParams = (uint32_t *)CommandDrvGetCommandParams();
    POINTERS_EQUAL((uint32_t *)CRYPTO_ICORE_SHARED_RAM, commandParams);
    LONGS_EQUAL(COMMANDDRV_UNITTEST_PARAM_VALUE, *commandParams);
}

TEST(CommandDrvGroup, TestSetResponse)
{
    CRYPTO_ICORE_REGS_T* icoreRegs_p = (CRYPTO_ICORE_REGS_T*)CRYPTO_ICORE_REG_BASE;

    CommandDrvSetResponse(COMMANDDRV_UNITTEST_GOOD_RESPONSE_VALUE);

    // test the response value in the command reg
    LONGS_EQUAL(COMMANDDRV_UNITTEST_GOOD_RESPONSE_VALUE, icoreRegs_p->command);
}

TEST(CommandDrvInitGroup, TestInit)
{
    uint16_t idx;
    uint32_t* sharedRam_p = (uint32_t*)CRYPTO_ICORE_SHARED_RAM;

    // call the FUT
    ErrorT status = CommandDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    // check that the shared ram is initialised to 0x00000000
    for (idx = 0; idx < (CRYPTO_ICORE_SHARED_RAM_SIZE_BYTES / sizeof(uint32_t)); idx += 1)
    {
        LONGS_EQUAL(0x00000000, *sharedRam_p);
        sharedRam_p++;
    }

}

