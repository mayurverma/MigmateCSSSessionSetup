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
# Build configuration control - uses BUILD_CONFIG to set the build control variables.
#
####################################################################################

# Include the branch specific configuration variables.
include branch.mk

####################################################################################
#
# The user of this makefile (component makefiles) MUST specify the following build
# control variables:
#
# BUILD_COMPONENT 	= component/module/library being built
# BUILD_ARTEFACT   	= indicates a library, executable, patch or application to be built
# BUILD_COMPONENT_ROOTDIR	= relative path to the artefact node
#

ifeq ($(BUILD_COMPONENT), )
$(error BUILD_COMPONENT not specfied - which component/module/library are you trying to build?)
endif

ifeq ($(BUILD_ARTEFACT), )
$(error BUILD_ARTEFACT not specfied - allowed values are LIB, EXE, PATCH or APP)
endif

ifeq ($(BUILD_COMPONENT_ROOTDIR), )
$(error BUILD_COMPONENT_ROOTDIR not specfied - need path to component node?)
endif

ifneq ($(BUILD_OUTPUT_DIR), )
$(error BUILD_OUTPUT_DIR should not be specfied)
endif

####################################################################################
#
# The BUILD_CONFIG variable configures a number of build control variables (not all are
# used for every build)
#
# Controls whether building for debugging or release
# BUILD_TYPE 					= debug | release
#
# Determines host for the tools (auto-detected)
# BUILD_HOST 					= windows | linux
#
# Determines the compiler/linker toolchain
# BUILD_TOOLCHAIN				= mingw32 | arm-none-eabi-gcc | armcc
#
# Determines the build toolchain version
# BUILD_TOOLCHAIN_VERSION		= 5.3.0 | 6.2.0 | 5.06.u5
#
# Determines the device being targetted:
# - pc 		= Windows (7) PC (RTL headers compatible)
# - asic 	= AR0820 silicon
# - proto 	= AR0820 prototype (virtual)
# BUILD_TARGET_DEVICE 			= pc | asic | proto
#
# Determines the version of the device being targetted:
# Represents the revision of the target device (pc and asic targets only)
# - rNNNN = specific RTL SVN revision
# - 1v0 	= rev 1.0 tag
# - 1v1 	= rev 1.1 tag
# BUILD_TARGET_DEVICE_VERSION	= rNNNN | 1v0 | 1v1 | 2v0 | ... | na
#
# Determines the prototype device (if BUILD_TARGET_DEVICE is 'proto'):
# Options include:
# - mps2_fvp = DS-5 fixed-virtual-platform
# - css_vp = CSS Virtual Prototype
# BUILD_TARGET_PROTO_DEVICE 	= mps2_fvp | css_vp | na
#
# Determines the version of the prototype (if BUILD_TARGET_DEVICE is 'proto'):
# Options include:
# - which RTL SVN revision
# - which FPGA build tag (and therefore what hardware it supports)
# - which VP build (and therefore what hardware it supports)
# BUILD_TARGET_PROTO_DEVICE_VERSION = rNNNN | 5v27r1 | 1v0 | 1v0 | ??? | na
#
# Determines the architecture of the target processor
# BUILD_TARGET_ARCH 			= x86 | cortex_m0plus
#
# Determines the software platform
# - barebones = C runtime support with standard libs, basic STDIO via UART
# - semihosted = C/C++ runtime with standard libs
# - cpputest  = C/C++ runtime with standard libs, CppUTest runner
# BUILD_TARGET_PLATFORM 		= barebones | semihosted | cpputest
#
# Determines the version of the software platform
# BUILD_TARGET_PLATFORM_VERSION = 1v0 | 1v0 | 109b436
#
####################################################################################

# Default to CppUTest for MinGW32 for now...
BUILD_CONFIG	?= cpputest_mingw32

