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
# Unit-test makefile for the Debug Uart component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := DebugUartDrv
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc
INC_PATHS		+= -Isrc/Platform/inc
INC_PATHS		+= -Isrc/CommandHandler/inc

INC_PATHS		+= -Isrc/DebugUartDrv/inc

INC_PATHS		+= -Icontrib/target/2v0/inc


CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= DebugUartDrv.c retarget.c
CPP_SRC			+= debuguartdrv_unit_test.cpp

# other modules' mocks


VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

