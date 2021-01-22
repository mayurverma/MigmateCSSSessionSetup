This document describes the AR0820 firmware build system (based on GNU Make).

GNU Make details are here:
https://www.gnu.org/software/make/manual/make.html

#-------------------------------------------------------------------------------
#
# Top-level Makefiles
#
#-------------------------------------------------------------------------------

The build system uses a series of top-level makefiles (with the .mk extension) 
stored in the root of the repository. These makefiles control how a 'component'
is built. A 'component' represents any 'unit' of software that can be built 
individually from any other component.

Each component must provide one (or more) makefiles which describe what needs to
be built, and how. Each makefile MUST define the following environment variables:

- BUILD_COMPONENT:  this represents the name of the component, and should match
                    the component's name in the src directory.

- BUILD_OUTPUT_DIR: this controls where the built artefacts will be placed in the
                    file system. Typically this is an output directory within the
                    component's tree.

- BUILD_ARTEFACT:   this controls what is built. Typically this is set to 'EXE' to
                    indicate a fully-linked executable (for example a Windows .exe
                    or an embedded ELF file). 

                    A 'LIB' indicates a partially-linked library.

- C_SRC:            list of C source modules to be compiled and linked
- CPP_SRC:          list of C++ source modules to be compiled and linked
- VPATH:            list of directories containing the referenced source modules

A makefile should include 'cpputest.mk' if the makefile is building a unit-test 
for the module. The makefile MUST include the Configuration makefile 'config.mk' 
which includes the top-level configuration and build control makefiles. An example 
makefile is shown below (see Example component makefile).

Components can optionally define the SUPPORTED_BUILD_CONFIGS environment variable 
to control which build configurations are supported by the component. Make will 
fail if the user attempts to build with an unsupported configuration. It will 
list the supported configuration (or configurations).

The Configuration makefile (<root>/config.mk) uses the BUILD_CONFIG environment
variable to control how the component is built. BUILD_CONFIG represents a
'configuration'; this is realised by setting a number of derived environment 
variables.

BUILD_CONFIG should be specified on the make invocation line - for example:

    % make -r BUILD_CONFIG=cpputest_armcc clean all

This indicates that the "CppUTest for ARMCC" configuration should be used to 
build the component. If BUILD_CONFIG is not specified on the command line the
'default' configuration is selected. This is typically the most-common 
configuration. At present this is 'cpputest_mingw32' as we're developing the 
firmware components standalone, which is using the CppUTest framework for MinGW32. 
However note that this default may change as the project progresses so you should 
get in the habit of always specifying the BUILD_CONFIG when invoking make.

BUILD_CONFIG is used within config.mk to set a number of internal environment 
variables. Full details are provided in config.mk, but a quick summary:

    - Controls whether building for debugging or release
      BUILD_TYPE = debug | release

    - Determines host for the tools (auto-detected) 
      BUILD_HOST = windows | linux

    - Determines the compiler/linker toolchain
      BUILD_TOOLCHAIN = mingw32 | arm-none-eabi-gcc | armcc

    - Determines the device being targetted:
      BUILD_TARGET_DEVICE = pc | asic | proto

    - Determines the version of the device being targetted:
        represents the revision of the target device (pc and asic-only)
      BUILD_TARGET_DEVICE_VERSION = 1v0 | 1v1 | 2v0 | ... | na

    - Determines the prototype device (if BUILD_TARGET_DEVICE is 'proto'):
      BUILD_TARGET_PROTO_DEVICE = fpga-115 | fpga-440 | mps2_fvp | mentor_vp | na

	- Determines the version of the prototype being targetted:
	    represents the revision/configuration of the prototype 
	    (what RTL revision, what additional test registers etc)
	  BUILD_TARGET_PROTO_DEVICE_VERSION = 1v0 | 1v1 | ...
	  
    - Determines the software platform 
      BUILD_TARGET_PLATFORM = barebones | semihosted | cpputest

    - Determines the version of the software platform
      BUILD_TARGET_PLATFORM_VERSION = 1v0 | 1v0 | 109b436

