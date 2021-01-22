This is the Toolbox API. It is a set of functions used by other components

To build the unit tests for MinGW32 (from src/Toolbox/Test)
make -r BUILD_CONFIG=cpputest_mingw32 clean all

To build the patch for ARMCC (from src/Toolbox)
make -r BUILD_CONFIG=barebones_armcc clean all
