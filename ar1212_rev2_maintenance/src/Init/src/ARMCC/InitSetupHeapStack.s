;//******************************************************************************
;/// \file
;///
;/// This file implements the optional C library startup call-out to setup the
;/// initial stack and heap
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
; * brief    CMSIS Cortex-M ARMv7-M based Core Device Startup File for Device HIP_1v0
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

                AREA    |.text|, CODE, READONLY
                ALIGN
; ---------------------------------------------------------------------------------------------------------------------------------
;
; The ARM C runtime supports either a single combined memory region containing the heap and the main stack,
; or two seperate regions. We use the two-region model to allow more flexibility. The heap and main stack
; regions are controlled by the $(BUILD_TARGET_PROTO_DEVICE).scat linker control file.
;
; Note we also support a separate Process stack; this is initialised prior to main being invoked.
;

; User Initial Stack & Heap
                IMPORT  __use_two_region_memory
                IMPORT  ||Image$$MAIN_STACK$$ZI$$Base||
                IMPORT  ||Image$$MAIN_STACK$$ZI$$Limit||
                IMPORT  ||Image$$HEAP$$ZI$$Base||
                IMPORT  ||Image$$HEAP$$ZI$$Limit||

                EXPORT  __user_setup_stackheap

__user_setup_stackheap PROC
                LDR     R0, =||Image$$HEAP$$ZI$$Base||          ; base of the heap (grows-up)
                LDR     R1, =||Image$$MAIN_STACK$$ZI$$Limit||   ; base of the stack (grows-down)
                LDR     R2, =||Image$$HEAP$$ZI$$Limit||         ; top of the heap
                LDR     R3, =||Image$$MAIN_STACK$$ZI$$Base||    ; top of the stack
                BX      LR
                ENDP

                ALIGN

                END
