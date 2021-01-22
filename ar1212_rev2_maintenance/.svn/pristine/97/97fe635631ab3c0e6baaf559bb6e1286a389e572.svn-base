/* Reset code for AR0820 (Cortex-M0plus)
 *
 * Version: Sourcery CodeBench 2016.11-77
 * Support: https://support.mentor.com/
 *
 * Copyright (c) 2007-2010 CodeSourcery, Inc.
 * Copyright (c) 2010-2017 Mentor Graphics, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#if defined(__thumb2__) || defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_6SM__)
#define THUMB 1
    .thumb
    .thumb_func
#else
    .arm
#endif
    .section .cs3.reset,"ax",%progbits
    .globl __cs3_reset_AR0820
    .type   __cs3_reset_AR0820, %function
__cs3_reset_AR0820:
/* Low level reset sequence */

    /* set Vector Table Offset Register to point to __cs3_interrupt_vector */
    ldr r0, =0xE000ED08             @ load SCB.VTOR register in r0
    ldr r1, =__cs3_interrupt_vector @ load address of interrupt vector in r1
    str r1, [r0]                    @ set SCB.VTOR to __cs3_interrupt_vector

    /* jump to common start code */
#if THUMB
    ldr r0,=_start
    bx  r0
#else
    b   _start
#endif
    .pool
    .size   __cs3_reset_AR0820,.-__cs3_reset_AR0820

