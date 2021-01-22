This is an integration test for the HardFault handler within the Init component.
It tests that the Init component correctly handles hard faults of various types.

The test can be executed against the CSS virtual prototype, and in RTL simulation.

The HardFault test supports 8 sub-tests, selected by the TEST_CASE build variable 
(which defaults to test case 1). The tests are not self-checking, because the hard
fault handler results in a fatal error and shutdown of the firmware. Therefore the
tester has to visually check that the expected outcome occurred. Each test documents
its expected outcome. 

Each test case must be built separately, specifying the test case using TEST_CASE=2
(for example).

To build:
$ make -r BUILD_CONFIG=barebones_armcc -C /proj/AR0820/workareas/<you>/svn/trunk/test/hardfault TEST_CASE=8

To simulate:
$ make irun NOBUILD=1 TESTNAME=hardfault DVFWPATH=/proj/AR0820/workareas/<you>/svn/trunk &

To model:
$ lt.x --application /proj/AR0820/<you>/svn/trunk/test/hardfault/output/asic.armcc/hardfault.axf
