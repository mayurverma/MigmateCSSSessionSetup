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
# Unit-test makefile for the System Manager component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= SystemMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc
INC_PATHS       += -Isrc/LifecycleMgr/inc -Isrc/AssetMgr/inc
INC_PATHS       += -Isrc/SessionMgr/inc -Isrc/CryptoMgr/inc
INC_PATHS       += -Isrc/NvmMgr/inc -Isrc/HealthMonitor/inc
INC_PATHS       += -Isrc/PatchMgr/inc -Isrc/CommandHandler/inc
INC_PATHS       += -Isrc/Platform/inc -Isrc/Diag/inc
INC_PATHS		+= -Isrc/Interrupts/inc -Isrc/Init/inc
INC_PATHS       += -Isrc/SystemDrv/inc -Isrc/CommandDrv/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc -Isrc/SystemMgr/src
CPPFLAGS		+= -DMOCK_DISABLE_COMMAND_HANDLER_ON_DOORBELL_EVENT
CPPFLAGS		+= -DMOCK_DISABLE_SYSTEM_DRV_WAIT_FOR_INTERRUPT

C_SRC			+= SystemMgr.c SystemMgrInternal.c
CPP_SRC			+= systemmgr_unit_test.cpp
CPP_SRC			+= diag_mock.cpp lifecyclemgr_mock.cpp patchmgr_mock.cpp
CPP_SRC			+= platform_mock.cpp interrupts_mock.cpp commandhandler_mock.cpp
CPP_SRC			+= healthmonitor_mock.cpp commanddrv_mock.cpp cryptomgr_mock.cpp
CPP_SRC			+= systemdrv_mock.cpp

VPATH			+= src/Diag/test src/LifecycleMgr/test src/PatchMgr/test
VPATH			+= src/Platform/test src/Interrupts/test
VPATH			+= src/HealthMonitor/test src/CommandDrv/test
VPATH			+= src/CryptoMgr/test src/CommandHandler/test
VPATH			+= src/SystemDrv/test

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

