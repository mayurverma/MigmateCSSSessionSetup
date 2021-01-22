;//******************************************************************************
;/// \file
;///
;/// This file implements the patched HardFault handler, used by the SpduApp and
;/// ProdTestApp patches. It checks if the HardFault was triggered by an OTPM
;/// ECC DED error as a result of an OTPM read. If so, it returns to the
;/// instruction immediately following the OTPM access.
;///
;/// Note that the data read from the OTPM is not available, therefore the user
;/// should not use the data returned by OtpmDrvRead(). The user MUST call
;/// OtpmDrvGetLastError() to confirm an ECC DED did not occur before using the
;/// data returned.
;///
;/// If the HardFault was not due to an OTPM ECC DED, the patched handler performs
;/// the same operations as the ROM handler.
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

HardFault_PatchedHandler\
                PROC
                EXPORT  HardFault_PatchedHandler
                IMPORT  InitHardFaultHandler
                IMPORT  OtpmDrvIsHardwareFault

                ; Note R0..R3, R12, R14, PC and xPSR are pushed to the stack
                ; - Save LR so we can return
                MOV     R12, LR

                ; Test if the processor RAM detected an ECC DED fault
                LDR     R0, NVIC_ISPR
                LDR     R0, [R0]                ; R0 = NVIC_ISPR
                MOVS    R1, #1
                LSLS    R1, #CyberRamDed_IRQn   ; R1 = 1 << CyberRamDed_IRQn
                ANDS    R0, R1                  ; R0 = R0 & R1
                BNE     HardFault_Shutdown

                ; Processor RAM is safe to access, check for OTPM ECC error
                BL      OtpmDrvIsHardwareFault    ; on return R0 != 0 if ECC DED error occurred
                BNE     HardFault_PatchedOtpmEccDed

                ; No OTPM ECC error, invoke the C handler
                MRS     R0, MSP
                MOV     R1, R12                 ; R12 = saved LR
                MRS     R2, CONTROL
                LDR     R3, =InitHardFaultHandler
                BX      R3

HardFault_PatchedOtpmEccDed
                ; OTPM ECC error, return to the instruction after the faulting access
                ; - the HardFault exception stacked the faulting PC at SP[6]
                ; - modify SP[6] so we return to the next (16-bit) instruction
                MRS     R0, MSP                 ; R0 = SP
                LDR     R1, [R0, #24]           ; R1 = SP[6]
                ADDS    R1, #2                  ; R1 += 2
                STR     R1, [R0, #24]           ; SP[6] = SP

                ; Return
                BX      R12                     ; R12 = saved LR

                ;
                ; ECC DED fault occurred in the processor RAM
                ;
HardFault_Shutdown
                IMPORT SystemDrvTerminate

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
                MOVS    R1, #SYSTEM_MGR_CSS_FAULT_RAM_ECC_FAILURE
                LSLS    R1, #CRYPTO_ICORE_CRYPTOERROR_FAULT_CODE__SHIFT
                STR     R1, [R0]                                ; CRYPTOERROR = FaultCode | 0x00

                ; 8. Terminate the firmware
                BL      SystemDrvTerminate
                ENDP

                ALIGN

CyberRamDed_IRQn    EQU 19
ERROR_SYSERR        EQU 0x002

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
