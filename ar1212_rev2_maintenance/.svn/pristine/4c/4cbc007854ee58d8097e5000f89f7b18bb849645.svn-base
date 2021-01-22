//******************************************************************************
/// \file
///
/// This file provides the exception and interrupt vector table
///
/// \addtogroup init
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

#include <stdint.h>

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------
#define HEAPSIZE 0x4000 					// 16Kbytes

unsigned char __cs3_heap_start[HEAPSIZE]
__attribute__ ((section (".bss"), aligned(8)));
unsigned char *__cs3_heap_limit = __cs3_heap_start + HEAPSIZE;

// Exception/interrupt handler
typedef void handler(void);

//------------------------------------------------------------------------------
//                                  Imports
//------------------------------------------------------------------------------

// Import the stack top from the Linker
extern uint32_t __cs3_stack;

// Import the reset handler
extern void __cs3_reset_AR0820(void);

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

void Default_Handler(void);     /* Default empty handler */

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

//  Exception / Interrupt Handlers

// Cortex-M0+ Processor Exceptions
void NMI_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));

// ARMCM0plus Specific Interrupts
void Reserved0_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved1_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved2_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved3_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved4_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved5_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved6_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved7_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved8_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved9_IrqHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved10_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved11_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved12_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved13_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved14_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved15_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved16_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved17_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved18_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved19_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved20_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved21_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved22_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved23_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved24_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved25_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved26_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved27_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved28_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved29_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved30_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));
void Reserved31_IrqHandler  (void) __attribute__ ((weak, alias("Default_Handler")));

// Exception / Interrupt Vector table
handler *__attribute__((section (".cs3.interrupt_vector")))
    __cs3_interrupt_vector_AR0820[] =
{
    // Cortex-M0+ Exception Handlers
    (handler *)((uint32_t)&__cs3_stack),    //      Initial Stack Pointer
    __cs3_reset_AR0820,                     //      Reset Handler
    NMI_Handler,                            //      NMI Handler
    HardFault_Handler,                      //      Hard Fault Handler
    0,                                      //      Reserved
    0,                                      //      Reserved
    0,                                      //      Reserved
    0,                                      //      Reserved
    0,                                      //      Reserved
    0,                                      //      Reserved
    0,                                      //      Reserved
    SVC_Handler,                            //      SVCall Handler
    0,                                      //      Reserved
    0,                                      //      Reserved
    PendSV_Handler,                         //      PendSV Handler
    SysTick_Handler,                        //      SysTick Handler

    // External interrupts
    Reserved0_IrqHandler,                   //  0:  Reserved
    Reserved1_IrqHandler,                   //  1:  Reserved
    Reserved2_IrqHandler,                   //  2:  Reserved
    Reserved3_IrqHandler,                   //  3:  Reserved
    Reserved4_IrqHandler,                   //  4:  Reserved
    Reserved5_IrqHandler,                   //  5:  Reserved
    Reserved6_IrqHandler,                   //  6:  Reserved
    Reserved7_IrqHandler,                   //  7:  Reserved
    Reserved8_IrqHandler,                   //  8:  Reserved
    Reserved9_IrqHandler,                   //  9:  Reserved
    Reserved10_IrqHandler,                  //  10: Reserved
    Reserved11_IrqHandler,                  //  11: Reserved
    Reserved12_IrqHandler,                  //  12: Reserved
    Reserved13_IrqHandler,                  //  13: Reserved
    Reserved14_IrqHandler,                  //  14: Reserved
    Reserved15_IrqHandler,                  //  15: Reserved
    Reserved16_IrqHandler,                  //  16: Reserved
    Reserved17_IrqHandler,                  //  17: Reserved
    Reserved18_IrqHandler,                  //  18: Reserved
    Reserved19_IrqHandler,                  //  19: Reserved
    Reserved20_IrqHandler,                  //  20: Reserved
    Reserved21_IrqHandler,                  //  21: Reserved
    Reserved22_IrqHandler,                  //  22: Reserved
    Reserved23_IrqHandler,                  //  23: Reserved
    Reserved24_IrqHandler,                  //  24: Reserved
    Reserved25_IrqHandler,                  //  25: Reserved
    Reserved26_IrqHandler,                  //  26: Reserved
    Reserved27_IrqHandler,                  //  27: Reserved
    Reserved28_IrqHandler,                  //  28: Reserved
    Reserved29_IrqHandler,                  //  29: Reserved
    Reserved30_IrqHandler,                  //  30: Reserved
    Reserved31_IrqHandler,                  //  31: Reserved
};

// Default Handler for Exceptions / Interrupts
void Default_Handler(void)
{
	while(1);
}

/// @} endgroup init


