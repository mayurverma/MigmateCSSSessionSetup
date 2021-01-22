//***********************************************************************************
/// \file
///
/// Example component (unit test boilerplate)
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
#include "Example.h"
}

static bool verbose = false;
static int num_tests = 0;

TEST_GROUP(ExampleTestGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        num_tests += 1;
        if (verbose) printf("in setup %d\n", num_tests);
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        if (verbose) printf("in teardown %d\n", num_tests);
    }
};

TEST(ExampleTestGroup, TestInit)
{
    int32_t res = Example_Init(8L);

    CHECK_EQUAL(res, 8L * 21L);
}

TEST(ExampleTestGroup, TestShutdown)
{
    int32_t res = Example_Shutdown();
    CHECK_EQUAL(res, 42L);
}