# Default build artefacts i.e. goals and build properties file contents
BUILD_PROPERTIES_LIST += BUILD_TAG BUILD_SVN_REVISION
ifeq ($(BUILD_ARTEFACT), LIB)
BUILD_PROPERTIES_LIST += BUILD_LIB_VERSION
ALL_GOALS ?= lib properties
endif
ifeq ($(BUILD_ARTEFACT), EXE)
BUILD_PROPERTIES_LIST += BUILD_ROM_VERSION
ALL_GOALS ?= exe properties
endif
ifeq ($(BUILD_ARTEFACT), PATCH)
BUILD_PROPERTIES_LIST += BUILD_PATCH_VERSION
ALL_GOALS ?= exe properties chunks
endif
ifeq ($(BUILD_ARTEFACT), APP)
BUILD_PROPERTIES_LIST += BUILD_PATCH_VERSION
ALL_GOALS ?= exe properties chunks
endif

# Default Patch entry function for the Patch Loader (can be overriden by Patch makefile)
BUILD_PATCH_ENTRY ?= $(BUILD_COMPONENT)Loader

ifeq ($(BUILD_CONFIG), default)
override BUILD_CONFIG	:= cpputest_mingw32
$(info !!! Selected default configuration '$(BUILD_CONFIG)')
endif

ifeq ($(BUILD_CONFIG), cpputest_default)
override BUILD_CONFIG	:= cpputest_mingw32
$(info !!! Selected default configuration '$(BUILD_CONFIG)')
endif

#
# Guards
#
AVAILABLE_BUILD_CONFIGS	:= cpputest_mingw32 cpputest_armcc cpputest_arm_gnu
AVAILABLE_BUILD_CONFIGS	+= semihosted_mingw32 semihosted_armcc semihosted_arm_gnu
AVAILABLE_BUILD_CONFIGS	+= barebones_armcc barebones_arm_gnu

SUPPORTED_BUILD_CONFIGS ?= $(AVAILABLE_BUILD_CONFIGS)

ifeq ($(findstring $(BUILD_CONFIG), $(AVAILABLE_BUILD_CONFIGS)), )
$(error BUILD_CONFIG '$(BUILD_CONFIG)' is not supported. Available configs are $(AVAILABLE_BUILD_CONFIGS))
endif

ifeq ($(findstring $(BUILD_CONFIG), $(SUPPORTED_BUILD_CONFIGS)), )
$(error BUILD_CONFIG '$(BUILD_CONFIG)' is not supported for this component, only '$(SUPPORTED_BUILD_CONFIGS)')
endif

#
# Host support
#
ifeq ($(OS), Windows_NT)
BUILD_HOST		?= windows
else
BUILD_HOST		?= linux
endif


ifeq ($(BUILD_CONFIG), cpputest_mingw32)
####################################################################################
#
# Setup configuration for building for CppUTest under MinGW32
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= mingw32
BUILD_TOOLCHAIN_VERSION 			?= 5.3.0
BUILD_TARGET_DEVICE 				?= pc
BUILD_TARGET_DEVICE_VERSION			?= 2v0
BUILD_TARGET_PROTO_DEVICE			?= na
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= na
BUILD_TARGET_ARCH 					?= x86
BUILD_TARGET_PLATFORM 				?= cpputest
BUILD_TARGET_PLATFORM_VERSION 		?= 109b436

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= gcc-mingw

else ifeq ($(BUILD_CONFIG), cpputest_armcc)
####################################################################################
#
# Setup configuration for building for CppUTest under ARMCC, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= armcc
BUILD_TOOLCHAIN_VERSION 			?= 5.06.u5
BUILD_TARGET_DEVICE 				?= proto
BUILD_TARGET_DEVICE_VERSION			?= na
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= cpputest
BUILD_TARGET_PLATFORM_VERSION 		?= 109b436

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= armcc

else ifeq ($(BUILD_CONFIG), cpputest_arm_gnu)
####################################################################################
#
# Setup configuration for building for CppUTest under ARM GNU, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= arm-none-eabi-gcc
BUILD_TOOLCHAIN_VERSION 			?= 6.2.0
BUILD_TARGET_DEVICE 				?= proto
BUILD_TARGET_DEVICE_VERSION			?= na
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= cpputest
BUILD_TARGET_PLATFORM_VERSION 		?= 109b436

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= arm-gnu

