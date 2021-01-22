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
# Common makefile for ARM GNU builds (arm-none-eabi-xxx)
#
####################################################################################

#
# Guards
#
ifneq ($(BUILD_TOOLCHAIN), arm-none-eabi-gcc)
$(error BUILD_TOOLCHAIN is invalid: '$(BUILD_TOOLCHAIN)')
endif

ifneq ($(BUILD_TOOLCHAIN_VERSION), 6.2.0)
$(error BUILD_TOOLCHAIN_VERSION is unsupported: '$(BUILD_TOOLCHAIN_VERSION)')
endif

#
# Tools
#

# Select the default Sourcery CodeBench license servers
export MGLS_LICENSE_FILE	?= 1717@licpx2;28100@bemc02ls187;1717@licg22;1717@licg32;1717@licg12

ifeq ($(BUILD_HOST), windows)
	#
	# Assume we're using the ARM GNU from Sourcery CodeBench
	#
	ARM_GNU_HOME	?= c:/mgc/embedded/toolchains/arm-none-eabi.2016.11
else
	#
	# Assume ARM GNU GCC 6.2.0 from Sourcery CodeBench
	#
	ARM_GNU_HOME	?= /devel/edatools/mgc/codebench/2017.02/toolchains/arm-none-eabi.2016.11
endif # ifeq ($(BUILD_HOST), windows)

CC 	:= $(ARM_GNU_HOME)/bin/arm-none-eabi-gcc
CPP	:= $(ARM_GNU_HOME)/bin/arm-none-eabi-g++
OBJCOPY := $(ARM_GNU_HOME)/bin/arm-none-eabi-objcopy
GCOV	:= $(ARM_GNU_HOME)/bin/arm-none-eabi-gcov
NM	:= $(ARM_GNU_HOME)/bin/arm-none-eabi-nm

ifeq ($(BUILD_TARGET_PLATFORM), cpputest)
#
# CppUTest-specific support
#

#
# Guards
#
ifeq ($(CPPUTEST_ACTIVE), )
$(error BUILD_TARGET_PLATFORM is 'cpputest', but cpputest.mk not included)
endif

ifeq ($(CPPUTEST_LIBS), )
$(error CPPUTEST_LIBS is not specified)
endif

else ifeq ($(BUILD_TARGET_PLATFORM), semihosted)
#
# Semihosting-specific support
#

else ifeq ($(BUILD_TARGET_PLATFORM), barebones)
#
# Barebones-specific support
#

else
$(error Toolchain does not support BUILD_TARGT_PLATFORM '$(BUILD_TARGET_PLATFORM)')
endif

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
# C compiler flags (common to all variants)
#
CFLAGS			+= -mcpu=$(CPU) -mthumb
CFLAGS			+= -fno-common
CFLAGS  		+= -Wall -Wextra -Werror
CFLAGS  		+= -std=c99
CFLAGS			+= -DBUILD_TOOLCHAIN_arm_none_eabi_gcc

#
# CPP compiler flags (common to all variants)
#
CPPFLAGS		+= -mcpu=$(CPU) -mthumb
CPPFLAGS		+= -fno-common
CPPFLAGS		+= -g3 -O0 -std=c++11
CPPFLAGS		+= -Wno-long-long -Wall -Wextra -Wshadow
CPPFLAGS		+= -Wswitch-default -Wswitch-enum -Wconversion -Wsign-conversion
CPPFLAGS		+= -Woverloaded-virtual -Wno-padded -Wno-old-style-cast
CPPFLAGS		+= -Wno-c++14-compat

ifneq ($(BUILD_SAVE_TEMPS), )
CFLAGS			+= -save-temps=obj
CPPFLAGS		+= -save-temps=obj
endif

#
# Linker flags (common to all variants)
#
LDFLAGS			+= -mcpu=$(CPU)
LDFLAGS			+= -mthumb
# INFO: <rw>: This library should be used instead of default to get gcov to work. Unfortunately no way to force this to be used
# before default lidraries are searched. Only way is to overwrite local install with what can be found in contrib nodes
LDFLAGS			+=  -Wl,-Lcontrib/mentor/embedded/toolchain/arm-none-eabi.2016.11/lib/gcc/arm-none-eabi/6.2.0/armv6-m -lgcov

# linker map file
LDFLAGS			+= -Wl,-Map=$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).map
# input file tracing
#LDFLAGS		+= -t

ADD_LIBPATH		+= -L
ADD_LIB			+= -l

