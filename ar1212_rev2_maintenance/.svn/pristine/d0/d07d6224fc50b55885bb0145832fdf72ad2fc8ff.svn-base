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
# Unit-test makefile for the system driver component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := SystemDrv
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/test
INC_PATHS		+= -Isrc/SystemMgr/inc
INC_PATHS		+= -Isrc/Platform/inc
INC_PATHS		+= -Isrc/Interrupts/inc
INC_PATHS		+= -Isrc/Diag/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

ifeq ($(BUILD_CONFIG), cpputest_armcc)
ASM_SRC			+= SystemDrvLowLevel.s
VPATH 			+= src/$(BUILD_COMPONENT)/src/ARMCC
endif

ifeq ($(BUILD_CONFIG), cpputest_arm_gnu)
ASM_SRC			+= SystemDrvLowLevel.S
VPATH 			+= src/$(BUILD_COMPONENT)/src/GCC
endif

ifeq ($(BUILD_CONFIG), cpputest_mingw32)
C_SRC			+= SystemDrvStubs.c
endif

C_SRC			+= SystemDrv.c
CPP_SRC			+= systemdrv_unit_test.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

