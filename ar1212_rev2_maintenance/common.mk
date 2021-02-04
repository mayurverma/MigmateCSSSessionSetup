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
# Common makefile for all builds
#
####################################################################################

# Suppress tool output (use 'QUIET=' on the make command line to override)
QUIET ?= @

# Make method for 'newline' creation in commands
BLANK :=
define NEWLINE

$(BLANK)
endef

.PHONY: all
.DEFAULT_GOAL: all
all: $(ALL_GOALS)

.PHONY: clean
clean::
	$(QUIET)$(RMDIR) $(BUILD_OUTPUT_DIR)

.PHONY: bare
bare::
	$(QUIET)$(RMDIR) $(BUILD_COMPONENT_ROOTDIR)/output

.PHONY: debug
debug::
	@echo BUILD_CONFIG            = $(BUILD_CONFIG)
	@echo BUILD_OUTPUT_DIR        = $(BUILD_OUTPUT_DIR)
	@echo BUILD_COMPONENT         = $(BUILD_COMPONENT)
	@echo BUILD_COMPONENT_VARIANT = $(BUILD_COMPONENT_VARIANT)
	@echo BUILD_TARGET            = $(BUILD_TARGET)
	@echo BUILD_ARTEFACT          = $(BUILD_ARTEFACT)
	@echo --
	@echo BUILD_HOST              = $(BUILD_HOST)
	@echo BUILD_TYPE              = $(BUILD_TYPE)
	@echo BUILD_TOOLCHAIN         = $(BUILD_TOOLCHAIN)
	@echo BUILD_TOOLCHAIN_VERSION = $(BUILD_TOOLCHAIN_VERSION)
	@echo --
	@echo BUILD_TARGET_DEVICE               = $(BUILD_TARGET_DEVICE)
	@echo BUILD_TARGET_DEVICE_VERSION       = $(BUILD_TARGET_DEVICE_VERSION)
	@echo BUILD_TARGET_PROTO_DEVICE         = $(BUILD_TARGET_PROTO_DEVICE)
	@echo BUILD_TARGET_PROTO_DEVICE_VERSION = $(BUILD_TARGET_PROTO_DEVICE_VERSION)
	@echo BUILD_TARGET_ARCH                 = $(BUILD_TARGET_ARCH)
	@echo BUILD_TARGET_PLATFORM             = $(BUILD_TARGET_PLATFORM)
	@echo BUILD_TARGET_PLATFORM_VERSION     = $(BUILD_TARGET_PLATFORM_VERSION)
	@echo --
	@echo CPPUTEST_LIBS = $(CPPUTEST_LIBS)
	@echo --
	@echo ASM_OBJS = $(ASM_OBJS)
	@echo C_OBJS   = $(C_OBJS)
	@echo CPP_OBJS = $(CPP_OBJS)
	@echo ALL_OBJS = $(ALL_OBJS)
	@echo ASFLAGS  = $(ASFLAGS)
	@echo CFLAGS   = $(CFLAGS)
	@echo CPPFLAGS = $(CPPFLAGS)
	@echo LDFLAGS  = $(LDFLAGS)
	@echo VPATH    = $(VPATH)
	@echo --

.PHONY: help
help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    properties         Generate build properties file"
	$(QUIET) echo "    presets            Generate Devware presets files"

.PHONY: properties
properties: $(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES)

# Build rule to generate the build properties file
$(BUILD_OUTPUT_DIR)/$(BUILD_PROPERTIES):
	@echo Writing build properties $@
	@echo \[DEFAULT\] > $@
	$(foreach property, $(BUILD_PROPERTIES_LIST), @echo $(property)=$($(property)) >> $@ $(NEWLINE))

