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
# Makefile for launching a vManager run on simulator
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
shellConfig mdv mdv18.03.004
shellConfig xcelium xcelium18.09.001
set -x

make -f ${DVFWPATH}/test/pyfwtest/sim.mk TESTNAME=pyfwtest SECTBROOT=${SECTBROOT} \
    DVFWPATH=${DVFWPATH} DEFINE=" -64 -seed ${BRUN_SV_SEED} -write_metrics " \
    DUMPFILE=0 NOBUILD=1 ${BRUN_SIM_ARGS} pyfwtest_batch
