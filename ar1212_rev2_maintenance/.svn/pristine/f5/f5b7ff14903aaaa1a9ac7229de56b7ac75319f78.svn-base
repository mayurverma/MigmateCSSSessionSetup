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
# Makefile for workspace build rules e.g. unit test, documentation and style check
#
####################################################################################

# Define build symbols that are mandatory for each component
BUILD_ARTEFACT := EXE
BUILD_COMPONENT := system
BUILD_COMPONENT_ROOTDIR := .

# List of all components
COMPONENT_LIST := $(shell ls src)

# List of components to be unit tested
# The following are deliberately excluded: CC312HalPal, CommandDrv, DebugUartDrv
# Init, Interrupts and SysTickDrv
UNIT_TEST_LIST := AssetMgr CC312HalPal CommandHandler CryptoMgr Diag HealthMonitor LifecycleMgr MacDrv
UNIT_TEST_LIST += NvmMgr PatchMgr Platform SessionMgr SystemDrv SystemMgr WatchdogDrv OtpmDrv Toolbox

# Define system level filenames
ASTYLE_OPTIONS := scripts/astyle.cfg
UNCRUSTIFY_OPTIONS := scripts/uncrustify.cfg

# Define build flags
CFLAGS		+= $(INC_PATHS)

# Allow Jenkins to ignore unit test errors to collate all results
ifdef BUILD_IGNORE_UNIT_TEST_ERRORS
	IGNORE_UNIT_TEST_ERRORS := -
endif

# Allow Jenkins to ignore style violations to collate all results
ifdef BUILD_IGNORE_STYLE_VIOLATIONS
	STYLE_CHECK_VIOLATIONS_LIMIT := -1
else
	STYLE_CHECK_VIOLATIONS_LIMIT := 0
endif

