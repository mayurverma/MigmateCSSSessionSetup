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
# Unit-test makefile for the NvmMgr component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := NvmMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32 cpputest_armcc

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src
INC_PATHS       += -Isrc/CommandHandler/inc -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc -Isrc/Diag/inc -Isrc/OtpmDrv/inc 
INC_PATHS       += -Isrc/AssetMgr/inc 
INC_PATHS		+= -Isrc/Platform/inc
INC_PATHS		+= -Isrc/Diag/inc
INC_PATHS		+= -Isrc/SystemDrv/inc
#INC_PATHS       += -Icontrib/sw-cc312-sbrom/inc -Icontrib/sw-cc312/inc

CFLAGS			+= $(INC_PATHS) 
CPPFLAGS		+= $(INC_PATHS) -Itest/inc -DMOCK_OTPM_DRV_READ_VERBOSE -DMOCK_OTPM_DRV_WRITE_VERBOSE -DMOCK_OTPM_DRV_CONFIGURE_VERBOSE

C_SRC			+= NvmMgr.c NvmMgrInternal.c


CPP_SRC			+= nvmmgr_unit_test.cpp 

CPP_SRC			+= systemmgr_mock.cpp otpmdrv_mock.cpp diag_mock.cpp assetmgr_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/SystemMgr/test src/OtpmDrv/test src/Diag/test src/AssetMgr/test


help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

