;//******************************************************************************
;/// \file
;///
;/// This file implements the AR0820 ROM entry point. It is responsible for
;/// initialising the Cortex-M0+ core and invoking the C/C++ library initialisation
;/// routines.
;///
;/// \addtogroup init
;///
;/// \note This file is based on the CMSIS framework provided by ARM and is therefore
;///       provided under a separate license - please see license details below.
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
;//
;//******************************************************************************
; * brief    CMSIS Cortex-M ARMv6-M based Core Device Startup File for Device AR0820
; * version  V5.00
; * date     02. March 2016
;//******************************************************************************
;/*
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */
;//******************************************************************************

                PRESERVE8
                THUMB

; ---------------------------------------------------------------------------------------------------------------------------------
; The default Vector Table
;
                AREA    VECTORS, DATA, READWRITE, ALIGN=3
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  ||Image$$MAIN_STACK$$ZI$$Limit||

__Vectors       DCD     ||Image$$MAIN_STACK$$ZI$$Limit||            ; +00 Initial stack pointer
                DCD     InitStart                                   ; +04 Entry point
                DCD     NMI_Handler                                 ; +08 NMI Handler
                DCD     HardFault_Handler                           ; +12 Hard Fault Handler
                DCD     Reserved_Handler                            ; +16 Reserved
                DCD     Reserved_Handler                            ; +20 Reserved
                DCD     Reserved_Handler                            ; +24 Reserved
                DCD     Reserved_Handler                            ; +28 Reserved
                DCD     Reserved_Handler                            ; +32 Reserved
                DCD     Reserved_Handler                            ; +36 Reserved
                DCD     Reserved_Handler                            ; +40 Reserved
                DCD     SVC_Handler                                 ; +44 SVCall Handler
                DCD     Reserved_Handler                            ; +48 Reserved
                DCD     Reserved_Handler                            ; +52 Reserved
                DCD     PendSV_Handler                              ; +56 PendSV Handler
                DCD     SysTick_Handler                             ; +60 SysTick Handler

                ; External Interrupts - <PH> preliminary see IMAGINGPRJ-2992
                ;                       <SP> Updated as per Micro Arch Doc v0.6 (24/5/18)
                ;                       <SP> Updated as per Micro Arch Doc v0.7 (8/6/18)
                ;                       <PH> Updated as per Micro Arch Doc v52.0 (2/Jul/18)

                ; INFO: <PH>: this must match the table in AR0820.h

                ;       Name                        Offset  IRQ#    Notes
                ;       ----------------------------------------------------------------
                DCD     Doorbell_IrqHandler         ; +64    0      Command register doorbell interrupt
                DCD     SOF_IrqHandler              ; +68    1      Start-of-frame interrupt
                DCD     EOF_IrqHandler              ; +72    2      End-of-frame interrupt
                DCD     SOL_IrqHandler              ; +76    3      Start-of-line interrupt
                DCD     EOL_IrqHandler              ; +80    4      End-of-line interrupt
                DCD     CCIsIdle_IrqHandler         ; +84    5      CC312 is_idle signal
                DCD     CCPowerdownReady_IrqHandler ; +88    6      CC312 powerdown-ready signal
                DCD     CCHostIntReq_IrqHandler     ; +92    7      CC312 host interrupt request signal
                DCD     PllLocked_IrqHandler        ; +96    8      Sensor PLL locked
                DCD     PllNotLocked_IrqHandler     ; +100   9      Sensor PLL not locked
                DCD     IcoreEccDed_IrqHandler      ; +104  10      Icore RAM ECC DED Failure interrupt
                DCD     IcoreEccSec_IrqHandler      ; +108  11      Icore RAM ECC SEC Correction interrupt
                DCD     Reserved12_IrqHandler       ; +112  12      Reserved
                DCD     DualTimerInt1_IrqHandler    ; +116  13      Dual timer interrupt 1
                DCD     DualTimerInt2_IrqHandler    ; +120  14      Dual timer interrupt 2
                DCD     DualTimerIntC_IrqHandler    ; +124  15      Dual timer interrupt C
                DCD     Watchdog_IrqHandler         ; +128  16      Watchdog timer interrupt
                DCD     CyberRomDed_IrqHandler      ; +132  17      Cyber ROM ECC DED Failure
                DCD     CyberRomSec_IrqHandler      ; +136  18      Cyber ROM ECC single bit correction
                DCD     CyberRamDed_IrqHandler      ; +140  19      Cyber RAM ECC DED Failure
                DCD     CyberRamSec_IrqHandler      ; +144  20      Cyber RAM ECC single bit correction
                DCD     Reserved21_IrqHandler       ; +148  21      Reserved
                DCD     Reserved22_IrqHandler       ; +152  22      Reserved
                DCD     Reserved23_IrqHandler       ; +156  23      Reserved
                DCD     Reserved24_IrqHandler       ; +160  24      Reserved
                DCD     Reserved25_IrqHandler       ; +164  25      Reserved
                DCD     Reserved26_IrqHandler       ; +168  26      Reserved
                DCD     Reserved27_IrqHandler       ; +172  27      Reserved
                DCD     Reserved28_IrqHandler       ; +176  28      Reserved
                DCD     Reserved29_IrqHandler       ; +180  29      Reserved
                DCD     Reserved30_IrqHandler       ; +184  30      Reserved
                DCD     Reserved31_IrqHandler       ; +188  31      Reserved
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

