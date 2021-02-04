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
# Makefile for the Python based Firmware use case test framework
#
####################################################################################
BUILD_COMPONENT := pyfwtest
BUILD_ARTEFACT := EXE
BUILD_COMPONENT_ROOTDIR := test/$(BUILD_COMPONENT)

SUPPORTED_BUILD_CONFIGS := barebones_armcc semihosted_armcc

ifeq ($(OS), Windows_NT)
$(error Windows is not supported.)
endif

QUIET	:= @
CP		:= cp
MKDIR	:= mkdir -p
PYTHON	:= python
RM		:= rm -f
RMDIR	:= rm -rf
SHELL	:= /bin/sh
SIMLINK := ln -fs

ROOT_DIR := $(shell pwd)

ifeq ($(BUILD_CONFIG), semihosted_armcc)
	BUILD_TOOLCHAIN		?= armcc
	BUILD_TARGET_DEVICE ?= proto
else ifeq ($(BUILD_CONFIG), barebones_armcc)
	BUILD_TOOLCHAIN		?= armcc
	BUILD_TARGET_DEVICE ?= asic
else
$(error BUILD_CONFIG '$(BUILD_CONFIG)' is not supported. Available configs are $(SUPPORTED_BUILD_CONFIGS))
endif

BUILD_OUTPUT_DIR := $(BUILD_COMPONENT_ROOTDIR)/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)
ROM_OUTPUT_DIR := src/Rom/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)

$(ROM_OUTPUT_DIR)/Rom.axf:
	$(QUIET)$(MKDIR) $(BUILD_OUTPUT_DIR)
	$(QUIET)$(MAKE) -r --no-print-directory -f src/Rom/Rom.mk all readmemh

.PHONY: clean all readmemh

all: $(ROM_OUTPUT_DIR)/Rom.axf readmemh

$(ROM_OUTPUT_DIR)/Rom.axf:
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/Rom.axf $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).axf
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/Rom.axf.map $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).axf.map
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/Rom.symbols $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).symbols

readmemh:
	$(eval THIS_ROM_VERSION := $(shell grep BUILD_ROM_VERSION $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/build.properties | cut -dx -f2))
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/CSS_Rom_v$(THIS_ROM_VERSION)_16384x39_0.rom.v $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT)_ROM_16384x39_0.rom.v
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/CSS_Rom_v$(THIS_ROM_VERSION)_16384x39_1.rom.v $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT)_ROM_16384x39_1.rom.v
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/CSS_Rom_v$(THIS_ROM_VERSION)_16384x39_2.rom.v $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT)_ROM_16384x39_2.rom.v
	$(QUIET)$(SIMLINK) $(ROOT_DIR)/$(ROM_OUTPUT_DIR)/CSS_Rom_v$(THIS_ROM_VERSION)_16384x39_3.rom.v $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT)_ROM_16384x39_3.rom.v

clean:
	@echo Cleaning...
	$(QUIET)$(MAKE) -r --no-print-directory -f src/Rom/Rom.mk clean
	-$(QUIET)$(RMDIR) $(BUILD_OUTPUT_DIR)

