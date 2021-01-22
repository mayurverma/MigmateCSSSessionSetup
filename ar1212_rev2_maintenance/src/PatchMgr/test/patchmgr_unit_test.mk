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
# Unit-test makefile for the Patch Manager component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := PatchMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32

INC_PATHS		+= -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src -Isrc/Diag/inc
INC_PATHS		+= -Isrc/$(BUILD_COMPONENT)/test
INC_PATHS		+= -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc -Isrc/CommandHandler/inc -Isrc/Platform/inc
INC_PATHS		+= -Isrc/Interrupts/inc -Isrc/LifecycleMgr/inc  -Isrc/CryptoMgr/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

# Enable basic fatal error reports (to prevent bloating code)
CFLAGS			+= -DUSE_BASIC_FATAL_ERROR_HANDLER

ASM_SRC			+= PatchMgrSvcHandler.s
C_SRC			+= PatchMgr.c PatchMgrInternal.c PatchMgrPatchSupport.c
ifeq ($(BUILD_CONFIG), cpputest_mingw32)
CPP_SRC			+= patchmgr_unit_test.cpp
CPP_SRC			+= diag_mock.cpp lifecyclemgr_mock.cpp systemmgr_mock.cpp cryptomgr_mock.cpp
VPATH			+= src/Diag/test
VPATH			+= src/LifecycleMgr/test
VPATH			+= src/SystemMgr/test
VPATH			+= src/CryptoMgr/test
endif

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/SystemDrv/src

VPATH			+= src/$(BUILD_COMPONENT)/src/ARMCC

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

