//***********************************************************************************
/// \file toolbox_unit_test.cpp
///
/// Toolbox unit test
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
#define WINDOWS
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

#include "cpputest_support.h"

extern "C"
{
#include "AR0820.h"
#include "Toolbox.h"

}


TEST_GROUP(ToolboxGroup)
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

TEST(ToolboxGroup, TestToolboxChecksum)
{
    uint16_t buffer[8];
    uint8_t * buffer8 = (uint8_t *)buffer;

    uint16_t expected = (uint16_t)~0xf2dd;
    uint16_t checksum = 0;

    // vector from https://tools.ietf.org/html/rfc1071
    buffer8[0] = 0x0;
    buffer8[1] = 0x1;
    buffer8[2] = 0xf2;
    buffer8[3] = 0x03;
    buffer8[4] = 0xf4;
    buffer8[5] = 0xf5;
    buffer8[6] = 0xf6;
    buffer8[7] = 0xf7;
    checksum = ToolboxCalcChecksum(buffer, 8);
    CHECK_EQUAL(expected, checksum);

    buffer8[8] = (uint8_t)checksum & 0xFF;
    buffer8[9] = (uint8_t)(checksum >> 8);

    checksum = ToolboxCalcChecksum(buffer, 10);
    CHECK_EQUAL(0, checksum);

    checksum = ToolboxCalcChecksum(buffer, 7);

    buffer8[7] = (uint8_t)0;
    buffer8[8] = (uint8_t)checksum & 0xFF;
    buffer8[9] = (uint8_t)(checksum >> 8);

    checksum = ToolboxCalcChecksum(buffer, 10);
    CHECK_EQUAL(0, checksum);

    // check to trigger the issue in AR0820FW-232 : RFC1071checksum implementation incorrect
    buffer[0] = 0xFFFF;
    buffer[1] = 0x0001;
    buffer[2] = 0xFFFF;
    buffer[3] = ToolboxCalcChecksum(buffer, 6);

    checksum = ToolboxCalcChecksum(buffer, 8);

    CHECK_EQUAL(0, checksum);
}


