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
# Common makefile for ARMCC builds
#
####################################################################################

#
# Guards
#
ifneq ($(BUILD_TOOLCHAIN), armcc)
$(error BUILD_TOOLCHAIN is invalid: '$(BUILD_TOOLCHAIN)')
endif

ifneq ($(BUILD_TOOLCHAIN_VERSION), 5.06.u5)
ifneq ($(BUILD_TOOLCHAIN_VERSION), 5.04.u5)
$(error BUILD_TOOLCHAIN_VERSION is unsupported: '$(BUILD_TOOLCHAIN_VERSION)')
endif
endif

#
# Tools
#

# Select the default ARMCC license servers
export ARMLMD_LICENSE_FILE	?= 28100@licus02;28100@bemc02ls187;28100@licg11

ifeq ($(BUILD_HOST),windows)
#
# Select the appropriate ARMCC version
# - assume if 5.06u5 is installed, we'll use it
#
COMPILER_5_06u5	= $(wildcard c:/Program\ Files/DS-5\ v5.27.1/sw/ARMCompiler5.06u5/bin/*.*)
COMPILER_5_04u5	= $(wildcard c:/SWTOOLS/arm-certcomp/5.04u5/bin/*.*)
ifneq ($(COMPILER_5_04u5), )
	ARMCC_HOME		?= c:/SWTOOLS/arm-certcomp/5.04u5
else ifneq ($(COMPILER_5_06u5), )
	ARMCC_HOME		?= c:/'Program Files'/'DS-5 v5.27.1'/sw/ARMCompiler5.06u5
else
	ARMCC_HOME		?= c:/'Program Files'/'DS-5 v5.26.2'/sw/ARMCompiler5.06u4
endif

else
	#
	# Assume ARMCC 5.06u5 for Linux
	#
	ARMCC_HOME		?= /tools/icad/arm/DS5/DS-5_v5.27.1/sw/ARMCompiler5.06u5

endif # ifeq ($(BUILD_HOST),windows)

AS				:= $(ARMCC_HOME)/bin/armasm
CC				:= $(ARMCC_HOME)/bin/armcc
AR				:= $(ARMCC_HOME)/bin/armar
LD				:= $(ARMCC_HOME)/bin/armlink
FROMELF			:= $(ARMCC_HOME)/bin/fromelf
ARM_DEBUGGER	:= $(ARMCC_HOME)/../../bin/debugger

ifeq ($(BUILD_TARGET_ARCH), )
# Default to M0+ support
CPU				:= Cortex-M0plus
else
ifeq ($(BUILD_TARGET_ARCH), cortex_m0plus)
CPU				:= Cortex-M0plus
else
$(error BUILD_TARGET_ARCH is unsupported: '$(BUILD_TARGET_ARCH)')
endif
endif

#
# Assembler flags
#
ASFLAGS			+= --cpu=$(CPU)
ifeq ($(BUILD_TYPE), debug)
ASFLAGS  		+= -g
endif

#
# C compiler flags
#
CFLAGS  		+= --cpu=$(CPU)
CFLAGS			+= --diag_error=warning
CFLAGS  		+= --c99
CFLAGS			+= -DBUILD_TOOLCHAIN_armcc
CFLAGS			+= --diag_suppress=191  	# suppresses the "type qualifier is meaningless on cast type" diagnostic warning

ifeq ($(BUILD_TYPE), debug)
CFLAGS  		+= -g -O1
endif
ifeq ($(BUILD_TYPE), release)
CFLAGS  		+= -O3 --no_autoinline
CFLAGS			+= -DNDEBUG					# disables assert macro
endif

#
# CPP compiler flags
#
CPPFLAGS		+= --cpu=$(CPU)
CPPFLAGS		+= -g -O1
CPPFLAGS		+= --diag_suppress 1300		# suppresses the "<entity> inherits implicit virtual" warning
CPPFLAGS		+= -DBUILD_TOOLCHAIN_armcc

#
# Linker flags
#
LDFLAGS 		+= --map --list=$(BUILD_COMPONENT).map
LDFLAGS			+= --info=totals,inputs,libraries,unused
#LDFLAGS			+= --info=unused
#LDFLAGS			+= --info=compression
#LDFLAGS			+= --info=exceptions
#LDFLAGS			+= --info=libraries
#LDFLAGS			+= --verbose
LDFLAGS			+= --feedback=$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).unused
ADD_LIBPATH		+= --userlibpath=
ADD_LIB			+= --library=

#
# Memory-map (scatter file) selection
#
ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), )
ifeq ($(BUILD_TARGET_DEVICE), proto)
BUILD_SCATTER_FILE ?= contrib/target/$(BUILD_TARGET_PROTO_DEVICE_VERSION)/linker/ARMCC/$(BUILD_TARGET_PROTO_DEVICE)_$(BUILD_TARGET_PROTO_DEVICE_VERSION).scat
else
BUILD_SCATTER_FILE ?= contrib/target/$(BUILD_TARGET_DEVICE_VERSION)/linker/ARMCC/$(BUILD_TARGET_DEVICE)_$(BUILD_TARGET_DEVICE_VERSION).scat
endif
LDFLAGS						+= --scatter=$(BUILD_SCATTER_FILE)
else
ifeq ($(BUILD_TARGET_DEVICE), proto)
BUILD_PATCH_SCATTER_FILE ?=  contrib/target/$(BUILD_TARGET_PROTO_DEVICE_VERSION)/linker/ARMCC/$(BUILD_TARGET_PROTO_DEVICE)_$(BUILD_TARGET_PROTO_DEVICE_VERSION)_patch.scat
else
BUILD_PATCH_SCATTER_FILE ?=  contrib/target/$(BUILD_TARGET_DEVICE_VERSION)/linker/ARMCC/$(BUILD_TARGET_DEVICE)_$(BUILD_TARGET_DEVICE_VERSION)_patch.scat
endif
LDFLAGS						+= --scatter=$(BUILD_PATCH_SCATTER_FILE)
endif

#
# exclude the Init component selection for patch apps and libraries
#
ifneq ($(filter $(BUILD_ARTEFACT), PATCH APP LIB), )
BUILD_ARMCC_EXCLUDE_INIT 	:= 1
endif

ifeq ($(BUILD_TARGET_PLATFORM), cpputest)
# *************************************************************************************
#
#  CppUTest support (full C and C++ library support, plus semi-hosting)
#

# Start-up support for full C and C++ libraries
ASM_SRC 		+= InitStartup.s InitSetupHeapStack.s InitHardFaultDebug.s
C_SRC			+= InitSystem.c InitMpuDisabled.c InitHardFaultHandlerBasic.c 

ASFLAGS			+= -Isrc/Diag/inc
CFLAGS			+= -DBUILD_TARGET_PLATFORM_cpputest
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= --entry=InitStart

VPATH			+= src/Init/src src/Init/src/ARMCC

ifneq ($(CPPUTEST_ACTIVE), )

ifeq ($(CPPUTEST_LIBS), )
$(error CppUTest is selected, but CPPUTEST_LIBS is not specified)
endif

# Additional C++ flags for CppUTest
CPPFLAGS		+= --exceptions -DCPPUTEST_MEM_LEAK_DETECTION_DISABLED -DCPPUTEST_STD_CPP_LIB_DISABLED

endif # ifneq ($(CPPUTEST_ACTIVE), )
endif # ifeq ($(BUILD_TARGET_PLATFORM), cpputest)

ifeq ($(BUILD_TARGET_PLATFORM), semihosted)
# *************************************************************************************
#
# Semihosted (full C/C++ libraries, semi-hosted)
#

CFLAGS			+= -DBUILD_TARGET_PLATFORM_semihosted

# Allow some DV tests to exclude the Init component when needed
ifndef BUILD_ARMCC_EXCLUDE_INIT

# Start-up support for full C and C++ libraries
ASM_SRC 		+= InitStartup.s InitSetupHeapStack.s
C_SRC			+= InitSystem.c

ifneq ($(BUILD_DISABLE_MPU), )
# Disable the MPU
C_SRC			+= InitMpuDisabled.c
else
# Enable the MPU
C_SRC			+= InitMpu.c
endif

ifneq ($(BUILD_USE_EXT_HARDFAULT_HANDLER), )
# Select the 'extended' HardFault handler
ASM_SRC 		+= InitHardFault.s
C_SRC			+= InitHardFaultHandlerExtended.c
C_SRC			+= InitPendSvHandler.c
C_SRC			+= InitHardFaultShutdown.c
ASFLAGS			+= -Isrc/SystemMgr/inc

else
# Select the 'basic' HardFault handler
ASM_SRC			+= InitHardFaultDebug.s
C_SRC			+= InitHardFaultHandlerBasic.c

endif

ASFLAGS			+= -Isrc/Diag/inc
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= --entry=InitStart

VPATH			+= src/Init/src src/Init/src/ARMCC

endif # ifndef BUILD_ARMCC_EXCLUDE_INIT

endif # ifeq ($(BUILD_TARGET_PLATFORM), semihosted)

ifeq ($(BUILD_TARGET_PLATFORM), barebones)
# *************************************************************************************
#
# Barebones-specific support
#

CFLAGS			+= -DBUILD_TARGET_PLATFORM_barebones

# Allow some DV tests to exclude the Init component when needed
ifndef BUILD_ARMCC_EXCLUDE_INIT

# Start-up for barebones (no C and C++ libraries)
ASM_SRC 		+= InitStartup.s InitSetupHeapStack.s
C_SRC			+= InitSystem.c InitSysRetarget.c
C_SRC			+= InitRamVectors.c

ifneq ($(BUILD_DISABLE_MPU), )
# Disable the MPU
C_SRC			+= InitMpuDisabled.c
else
# Enable the MPU
C_SRC			+= InitMpu.c
endif

ifneq ($(BUILD_USE_EXT_HARDFAULT_HANDLER), )
# Select the 'extended' HardFault handler
ASM_SRC 		+= InitHardFault.s
C_SRC			+= InitHardFaultHandlerExtended.c
C_SRC			+= InitPendSvHandler.c
C_SRC			+= InitHardFaultShutdown.c
ASFLAGS			+= -Isrc/SystemMgr/inc

else
# Select the 'basic' HardFault handler
ASM_SRC			+= InitHardFaultDebug.s
C_SRC			+= InitHardFaultHandlerBasic.c

endif

ifneq ($(BUILD_USE_UNEXPECTED_HANDLER), )
# Include the unexpected exception/interrupt handlers
C_SRC			+= InitUnexpectedHandler.c
endif

ASFLAGS			+= -Isrc/Diag/inc
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= --entry=InitStart

VPATH			+= src/Init/src src/Init/src/ARMCC

endif # ifndef BUILD_ARMCC_EXCLUDE_INIT

endif # ifeq ($(BUILD_TARGET_PLATFORM), barebones)

#
# Binary artefact extensions
#
EXE				?= axf
LIB				?= a
PATCH			?= patch

#
# Objects
#
ASM_OBJS		+= $(patsubst %.s, $(BUILD_OUTPUT_DIR)/%.o, $(ASM_SRC))
C_OBJS			+= $(patsubst %.c, $(BUILD_OUTPUT_DIR)/%.o, $(C_SRC))
CPP_OBJS		+= $(patsubst %.cpp, $(BUILD_OUTPUT_DIR)/%.o, $(CPP_SRC))
ALL_OBJS		+= $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)

#
# Target
#
ifeq ($(BUILD_ARTEFACT), EXE)
BUILD_TARGET	= $(BUILD_COMPONENT).$(EXE)
else ifeq ($(BUILD_ARTEFACT), LIB)
# arm libraries start with lib prefix
BUILD_TARGET	?= lib$(BUILD_COMPONENT).$(LIB)
else ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))

# key storage identifiers
PATCH_KEY_IDENTIFIERS = clear test eval
BUILD_ENCRYPTED_PATCH ?=
ifneq ($(BUILD_ENCRYPTED_PATCH), )
PATCH_KEY_IDENTIFIERS += $(BUILD_ENCRYPTED_PATCH)
endif

PATCH_BASENAME	= $(PRODUCT_PREFIX)_$(BUILD_COMPONENT)_$(subst 0x,,v$(BUILD_PATCH_VERSION))
BUILD_TARGET	= $(BUILD_COMPONENT).$(EXE)
PATCH_BIN_FILE := $(BUILD_COMPONENT).bin

CFLAGS			+= -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= --entry=$(BUILD_PATCH_ENTRY)
else
$(error BUILD_ARTEFACT $(BUILD_ARTEFACT) is not supported)
endif

# Linker output files
LINKER_MAP_FILE := $(BUILD_COMPONENT).map
LINKER_SYMBOL_FILE := $(BUILD_COMPONENT).symbols

#
# Rules
#

ifeq ($(BUILD_ARTEFACT), LIB)
# Pattern rule for library
$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET):	$(ALL_OBJS)
	@echo Creating static library $@...
	$(QUIET) $(AR) -rv $@ $^
else ifeq ($(filter $(BUILD_ARTEFACT), PATCH APP), $(BUILD_ARTEFACT))
# Pattern rule for patching
$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET):	$(ALL_OBJS) $(BUILD_ROM_SYMBOL_FILE)
	@echo Linking $@...
	$(QUIET) $(LD) -o $@ $^ $(LDFLAGS) --symdefs=$(BUILD_OUTPUT_DIR)/$(LINKER_SYMBOL_FILE)
else
# Pattern rule for executable
$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET):	$(ALL_OBJS)
	@echo Linking $@...
	$(QUIET) $(LD) -o $@ $^ $(LDFLAGS) --symdefs=$(BUILD_OUTPUT_DIR)/$(LINKER_SYMBOL_FILE)
#	$(LD) -o $@.axf $^ $(LDFLAGS) --keep=libcc_312.a(chacha_driver.o) "--symdefs=$(BUILD_OUTPUT_DIR)/$(LINKER_SYMBOL_FILE)"
endif

# Build patch chunks
.PHONY: chunks
chunks: $(foreach keystorage, $(PATCH_KEY_IDENTIFIERS), $(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_$(keystorage).chunks)

# Convert patch bin file into chunk files
$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME)_%.chunks: $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).$(EXE)
	$(QUIET) $(FROMELF) --vhx --16x1 --output=$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vhx $<
	$(QUIET) $(FROMELF) --bin --output=$(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE) $<
	$(QUIET)$(QUIET)$(PYTHON) scripts/create_patch.py -i $(BUILD_OUTPUT_DIR)/$(PATCH_BIN_FILE) \
		-o $(basename $@) -r $(BUILD_ROM_VERSION) -p $(BUILD_PATCH_VERSION) \
		-m  $(BUILD_OUTPUT_DIR)/$(LINKER_MAP_FILE) $(if $(findstring clear, $*), , -k $*) \
		-n $(PRODUCT_PREFIX)

$(BUILD_OUTPUT_DIR)/%.o: %.s
	@echo Assembling $@...
	$(QUIET) $(AS) -o $@ $^ $(ASFLAGS)

$(BUILD_OUTPUT_DIR)/%.o: %.c
	@echo Compiling $@...
	$(QUIET) $(CC) -c -o $@ $^ $(CFLAGS)

$(BUILD_OUTPUT_DIR)/%.o: %.cpp
	@echo Compiling $@...
	$(QUIET) $(CC) -c -o $@ $^ $(CPPFLAGS)

# Converts linker axf file to readmemh format suitable for loading
# into virtual prototype, simulator or physical implementation
ifeq ($(BUILD_ARTEFACT), EXE)
.PHONY: readmemh
readmemh: $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances $(BUILD_OUTPUT_DIR)/$(DIST_BASENAME).rom.v.instances
endif

$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME).rom.v.instances: $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances

$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances: $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Generating readmemh files ...
	$(QUIET)$(FROMELF) --bin --output $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).bin $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	$(QUIET)$(PYTHON) scripts/bin2readmemh.py --output $(BUILD_OUTPUT_DIR) --size 262144 --instances 4 \
		--base $(BUILD_ROM_BASENAME) --vpm_base $(BUILD_VPM_BASENAME) $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).bin

.PHONY: disassemble
disassemble: create_output_dir
	@echo Generating disassembly ...
	$(QUIET)$(FROMELF) -c $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) > $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).disassemble

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    disassemble        Convert executable back into assembler"
	$(QUIET) echo "    chunks             Generate patch chunks"
	$(QUIET) echo "    readmemh           Generate executable in readmemh format (for PI/simulation/modelling)"

