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
# Unit-test makefile for the Command Handler component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := CommandHandler
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc
INC_PATHS		+= -Isrc/SystemMgr/inc -Isrc/Interrupts/inc
INC_PATHS		+= -Isrc/CryptoMgr/inc -Isrc/LifecycleMgr/inc
INC_PATHS		+= -Isrc/AssetMgr/inc -Isrc/SessionMgr/inc
INC_PATHS		+= -Isrc/PatchMgr/inc -Isrc/HealthMonitor/inc
INC_PATHS		+= -Isrc/Platform/inc -Isrc/Interrupts/inc
INC_PATHS		+= -Isrc/Diag/inc -Isrc/SystemDrv/inc -Isrc/CommandDrv/inc
INC_PATHS		+= -Isrc/CommandHandler/src

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= CommandHandler.c CommandHandlerInternal.c
CPP_SRC			+= commandhandler_unit_test.cpp

# other modules' mocks
CPP_SRC			+= diag_mock.cpp cryptomgr_mock.cpp interrupts_mock.cpp assetmgr_mock.cpp
CPP_SRC			+= healthmonitor_mock.cpp lifecyclemgr_mock.cpp patchmgr_mock.cpp sessionmgr_mock.cpp
CPP_SRC			+= systemmgr_mock.cpp systemdrv_mock.cpp commanddrv_mock.cpp

VPATH			+= src/CryptoMgr/test src/Interrupts/test src/AssetMgr/test src/HealthMonitor/test
VPATH			+= src/LifecycleMgr/test src/PatchMgr/test src/SessionMgr/test src/SystemMgr/test
VPATH			+= src/CommandDrv/test src/SystemDrv/test src/Diag/test

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

