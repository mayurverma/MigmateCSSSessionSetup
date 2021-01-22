This is an integration test for the 'system' - it uses the ROM build. It tests 
that the System Manager correctly iniializes the system, that the 'doorbell' 
bit is cleared once the firmware has initialized, and that the GetStatus command 
completes successfully.

This test should be executed prior to commencing integration tests with Python.
It is self-checking, and will complete once the GetStatus command completes 
successfully.

The test can be executed only against the RTL simulation.

To build:
$ make -r BUILD_CONFIG=barebones_armcc -C /proj/AR0820/workareas/<you>/svn/trunk/test/system clean all readmemh

To simulate:
$ make irun NOBUILD=1 TESTNAME=system DVFWPATH=/proj/AR0820/workareas/<you>/svn/trunk &