#
# Gcov flags
#
GCOVFLAGS		+= -b	# Generate branch summary info
GCOVFLAGS		+= -c 	# Write branch frequencies as counts (rather than percentages)

#
# Debug vs Release build types
#
ifeq ($(BUILD_TYPE), debug)
CFLAGS  		+= -g3 -O0
else
CFLAGS  		+= -g0 -Os
endif # ($(BUILD_TYPE), debug)

ifeq ($(BUILD_TARGET_PLATFORM), cpputest)
# *************************************************************************************
#
#  CppUTest support (full C and C++ library support, semi-hosting, CppUTest and Gcov)
#

# Start-up support for full C and C++ libraries
ASM_SRC			+= InitReset.S InitHardFault.S
C_SRC			+= InitVectors.c InitSystem.c InitMpu.c InitHardFaultHandler.c

CFLAGS			+= -DBUILD_TARGET_PLATFORM_cpputest
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
# for code coverage
#CFLAGS  		+= -ftest-coverage -fprofile-arcs
CFLAGS  		+= --coverage
LDFLAGS			+= -lstdc++
LDFLAGS			+= -T contrib/target/$(BUILD_TARGET_DEVICE_VERSION)/linker/GCC/ram-hosted.ld
# for code coverage
#LDFLAGS 		+= -fprofile-arcs -lgcov
LDFLAGS  		+= --coverage
#LDFLAGS 		+= -lgcov

VPATH			+= src/Init/src src/Init/src/GCC

ifneq ($(CPPUTEST_ACTIVE), )

ifeq ($(CPPUTEST_LIBS), )
$(error CppUTest is selected, but CPPUTEST_LIBS is not specified)
endif

# Additional C++ flags for CppUTest
CPPFLAGS		+= -DCPPUTEST_MEM_LEAK_DETECTION_DISABLED -DCPPUTEST_STD_CPP_LIB_DISABLED

endif # ifneq ($(CPPUTEST_ACTIVE), )
endif # ifeq ($(BUILD_TARGET_PLATFORM), cpputest)

ifeq ($(BUILD_TARGET_PLATFORM), semihosted)
# *************************************************************************************
#
# Semihosting-specific support (full C/C++ library, semi-hosting)
#

ASM_SRC			+= InitReset.S InitHardFault.S
C_SRC			+= InitVectors.c InitSystem.c InitMpu.c InitHardFaultHandler.c
CFLAGS			+= -DBUILD_TARGET_PLATFORM_semihosted
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= -T contrib/target/$(BUILD_TARGET_DEVICE_VERSION)/linker/GCC/ram-hosted.ld

VPATH			+= src/Init/src src/Init/src/GCC

endif # ifeq ($(BUILD_TARGET_PLATFORM), semihosted)

ifeq ($(BUILD_TARGET_PLATFORM), barebones)
# *************************************************************************************
#
# Barebones-specific support (limited C library, no C++, no semi-hosting)
#

ASM_SRC			+= InitReset.S InitHardFault.S
C_SRC			+= InitVectors.c InitSystem.c InitMpu.c InitHardFaultHandler.c InitSysRetarget.c
CFLAGS			+= -DBUILD_TARGET_PLATFORM_barebones
CFLAGS			+= -Isrc/Init/inc -Icontrib/cmsis/core/include
CPPFLAGS		+= -Icontrib/cmsis/core/include
LDFLAGS			+= -T contrib/target/$(BUILD_TARGET_DEVICE_VERSION)/linker/GCC/ram-hosted.ld

VPATH			+= src/Init/src src/Init/src/GCC

endif # ifeq ($(BUILD_TARGET_PLATFORM), barebones)

#
# Objects
#
ASM_OBJS		+= $(patsubst %.S, $(BUILD_OUTPUT_DIR)/%.o, $(ASM_SRC))
C_OBJS			+= $(patsubst %.c, $(BUILD_OUTPUT_DIR)/%.o, $(C_SRC))
CPP_OBJS		+= $(patsubst %.cpp, $(BUILD_OUTPUT_DIR)/%.o, $(CPP_SRC))
ALL_OBJS		+= $(ASM_OBJS) $(C_OBJS) $(CPP_OBJS)

#
# Target
#
ifeq ($(BUILD_ARTEFACT), EXE)
BUILD_TARGET	= $(BUILD_COMPONENT).axf
else ifeq ($(BUILD_ARTEFACT), LIB)
BUILD_TARGET	= $(BUILD_COMPONENT).a
else
$(error BUILD_ARTEFACT $(BUILD_ARTEFACT) is not supported)
endif

#
# Rules
#

