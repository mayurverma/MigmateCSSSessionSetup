#!/bin/bash
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
# Script for generating function coverage after a vManager session has finished.
#
####################################################################################
set +ex
#source /tools/icad/MICE/etc/.bashrc_ukcad
# Setup BAIN CAD/ CDE & LSF   
. /site/custom/envsys/dotfiles/bashrc.site
source /custom/tools/wrappers/bin/.cde_functions
pushd /proj/AR0820/REV
init_cde -silent
popd
set -e

# Determine the coverage script options
COVERAGE_OPTIONS="--elf ${DVFWPATH}/src/Rom/output/asic.armcc/Rom.axf --srcdir ${DVFWPATH}"
if [[ -n "${COVERAGE_REPORT}" ]]; then
    COVERAGE_OPTIONS="--output ${COVERAGE_REPORT} ${COVERAGE_OPTIONS}"
fi

# Call the function coverage generation script
python ${DVFWPATH}/test/scripts/function_coverage.py ${COVERAGE_OPTIONS} ${BRUN_SESSION_DIR}

# Call the post session cleanup script
${DVFWPATH}/test/scripts/vmanager_post_session_cleanup.sh
