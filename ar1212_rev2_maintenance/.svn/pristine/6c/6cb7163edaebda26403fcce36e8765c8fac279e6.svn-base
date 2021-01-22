# ***********************************************************************************
#  Copyright 2018 ON Semiconductor.  All rights reserved.
#
#  This software and/or documentation is licensed by ON Semiconductor under limited
#  terms and conditions. The terms and conditions pertaining to the software and/or
#  documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
#  ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
#  Do not use this software and/or documentation unless you have carefully read and
#  you agree to the limited terms and conditions. By using this software and/or
#  documentation, you agree to the limited terms and conditions.
#
# *************************************************************************************
#
# Common makefile for MinGW32 builds
#
# *************************************************************************************

#
# Guards
#
ifneq ($(BUILD_TOOLCHAIN), mingw32)
$(error BUILD_TOOLCHAIN is invalid: '$(BUILD_TOOLCHAIN)')
endif

ifneq ($(BUILD_TOOLCHAIN_VERSION), 5.3.0)
$(error BUILD_TOOLCHAIN_VERSION is unsupported: '$(BUILD_TOOLCHAIN_VERSION)')
endif

ifneq ($(BUILD_TYPE), debug)
$(error BUILD_TYPE is unsupported: '$(BUILD_TYPE)')
endif

#
# Tools
#

# We assume MinGW lives in c:\mingw - this is highly-recommended by the MinGW installer!
MINGW32_HOME	?= c:/mingw

CC		:= $(MINGW32_HOME)/bin/gcc
CPP		:= $(MINGW32_HOME)/bin/g++
GCOV	:= $(MINGW32_HOME)/bin/gcov
AR		:= $(MINGW32_HOME)/bin/ar
WINDRES	:= $(MINGW32_HOME)/bin/windres
CPPLINT	:= $(PYTHON) contrib/cpplint/cpplint.py

#
# C compiler flags
#
CFLAGS  		+= -g -Og -Wall -Wextra -Werror
CFLAGS  		+= -std=c99
CFLAGS  		+= -ftest-coverage -fprofile-arcs	# for code coverage
CFLAGS			+= -DBUILD_TOOLCHAIN_mingw32
CFLAGS			+= -Icontrib/cmsis/core/include

#
# CPP compiler flags
#
CPPFLAGS		+= -g -Og
CPPFLAGS		+= -Wall -Wextra -Wno-c++14-compat -Wno-padded
CPPFLAGS		+= -Wno-long-long -Wshadow -Wswitch-default -Wswitch-enum -Wconversion
CPPFLAGS		+= -Wsign-conversion -Woverloaded-virtual -Wno-old-style-cast
CPPFLAGS		+= -DBUILD_TOOLCHAIN_mingw32
CPPFLAGS		+= -Icontrib/cmsis/core/include

ifneq ($(BUILD_SAVE_TEMPS), )
CFLAGS			+= -save-temps=obj
CPPFLAGS		+= -save-temps=obj
endif

#
# Linker flags
#
LDFLAGS 		+= -lpthread
LDFLAGS 		+= -fprofile-arcs -lgcov			# for code coverage
ADD_LIBPATH		+= -L
ADD_LIB			+= -l

#
# Gcov flags
#
GCOVFLAGS		+= -b	# Generate branch summary info
GCOVFLAGS		+= -c 	# Write branch frequencies as counts (rather than percentages)

#
# Objects
#
C_OBJS			+= $(patsubst %.c, $(BUILD_OUTPUT_DIR)/%.o, $(C_SRC))
CPP_OBJS		+= $(patsubst %.cpp, $(BUILD_OUTPUT_DIR)/%.o, $(CPP_SRC))
ALL_OBJS		+= $(C_OBJS) $(CPP_OBJS)

#
# Target
#
ifeq ($(BUILD_ARTEFACT), EXE)
BUILD_TARGET	= $(BUILD_COMPONENT).exe
else ifeq ($(BUILD_ARTEFACT), LIB)
BUILD_TARGET	= $(BUILD_COMPONENT).a
else
$(error BUILD_ARTEFACT $(BUILD_ARTEFACT) is not supported)
endif

#
# Rules
#

$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).exe:	$(ALL_OBJS) $(BUILD_OUTPUT_DIR)/Manifest.res
	@echo Linking $@...
	$(QUIET) $(CPP) -o $@ $(ALL_OBJS) $(LDFLAGS) $(BUILD_OUTPUT_DIR)/Manifest.res

$(BUILD_OUTPUT_DIR)/$(BUILD_COMPONENT).a:	$(ALL_OBJS)
	@echo Building static library $@...
	$(QUIET) $(AR) rvs $@ $^

$(BUILD_OUTPUT_DIR)/%.o: %.c
	@echo Compiling $@...
	$(QUIET) $(CC) -c -o $@ $^ $(CFLAGS)

$(BUILD_OUTPUT_DIR)/%.o: %.cpp
	@echo Compiling $@...
	$(QUIET) $(CPP) -c -o $@ $^ $(CPPFLAGS)

$(BUILD_OUTPUT_DIR)/Manifest.res: scripts/Manifest.rc scripts/Manifest.xml
	@echo Generating $(BUILD_OUTPUT_DIR)/Manifest.res...
	$(QUIET) $(WINDRES) --input scripts/Manifest.rc --output $(BUILD_OUTPUT_DIR)/Manifest.res --output-format=coff

.PHONY:gcov_clean bare cpplint

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

cpplint: $(BUILD_OUTPUT_DIR)/cpplint.txt

CPPLINT_FILTERS := -readability/todo,-whitespace/todo,-whitespace/braces,-whitespace/end_of_line

$(BUILD_OUTPUT_DIR)/cpplint.txt: $(CPP_SRC)
	@echo Running CPPLINT...
	$(QUIET) -$(CPPLINT) --counting=detailed --output=eclipse --filter=$(CPPLINT_FILTERS), --linelength=120 $^ > $@ 2>&1
	$(QUIET) cat $@

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    gcov               Run GCOV with summary results"
	$(QUIET) echo "    gcov_verbose       Run GCOV with per-function results"
	$(QUIET) echo "    gcov_reset         Reset the GCOV result database"
	$(QUIET) echo "    gcov_clean         Delete all GCOV artefacts"
	$(QUIET) echo "    --"
	$(QUIET) echo "    Use BUILD_SAVE_TEMPS=1 to save compiler temporary files"

