;//******************************************************************************
;/// \file
;///
;/// This file implements the System Driver 'low-level' assembler functions
;///
;/// \addtogroup systemdrv
;///
;/// @{
;//******************************************************************************
;// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
;// documentation is licensed by ON Semiconductor under limited terms and
;// conditions. The terms and conditions pertaining to the software and/or
;// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
;// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
;// Do not use this software and/or documentation unless you have carefully read
;// and you agree to the limited terms and conditions. By using this software
;// and/or documentation, you agree to the limited terms and conditions.
;//******************************************************************************

    .thumb
    .thumb_func

    .section .test,"ax",%progbits
    .globl SystemDrvWaitForInterrupt
    .type  SystemDrvWaitForInterrupt, %function
;// void SystemDrvWaitForInterrupt(void)
SystemDrvWaitForInterrupt:
    dsb     0xf
    wfi
    bx      lr

    .globl SystemDrvTerminate
    .type  SystemDrvTerminate, %function
;// void SystemDrvTerminate(void)
SystemDrvTerminate:
;// disable interrupts
    cpsid   i

SystemDrvHalt:
;// execute WFI in an infinite loop because a spurious exception
;// at this point could cause WFI to complete (because PRIMASK is ignored
;// when determining a WFI wake event), but the exception will actually
;// only be taken if its priority is less than 0 since PRIMASK was
;// set to 1 above (interrupts disabled); this means only Reset, NMI,
;// and HardFault exceptions would actually be handled, otherwise, WFI
;// is just executed again. We don't reenable interrupts so the
;// interrupt will just remain pending.
    wfi
    b       SystemDrvHalt

    .pool

