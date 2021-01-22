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
# Makefile for the Init HardFault integration test
#
# *************************************************************************************

# Pre-requisits for build system
BUILD_COMPONENT	:= hardfault
BUILD_COMPONENT_ROOTDIR	:= test/$(BUILD_COMPONENT)
BUILD_ARTEFACT	:= EXE

# Optional build configuration to restict scope for this node
SUPPORTED_BUILD_CONFIGS = barebones_armcc semihosted_armcc

# Define the filename base for system ROM output files
BUILD_ROM_BASENAME := $(BUILD_COMPONENT)_ROM
BUILD_VPM_BASENAME := $(BUILD_COMPONENT)_ROM

# Select a default test case
TEST_CASE		?= 1

#
# Common settings
#
COMMON_INCLUDES	+= -Iinc -Itest/hardfault/inc
COMMON_INCLUDES	+= -Icontrib/cmsis/core/include -Icontrib/sw-cc312-sbrom/inc -Icontrib/sw-cc312/inc
COMMON_INCLUDES += -Isrc/Init/inc -Isrc/SystemMgr/inc -Isrc/AssetMgr/inc -Isrc/CommandHandler/inc
COMMON_INCLUDES += -Isrc/Platform/inc -Isrc/Diag/inc -Isrc/Interrupts/inc
COMMON_INCLUDES += -Isrc/SystemDrv/inc -Isrc/CommandDrv/inc
COMMON_INCLUDES	+= -Isrc/MacDrv/inc -Isrc/WatchdogDrv/inc

ifeq ($(BUILD_CONFIG),barebones_armcc)
COMMON_INCLUDES	+= -Isrc/DebugUartDrv/inc
C_SRC			+= DebugUartDrv.c retarget.c
endif

ASFLAGS			+= -Isrc/Diag/inc -Isrc/SystemMgr/inc
CFLAGS          += $(COMMON_INCLUDES) -DTEST_CASE=$(TEST_CASE)
CPPFLAGS		+= $(COMMON_INCLUDES)

# Use the extended hard fault handler
BUILD_USE_EXT_HARDFAULT_HANDLER := yes

# Use the unexpected exception handler
BUILD_USE_UNEXPECTED_HANDLER := yes

C_SRC			+= main.c hardfault_test.c
C_SRC			+= Interrupts.c
C_SRC			+= SystemDrv.c CommandDrv.c MacDrv.c WatchdogDrv.c
C_SRC			+= bsv_otp_api.c
ASM_SRC			+= SystemDrvLowLevel.s

VPATH			+= test/hardfault/src
VPATH			+= src/Interrupts/src
VPATH			+= src/OtpmDrv/src/ARMCC
VPATH			+= src/SystemDrv/src src/SystemDrv/src/ARMCC
VPATH			+= src/CommandDrv/src src/WatchdogDrv/src
VPATH			+= src/MacDrv/src
VPATH			+= src/CC312HalPal/src
VPATH 			+= src/DebugUartDrv/src

# Link against the CC312 Secure Boot lib
LDFLAGS			+= --userlibpath=contrib/sw-cc312-sbrom/lib/$(BUILD_TYPE)
LDFLAGS			+= --library=cc_312bsv

help::
	$(QUIET) echo Builds the tests for: $(BUILD_COMPONENT)

include src/OtpmDrv/OtpmDrv.mk
CFLAGS			+= $(INC_PATHS)

# include the configuration makefile
include config.mk