# Recipe for running the unit tests
define do-unit-tests
	@echo Running unit tests for $(component)
	$(IGNORE_UNIT_TEST_ERRORS)$(QUIET)$(MAKE) -r --no-print-directory -C src/$(component)/test clean all test_junit
	$(QUIET)$(CP) src/$(component)/output/$(BUILD_TARGET_DEVICE).$(BUILD_TOOLCHAIN)/*.xml $(BUILD_OUTPUT_DIR)

endef

# Recipe for applying code layout style to a source file
define do-apply-style
	@echo Style checking: $(filename)
	$(QUIET)$(ASTYLE) -q -n --project=none --options=$(ASTYLE_OPTIONS) $(filename)
	$(QUIET)$(UNCRUSTIFY) -q --no-backup -l C -c $(UNCRUSTIFY_OPTIONS) $(filename)

endef

.PHONY: unittest unittest_report unittest-report
unittest-report: unittest_report
ifeq ($(BUILD_CONFIG), cpputest_mingw32)
# Run the unit tests on the components
unittest: create_output_dir
	@echo Running unit tests ...
	$(foreach component,$(UNIT_TEST_LIST),$(do-unit-tests))
# Generate the unit test coverage report
unittest_report:
	@echo Generating $(BUILD_OUTPUT_DIR)/unittest_report.xml
	$(QUIET) $(PYTHON) contrib/gcovr/gcovr.py  --xml -b -o $(BUILD_OUTPUT_DIR)/unittest_report.xml
	@echo Generating $(BUILD_OUTPUT_DIR)/unittest_report.html
	$(QUIET) $(PYTHON) contrib/gcovr/gcovr.py  --html --html-details -b -o $(BUILD_OUTPUT_DIR)/unittest_report.html
else
unittest:
unittest_report:
	$(error Unit tests are not supported by BUILD_CONFIG '$(BUILD_CONFIG)'. Use cpputest_mingw32 instead.)
endif

# Check code layout style violations
.PHONY: check_style check-style
check-style: check_style
check_style: create_output_dir
	@echo Finding source files ...
	@echo $(wildcard inc/*.h src/*/inc/*.h src/*/src/*.[ch]) > $(BUILD_OUTPUT_DIR)/style_check_input.tmp
	$(QUIET)$(PYTHON) scripts/check_style.py --violations=$(STYLE_CHECK_VIOLATIONS_LIMIT) \
		--input=$(BUILD_OUTPUT_DIR)/style_check_input.tmp --tempdir=$(BUILD_OUTPUT_DIR) \
		--launcher astyle $(ASTYLE) $(ASTYLE_OPTIONS) --launcher uncrustify $(UNCRUSTIFY) $(UNCRUSTIFY_OPTIONS) \
		--report=$(BUILD_OUTPUT_DIR)/style_check_report.xml

# Apply code layout style to all source files except the unit tests
.PHONY: apply_style apply-style
# Apply coding style
apply-style: apply_style
apply_style:
	@echo Finding source files ...
	$(eval APPLY_STYLE_FILES := $(wildcard inc/*.h src/*/inc/*.h src/*/src/*.[ch]))
	$(foreach filename,$(APPLY_STYLE_FILES),$(do-apply-style))

# Generate the C and assembler header files
.PHONY: generate-headers generate_headers
generate-headers: generate_headers
generate_headers:
	$(QUIET)$(PYTHON) scripts/generate_headers.py --memmap contrib/target/2v0/memmap.ini --input contrib/target/2v0/ipxact --output contrib/target/2v0/inc
	$(QUIET)$(PYTHON) scripts/generate_errors.py --csvfile inc/CommonError.csv --validate --output inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_checkpoints.py --csvfile src/Diag/src/diag_checkpoints.csv --validate --output src/Diag/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_commands.py --csvfile src/CommandHandler/src/CommandHandlerHostCommandCode.csv --validate --output src/CommandHandler/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_css_faults.py --csvfile src/SystemMgr/src/SystemMgrCssFault.csv --validate --output src/SystemMgr/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_fatal_errors.py --csvfile src/SystemMgr/src/SystemMgrFatalError.csv --validate --output src/SystemMgr/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_system_phases.py --csvfile src/SystemMgr/src/SystemMgrPhase.csv --validate --output src/SystemMgr/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_lifecycle_states.py --csvfile src/LifecycleMgr/src/LifecycleMgrLifecycleState.csv --validate --output src/LifecycleMgr/inc --scripts test/scripts
	$(QUIET)$(PYTHON) scripts/generate_verification_app_debug_id.py --csvfile src/VerificationApp/src/VerificationAppDebugId.csv --validate --output src/VerificationApp/src --scripts test/scripts

# Generate the (pre-canned) OTPM test images
.PHONY: generate_otpm
generate_otpm:
	# Original Images
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --base blank_otpm --size 8192 --instances 4 test/src/blank_otpm.txt
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --base dm_otpm --size 8192 --instances 4 test/src/dm_otpm.txt
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --base secure_boot_otpm --size 8192 --instances 4 test/src/secure_boot_otpm.txt
	# CM Images
	# DM Images
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --size 8192 --instances 4 \
	    --base dm_corrupted_length_free_location_0x2c_otpm \
	    --address_value_file test/src/corrupted_length_free_location_0x2c_avf.txt \
	    test/src/dm_otpm.txt
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --size 8192 --instances 4 \
	    --base dm_ecc_ded_0x30_otpm \
	    --address_value_file test/src/dummy_record_0x2c_0x2f_avf.txt \
	    --bit_flip_file test/src/ecc_ded_0x30_bff.txt \
	    test/src/dm_otpm.txt
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --size 8192 --instances 4 \
	    --base cm_ecc_ded_0x10_otpm \
	    --bit_flip_file test/src/ecc_ded_0x10_bff.txt \
	    test/src/blank_otpm.txt
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py --output test/src --size 8192 --instances 4 \
	    --base cm_dummy_record_otpm \
	    --address_value_file test/src/dummy_record_0x2c_0x2f_avf.txt \
	    test/src/blank_otpm.txt

	# SE Images
	$(QUIET)$(PYTHON) scripts/otpm2readmemh.py test/src/secure_boot_depu_test_otpm.txt --output test/src --size 8192 --instances 4 --base secure_boot_depu_test_otpm
	    
# Clean the whole workspace (removes all output directories)
bare::
	 @echo Cleaning workspace...
	-$(QUIET)$(RMDIR) $(patsubst %,src/%/output,$(COMPONENT_LIST))

help::
	$(QUIET)echo "    apply_style         Apply coding style formatting rules to source files"
	$(QUIET)echo "    check_style         Check source files for coding style violations"
	$(QUIET)echo "    generate_headers    Generate the C and assembler header files"
	$(QUIET)echo "    generate_otpm       Generate the default OTPM image files"
	$(QUIET)echo "    unittest            Runs the component unit tests"

ALL_GOALS := help

# include the configuration makefile
include config.mk

ifndef BUILD_TOOLCHAIN
$(error BUILD_TOOLCHAIN is not set)
endif

