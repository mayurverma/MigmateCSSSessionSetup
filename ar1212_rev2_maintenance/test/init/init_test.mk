# ***********************************************************************************
#  Copyright 2018 ON Semiconductor.  All rights reserved.
#
#  This software and/or documentation is licensed by ON Semiconductor under limited
#  terms and conditions. The terms and conditions pertaining to the software and/or
#  documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
#  ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
#  Do not use this software and/or documentation unless you have carefully read and
#  you agree to the limited terms and conditions. By using this software and/or
#  documentation, you agree to the limited terms and conditions.
#
# *************************************************************************************
#
# Makefile for the Init integration test
#
# *************************************************************************************

# Pre-requisits for build system
BUILD_COMPONENT	:= init
BUILD_COMPONENT_ROOTDIR	:= test/$(BUILD_COMPONENT)
BUILD_ARTEFACT	:= EXE

# Optional build configuration to restict scope for this node
SUPPORTED_BUILD_CONFIGS = barebones_armcc semihosted_armcc

# Define the filename base for system ROM output files
BUILD_ROM_BASENAME := $(BUILD_COMPONENT)_ROM
BUILD_VPM_BASENAME := $(BUILD_COMPONENT)_ROM

#
# Common settings
#
COMMON_INCLUDES	+= -Iinc -Icontrib/cmsis/core/include -Itest/init/inc
COMMON_INCLUDES += -Isrc/Init/inc -Isrc/Diag/inc -Isrc/SystemMgr/inc
COMMON_INCLUDES += -Isrc/SystemDrv/inc
COMMON_INCLUDES += -Isrc/Platform/inc

ifeq ($(BUILD_CONFIG),barebones_armcc)
COMMON_INCLUDES	+= -Isrc/DebugUartDrv/inc
C_SRC			+= DebugUartDrv.c retarget.c
endif

ASFLAGS			+= -Isrc/Diag/inc
CFLAGS          += $(COMMON_INCLUDES)
CPPFLAGS		+= $(COMMON_INCLUDES)

# Exclude the default Init source files to allow override of HardFault exception handler
BUILD_ARMCC_EXCLUDE_INIT	:= 1

ASM_SRC			+= InitStartup.s InitSetupHeapStack.s InitHardFaultDebug.s
C_SRC			+= InitSystem.c InitMpu.c InitRamVectors.c InitSysRetarget.c
C_SRC			+= main.c init_test.c init_test_hardfault_handler.c
C_SRC			+= SystemDrv.c
ASM_SRC			+= SystemDrvLowLevel.s
LDFLAGS			+= --entry=InitStart

VPATH			+= test/init/src
VPATH			+= src/Init/src src/Init/src/ARMCC
VPATH			+= src/SystemDrv/src src/SystemDrv/src/ARMCC
VPATH 			+= src/DebugUartDrv/src

help::
	$(QUIET) echo Builds the tests for: $(BUILD_COMPONENT)

# include the configuration makefile
include config.mk
