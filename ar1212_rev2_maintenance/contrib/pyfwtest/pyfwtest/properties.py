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
# Firmware test framework properties
####################################################################################

class Properties(dict):
    """Class that defines a write-once set of properties."""

    # The property keys understood by the test framework.
    # PROJECT_ROOT_DIR          : The top of the project directory tree (usually the root of the subversion working copy)
    # PROJECT_OUTPUT_DIR        : The project output directory (where test runner will create any files)
    # PROJECT_TEST_DIR          : The project test directory (where test cases reside)
    # FIXTURE_CLASSPATH         : The fully qualified test fixture class name
    # RUNNER_MODE_STOP_ON_ERROR : Defines whether the runner will stop on encountering the first error
    # RUNNER_MODE_DESCRIPTIONS  : Defines the level of test descriptions that are logged
    # RUNNER_MODE_VERBOSITY     : Defines how verbose the test runner is whilst executing
    # SUITE_FILTER_EXCLUDES     : List of test suite pre-requisites that will skip test cases
    # SUITE_FILTER_INCLUDES     : List of test suite pre-requisites that will execute test cases
    # SUITE_LOADER_DEBUG        : Suite loader debug level 0: off, 1: module reloading, 2: skipped modules, 3: dependency graph

  
    def __getattr__(self, name):
        if name in self:
            return self[name]
        else:
            raise AttributeError('No property named : {}'.format(name))
    
    def __setattr__(self, name, value):
        if name in self:
            # Make the dictionary write-once
            raise AttributeError('Duplicate properties are not allowed')
        else:
            # Otherwise insert the property in the dictionary
            self[name] = value
    
    def __delattr__(self, name):
        # Make the dictionary read-only
        raise AttributeError('Deleting properties is not allowed')
    
    def __iter__(self):
        for value in self.values():
            yield value
        

