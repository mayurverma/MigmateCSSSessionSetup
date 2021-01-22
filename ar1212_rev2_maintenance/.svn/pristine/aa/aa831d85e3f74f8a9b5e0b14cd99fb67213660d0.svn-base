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
# Common makefile for native CppUTest-based unit tests
#
####################################################################################

#
# Guard - check that configuration supports CppUTest
#
ifneq ($(BUILD_CONFIG), )
 ifneq ($(BUILD_CONFIG), default)
  ifeq ($(findstring cpputest, $(BUILD_CONFIG)), )
$(error BUILD_CONFIG '$(BUILD_CONFIG)' does not support CppUTest)
  endif
 endif
endif

CPPUTEST_ACTIVE = yes
CPPUTEST_HOME 	= contrib/cpputest
FFF_HOME		= contrib/fff

CPP_SRC			+= cpputest_main.cpp
VPATH			+= test/src

# Include paths for CppUTest and FFF
CPPFLAGS		+= -I$(CPPUTEST_HOME)/include
CPPFLAGS		+= -I$(FFF_HOME)
CPPFLAGS		+= -Itest/inc

# CppUTest libraries
LDFLAGS 		+= $(ADD_LIBPATH)$(CPPUTEST_HOME)/$(CPPUTEST_LIBS)/lib
LDFLAGS 		+= $(ADD_LIB)CppUTest
LDFLAGS 		+= $(ADD_LIB)CppUTestExt

.PHONY: test test_junit

# Execute the test (output to stdout/console)
# In order for this to work the DS-5 has to be installed and configured personally following the project steps
DS5_DEBUG_CONFIG:= Imported-CryptoCcore
CDBROOT			:= "/home/$(USER)/DS-5-Workspace/CryptoCcore"
LAUNCHCONFIG	:= "/home/$(USER)/DS-5-Workspace/.metadata/.plugins/org.eclipse.debug.core/.launches/$(DS5_DEBUG_CONFIG).launch"
#CDBENTRY		:= "ARM FVP::MPS2_Cortex_M0plus::Bare Metal Debug::Bare Metal Debug::Debug Cortex-M0+"
DEBUGGER_SCRIPT	= $(BUILD_OUTPUT_DIR)/ds5_script.ds

#ARM_DEBUGGER_FLAGS += --cdb-entry $(CDBENTRY)
ARM_DEBUGGER_FLAGS += --cdb-root $(CDBROOT)
# Note "launch-config" only works with ARM-Debugger v5.27.1
ARM_DEBUGGER_FLAGS += --launch-config $(LAUNCHCONFIG)
ARM_DEBUGGER_FLAGS += --script $(DEBUGGER_SCRIPT)

test:
ifeq ($(BUILD_CONFIG), cpputest_armcc)
	#$(QUIET) echo Generating $(DEBUGGER_SCRIPT)
	#$(QUIET) echo loadfile \"$(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)\" > $(DEBUGGER_SCRIPT)
	#$(QUIET) echo run $(TEST_FLAGS) $(JUNIT_OUTPUT) >> $(DEBUGGER_SCRIPT)
	#$(QUIET) echo wait >> $(DEBUGGER_SCRIPT)
	#$(QUIET) echo quit >> $(DEBUGGER_SCRIPT)
	#$(QUIET) $(ARM_DEBUGGER) $(ARM_DEBUGGER_FLAGS)
	$(QUIET) cd $(BUILD_OUTPUT_DIR) && lt.x --application ./$(BUILD_TARGET) --simlimit 1000 
else
	$(QUIET) cd $(BUILD_OUTPUT_DIR) && ./$(BUILD_TARGET) $(TEST_FLAGS) $(JUNIT_OUTPUT)
endif

# Execute the test (output to JUnit .xml files)
test_junit: JUNIT_OUTPUT = -ojunit
test_junit: TEST_FLAGS=-k cpputest
test_junit: test

clean::
	$(QUIET) $(RM) $(DEBUGGER_SCRIPT)
	$(QUIET) $(RM) $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET).out
	$(QUIET) $(RM) $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET).err
	$(QUIET) $(RM) $(BUILD_OUTPUT_DIR)/arm_debugger.log

help::
	$(QUIET) echo 'test:         execute the unit tests, output to stdout'
	$(QUIET) echo '              - use TEST_FLAGS= to add optional arguments to the Test Runner'
	$(QUIET) echo '              - use DS5_DEBUG_CONFIG=name to change launch configuration file ('name'.launch - default = AR0820)'
	$(QUIET) echo 'test_junit:   execute the unit tests, output to JUnit .xml - use TEST_FLAGS= to add optional arguments to the Test Runner'