; ---------------------------------------------------------------------------------------------------------------------------------
                AREA    |.text|, CODE, READONLY

                INCLUDE DiagCheckpoint.inc

; This is the ROM entry point. The Cortex-M0+ automatically jumps here after reset. We can therefore
; assume that the processor is in a 'safe' state (it has just been reset):
;    - Processor is in priviledged 'thread' mode.
;    - MPU is disabled.
;    - Only the Reset, NMI and Hard fault exceptions are enabled.
;
; We therefore only need to initialize our stack pointer, initialize the stack and heap contents,
; and invoke the C library start-up routine.
;
InitStart       PROC
                EXPORT  InitStart                   [WEAK]
                IMPORT  InitSystem
                IMPORT  __main

; set the 'main' stack pointer. In operation the stack grows down from $$Limit to $$Base
                MOVS    R0, #DIAG_CHECKPOINT_INIT_START
                BL      InitSetCheckpoint

                IMPORT  ||Image$$MAIN_STACK$$ZI$$Limit||
                LDR     R0, =||Image$$MAIN_STACK$$ZI$$Limit||
                MOV     SP, R0


; initialize the entire stack region to a known value, starting at lowest address
                MOVS    R0, #DIAG_CHECKPOINT_INIT_STACK
                BL      InitSetCheckpoint

                IMPORT  ||Image$$MAIN_STACK$$ZI$$Base||             ; lowest address
                LDR     R0, =||Image$$MAIN_STACK$$ZI$$Base||        ; R0 = currentAddr

                IMPORT  ||Image$$MAIN_STACK$$ZI$$Length||           ; Size of stack
                LDR     R1, =||Image$$MAIN_STACK$$ZI$$Length||      ; R1 = stackLength
                ADD     R1, R0                                      ; R1 = stackTop + 1

                LDR     R2, STACK_FILL_VALUE                        ; R2 = fill
                MOV     R3, R2                                      ; R3 = fill
                MOV     R4, R2                                      ; R4 = fill
                MOV     R5, R2                                      ; R5 = fill

FillStack       STM     R0!, {R2, R3, R4, R5}
                CMP     R1, R0                                      ; if ((stackTop + 1) == currentAddr)
                BNE     FillStack                                   ;     break;

; initialize the entire heap region to prevent ECC hardfault on read-modify-write
                MOVS    R0, #DIAG_CHECKPOINT_INIT_HEAP
                BL      InitSetCheckpoint

                IMPORT  ||Image$$HEAP$$ZI$$Base||                   ; lowest address
                LDR     R0, =||Image$$HEAP$$ZI$$Base||              ; R0 = currentAddr

                IMPORT  ||Image$$HEAP$$ZI$$Length||                 ; Size of Heap
                LDR     R1, =||Image$$HEAP$$ZI$$Length||            ; R1 = heapLength
                ADD     R1, R0                                      ; R1 = heapTop + 1

                LDR     R2, HEAP_FILL_VALUE                         ; R2 = fill
                MOV     R3, R2                                      ; R3 = fill
                MOV     R4, R2                                      ; R4 = fill
                MOV     R5, R2                                      ; R5 = fill

FillHeap        STM     R0!, {R2, R3, R4, R5}
                CMP     R1, R0                                      ; if ((heapTop + 1) == currentAddr)
                BNE     FillHeap                                    ;     break;

