;//******************************************************************************
;/// \file
;///
;/// This file implements the production HardFault handler. A HardFault can
;/// occur for a number of reasons:
;///
;/// | Fault Class  | Fault Condition                                            |
;/// |:------------:|:-----------------------------------------------------------|
;/// |Memory related| 1. Bus error (can be program accesses or data accesses,    |
;/// |              |    also referred to as bus faults in Cortex-M3):           |
;/// |              |    a. Bus error generated by bus infrastructure because of |
;/// |              |       an invalid address during bus transaction            |
;/// |              |    b. Bus error generated by bus slave                     |
;/// |              | 2. Attempt to execute the program from a memory region     |
;/// |              |    marked as nonexecutable                                 |
;/// | Program error| 3. Execution of undefined instruction                      |
;/// |(usage faults)| 4. Trying to switch to ARM state (Cortex-M0+ only supports |
;/// |              |    Thumb instructions)                                     |
;/// |              | 5. Attempt to generate an unaligned memory access (not     |
;/// |              |    allowed in ARMv6-M)                                     |
;/// |              | 6. Attempt to execute an SVC when the SVC exception        |
;/// |              |    priority level is the same or lower than the current    |
;/// |              |    exception level                                         |
;/// |              | 7. Invalid EXC_RETURN value during exception return        |
;/// |              | 8. Attempt to execute a breakpoint instruction (BKPT) when |
;/// |              |    debug is not enabled (no debugger attached)             |
;///
;/// Bus errors can be generated by the ECC 'wrappers' that control the processor
;/// ROM and RAM instances. If an ECC uncorrectable error occurs, the wrapper will
;/// assert the PSLVERROR bus signal which triggers a HardFault exception. Similarly,
;/// if the OTPM hardware detects an uncorrectable error when the OTPM is read, it
;/// will assert PSLVERROR which also triggers a HardFault exception.
;///
;/// If the processor data RAM wrapper detects an ECC uncorrectable error, the
;/// firmware cannot access the stack in case the ECC fault occurred in the stack
;/// region of the data RAM. Accessing it again will cause a second HardFault
;/// within the HardFault handler, which results in processor lock-up.
;///
;/// The HardFault handler tests whether a processor data RAM ECC uncorrectable
;/// error occurred. If it did, it performs a limited shutdown of the cybersecurity
;/// subsystem without accessing data RAM. The firmware perfoms the same shutdown if
;/// the HardFault originated from a stack overflow. If none of this caused the fault,
;/// the Firmware assumes it can use the data RAM, and therefore it invokes the
;/// C HardFault handler which performs a full shutdown of the system.
;///
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

                PRESERVE8
                THUMB

                AREA    |.text|, CODE, READONLY

                INCLUDE AR0820_memmap.inc
                INCLUDE crypto_icore.inc
                INCLUDE crypto_vcore.inc
                INCLUDE mcu_sysctrl.inc
                INCLUDE SystemMgrCssFault.inc

HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler
                IMPORT  InitHardFaultHandler
                IMPORT  InitHardFaultShutdown
                IMPORT  ||Image$$MAIN_STACK$$ZI$$Base||
                ;
                ; Save the main stack pointer, LR and CONTROL reg contents
                ; - we'll need these later if we invoke the C handler
                ;
                MRS     R0, MSP
                MOV     R4, R0                  ; R4 = stack pointer
                MOV     R5, LR                  ; R5 = LR
                MRS     R0, CONTROL
                MOV     R6, R0                  ; R6 = CONTROL

                ; Test if the processor RAM detected an ECC DED fault
                LDR     R0, NVIC_ISPR
                LDR     R0, [R0]                ; R0 = NVIC_ISPR
                MOVS    R1, #1
                LSLS    R1, #CyberRamDed_IRQn   ; R1 = 1 << CyberRamDed_IRQn
                ANDS    R0, R1                  ; R0 = R0 & R1
                BNE     HardFault_Shutdown_Ecc

                ; Test if the MPU detected an stack overflow
                MOV     R0, R4
                LDR     R1, StackBottomAddr     ; R1 = lowest address of the stack + safety
                CMP     R0, R1                  ; Subtract R1 from R0
                BLS     HardFault_Shutdown_Stack_Overflow         ; Branch if stack pointer (R0) <= Bottom of stack (R1)

                ; Processor RAM is safe to access, invoke the C handler
                MOV     R0, R4
                MOV     R1, R5
                MOV     R2, R6
                LDR     R3, =InitHardFaultHandler
                BX      R3

HardFault_Shutdown_Ecc ; ECC DED fault occurred in the processor RAM
                MOVS    R0, #SYSTEM_MGR_CSS_FAULT_RAM_ECC_FAILURE
                B       HardFault_Shutdown

