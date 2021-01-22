#!/bin/bash
set -e

CODE_UNDER_TEST=/home/jtmzyj/Projects/AR0820/svn/linux_trunk_clean
DUT=/proj/AR0820/workareas/jtmzyj/svn/C_AR0820_REV2_branch/asic/logic/security/secure-tb
TEST_RESULTS=./test_results/rc917_r919

echo "=========================="
echo "TestGroup: SetVideoAuthRoi"
echo "=========================="
echo

export TEST_SCRIPT=test_set_video_auth_roi
export TEST_CLASS=TestSetVideoAuthROI
TEST_NAMES="blank:test_sanity_cm dm:test_sanity_dm secure_boot:test_sanity_se"

for ENTRY in ${TEST_NAMES}; do
	TEST_DESC=(${ENTRY//:/ })
	OTPM_IMAGE=${TEST_DESC[0]}
	TEST_NAME=${TEST_DESC[1]}
	
	echo
	echo
	echo "######################################################################"
	echo "Test: ${TEST_NAME}"
	echo "OTPM: ${OTPM_IMAGE}"
	echo "######################################################################"
	echo
	
	mkdir -p ${TEST_RESULTS}/${TEST_CLASS}/${TEST_NAME}

	make -r -f ${DUT}/Makefile DVFWPATH=${CODE_UNDER_TEST} SECTBROOT=${DUT} TESTNAME=pyfwtest link_code NOBUILD=1 OTPMIMAGE=${OTPM_IMAGE}

	sleep 1

	make -r -f ${DUT}/Makefile DVFWPATH=${CODE_UNDER_TEST} SECTBROOT=${DUT} TESTNAME=pyfwtest TESTLIST=${TEST_SCRIPT}.${TEST_CLASS}.${TEST_NAME} pyfwtest

	sleep 1

	if grep -q failure results.xml; then
		echo
		echo
	    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	    echo "Test: ${TEST_NAME} failed"
	    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		echo
		echo
		exit 1
	fi

	if grep -q error results.xml; then
		echo
		echo
	    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	    echo "Test: ${TEST_NAME} errored"
	    echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
		echo
		echo
		exit 2
	fi

	mv results.xml ${TEST_RESULTS}/${TEST_CLASS}/${TEST_NAME}/sim_results.xml
done