# Build rule for Devware presets, ini files, supporting Python, etc.
.PHONY: presets
ifeq ($(BUILD_ARTEFACT), PATCH)
presets: $(BUILD_OUTPUT_DIR)/$(DEVWARE_PATCH_INI_FILE)
$(BUILD_OUTPUT_DIR)/$(DEVWARE_PATCH_INI_FILE): $(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_eval.chunks
	@echo Creating DevWare CSS patch INI
	$(QUIET) $(PYTHON) scripts/create_devware_css_patch.py --input $< \
		--version $(BUILD_PATCH_VERSION) \
		--output $@
else
presets:
	$(error "presets target is only supported for patch artefacts")
endif

###################################################################################################
# Patterns for distribution creation and use
###################################################################################################

# Distribution file names for developers (supplements customer release)
DIST_DEV_BASENAME := $(DIST_BASENAME)_dev
DIST_DEV_MANIFEST := $(DIST_DEV_BASENAME).manifest
DIST_DEV_MANIFEST_MD5 := $(DIST_DEV_MANIFEST).md5
DIST_DEV_ARCHIVE := $(DIST_DEV_BASENAME).zip
DIST_DEV_ARCHIVE_MD5 := $(DIST_DEV_ARCHIVE).md5

# Distribution file names for customer release
DIST_REL_BASENAME := $(DIST_BASENAME)_rel
DIST_REL_MANIFEST := $(DIST_REL_BASENAME).manifest
DIST_REL_MANIFEST_MD5 := $(DIST_REL_MANIFEST).md5
DIST_REL_ARCHIVE := $(DIST_REL_BASENAME).zip
DIST_REL_ARCHIVE_MD5 := $(DIST_REL_ARCHIVE).md5

DIST_PATCH_INC := $(patsubst %,$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_%.chunks,$(PATCH_KEY_IDENTIFIERS))

# Build the manifest for the developer archive
$(BUILD_OUTPUT_DIR)/$(DIST_DEV_MANIFEST): $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Creating manifest $@
	$(QUIET)echo $(BUILD_OUTPUT_DIR)/$(DIST_DEV_MANIFEST) > $@
	$(QUIET)echo $(BUILD_OUTPUT_DIR)/$(DIST_DEV_MANIFEST_MD5) >> $@
	$(QUIET)$(foreach file, $(DIST_DEV_SRC), @echo "$(file)" >> $@ $(NEWLINE))
ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))
	$(foreach file, $(DIST_DEV_INC), \
		$(QUIET)if [[ "$(DIST_PATCH_INC)" =~ "$(file)" ]]; then \
		$(CAT) $(file) >> $@; fi \
		$(NEWLINE))
else
	$(foreach file, $(DIST_DEV_INC), $(QUIET)$(CAT) $(file) >> $@ $(NEWLINE))
endif

# Build the manifest for the customer release archive
$(BUILD_OUTPUT_DIR)/$(DIST_REL_MANIFEST): $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Creating manifest $@
	$(QUIET)echo $(BUILD_OUTPUT_DIR)/$(DIST_REL_MANIFEST) > $@
	$(QUIET)echo $(BUILD_OUTPUT_DIR)/$(DIST_REL_MANIFEST_MD5) >> $@
	$(QUIET)$(foreach file, $(DIST_REL_SRC), @echo "$(file)" >> $@ $(NEWLINE))
ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))
	$(foreach file, $(DIST_REL_INC), \
		$(QUIET)if [[ "$(DIST_PATCH_INC)" =~ "$(file)" ]]; then \
		$(CAT) $(file) >> $@; fi \
		$(NEWLINE))
else
	$(foreach file, $(DIST_REL_INC), $(QUIET)$(CAT) $(file) >> $@ $(NEWLINE))
endif

# Calculate the checksums for each file on the manifest
$(BUILD_OUTPUT_DIR)/%.manifest.md5: $(BUILD_OUTPUT_DIR)/%.manifest
	@echo Checksum manifest $@
	$(QUIET)cd $(BUILD_OUTPUT_DIR) && $(MD5SUM) $(filter-out %.md5, $(notdir $(shell $(CAT) $<))) > $(notdir $@)

# Build the zip archive
$(BUILD_OUTPUT_DIR)/%.zip: $(BUILD_OUTPUT_DIR)/%.manifest $(BUILD_OUTPUT_DIR)/%.manifest.md5
	@echo Creating archive $@
