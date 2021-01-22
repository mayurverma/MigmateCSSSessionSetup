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
# Unit-test makefile for the Health Monitor component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := HealthMonitor
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32 cpputest_armcc

INC_PATHS		+= -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src
INC_PATHS		+= -Isrc/SystemMgr/inc -Isrc/CommandHandler/inc
INC_PATHS		+= -Isrc/LifecycleMgr/inc -Isrc/PatchMgr/inc
INC_PATHS		+= -Isrc/SessionMgr/inc -Isrc/Interrupts/inc

INC_PATHS		+= -Isrc/Platform/inc -Isrc/Diag/inc -Isrc/CryptoMgr/inc
INC_PATHS		+= -Isrc/SystemDrv/inc -Isrc/WatchdogDrv/inc -Isrc/WatchdogDrv/src
INC_PATHS		+= -Isrc/Init/inc -Isrc/MacDrv/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= HealthMonitor.c HealthMonitorInternal.c
CPP_SRC			+= healthmonitor_unit_test.cpp
CPP_SRC			+= diag_mock.cpp lifecyclemgr_mock.cpp patchmgr_mock.cpp watchdogdrv_mock.cpp init_mock.cpp
CPP_SRC			+= cryptomgr_mock.cpp systemmgr_mock.cpp interrupts_mock.cpp sessionmgr_mock.cpp
CPP_SRC			+= macdrv_mock.cpp systemdrv_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/WatchdogDrv/test
VPATH			+= src/CryptoMgr/test
VPATH			+= src/SystemMgr/test
VPATH			+= src/Diag/test
VPATH			+= src/LifecycleMgr/test
VPATH			+= src/PatchMgr/test
VPATH			+= src/Init/test
VPATH			+= src/Interrupts/test
VPATH			+= src/SessionMgr/test
VPATH			+= src/MacDrv/test
VPATH			+= src/SystemDrv/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

