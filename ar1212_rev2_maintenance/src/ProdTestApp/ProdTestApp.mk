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
# Makefile for Production Test application
#
####################################################################################

# Building the Production Test application
BUILD_ARTEFACT := APP
BUILD_COMPONENT	:= ProdTestApp
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

BUILD_TYPE	?= release
ARM_LIB_BUILD_TYPE ?= release
PATCH_VERSION_BASE ?= 0xF000

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc semihosted_armcc

INC_PATHS += -Iinc
INC_PATHS += -Isrc/Patch/inc -Isrc/Init/inc
INC_PATHS += -I$(BUILD_COMPONENT_ROOTDIR)/inc -I$(BUILD_COMPONENT_ROOTDIR)/src
INC_PATHS += -Isrc/PatchMgr/inc -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc  -Isrc/CommandHandler/inc -Isrc/CommandHandler/src
INC_PATHS += -Isrc/Diag/inc -Isrc/OtpmDrv/src -Isrc/OtpmDrv/inc -Isrc/AssetMgr/inc -Isrc/NvmMgr/inc -Isrc/LifecycleMgr/inc
INC_PATHS += -Isrc/CC312HalPal/inc
INC_PATHS += -Icontrib/target/2v0/inc -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312-sbrom/inc
INC_PATHS += -Isrc/Toolbox/inc

CFLAGS += $(INC_PATHS)
CFLAGS += -DPRODTESTAPP_PATCH_VERSION=$(BUILD_PATCH_VERSION)U
CFLAGS += -DCC_TEE
ASFLAGS	+= -Isrc/Diag/inc -Isrc/SystemMgr/inc

LDFLAGS	+= --no_remove					# Force all section to be included even if not used

ASM_SRC	+= InitHardFaultPatch.s
C_SRC += ProdTestAppPatchLoader.c ProdTestApp.c ProdTestAppInternal.c ProdTestAppPatchTable.c ProdTestAppGetMaxErrorArrayLengthLongWords.c
C_SRC += cc_pal_trngPatch.c
VPATH += $(BUILD_COMPONENT_ROOTDIR)/src
VPATH += src/Init/src src/Init/src/ARMCC src/NvmMgr/src src/Patch/src src/CommandHandler/src
VPATH += src/Toolbox/src
VPATH += src/PatchMgr/src
VPATH += src/CC312HalPal/src

# include the configuration makefile
include config.mk

##############################################################################################################
# List of generated artifacts which wil be added to distribution archive
##############################################################################################################

# Distribution manifest for customer release
DIST_REL_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_clear.chunks \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_eval.chunks
DIST_REL_SRC :=

# Distribution manifest for developers (supplements customer release)
DIST_DEV_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_test.chunks
DIST_DEV_SRC := \
	$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES) \
	$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) \
	$(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE)

