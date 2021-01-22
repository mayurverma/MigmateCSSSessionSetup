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
BUILD_COMPONENT	:= CryptoMgr
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_armcc cpputest_arm_gnu cpputest_mingw32

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/$(BUILD_COMPONENT)/src
INC_PATHS       += -Isrc/LifecycleMgr/inc
INC_PATHS       += -Isrc/Platform/inc -Isrc/Diag/inc
INC_PATHS       += -Isrc/Interrupts/inc -Isrc/Init/inc
INC_PATHS       += -Icontrib/sw-cc312-sbrom/inc
INC_PATHS       += -Icontrib/sw-cc312/inc
INC_PATHS       += -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc -Isrc/CommandDrv/inc -Isrc/CommandHandler/inc
INC_PATHS       += -Isrc/PatchMgr/inc
INC_PATHS       += -Isrc/AssetMgr/inc
INC_PATHS       += -Isrc/CC312HalPal/inc
INC_PATHS       += -Isrc/OtpmDrv/inc

CFLAGS			+= $(INC_PATHS)
CFLAGS			+= -DMBEDTLS_CONFIG_FILE='<config-cc312-ar0820.h>'
CPPFLAGS		+= $(INC_PATHS) -Itest/inc
CPPFLAGS		+= -DMBEDTLS_CONFIG_FILE='<config-cc312-ar0820.h>'

# include the code files
include src/CryptoMgr/CryptoMgr.mk
include src/CC312HalPal/CC312HalPal.mk
include src/OtpmDrv/OtpmDrv.mk

CPP_SRC			+= cryptomgr_armcc_test.cpp
CPP_SRC			+= diag_mock.cpp lifecyclemgr_mock.cpp patchmgr_mock.cpp systemmgr_mock.cpp

# Select the appropriate ARM libs to be linked in.
# SW-C312 libraries
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312/lib/debug
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312-sbrom/lib/debug
LDFLAGS	+= $(ADD_LIB)cc_312
LDFLAGS	+= $(ADD_LIB)cc_312bsv
LDFLAGS	+= $(ADD_LIB)mbedtls
LDFLAGS	+= $(ADD_LIB)mbedcrypto
LDFLAGS			+= --verbose

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/Diag/test src/LifecycleMgr/test src/PatchMgr/test src/SystemMgr/test
VPATH			+= src/CC312HalPal/src
VPATH			+= src/OtpmDrv/src
help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

