This is an integration test for the Init component. It tests that the Init component
correctly starts-up the M0+, initializes the stack and heap, and configures the
MPU. It also tests that the VTOR can be used to relocate the vector table.

The test can be executed against the CSS virtual prototype, and in RTL simulation.

The test is self-checking, and will print a pass or fail notification to the console. 

To build:
$ make -r BUILD_CONFIG=barebones_armcc -C /proj/AR0820/workareas/<you>/svn/trunk/test/init clean all readmemh

To simulate:
$ make irun NOBUILD=1 TESTNAME=init DVFWPATH=/proj/AR0820/workareas/<you>/svn/trunk &

To model:
$ lt.x --application /proj/AR0820/<you>/svn/trunk/test/init/output/asic.armcc/init.axf