else ifeq ($(BUILD_CONFIG), semihosted_mingw32)
####################################################################################
#
# Setup configuration for building semihosted C/C++ code under MinGW32
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= mingw32
BUILD_TOOLCHAIN_VERSION 			?= 5.3.0
BUILD_TARGET_DEVICE 				?= pc
BUILD_TARGET_DEVICE_VERSION			?= 2v0
BUILD_TARGET_PROTO_DEVICE			?= na
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= na
BUILD_TARGET_ARCH 					?= x86
BUILD_TARGET_PLATFORM 				?= semihosted
BUILD_TARGET_PLATFORM_VERSION 		?= 109b436

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= n/a

else ifeq ($(BUILD_CONFIG), semihosted_armcc)
####################################################################################
#
# Setup configuration for building semihosted C/C++ code under ARMCC, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= armcc
BUILD_TOOLCHAIN_VERSION 			?= 5.06.u5
BUILD_TARGET_DEVICE 				?= proto
BUILD_TARGET_DEVICE_VERSION			?= na
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= semihosted
BUILD_TARGET_PLATFORM_VERSION 		?= 1v0
# Patch related configuration
BUILD_ROM_SYMBOL_FILE               ?= src/Rom/output/css_vp.armcc/Rom.symbols

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= na

else ifeq ($(BUILD_CONFIG), semihosted_arm_gnu)
####################################################################################
#
# Setup configuration for building semihosted C/C++ code under ARM GNU, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= arm-none-eabi-gcc
BUILD_TOOLCHAIN_VERSION 			?= 6.2.0
BUILD_TARGET_DEVICE 				?= proto
BUILD_TARGET_DEVICE_VERSION			?= na
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= semihosted
BUILD_TARGET_PLATFORM_VERSION 		?= 1v0

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= na

else ifeq ($(BUILD_CONFIG), barebones_armcc)
####################################################################################
#
# Setup configuration for building barebones under ARMCC, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= armcc
BUILD_TOOLCHAIN_VERSION 			?= 5.06.u5
BUILD_TARGET_DEVICE 				?= asic
BUILD_TARGET_DEVICE_VERSION			?= 2v0
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= barebones
BUILD_TARGET_PLATFORM_VERSION 		?= 1v0
# Patch related configuration
BUILD_ROM_SYMBOL_FILE               ?= src/Rom/output/asic.armcc/Rom.symbols

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= na

else ifeq ($(BUILD_CONFIG), barebones_arm_gnu)
####################################################################################
#
# Setup configuration for building barebones under ARM GNU, targetting the CSS virtual-platform
#
BUILD_TYPE 							?= debug
BUILD_TOOLCHAIN						?= arm-none-eabi-gcc
BUILD_TOOLCHAIN_VERSION 			?= 6.2.0
BUILD_TARGET_DEVICE 				?= asic
BUILD_TARGET_DEVICE_VERSION			?= 2v0
BUILD_TARGET_PROTO_DEVICE			?= css_vp
BUILD_TARGET_PROTO_DEVICE_VERSION 	?= 2v0
BUILD_TARGET_ARCH 					?= cortex_m0plus
BUILD_TARGET_PLATFORM 				?= barebones
BUILD_TARGET_PLATFORM_VERSION 		?= 1v0

# Select the appropriate CppUTest libs
CPPUTEST_LIBS						?= na

else
####################################################################################
#
# Unsupported configuration
#
$(error BUILD_CONFIG '$(BUILD_CONFIG)' is not supported. Available configs are $(AVAILABLE_BUILD_CONFIGS))
endif

####################################################################################
# Extend output directory path depending on configuration
#
ifeq ($(BUILD_TARGET_DEVICE), proto)
BUILD_OUTPUT_DIR := $(BUILD_COMPONENT_ROOTDIR)/output/$(BUILD_TARGET_PROTO_DEVICE).$(BUILD_TOOLCHAIN)
else
BUILD_OUTPUT_DIR := $(BUILD_COMPONENT_ROOTDIR)/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)
endif
# Remove leading dot
BUILD_OUTPUT_DIR := $(subst ./,,$(BUILD_OUTPUT_DIR))

