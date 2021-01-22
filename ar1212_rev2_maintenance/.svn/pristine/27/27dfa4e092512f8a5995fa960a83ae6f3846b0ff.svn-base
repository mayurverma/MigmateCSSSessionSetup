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
# Unit-test makefile for the DepuApp
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := DepuApp
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32 cpputest_armcc

INC_PATHS += -Iinc
INC_PATHS += -Isrc/Patch/inc -Isrc/Init/inc
INC_PATHS += -I$(BUILD_COMPONENT_ROOTDIR)/inc -I$(BUILD_COMPONENT_ROOTDIR)/src -I$(BUILD_COMPONENT_ROOTDIR)/test
INC_PATHS += -Isrc/DepuApp/inc -Isrc/DepuApp/src
INC_PATHS += -Isrc/PatchMgr/inc -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc  -Isrc/CommandHandler/inc -Isrc/CommandHandler/src
INC_PATHS += -Isrc/Diag/inc -Isrc/LifecycleMgr/inc -Isrc/OtpmDrv/src -Isrc/OtpmDrv/inc -Isrc/OtpmDrv/test -Isrc/AssetMgr/inc -Isrc/AssetMgr/src
INC_PATHS += -Isrc/CC312HalPal/inc -Isrc/CryptoMgr/inc -Isrc/NvmMgr/inc -Isrc/NvmMgr/src
INC_PATHS += -Isrc/OtpmDrv/inc
INC_PATHS += -Icontrib/target/2v0/inc -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312-sbrom/inc
INC_PATHS += -Isrc/Toolbox/inc

INC_PATHS += -Isrc/DepuLib/inc

CFLAGS			+= $(INC_PATHS)
CFLAGS			+= -DCC_TEE
BUILD_PATCH_VERSION ?= 36865
CFLAGS += -DDEPU_PATCH_VERSION=$(BUILD_PATCH_VERSION)U

CPPFLAGS		+= $(INC_PATHS) -Itest/inc

# Enable basic fatal error reports (to prevent bloating code)
CFLAGS			+= -DUSE_BASIC_FATAL_ERROR_HANDLER

C_SRC			+= DepuApp.c DepuAppInternal.c DepuAppOtpmProvisioning.c DepuAppPatchLoader.c
CPP_SRC			+= depuapp_unit_test.cpp depuapp_mock.cpp
CPP_SRC			+= diag_mock.cpp lifecyclemgr_mock.cpp systemmgr_mock.cpp
CPP_SRC			+= assetmgr_mock.cpp cryptomgr_mock.cpp otpmdrv_mock.cpp nvmmgr_mock.cpp
CPP_SRC			+= commandhandler_mock.cpp
CPP_SRC			+= init_mock.cpp
CPP_SRC 		+= cc312halpal_mock.cpp
CPP_SRC 		+= toolbox_mock.cpp

VPATH 			+= src/OtpmDrv/test src/Diag/test src/LifecycleMgr/test src/AssetMgr/test
VPATH 			+= src/CryptoMgr/test src/SystemMgr/test src/NvmMgr/test
VPATH 			+= src/Toolbox/test

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH 			+= src/OtpmDrv/src
VPATH			+= src/CommandHandler/test
VPATH			+= src/Init/test
VPATH 			+= src/CC312HalPal/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

