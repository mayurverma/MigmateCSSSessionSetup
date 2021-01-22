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
# Unit-test makefile for the Session Manager component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= SessionMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc
INC_PATHS		+= -Isrc/CommandHandler/inc
INC_PATHS		+= -Isrc/SystemMgr/inc
INC_PATHS		+= -Isrc/AssetMgr/inc
INC_PATHS		+= -Isrc/LifecycleMgr/inc
INC_PATHS		+= -Isrc/CryptoMgr/inc
INC_PATHS		+= -Isrc/SystemDrv/inc
INC_PATHS		+= -Isrc/Diag/inc
INC_PATHS		+= -Isrc/Platform/inc
INC_PATHS		+= -Isrc/MacDrv/inc
INC_PATHS		+= -Isrc/HealthMonitor/src

CFLAGS			+= $(INC_PATHS)
CFLAGS			+= -Dcalloc=unittest_calloc
CFLAGS			+= -Dmalloc=unittest_malloc
CFLAGS			+= -Dfree=unittest_free
CPPFLAGS		+= $(INC_PATHS) -Itest/inc -Isrc/SessionMgr/src
CPPFLAGS		+= -DMOCK_DISABLE_MAC_DRV_SET_CONFIG -DMOCK_DISABLE_MAC_DRV_SET_ROI_CONFIG

C_SRC			+= SessionMgr.c SessionMgrInternal.c
C_SRC			+= SessionMgrValidation.c SessionMgrVideoAuthentication.c
C_SRC			+= SessionMgrSetupSession.c SessionMgrRetrieveCertificates.c
CPP_SRC			+= sessionmgr_unit_test.cpp
CPP_SRC			+= sessionmgr_setpsksessionkeys_unit_test.cpp
CPP_SRC			+= sessionmgr_setsessionkeys_unit_test.cpp
CPP_SRC			+= sessionmgr_setvideoauthroi_unit_test.cpp
CPP_SRC			+= systemmgr_mock.cpp
CPP_SRC			+= diag_mock.cpp
CPP_SRC			+= systemdrv_mock.cpp
CPP_SRC			+= macdrv_mock.cpp
CPP_SRC			+= assetmgr_mock.cpp
CPP_SRC			+= lifecyclemgr_mock.cpp
CPP_SRC			+= cryptomgr_mock.cpp

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/SystemMgr/test
VPATH			+= src/Diag/test
VPATH			+= src/SystemDrv/test
VPATH			+= src/MacDrv/test
VPATH			+= src/AssetMgr/test
VPATH			+= src/LifecycleMgr/test
VPATH			+= src/CryptoMgr/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

