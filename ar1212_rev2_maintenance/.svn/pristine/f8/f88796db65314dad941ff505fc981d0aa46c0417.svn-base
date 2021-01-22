####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
#
# Test makefile for the Init startup code
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := Init
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

# Set a 'default' variant to build in the case where none is specified
BUILD_COMPONENT_VARIANT ?= unit_test

# Supported variants
BUILD_COMPONENT_SUPPORTED_VARIANTS = unit_test c_library_test barebones_test

#
# Common settings
#
COMMON_INCLUDES	+= -Iinc -Icontrib/cmsis/core/include -Isrc/Init/inc
COMMON_INCLUDES += -Isrc/Diag/inc -Isrc/SystemMgr/inc
COMMON_INCLUDES += -Isrc/Platform/inc
COMMON_INCLUDES += -Isrc/SystemDrv/inc -Isrc/MacDrv/inc
COMMON_INCLUDES += -Isrc/DebugUartDrv/inc
CFLAGS          += $(COMMON_INCLUDES) -DBUILD_TEST_INIT
ASFLAGS			+= -Isrc/Diag/inc

ASM_SRC			+=
C_SRC			+=

VPATH			+= src/Init/src src/Init/test src/SystemDrv/src src/PatchMgr/src
VPATH			+= src/DebugUartDrv/src

#
# Variant-specific settings
#
ifeq ($(BUILD_COMPONENT_VARIANT), unit_test)
# Control which build configurations are supported by this component variant
SUPPORTED_BUILD_CONFIGS = cpputest_armcc

CPPFLAGS		+= $(COMMON_INCLUDES) -Itest/inc
CPP_SRC			+= init_unit_test.cpp

include cpputest.mk

else ifeq ($(BUILD_COMPONENT_VARIANT), c_library_test)
# Control which build configurations are supported by this component variant
SUPPORTED_BUILD_CONFIGS = semihosted_armcc barebones_armcc

CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include

C_SRC			+= Init_c_library_test.c

else ifeq ($(BUILD_COMPONENT_VARIANT), barebones_test)
# Control which build configurations are supported by this component variant
SUPPORTED_BUILD_CONFIGS = barebones_armcc

C_SRC			+= Init_barebones_test.c
C_SRC			+= DebugUartDrv.c retarget.c

else
$(error BUILD_COMPONENT_VARIANT '$(BUILD_COMPONENT_VARIANT)' is not supported. Supported variants are '$(BUILD_COMPONENT_SUPPORTED_VARIANTS)'))
endif

help::
	$(QUIET) echo Builds the tests for: $(BUILD_COMPONENT).$(BUILD_COMPONENT_VARIANT)

# include the configuration makefile
include config.mk
