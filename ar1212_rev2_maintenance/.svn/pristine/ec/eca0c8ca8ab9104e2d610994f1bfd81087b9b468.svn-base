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
# Makefile for the system ROM application
#
####################################################################################

# Building the system ROM executable
BUILD_ARTEFACT := EXE
BUILD_COMPONENT	:= Rom
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)

# Override the product prefix so the ROM distribution has same name for any branch
PRODUCT_PREFIX := CSS

# If not define default to release build, also needs to be included before component makefiles incase these have any conditional includes
BUILD_TYPE ?= release

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc barebones_arm_gnu semihosted_armcc semihosted_arm_gnu

# List of components in the system ROM
BUILD_COMPONENT_LIST := AssetMgr CommandDrv CommandHandler CC312HalPal CryptoMgr DebugUartDrv \
	Diag HealthMonitor Interrupts LifecycleMgr MacDrv NvmMgr OtpmDrv PatchMgr Platform SessionMgr \
	SystemDrv SystemMgr SysTickDrv WatchdogDrv Toolbox

# Use the extended hard fault handler
BUILD_USE_EXT_HARDFAULT_HANDLER := yes

# Use the unexpected exception/interrupt handler
BUILD_USE_UNEXPECTED_HANDLER := yes

# include the component makefiles
BUILD_COMPONENT_MAKEFILES := $(foreach component,$(BUILD_COMPONENT_LIST), $(addprefix src/$(component)/, $(addsuffix .mk, $(component))))
include $(BUILD_COMPONENT_MAKEFILES)

# Select whether to use the debug or release builds of the ARM libraries (default to release)
ARM_LIB_BUILD_TYPE ?= release

INC_PATHS += -Iinc

CFLAGS += $(INC_PATHS)

CFLAGS += -DSYSTEM_MGR_ROM_VERSION=$(BUILD_ROM_VERSION)U

C_SRC += main.c

VPATH += src/Rom/src

CC312BSV_LIB := contrib/sw-cc312-sbrom/lib/$(ARM_LIB_BUILD_TYPE)/libcc_312bsv.a
CC312_LIB := contrib/sw-cc312/lib/$(ARM_LIB_BUILD_TYPE)/libcc_312.a
MBEDTLS_LIB := contrib/sw-cc312/lib/$(ARM_LIB_BUILD_TYPE)/libmbedtls.a
MBEDCRYPTO_LIB := contrib/sw-cc312/lib/$(ARM_LIB_BUILD_TYPE)/libmbedcrypto.a

ifeq ($(BUILD_CONFIG), barebones_armcc)
# Force all objects to be included even if not used
LDFLAGS	+= --no_remove
# AR0820FW-294: An alternative way to unpack CC312 libs and link all functions
# The following syntax is used to put all CC312 lib functions in ROM without unpacking *.a files
# Note that the "--no_remove" option doesn't affect the inside of the *.a files by default unless is forces with (*)
LDFLAGS	+= "$(CC312BSV_LIB)(*)"
LDFLAGS	+= "$(CC312_LIB)(*)"
LDFLAGS	+= "$(MBEDTLS_LIB)(*)"
LDFLAGS	+= "$(MBEDCRYPTO_LIB)(*)"
else
# Select the appropriate ARM libs to be linked in.
# SW-C312 libraries
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312-sbrom/lib/$(ARM_LIB_BUILD_TYPE)
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312/lib/$(ARM_LIB_BUILD_TYPE)
LDFLAGS	+= $(ADD_LIB)cc_312bsv
LDFLAGS	+= $(ADD_LIB)cc_312
LDFLAGS	+= $(ADD_LIB)mbedtls
LDFLAGS	+= $(ADD_LIB)mbedcrypto
endif

# include the configuration makefile
include config.mk

ifeq ($(filter $(BUILD_CONFIG), barebones_armcc barebones_arm_gnu), $(BUILD_CONFIG))
# Build the extra files needed for verification and release distribution
all: readmemh
endif

##############################################################################################################
# List of build artifacts which will be added to the distribution archives
##############################################################################################################

# Distribution manifest for customer release
DIST_REL_INC := $(BUILD_OUTPUT_DIR)/$(BUILD_ROM_BASENAME).rom.v.instances
DIST_REL_SRC :=

# Distribution manifest for developers (supplements customer release)
DIST_DEV_INC := $(BUILD_OUTPUT_DIR)/$(BUILD_VPM_BASENAME).vpm.instances
DIST_DEV_SRC := \
	$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES) \
	$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_SYMBOL_FILE)