; initialize the entire data region to prevent ECC hardfault on startup
                MOVS    R0, #DIAG_CHECKPOINT_INIT_DATA
                BL      InitSetCheckpoint

                IMPORT  ||Image$$DATA$$Base||                       ; lowest address
                LDR     R0, =||Image$$DATA$$Base||                  ; R0 = currentAddr

                IMPORT  ||Image$$DATA$$ZI$$Limit||                  ; dataLimit
                LDR     R1, =||Image$$DATA$$ZI$$Limit||             ; R1 = dataLimit

                MOVS    R2, #0                                      ; R2 = fill
                MOV     R3, R2                                      ; R3 = fill
                MOV     R4, R2                                      ; R4 = fill
                MOV     R5, R2                                      ; R5 = fill

FillData        STM     R0!, {R2, R3, R4, R5}
                CMP     R1, R0                                      ; if ((dataTop + 1) == currentAddr)
                BGT     FillData                                    ;     break;

; initialize the entire patch region to prevent ECC hardfault on startup
                MOVS    R0, #DIAG_CHECKPOINT_INIT_PATCH
                BL      InitSetCheckpoint

                IMPORT  ||Image$$PATCH$$Base||                      ; lowest address
                LDR     R0, =||Image$$PATCH$$Base||                 ; R0 = currentAddr

                IMPORT  ||Image$$PATCH$$ZI$$Length||                ; Size of Patch
                LDR     R1, =||Image$$PATCH$$ZI$$Length||           ; R1 = patchLength
                ADD     R1, R0                                      ; R1 = patchTop + 1

                MOVS    R2, #0                                      ; R2 = fill
                MOV     R3, R2                                      ; R3 = fill
                MOV     R4, R2                                      ; R4 = fill
                MOV     R5, R2                                      ; R5 = fill

FillPatch       STM     R0!, {R2, R3, R4, R5}
                CMP     R1, R0                                      ; if ((patchTop + 1) == currentAddr)
                BNE     FillPatch                                   ;     break;

; initialize the remaining registers to prevent C code push/pop of undefined (simulation 'x') values
                MOV     R6,  R2
                MOV     R7,  R2
                MOV     R8,  R2
                MOV     R9,  R2
                MOV     R10, R2
                MOV     R11, R2
                MOV     R12, R2

; we can now initialize the AR0820 system ready to execute C code
                MOVS    R0, #DIAG_CHECKPOINT_INIT_SYSTEM
                BL      InitSetCheckpoint

                LDR     R0, =InitSystem
                BLX     R0

; The ARM C library start-up code is invoked via __main
;
; Note: a description of the ARM C library's startup __main functionality is here:
; http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0241b/index.html
                MOVS    R0, #DIAG_CHECKPOINT_INIT_C_STARTUP
                BL      InitSetCheckpoint

                LDR     R0, =__main
                BX      R0
                ENDP

                ALIGN
STACK_FILL_VALUE    DCD 0xfeedface
HEAP_FILL_VALUE     DCD 0xcafebabe

; ---------------------------------------------------------------------------------------------------------------------------------
                INCLUDE AR0820_memmap.inc
                INCLUDE crypto_icore.inc

; Checkpoint support
; void InitSetCheckpoint(uint32_t const checkpoint);
;
InitSetCheckpoint\
                PROC
                                                    ; R0 = checkpoint
                LDR     R1, CheckpointReg           ; R1 = &CHECKPOINT
                STR     R0, [R1]                    ; CHECKPOINT = checkpoint

                LDR     R1, PausepointReg           ; R1 = &PAUSEPOINT
                LDR     R2, [R1]                    ; R2 = PAUSEPOINT
                LDR     R3, PausepointMask          ; R3 = PAUSEPOINT__MASK
                ANDS    R2, R3                      ; R2 = PAUSEPOINT & PAUSEPOINT__MASK
                CMP     R0, R2                      ; if R0 == R2
                BEQ     PausepointMatch             ;      pausepoint == checkpoint
                BX      LR                          ; else return

PausepointMatch
                LDR     R2, [R1]                    ; R2 = PAUSEPOINT
                MOVS    R3, #1
                LSLS    R3, #15                     ; R3 = PAUSED__MASK
                ORRS    R2, R3                      ; R2 = PAUSEPOINT | PAUSED__MASK
                STR     R2, [R1]                    ; PAUSEPOINT |= PAUSED_MASK

