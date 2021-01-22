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
# Unit-test makefile for the CC312HalPal component
#
####################################################################################

BUILD_ARTEFACT := EXE
BUILD_COMPONENT := CC312HalPal
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS = cpputest_mingw32 cpputest_armcc

CFLAGS += -DCC_IOT -D__ARM_DS5__
CFLAGS += -DMBEDTLS_CONFIG_FILE='<config-cc312-ar0820.h>'
ifeq ($(BUILD_TYPE), debug)
	CFLAGS += -DCC_CONFIG_TRNG_MODE=1
endif
ifeq ($(BUILD_TYPE), release)
	CFLAGS += -DCC_CONFIG_TRNG_MODE=0
endif

INC_PATHS       += -Iinc -Isrc/$(BUILD_COMPONENT)/inc -Isrc/Diag/inc
INC_PATHS		+= -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc -Isrc/CommandHandler/inc -Isrc/Platform/inc
INC_PATHS		+= -Isrc/Interrupts/inc -Isrc/LifecycleMgr/inc -Isrc/CryptoMgr/inc -Isrc/CC312HalPal/inc
INC_PATHS		+= -Isrc/AssetMgr/inc -Isrc/OtpmDrv/inc
INC_PATHS		+= -Icontrib/sw-cc312/inc -Icontrib/sw-cc312-sbrom/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

# Enable basic fatal error reports (to prevent bloating code)
CFLAGS			+= -DUSE_BASIC_FATAL_ERROR_HANDLER

ASM_SRC +=

C_SRC			+= cc_hal.c cc_pal.c cc_pal_mutex.c cc_pal_pm.c
C_SRC			+= cc_pal_interrupt_ctrl.c cc_pal_mem.c cc_pal_buff_attr.c cc_pal_abort_plat.c cc_pal_trng.c
C_SRC			+=
ifeq ($(BUILD_TYPE), debug)
   CFLAGS += -DDEBUG
   C_SRC		+= cc_pal_log.c
endif
C_SRC			+= bsv_otp_api.c

CPP_SRC			+= cc312halpal_unit_test.cpp
CPP_SRC			+= systemmgr_mock.cpp otpmdrv_mock.cpp
CPP_SRC			+= cryptomgr_mock.cpp

VPATH			+= src/SystemMgr/test
VPATH			+= src/OtpmDrv/test
VPATH			+= src/LifecycleMgr/test
VPATH			+= src/SystemMgr/test
VPATH			+= src/CryptoMgr/test

VPATH			+= src/$(BUILD_COMPONENT)/src src/$(BUILD_COMPONENT)/test
VPATH			+= src/SystemDrv/src
VPATH			+= src/CC312HalPal/src


help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

