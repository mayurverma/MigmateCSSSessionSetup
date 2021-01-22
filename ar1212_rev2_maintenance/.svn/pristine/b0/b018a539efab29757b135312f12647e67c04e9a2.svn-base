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
# Unit-test makefile for the Crypto Manager component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= AssetMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src
INC_PATHS       += -Isrc/LifecycleMgr/inc
INC_PATHS       += -Isrc/Platform/inc -Isrc/Diag/inc
INC_PATHS       += -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc -Isrc/CommandDrv/inc
INC_PATHS		+= -Isrc/CommandHandler/inc -Isrc/LifecycleMgr/inc
INC_PATHS       += -Isrc/NvmMgr/inc
INC_PATHS       += -Isrc/CryptoMgr/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

# include the code files
include src/AssetMgr/AssetMgr.mk

CPP_SRC			+= assetmgr_unit_test.cpp nvmmgr_mock.cpp systemmgr_mock.cpp diag_mock.cpp
CPP_SRC			+= cryptomgr_mock.cpp lifecyclemgr_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/Diag/test src/NvmMgr/test src/SystemMgr/test src/CryptoMgr/test
VPATH			+= src/LifecycleMgr/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

