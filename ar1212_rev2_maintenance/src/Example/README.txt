This is an example component providing boilerplate for actual component development
and testing. The example provides a CppUTest unit test to demonstrate how to build and test
the component. The example also includes Eclipse project settings and run launches to allow the
unit test to be built, run and debugged in Eclipse.

To build the unit tests for MinGW
make -r BUILD_CONFIG=cpputest_mingw32 clean all

To run the unit tests:
make -r BUILD_CONFIG=cpputest_mingw32 test

To build the unit tests for ARMCC
make -r BUILD_CONFIG=cpputest_armcc clean all