Note that some BUILD_xxx environment variables are only used for a subset of 
configuration. Each configuration will test that the appropriate variables 
are set, and are valid.

Note the 'meaning' of these variables may change (or be extended) during the
duration of the project. However in most cases the user will not be aware of
the change, or need to know about it. Just knowledge of the appropriate 
BUILD_CONFIG should be sufficient in most cases.

The Configuration makefile includes a number of additional makefiles to realise
the configuration:

- toolchain.mk:     this is a wrapper for all the supported compiler toolchains 
                    (e.g. MinGW32, ARMCC). It includes the appropriate toolchain's 
                    makefile. It also defines the 'shell' utilities (e.g. cp, 
                    mkdir, rm) that differ between Windows and Linux (using the 
                    BUILD_HOST environment variable).
                    
                    Each toolchain's makefile (e.g. mingw32.mk, armcc.mk) defines
                    the compiler and linker to be used, various compiler and linker
                    flags, and the compiler and linker build rules which will 
                    compile and link the code.

- target_device.mk: this exports the target device settings to the C and C++ compilers
                    via the preprocessor, and also adds the appropriate include path
                    to the RTL headers.

- common.mk:        this provides 'common' variables and rules for all builds and 
                    configurations. The 'debug' rule aids debugging makefile 
                    issues. Use 'make -r BUILD_CONFIG=xxx debug' to get a dump of
                    the internal environment variable settings for the chosen
                    configuration.

- doxygen.mk:       this defines variables and rules to generate the Doxygen auto-
                    generated documentation.

An additional top-level makefile is provided:

- cpputest.mk:      this should be included explicitly from each component's 
                    makefile (or makefiles) that generate unit tests. It defines
                    variables and rules for configuring CppUTest and resolving
                    the appropriate CppUTest libraries.

#-------------------------------------------------------------------------------
#
# Component Variants
#
#-------------------------------------------------------------------------------

Each component (driver, library, etc) may support multiple build 'variants' - for
example, a component may optionally generate diagnostic data to aid debugging.

