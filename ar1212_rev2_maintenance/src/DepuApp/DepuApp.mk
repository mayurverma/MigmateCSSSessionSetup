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
# Makefile for Depu application (added using patch mechanism)
#
####################################################################################

# Building the DEPU application
BUILD_ARTEFACT := APP
BUILD_COMPONENT	:= DepuApp
BUILD_COMPONENT_ROOTDIR := src/$(BUILD_COMPONENT)


BUILD_TYPE	?= release
ARM_LIB_BUILD_TYPE ?= release
PATCH_VERSION_BASE ?= 0x9000

FINGERPRINT_DB_FILE ?= VendorFingerprintDB

# Restrict the supported build configurations
SUPPORTED_BUILD_CONFIGS := barebones_armcc

INC_PATHS += -Iinc
INC_PATHS += -Isrc/Patch/inc -Isrc/Init/inc
INC_PATHS += -I$(BUILD_COMPONENT_ROOTDIR)/inc -I$(BUILD_COMPONENT_ROOTDIR)/src
INC_PATHS += -Isrc/DepuApp/inc -Isrc/DepuApp/src
INC_PATHS += -Isrc/PatchMgr/inc -Isrc/SystemMgr/inc -Isrc/SystemDrv/inc  -Isrc/CommandHandler/inc -Isrc/CommandHandler/src
INC_PATHS += -Isrc/Diag/inc -Isrc/LifecycleMgr/inc -Isrc/OtpmDrv/src -Isrc/OtpmDrv/inc -Isrc/AssetMgr/inc -Isrc/AssetMgr/src
INC_PATHS += -Isrc/CC312HalPal/inc -Isrc/CryptoMgr/inc -Isrc/NvmMgr/inc -Isrc/NvmMgr/src
INC_PATHS += -Isrc/OtpmDrv/inc
INC_PATHS += -Icontrib/target/2v0/inc -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312/inc
INC_PATHS += -Icontrib/sw-cc312-sbrom/inc
INC_PATHS += -Isrc/Toolbox/inc


CFLAGS += $(INC_PATHS)
CFLAGS += -DDEPU_PATCH_VERSION=$(BUILD_PATCH_VERSION)U

# CC312 header file cc_regs.h requires one of CC_REE, CC_TEE or CC_IOT must be defined for some macros.
# If not, the compiling would hit #error
# There is no difference which one is defined. It would be more accurate to define CC_NO_OS, but there is no such a define in ARM's code.
# CC_TEE has been used in ARM's delivery template. We kept it to build CC312 libs without any side effects. Hence we carry on using it.
CFLAGS += -DCC_TEE

ASFLAGS			+= -Isrc/Diag/inc -Isrc/SystemMgr/inc

ASM_SRC	+= InitHardFaultPatch.s
C_SRC += DepuAppPatchTable.c DepuAppPatchLoader.c DepuApp.c DepuAppInternal.c DepuAppOtpmProvisioning.c
C_SRC += ccProdOtpPatch.c
C_SRC += cc_pal_trngPatch.c

VPATH += $(BUILD_COMPONENT_ROOTDIR)/src
VPATH += src/DepuApp/src
VPATH += src/CC312HalPal/src
VPATH += src/Init/src src/Init/src/ARMCC
VPATH += src/CC312HalPal/src
VPATH += src/Toolbox/src


# Select the appropriate ARM libs to be linked in.
# SW-C312 CMPU and DMPU libraries
LDFLAGS	+= $(ADD_LIBPATH)contrib/sw-cc312/lib/$(ARM_LIB_BUILD_TYPE)
LDFLAGS	+= $(ADD_LIB)cmpu
LDFLAGS	+= $(ADD_LIB)dmpu

# include the configuration makefile
include config.mk

ROM_OUTPUT_DIR := src/Rom/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)

