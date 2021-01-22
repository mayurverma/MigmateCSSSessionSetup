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
# Unit-test makefile for the system tick driver component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := SysTickDrv
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc
INC_PATHS		+= -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc
INC_PATHS		+= -Isrc/Diag/inc -Isrc/Platform/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

# Enable basic fatal error reports (to prevent bloating code)
CFLAGS			+= -DUSE_BASIC_FATAL_ERROR_HANDLER

C_SRC			+= SysTickDrv.c
CPP_SRC			+= systickdrv_unit_test.cpp diag_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/Diag/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

