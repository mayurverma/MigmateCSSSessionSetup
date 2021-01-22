####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Makefile for the default Patch
#
####################################################################################

# Building the maintenance patch
BUILD_ARTEFACT := PATCH
BUILD_COMPONENT	:= Patch
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

BUILD_TYPE ?= release
PATCH_VERSION_BASE ?= 0x0000

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc 

INC_PATHS += -Iinc
INC_PATHS += -I$(BUILD_COMPONENT_ROOTDIR)/inc -I$(BUILD_COMPONENT_ROOTDIR)/src
INC_PATHS += -Isrc/PatchMgr/inc -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc
INC_PATHS += -Isrc/CommandHandler/inc -Isrc/CommandHandler/src

CFLAGS += $(INC_PATHS)

LDFLAGS	+= --no_remove					# Force all section to be included even if not used

C_SRC += PatchTable.c PatchLoader.c

VPATH += $(BUILD_COMPONENT_ROOTDIR)/src

# include the configuration makefile
include config.mk

##############################################################################################################
# List of build artifacts which will be added to the distribution archives
##############################################################################################################

# Distribution manifest for customer release
DIST_REL_INC := $(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_eval.chunks
DIST_REL_SRC :=

# Distribution manifest for developers (supplements customer release)
DIST_DEV_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_clear.chunks \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_test.chunks
DIST_DEV_SRC := \
	$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES) \
	$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) \
	$(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE)