BUILD_KEY_SET_LIST = $(filter-out clear, $(PATCH_KEY_IDENTIFIERS))
DEPU_APP_CONTENTS_PREFIX = $(PRODUCT_PREFIX)_$(BUILD_COMPONENT)Contents_v$(subst 0x,,$(BUILD_PATCH_VERSION))
DEPU_APP_CONTENTS_FILENAMES = $(patsubst %, $(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_%.csv, $(BUILD_KEY_SET_LIST))
DEPU_APP_CONTENTS_PY_PREFIX = depu_app_content

$(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_%.csv: $(BUILD_COMPONENT_ROOTDIR)/scripts/generate_depu_contents.py
	@echo Creating content $@
	$(QUIET)$(PYTHON) $(BUILD_COMPONENT_ROOTDIR)/scripts/generate_depu_contents.py \
					--output_csv $@ \
					$(if $(findstring test, $@), \
						--output_py $(patsubst $(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_%.csv, $(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PY_PREFIX)_%.py, $@) \
					,) \
					-k $(patsubst $(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_%.csv, %, $@)

BUILD_SENSOR_CERT_VERSION ?= 0
BUILD_SENSOR_CERT_NUMBER  ?= 1
BUILD_VENDOR_CERT_VERSION ?= 0
BUILD_VENDOR_CERT_NUMBER  ?= 1

DEPU_APP_VENDOR_CERTIFICATES_FILENAMES = $(foreach keystorage, $(BUILD_KEY_SET_LIST), $(BUILD_COMPONENT_ROOTDIR)/src/depu_app_vendor_cert_$(keystorage).py)
DEPU_APP_SENSOR_CERTIFICATES_FILENAMES = $(foreach keystorage, $(BUILD_KEY_SET_LIST), $(BUILD_COMPONENT_ROOTDIR)/src/depu_app_sensor_cert_$(keystorage).py)

$(DEPU_APP_VENDOR_CERTIFICATES_FILENAMES): create_output_dir
	@echo Creating vendor certificates...
	$(QUIET)$(PYTHON) $(BUILD_COMPONENT_ROOTDIR)/scripts/generate_depu_certificates.py \
					--key_set $(patsubst $(BUILD_COMPONENT_ROOTDIR)/src/depu_app_vendor_cert_%.py, %, $@) \
					--fingerprint $(BUILD_COMPONENT_ROOTDIR)/src/$(FINGERPRINT_DB_FILE)_$(patsubst $(BUILD_COMPONENT_ROOTDIR)/src/depu_app_vendor_cert_%.py,%, $@).csv \
					--output $@ \
					--cert_version $(BUILD_VENDOR_CERT_VERSION) \
					--cert_number $(BUILD_VENDOR_CERT_NUMBER) \
					--purpose vendor

$(DEPU_APP_SENSOR_CERTIFICATES_FILENAMES): create_output_dir
	@echo Creating sensor certificates...
	$(QUIET)$(PYTHON) $(BUILD_COMPONENT_ROOTDIR)/scripts/generate_depu_certificates.py \
					--key_set $(patsubst $(BUILD_COMPONENT_ROOTDIR)/src/depu_app_sensor_cert_%.py,%, $@) \
					--output $@ \
					--cert_version $(BUILD_SENSOR_CERT_VERSION) \
					--cert_number $(BUILD_SENSOR_CERT_NUMBER) \
					--purpose sensor

generate_certificates: $(DEPU_APP_VENDOR_CERTIFICATES_FILENAMES) $(DEPU_APP_SENSOR_CERTIFICATES_FILENAMES)

generate_keys: create_output_dir
	@echo Creating keys in $(BUILD_OUTPUT_DIR)

	$(QUIET)$(PYTHON) $(BUILD_COMPONENT_ROOTDIR)/scripts/generate_depu_keys.py \
					--outputdir $(BUILD_OUTPUT_DIR)



##############################################################################################################
# List of generated artifacts which wil be added to distribution archive
##############################################################################################################

all: $(DEPU_APP_CONTENTS_FILENAMES) $(BUILD_OUTPUT_DIR)/$(FINGERPRINT_DB_FILE)_eval.csv

$(BUILD_OUTPUT_DIR)/$(FINGERPRINT_DB_FILE)_%.csv: $(BUILD_COMPONENT_ROOTDIR)/src/$(FINGERPRINT_DB_FILE)_%.csv
	@echo Copying files for Distribution...
	cp $^ $@

# Distribution manifest for customer release
DIST_REL_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_clear.chunks
DIST_REL_SRC := \
	$(BUILD_OUTPUT_DIR)/$(FINGERPRINT_DB_FILE)_eval.csv \
	$(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_eval.csv

# Distribution manifest for developers (supplements customer release)
DIST_DEV_INC := \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_eval.chunks \
	$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_test.chunks
DIST_DEV_SRC := \
	$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES) \
	$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) \
	$(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) \
	$(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE) \
	$(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PY_PREFIX)_test.py \
	$(BUILD_OUTPUT_DIR)/$(DEPU_APP_CONTENTS_PREFIX)_test.csv

