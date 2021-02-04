;//******************************************************************************
;/// \file
;///
;/// This file implements the SVC handler for Patch Manager.
;///
;/// \addtogroup patchmgr
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
                INCLUDE     AR0820_memmap.inc
                INCLUDE     patcher.inc

PATCH_MGR_STACKED_PC_OFFSET         EQU  24

                PRESERVE8
                THUMB

; ---------------------------------------------------------------------------------------------------------------------------------
                AREA    |.text|, CODE, READONLY


SVC_Handler     PROC
                EXPORT  SVC_Handler

                ; On entry to the SVC handler the stack pointer references the exception stack
                ; frame. It contains: R0-R3, R12, LR, PC, xPSR. Therefore we can safely
                ; clobber R0-R3.
                ;
                ; The stacked PC indicates the return address from the handler. This is the
                ; function that has been patched. We lookup the address of the patch in the
                ; patch table, and fix-up the stacked PC so that we return to the patch.

                ; Retrieve the index from the memory subsystem
                LDR     R0, =PATCHER_INDEX
                LDR     R0, [R0, #0]
                ; The index is a 5 bit field range 0-31, shift left by 2 (multiply by 4) to create a byte offset,
                ; value used to access the patch table which consists of 32 32-bit values
                LSLS    R0, #2

                ; Retrieve the address of the patch from the patch table
                LDR     R1, =PATCH_TABLE_BASE
                LDR     R1, [R1, R0]

                ; Overwrite the stacked PC with the patch address
                STR     R1, [SP, #PATCH_MGR_STACKED_PC_OFFSET]

                ; Return to the patched function
                BX      LR
                ENDP

                ALIGN
                END