ifeq ($(BUILD_HOST),windows)
	$(QUIET)$(RM) $@
	$(QUIET)$(ZIP) $@ @$<
else
	$(QUIET)$(CAT) $< | $(ZIP) $@
endif

# Calculate the checksum for the zip archive
$(BUILD_OUTPUT_DIR)/%.zip.md5: $(BUILD_OUTPUT_DIR)/%.zip
	@echo Checksum archive $@
	$(QUIET)cd $(BUILD_OUTPUT_DIR) && $(MD5SUM) $(notdir $<) > $(notdir $@)
ifeq ($(BUILD_HOST),windows)
	$(QUIET)$(DOS2UNIX) -q $@
endif

DIST_DEV_OBJS := $(BUILD_OUTPUT_DIR)/$(DIST_DEV_MANIFEST_MD5)
DIST_DEV_OBJS += $(BUILD_OUTPUT_DIR)/$(DIST_DEV_ARCHIVE)
DIST_DEV_OBJS += $(BUILD_OUTPUT_DIR)/$(DIST_DEV_ARCHIVE_MD5)

DIST_REL_OBJS := $(BUILD_OUTPUT_DIR)/$(DIST_REL_MANIFEST_MD5)
DIST_REL_OBJS += $(BUILD_OUTPUT_DIR)/$(DIST_REL_ARCHIVE)
DIST_REL_OBJS += $(BUILD_OUTPUT_DIR)/$(DIST_REL_ARCHIVE_MD5)

# Build rule to generate the distribution archive
.PHONY: dist
ifeq ($(BUILD_CONFIG), barebones_armcc)
dist: all $(DIST_DEV_OBJS) $(DIST_REL_OBJS)
else
dist:
	$(error Build target 'dist' is not supported in this configuration.)
endif

define do-dist-fetch
	@echo Fetching distribution $(file)
	$(QUIET)$(CP) $(file) $(BUILD_OUTPUT_DIR)
	$(QUIET)$(CP) $(file).md5 $(BUILD_OUTPUT_DIR)

endef

define do-dist-extract
	@echo Verifying archive $(file)
	$(QUIET)cd $(BUILD_OUTPUT_DIR) && $(MD5SUM) -c $(notdir $(file)).md5
	@echo Unpacking archive ...
	$(QUIET)$(UNZIP) $(file)
	$(QUIET)cd $(BUILD_OUTPUT_DIR) && $(MD5SUM) -c $(notdir $(patsubst %.zip,%.manifest.md5,$(file)))

endef

# Build rule to unpack the distribution archive
.PHONY: dist-unpack dist_unpack dist_extract
dist-unpack: dist_unpack
ifeq ($(BUILD_CONFIG), barebones_armcc)
dist_unpack: create_output_dir
	$(eval DIST_UNPACK_LIST := $(wildcard contrib/$(BUILD_COMPONENT)/$(PRODUCT_PREFIX)_$(BUILD_COMPONENT)_v*.zip))
	$(QUIET)$(foreach file, $(DIST_UNPACK_LIST), $(do-dist-fetch))
	$(QUIET)$(foreach file, $(DIST_UNPACK_LIST), $(do-dist-extract))
dist_extract:
	$(eval DIST_EXTRACT_LIST := $(wildcard $(BUILD_OUTPUT_DIR)/$(PRODUCT_PREFIX)_$(BUILD_COMPONENT)_v*.zip))
	$(QUIET)$(foreach file, $(DIST_EXTRACT_LIST), $(do-dist-extract))
else
dist_unpack dist_extract:
	@echo Build target '$@' is not supported in this configuration.
	exit 1
endif

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    dist               Create the distribution archives"
	$(QUIET) echo "    dist_unpack        Unpack (fetch, verify and extract) the distribution archives (fetch from the contrib directory)"
	$(QUIET) echo "    dist_extract       Extract (verify and extract) the distribution archives (in the build output directory)"

