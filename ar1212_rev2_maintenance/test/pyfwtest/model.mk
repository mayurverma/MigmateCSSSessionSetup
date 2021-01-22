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
# Makefile for the system integration use case tests on virtual protype model
#
####################################################################################
SHELL := /bin/bash

# Suppress tool output (use 'QUIET=' on the make command line to override)
QUIET ?= @

OTPMIMAGE ?= blank
COVERAGE ?= 0
VERBOSITY ?= 2

PYTHON = /custom/tools/lang/release6/python3.6.5/bin/python

TEST_OUTPUT_DIR ?= .
TEST_ACCESS_TIMEOUT ?= 120

# Creates an OTPM symbolic link if the file exists otherwise returns an error
OTPM_INSTANCES := 0 1 2 3
define otpm-symlink
	$(QUIET)test -f ${DVFWPATH}/test/src/$(OTPMIMAGE)_otpm_1024x22_${instance}.otpm.v || \
		{ echo OTPM file not found: ${DVFWPATH}/test/src/$(OTPMIMAGE)_otpm_1024x22_${instance}.otpm.v; exit 1; }
	ln -fs ${DVFWPATH}/test/src/$(OTPMIMAGE)_otpm_1024x22_${instance}.otpm.v AR0820_OTPM_1024x22_${instance}.otpm.v

endef

# Creates a ROM symbolic link if the file exists otherwise returns an error
ROM_INSTANCES := 0 1 2 3
define rom-symlink
	$(QUIET)test -f ${DVFWPATH}/src/Rom/output/asic.armcc/CSS_Rom_v${BUILD_ROM_VERSION}_16384x39_${instance}.rom.v || \
		{ echo ROM file not found: ${DVFWPATH}/src/Rom/output/asic.armcc/CSS_Rom_v${BUILD_ROM_VERSION}_16384x39_${instance}.rom.v; exit 1; }
	ln -fs ${DVFWPATH}/src/Rom/output/asic.armcc/CSS_Rom_v${BUILD_ROM_VERSION}_16384x39_${instance}.rom.v AR0820_NVIC_ROM_16384x39_${instance}.rom.v

endef

.PHONY: pyfwtest
pyfwtest:
	$(QUIET) bsub -Is -q interactive $(MAKE) --no-print-directory -f $(DVFWPATH)/test/pyfwtest/model.mk pyfwtest_batch 

# WARNING: Do not use bsub as this will be done by the batch test launcher script
.PHONY: pyfwtest_batch
pyfwtest_batch: symlinks
	$(QUIET)$(PYTHON) -V
	$(eval TEST_ACCESS_TOKEN = $(shell $(PYTHON) ${DVFWPATH}/test/scripts/token_server.py new))
	@echo "TEST_ACCESS_TOKEN=${TEST_ACCESS_TOKEN}"
	$(QUIET)$(MAKE) --no-print-directory -r -f ${MODELROOT}/Makefile pyfwtest NOBUILD=1 COVERAGE=${COVERAGE} \
		MODELROOT=${MODELROOT} DVFWPATH=${DVFWPATH} TEST_ACCESS_TOKEN=${TEST_ACCESS_TOKEN} &
	$(QUIET)$(PYTHON) ${DVFWPATH}/test/scripts/test_runner.py --output ${TEST_OUTPUT_DIR} --tests ${TESTLIST} \
		--verbosity ${VERBOSITY} model --token ${TEST_ACCESS_TOKEN} --timeout ${TEST_ACCESS_TIMEOUT}

.PHONY: pyfwdebug
pyfwdebug: symlinks
	$(QUIET)$(PYTHON) -V
	$(eval TEST_ACCESS_TOKEN = $(shell $(PYTHON) ${DVFWPATH}/test/scripts/token_server.py new))
	@echo "TEST_ACCESS_TOKEN=${TEST_ACCESS_TOKEN}"
	$(QUIET)$(MAKE) --no-print-directory -r -f ${MODELROOT}/Makefile pyfwdebug NOBUILD=1 \
		MODELROOT=${MODELROOT} DVFWPATH=${DVFWPATH} TEST_ACCESS_TOKEN=${TEST_ACCESS_TOKEN} &
	$(QUIET)$(PYTHON) ${DVFWPATH}/test/scripts/test_runner.py --output ${TEST_OUTPUT_DIR} --tests ${TESTLIST} \
		--verbosity ${VERBOSITY} model --token ${TEST_ACCESS_TOKEN} --timeout ${TEST_ACCESS_TIMEOUT}

.PHONY: symlinks
symlinks:
	$(eval BUILD_ROM_VERSION := $(shell grep BUILD_ROM_VERSION ${DVFWPATH}/src/Rom/output/asic.armcc/build.properties | cut -dx -f2))
	$(foreach instance,$(ROM_INSTANCES),$(rom-symlink))
	$(foreach instance,$(OTPM_INSTANCES),$(otpm-symlink))

help:
	@echo "    pyfwtest          Run a test suite on the virtual prototype model using LSF"
	@echo "    pyfwtest_batch    Run a batch test on the virtual prototype model"
	@echo "    pyfwdebug         Run a test on the virtual prototype model when connected to a DS-5 debugger"