ifneq ($(BUILD_COMPONENT_VARIANT), )
# Add variant to the build directory name
BUILD_OUTPUT_DIR := $(addsuffix .$(BUILD_COMPONENT_VARIANT), $(BUILD_OUTPUT_DIR))
endif

# Build properties file for Python import
BUILD_PROPERTIES := build.properties
ROM_BUILD_PROPERTIES := src/Rom/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)/$(BUILD_PROPERTIES)

# Trap the deprecated case where component makefiles were setting the default
ifeq ($(origin BUILD_ROM_VERSION),file)
$(error BUILD_ROM_VERSION already defined by a component makefile)
endif

# When building the maintenance patch or applications that link against the ROM,
# the BUILD_ROM_VERSION should derive from the ROM build properties file. If the
# symbol is defined by the user user environment or on the command line then the
# build system needs to generate an error to tell the user it is deprecated.
#
# When building the ROM component, BUILD_ROM_VERSION is an input to the build
# system specified by the user environment or on the command line argument. The
# input format may be decimal or hexadecimal so the build system needs to force
# the input to hexadecimal.
#
ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))
ifneq (,$(filter $(origin BUILD_ROM_VERSION), environment command line))
$(error BUILD_ROM_VERSION should not be defined when BUILD_ARTEFACT is a PATCH or APP)
else
BUILD_ROM_VERSION := $(shell grep BUILD_ROM_VERSION $(ROM_BUILD_PROPERTIES) | cut -d= -f2)
endif
else # BUILD_ARTEFACT != PATCH or APP
ifneq (,$(filter $(origin BUILD_ROM_VERSION), environment command line))
# User can input decimal/hexadecimal so convert to hex string
override BUILD_ROM_VERSION :=  $(shell printf '0x%04X\n' $(BUILD_ROM_VERSION))
else
BUILD_ROM_VERSION := 0x0000
endif
endif

# Default Maintenance Release Candidate Number (set by Jenkins build)
BUILD_PATCH_MRCN ?= 1

# Default patch/application base version number (set by component makefile)
PATCH_VERSION_BASE ?= 0x0000

# Determine patch/application version as hex string
BUILD_PATCH_VERSION := $(shell printf '0x%04X\n' "$$(( $(PATCH_VERSION_BASE) + $(BUILD_PATCH_MRCN) ))")

# Distribution basename for build arifact filenames
ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))
DIST_BASENAME := $(PRODUCT_PREFIX)_$(BUILD_COMPONENT)_v$(subst 0x,,$(BUILD_PATCH_VERSION))
else
DIST_BASENAME := $(PRODUCT_PREFIX)_$(BUILD_COMPONENT)_v$(subst 0x,,$(BUILD_ROM_VERSION))
endif

# Allow DV tests to override the basename of readmemh output files
BUILD_ROM_BASENAME ?= $(DIST_BASENAME)
BUILD_VPM_BASENAME ?= $(BUILD_COMPONENT)

# DevWare file names
DEVWARE_PATCH_INI_FILE ?= $(DEVWARE_PREFIX)_PATCH.ini

# Export some of the build configuration to C/C++
CFLAGS 		+= -DBUILD_TYPE_$(BUILD_TYPE) -DBUILD_TARGET_ARCH_$(BUILD_TARGET_ARCH)
CPPFLAGS 	+= -DBUILD_TYPE_$(BUILD_TYPE) -DBUILD_TARGET_ARCH_$(BUILD_TARGET_ARCH)

.PHONY: help
help::
	$(QUIET) echo "    all                Build all goals e.g. libraries and executables"
	$(QUIET) echo "    clean              Delete all compiler and linker artefacts"
	$(QUIET) echo "    bare               Delete all artefacts"
	$(QUIET) echo "    debug              Display make variables for diagnostics"

# Include the target device makefile
include target_device.mk

# Include the toolchain makefile
include toolchain.mk

# Include Doxygen support
include doxygen.mk

# Include the makefile common to all builds
include common.mk

