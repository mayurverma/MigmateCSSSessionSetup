//***********************************************************************************
//! \file Init_barebones_test.c
//! \brief AR0820 Initialisation test harness for C barebones support
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

uint32_t static_val;

uint32_t uninit_val;
uint32_t init_val = 0xbeefcafe;

int main(void)
{
    uninit_val = init_val;
    static_val = 0xfaceac4e;

    printf("\nHello Bracknell\n");

    // do not return from main()
    while (1);
}