$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET):	$(ALL_OBJS)
	@echo Linking $@...
	$(QUIET) $(CPP) -o $@ $^ $(LDFLAGS)
	$(QUIET) $(NM) $@ > $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).symbols


$(BUILD_OUTPUT_DIR)/%.o: %.S
	@echo Assembling $@...
	$(QUIET) $(CC) -x assembler-with-cpp -c -o $@ $^ $(CFLAGS)

$(BUILD_OUTPUT_DIR)/%.o: %.c
	@echo Compiling $@...
	$(QUIET) $(CC) -c -o $@ $^ $(CFLAGS)

$(BUILD_OUTPUT_DIR)/%.o: %.cpp
	@echo Compiling $@...
	$(QUIET) $(CPP) -c -o $@ $^ $(CPPFLAGS)

# Converts linker axf file to readmemh format suitable for loading
# into virtual prototype, simulator or physical implementation
ifeq ($(BUILD_ARTEFACT), EXE)
.PHONY: readmemh
readmemh: $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances $(BUILD_OUTPUT_DIR)/$(DIST_BASENAME).rom.v.instances
endif

$(BUILD_OUTPUT_DIR)/$(DIST_BASENAME).rom.v.instances: $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances

$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).vpm.instances: $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Generating readmemh files ...
	$(QUIET)$(OBJCOPY) -O binary $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET) $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).bin
	$(QUIET)$(PYTHON) scripts/bin2readmemh.py --output $(BUILD_OUTPUT_DIR) --size 262144 --instances 4 \
		--base $(BUILD_ROM_BASENAME) --vpm_base $(BUILD_VPM_BASENAME) $(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).bin

.PHONY:gcov_clean

# GCOV support - writes a summary to stdout
# Note GCOV writes its output file to the working directory, so we have to cp/rm
# it to the project's output directory
gcov: $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Running GCOV...
	$(QUIET) $(GCOV) $(GCOVFLAGS) $(BUILD_OUTPUT_DIR)/*.gcda
	$(QUIET) $(CP) *.c.gcov $(BUILD_OUTPUT_DIR)
	$(QUIET) $(RM) *.c.gcov

# Verbose GCOV support - adds per-function summary to stdout
gcov_verbose: $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	@echo Running GCOV...
	$(QUIET) $(GCOV) $(GCOVFLAGS) -f $(BUILD_OUTPUT_DIR)/*.gcda
	@echo Generating $(BUILD_OUTPUT_DIR)/gcov_results.html
	$(QUIET) $(PYTHON) contrib/gcovr/gcovr.py  --html --html-details -b -o $(BUILD_OUTPUT_DIR)/gcov_results.html # -v -f $(BUILD_COMPONENT) -r .
	$(QUIET) $(CP) *.c.gcov $(BUILD_OUTPUT_DIR)
	$(QUIET) $(RM) *.c.gcov

# Remove the gcda result database file
gcov_reset:
	@echo Resetting GCOV result database
	$(QUIET) $(RM) "$(BUILD_OUTPUT_DIR)/*.gcda"

# Remove GCOV arcs/test-coverage derived-objects
# NOTE: Use quotes for pattern matching expressions to avoid ZSH expansion errors
gcov_clean::
	@echo GCOV clean...
	$(QUIET) $(RM) "$(BUILD_OUTPUT_DIR)/*.gcda" "$(BUILD_OUTPUT_DIR)/*.gcno" "$(BUILD_OUTPUT_DIR)/*.c.gcov"  "$(BUILD_OUTPUT_DIR)/*.html"


.PHONY: clean
# Remove GCC derived-objects
# NOTE: Use quotes for pattern matching expressions to avoid ZSH expansion errors
clean::
	@echo clean...
	$(QUIET) $(RM) $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)
	$(QUIET) $(RM) $(ALL_OBJS)
	$(QUIET) $(RM) "$(BUILD_OUTPUT_DIR)/*.i" "$(BUILD_OUTPUT_DIR)/*.ii" "$(BUILD_OUTPUT_DIR)/*.s" "$(BUILD_OUTPUT_DIR)/*.map"

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    gcov               Run GCOV with summary results"
	$(QUIET) echo "    gcov_verbose       Run GCOV with per-function results"
	$(QUIET) echo "    gcov_reset         Reset the GCOV result database"
	$(QUIET) echo "    gcov_clean         Delete all GCOV artefacts"
	$(QUIET) echo "    --"
	$(QUIET) echo "    Use BUILD_SAVE_TEMPS=1 to save compiler temporary files"