Components supporting multiple variants use the BUILD_COMPONENT_VARIANT environment 
variable to select which variant to be built. This should default to the most-
common variant (typically the component's unit test). The BUILD_COMPONENT_VARIANT
name is appended to the BUILD_OUTPUT_DIR to give a unique output directory for 
each variant (allowing them all to co-exist). 

Note that not all variants will support every BUILD_CONFIG option. The component's 
makefile should provide a guard to check that the selected variant is supported 
for the current BUILD_CONFIG (set SUPPORTED_BUILD_CONFIGS with the component's
makefile dependent on the BUILD_COMPONENT_VARIANT).

An example is the Init component, which supports the 'c_library_test', and 
'barebones_test' variants. To build the c_library_test variant:

    % make -r BUILD_CONFIG=semihosted_armcc BUILD_COMPONENT_VARIANT=c_library_test -C src/Init/test clean all

#-------------------------------------------------------------------------------
#
# Using the Build System
#
#-------------------------------------------------------------------------------

The build system supports a number of build targets and options, some of which
depend on the build configuration (BUILD_CONFIG setting). For any chosen 
configuration, the 'help' build target will list the available targets. E.g.:

    % make -r BUILD_CONFIG=semihosted_armcc -C src/Init/test help

The available targets (at the time of writing) are:

- clean:            deletes all compiler and linker artefacts (e.g. *.o, *.axf)
- bare:             deletes all artefacts

- all:              compiles the component's source files and generates the 
                    indicated artefact(s)
                    
- test:             (MinGW32-only) executes the component's unit-test. The TEST_FLAGS 
                    environment variable can be specified when invoking make. For
                    example:
                    
                      make -r BUILD_CONFIG=cpputest_mingw32 test TEST_FLAGS="-v"
      
                    will generate verbose output when running the unit-tests.
                    
- gcov:             (MinGW32-only) runs the GCOV coverage analyser, producing summary results.
- gcov_verbose:     (MinGW32-only) runs the GCOV coverage analyser, producing per-function results.
- gcov_reset:       (MinGW32-only) resets the GCOV run results database (*.gcda)
- gcov_clean:       (MinGW32-only) deletes all GCOV-generated artefacts

- docs:             builds the (public) Doxygen documentation artefacts
- docs_clean:       deletes all Doxygen artefacts

- debug:            dumps internal configuration and control variables
- help:             displays available make targets and options

The build system does not rely on any of the compiler toolchains being in the
system PATH variable. However, when building and analysing the code it is helpful
to have then in the PATH, so that other toolchain utilities can be used (for 
example 'objdump'). 

For Windows the 'setenv.bat' file at the top-level will setup your PATH to include 
all the toolchains. For example:

    w:\trunk>setenv
    Adding UNXUTILS to your PATH environment variable
    Adding DOXYGEN to your PATH environment variable
    Adding MinGW32 to your PATH environment variable
    Adding ARM_TOOLS environment variable
    Adding ARM_TOOLS to your PATH environment variable
    Adding ARM-GNU to your PATH environment variable

For Linux use the following:

    % bs bash
    % shellConfigDev DS5 5.27.1
    % shellConfigDev codebench 2017.02

Note the "bs bash" is necessary to run an interactive shell on the farm, otherwise you 
won’t get licenses!

You invoke make to build a component. For example:

    % make -r BUILD_CONFIG=semihosted_armcc -C src/Init/test clean all
    make: Entering directory '/proj/AR0820/workareas/jtmzyj/svn/trunk/src/Init/test'
    clean...
    Assembling src/Init/output/armcc/c_library_test/AR0820_Startup.o...
    Assembling src/Init/output/armcc/c_library_test/AR0820_SetupHeapStack.o...
    Compiling src/Init/output/armcc/c_library_test/Init_c_library_test.o...
    Compiling src/Init/output/armcc/c_library_test/AR0820_SystemInit.o...
    Compiling src/Init/output/armcc/c_library_test/AR0820_CoreInit.o...
    Linking src/Init/output/armcc/c_library_test/Init.axf...
    make: Leaving directory '/proj/AR0820/workareas/jtmzyj/svn/trunk/src/Init/test'

will first clean, then build the Init component's c_library_test.

For CppUTest unit test executables (under Windows), the following will build then run
the unit test, then run the Gcov coverage analyser:

    w:\trunk>make -r BUILD_CONFIG=cpputest_mingw32 -C src/Example/test clean all test gcov
    make: Entering directory `w:/trunk/src/Example/test'
    clean...
    Compiling src/Example/output/mingw32/Example.o...
    Compiling src/Example/output/mingw32/example_unit_test.o...
    Compiling src/Example/output/mingw32/cpputest_main.o...
    Linking src/Example/output/mingw32/Example.exe...
    ..
    OK (2 tests, 2 ran, 2 checks, 0 ignored, 0 filtered out, 0 ms)
    
    Running GCOV...
    File 'src/Example/src/Example.c'
    Lines executed:100.00% of 4
    No branches
    No calls
    Creating 'Example.c.gcov'

    make: Leaving directory `w:/trunk/src/Example/test'

#-------------------------------------------------------------------------------
#
# Example component makefile (Example_unit_test.mk)
#
#-------------------------------------------------------------------------------

BUILD_COMPONENT	= Example
BUILD_OUTPUT_DIR= src/Example/output/$(BUILD_TOOLCHAIN)
BUILD_ARTEFACT	= EXE

INC_PATHS       += -Iinc -Isrc/Example/inc

CFLAGS			+= $(INC_PATHS)
CPPFLAGS		+= $(INC_PATHS) -Itest/inc

C_SRC			+= Example.c
CPP_SRC			+= example_unit_test.cpp

VPATH			+= src/Example/src src/Example/test

help::
	$(QUIET) echo Builds the unit tests for: $(BUILD_COMPONENT)

# we're building a CppUTest unit test
include cpputest.mk

# include the configuration makefile
include config.mk

#-------------------------------------------------------------------------------