HardFault_Shutdown_Stack_Overflow ; Stack Overflow occurred in the processor RAM
                MOVS    R0, #SYSTEM_MGR_CSS_FAULT_STACK_OVERFLOW
                ; not needed -> B HardFault_Shutdown

HardFault_Shutdown
                IMPORT SystemDrvTerminate

                ; Save R0 in R3 to be used in step 7
                MOVS    R3, R0

                ;  1. Disable video authentication
                ;     1a. Apply pulsed reset to the VCore
                LDR     R0, VcoreCnfReg
                LDR     R1, [R0]                                ; R1 = CRYPTO_VCORE_CNFG
                MOVS    R2, #1
                LSLS    R2, #CRYPTO_VCORE_CNFG_CPU_VCORE_RESET__SHIFT   ; R2 = 1 << CRYPTO_VCORE_CNFG_CPU_VCORE_RESET__SHIFT
                ORRS    R1, R2
                STR     R1, [R0]                                ; CRYPTO_VCORE_CNFG != CPU_VCORE_RESET

                ;     1b. Disable the VCore clock
                LDR     R0, McuSysctrlResetReg
                LDR     R1, [R0]                                ; R1 = MCU_SYSCTRL_RESET
                MOVS    R2, #1
                LSLS    R2, #MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__SHIFT  ; R2 = 1 << MCU_SYSCTRL_RESET_CONTROL_VCORE_CLK_EN__SHIFT
                BICS    R1, R2                                  ; R1 = R1 & ~R2
                STR     R1, [R0]                                ; MCU_SYSCTRL_RESET &= ~VCORE_CLK_EN

                ; 2. Reset the CC312
                MOVS    R2, #1
                LSLS    R2, #MCU_SYSCTRL_RESET_CONTROL_HARD_RESET__SHIFT; R2 = 1 << MCU_SYSCTRL_RESET_CONTROL_HARD_RESET__SHIFT
                ORRS    R1, R2
                STR     R1, [R0]                                ; MCU_SYSCTRL_RESET |= CC312_HARD_RESET

                ; 3. Disable watchdog
                ;    3a. unlock the registers
                LDR     R0, WatchdogLockReg
                LDR     R1, WatchdogAccess
                STR     R1, [R0]                                ; WDOGLOCK = 0x1ACCE551

                ;    3b. clear the reset-enable and interrupt-enable bits
                LDR     R0, WatchdogControlReg
                MOVS    R1, #0
                STR     R1, [R0]                                ; WDOGCONTROL = 0

                ; 4. Complete host command with SYSERR
                LDR     R0, CommandReg
                MOVS    R1, #ERROR_SYSERR
                STR     R1, [R0]                                ; COMMAND = ERROR_SYSERR

                ; 5. Set the fatal error bit in the CHECKPOINT register
                LDR     R0, CheckpointReg
                LDR     R1, [R0]                                ; R1 = CHECKPOINT
                MOVS    R2, #1
                LSLS    R2, #CRYPTO_ICORE_CHECKPOINT_FATAL_ERROR__SHIFT; R2 = 1 << 15
                ORRS    R1, R2
                STR     R1, [R0]                                ; CHECKPOINT |= 0x8000

                ; 6. Clear the Info reg
                LDR     R0, InfoReg
                MOVS    R1, #0
                STR     R1, [R0]                                ; INFO = 0

                ; 7. Set the CryptoError reg
                LDR     R0, CryptoErrorReg
                MOVS    R1, R3                                  ; Using R3 saved at the begining of HardFault_Shutdown
                LSLS    R1, #CRYPTO_ICORE_CRYPTOERROR_FAULT_CODE__SHIFT
                STR     R1, [R0]                                ; CRYPTOERROR = FaultCode | 0x00

                ; 8. Terminate the firmware
                BL      SystemDrvTerminate
                ENDP

                ALIGN
CyberRamDed_IRQn    EQU 19
ERROR_SYSERR        EQU 0x002
MPU_STACK_GUARD_SIZE_BYTES    EQU  32 ; Note: keep in sync with c header

StackBottomAddr     DCD ||Image$$MAIN_STACK$$ZI$$Base|| + MPU_STACK_GUARD_SIZE_BYTES
NVIC_ISPR           DCD 0xE000E200
VcoreCnfReg         DCD CRYPTO_VCORE_CNFG
McuSysctrlResetReg  DCD MCU_SYSCTRL_RESET_CONTROL
WatchdogControlReg  DCD (WATCHDOG_REG_BASE + 0x008)
WatchdogLockReg     DCD (WATCHDOG_REG_BASE + 0xC00)
WatchdogAccess      DCD 0x1ACCE551
CommandReg          DCD CRYPTO_ICORE_COMMAND
CheckpointReg       DCD CRYPTO_ICORE_CHECKPOINT
InfoReg             DCD CRYPTO_ICORE_INFO
CryptoErrorReg      DCD CRYPTO_ICORE_CRYPTOERROR

                END
