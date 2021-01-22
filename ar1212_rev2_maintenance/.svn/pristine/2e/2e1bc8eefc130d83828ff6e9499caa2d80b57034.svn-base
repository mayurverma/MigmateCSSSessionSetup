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
# Unit-test makefile for the Platform component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := Platform
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src
INC_PATHS		+= -Isrc/SystemMgr/inc
INC_PATHS		+= -Isrc/Diag/inc
INC_PATHS		+= -Isrc/Interrupts/inc
INC_PATHS		+= -Isrc/SystemDrv/inc
INC_PATHS		+= -Isrc/OtpmDrv/inc
INC_PATHS		+= -Isrc/WatchdogDrv/inc
INC_PATHS		+= -Isrc/CommandDrv/inc
INC_PATHS		+= -Isrc/MacDrv/inc
INC_PATHS		+= -Isrc/AssetMgr/inc
INC_PATHS		+= -Isrc/CommandHandler/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= Platform.c
CPP_SRC			+= platform_unit_test.cpp
CPP_SRC			+= diag_mock.cpp
CPP_SRC			+= systemmgr_mock.cpp
CPP_SRC			+= interrupts_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/Diag/test
VPATH			+= src/SystemMgr/test
VPATH			+= src/Interrupts/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

