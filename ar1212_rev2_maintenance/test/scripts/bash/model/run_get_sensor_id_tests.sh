#!/bin/bash
set -e

CODE_UNDER_TEST=/home/jtmzyj/Projects/AR0820/svn/linux_trunk_clean
MODEL_UNDER_TEST=/proj/AR0820/workareas/jtmzyj/model_build
TEST_RESULTS=./test_results/rc917_r920

echo "======================"
echo "TestGroup: GetSensorId"
echo "======================"
echo

export TEST_SCRIPT=test_get_sensor_id
export TEST_CLASS=TestGetSensorId
TEST_NAMES="blank:test_get_sensor_id_in_cm dm:test_get_sensor_id_in_dm secure_boot:test_get_sensor_id_in_se"

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

	make -r -f ${MODEL_UNDER_TEST}/Makefile DVFWPATH=${CODE_UNDER_TEST} MODELROOT=${MODEL_UNDER_TEST} link_code NOBUILD=1 OTPMIMAGE=${OTPM_IMAGE}

	sleep 1

	make -r -f ${MODEL_UNDER_TEST}/Makefile DVFWPATH=${CODE_UNDER_TEST} MODELROOT=${MODEL_UNDER_TEST} TESTLIST=${TEST_SCRIPT}.${TEST_CLASS}.${TEST_NAME} pyfwtest

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

	mv results.xml ${TEST_RESULTS}/${TEST_CLASS}/${TEST_NAME}/vp_results.xml
done


