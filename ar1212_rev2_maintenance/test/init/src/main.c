//***********************************************************************************
/// \file
/// Main entry file for Init integration test
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

#include "Common.h"
#include "AR0820.h"
#include "init_test.h"

int main(void)
{
    int ret;

    printf("Init integration test running\n");

    ret = init_test();

    printf("Exiting with %d\n", ret);
    __asm ("WFE");
}