PausepointWait
                LDR     R2, [R1]                    ; R2 = PAUSEPOINT
                ANDS    R2, R3                      ; R2 = PAUSEPOINT & PAUSED__MASK
                BNE     PausepointWait              ; if paused then wait
                BX      LR                          ; else return

                ENDP

                ALIGN
CheckpointReg   DCD     CRYPTO_ICORE_CHECKPOINT
PausepointReg   DCD     CRYPTO_ICORE_PAUSEPOINT
PausepointMask  DCD     CRYPTO_ICORE_PAUSEPOINT_PAUSEPOINT__MASK

; ---------------------------------------------------------------------------------------------------------------------------------
; Default Exception Handlers (infinite loops which can be overriden)

NMI_Handler\
                PROC
                EXPORT  NMI_Handler                 [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler           [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler                 [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler              [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler             [WEAK]
                B       .
                ENDP
Reserved_Handler\
                PROC
                EXPORT  Reserved_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC
; Default definitions for the interrupt handlers (infinite loops which can be overriden)
                EXPORT  Doorbell_IrqHandler         [WEAK]
                EXPORT  SOF_IrqHandler              [WEAK]
                EXPORT  EOF_IrqHandler              [WEAK]
                EXPORT  SOL_IrqHandler              [WEAK]
                EXPORT  EOL_IrqHandler              [WEAK]
                EXPORT  CCIsIdle_IrqHandler         [WEAK]
                EXPORT  CCPowerdownReady_IrqHandler [WEAK]
                EXPORT  CCHostIntReq_IrqHandler     [WEAK]
                EXPORT  PllLocked_IrqHandler        [WEAK]
                EXPORT  PllNotLocked_IrqHandler     [WEAK]
                EXPORT  IcoreEccDed_IrqHandler      [WEAK]
                EXPORT  IcoreEccSec_IrqHandler      [WEAK]
                EXPORT  Reserved12_IrqHandler       [WEAK]
                EXPORT  DualTimerInt1_IrqHandler    [WEAK]
                EXPORT  DualTimerInt2_IrqHandler    [WEAK]
                EXPORT  DualTimerIntC_IrqHandler    [WEAK]
                EXPORT  Watchdog_IrqHandler         [WEAK]
                EXPORT  CyberRomDed_IrqHandler      [WEAK]
                EXPORT  CyberRomSec_IrqHandler      [WEAK]
                EXPORT  CyberRamDed_IrqHandler      [WEAK]
                EXPORT  CyberRamSec_IrqHandler      [WEAK]
                EXPORT  Reserved21_IrqHandler       [WEAK]
                EXPORT  Reserved22_IrqHandler       [WEAK]
                EXPORT  Reserved23_IrqHandler       [WEAK]
                EXPORT  Reserved24_IrqHandler       [WEAK]
                EXPORT  Reserved25_IrqHandler       [WEAK]
                EXPORT  Reserved26_IrqHandler       [WEAK]
                EXPORT  Reserved27_IrqHandler       [WEAK]
                EXPORT  Reserved28_IrqHandler       [WEAK]
                EXPORT  Reserved29_IrqHandler       [WEAK]
                EXPORT  Reserved30_IrqHandler       [WEAK]
                EXPORT  Reserved31_IrqHandler       [WEAK]
Doorbell_IrqHandler
SOF_IrqHandler
EOF_IrqHandler
SOL_IrqHandler
EOL_IrqHandler
CCIsIdle_IrqHandler
CCPowerdownReady_IrqHandler
CCHostIntReq_IrqHandler
PllLocked_IrqHandler
PllNotLocked_IrqHandler
IcoreEccDed_IrqHandler
IcoreEccSec_IrqHandler
Reserved12_IrqHandler
DualTimerInt1_IrqHandler
DualTimerInt2_IrqHandler
DualTimerIntC_IrqHandler
Watchdog_IrqHandler
CyberRomDed_IrqHandler
CyberRomSec_IrqHandler
CyberRamDed_IrqHandler
CyberRamSec_IrqHandler
Reserved21_IrqHandler
Reserved22_IrqHandler
Reserved23_IrqHandler
Reserved24_IrqHandler
Reserved25_IrqHandler
Reserved26_IrqHandler
Reserved27_IrqHandler
Reserved28_IrqHandler
Reserved29_IrqHandler
Reserved30_IrqHandler
Reserved31_IrqHandler
                B       .
                ENDP

                ALIGN
                END
