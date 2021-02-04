####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Unit-test makefile for the User Guide Examples
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= UserGuideExamples
BUILD_COMPONENT_ROOTDIR := doc/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32

INC_PATHS       += -Idoc/$(BUILD_COMPONENT)/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= css_shared_mem.c css_host_command.c
CPP_SRC			+= userguideexamples_unit_test.cpp

VPATH			+= doc/$(BUILD_COMPONENT)/src doc/$(BUILD_COMPONENT)/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

