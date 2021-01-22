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
# Makefile for the System integration test
#
# *************************************************************************************

# Building the system ROM executable
BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= system
BUILD_COMPONENT_ROOTDIR := test/$(BUILD_COMPONENT)

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc semihosted_armcc

# List of components in the system ROM
BUILD_COMPONENT_LIST := AssetMgr CommandDrv CommandHandler CC312HalPal CryptoMgr DebugUartDrv \
	Diag HealthMonitor Interrupts LifecycleMgr MacDrv NvmMgr OtpmDrv PatchMgr Platform SessionMgr \
	SystemDrv SystemMgr SysTickDrv WatchdogDrv

# Define the filename base for system ROM output files
BUILD_ROM_BASENAME := $(BUILD_COMPONENT)_ROM
BUILD_VPM_BASENAME := $(BUILD_COMPONENT)_ROM

# Use the extended hard fault handler
BUILD_USE_EXT_HARDFAULT_HANDLER := yes

# Use the unexpected exception/interrupt handler
BUILD_USE_UNEXPECTED_HANDLER := yes

# include the component makefiles
BUILD_COMPONENT_MAKEFILES := $(foreach component,$(BUILD_COMPONENT_LIST), $(addprefix src/$(component)/, $(addsuffix .mk, $(component))))
include $(BUILD_COMPONENT_MAKEFILES)

INC_PATHS += -Iinc

CFLAGS += $(INC_PATHS)

C_SRC += main.c

VPATH += src/Rom/src

# Select the appropriate ARM libs to be linked in.
# SW-C312 libraries
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312/lib/release
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312-sbrom/lib/release
LDFLAGS	+= $(ADD_LIB)cc_312
LDFLAGS	+= $(ADD_LIB)cc_312bsv
LDFLAGS	+= $(ADD_LIB)mbedtls
LDFLAGS	+= $(ADD_LIB)mbedcrypto

# include the configuration makefile
include config.mk

