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
# Makefile for Verification application
#
####################################################################################

# Building the system ROM executable
BUILD_ARTEFACT := APP
BUILD_COMPONENT	:= VerificationApp
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

PATCH_VERSION_BASE ?= 0x0000

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc semihosted_armcc

INC_PATHS += -Iinc
INC_PATHS += -I$(BUILD_COMPONENT_ROOTDIR)/inc -I$(BUILD_COMPONENT_ROOTDIR)/src
INC_PATHS += -Isrc/Patch/inc
INC_PATHS += -Isrc/SystemMgr/inc -Isrc/SystemMgr/src
INC_PATHS += -Isrc/PatchMgr/inc -Isrc/SystemDrv/inc  -Isrc/CommandHandler/inc
INC_PATHS += -Isrc/CommandHandler/src -Isrc/NvmMgr/inc -Isrc/AssetMgr/inc
INC_PATHS += -Isrc/Init/inc -Isrc/Init/src
INC_PATHS += -Isrc/Diag/inc
INC_PATHS += -Icontrib/target/2v0/inc
INC_PATHS += -Isrc/LifecycleMgr/inc -Isrc/LifecycleMgr/src
INC_PATHS += -Icontrib/sw-cc312-sbrom/inc -Icontrib/sw-cc312/inc
INC_PATHS += -Isrc/Interrupts/inc
INC_PATHS += -Isrc/HealthMonitor/inc -Isrc/HealthMonitor/src
INC_PATHS += -Isrc/MacDrv/inc
INC_PATHS += -Isrc/WatchdogDrv/inc
INC_PATHS += -Isrc/Toolbox/inc
INC_PATHS += -Isrc/OtpmDrv/inc

CFLAGS += $(INC_PATHS)

LDFLAGS	+= --no_remove					# Force all section to be included even if not used

C_SRC += VerificationAppPatchTable.c VerificationAppPatchLoader.c VerificationApp.c

VPATH += $(BUILD_COMPONENT_ROOTDIR)/src

# include the configuration makefile
include config.mk

##############################################################################################################
# List of build artifacts which will be added to the distribution archives
##############################################################################################################

# Distribution manifest for customer release
DIST_REL_INC :=
DIST_REL_SRC :=

# Distribution manifest for developers (supplements customer release)
DIST_DEV_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_clear.chunks \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_test.chunks \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_eval.chunks
DIST_DEV_SRC := \
	$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES) \
	$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) \
	$(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE)

