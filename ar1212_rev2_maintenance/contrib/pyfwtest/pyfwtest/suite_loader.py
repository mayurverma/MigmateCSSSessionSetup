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
# Firmware test framework test suite loader class
####################################################################################

import importlib
import logging
import os
import re
import sys
import types
import unittest

from .decorators import testIdentifier, testGroups
from .properties import Properties
from collections import OrderedDict

class TestSuiteLoader(object):
    """Test Suite Loader class"""

    def __init__(self, properties, doNotReload=None):
        self._logger = logging.getLogger(self.__class__.__name__)

        self._testdir = properties.get('PROJECT_TEST_DIR', 'test')
        assert self._testdir is not None and os.path.exists(self._testdir)

        if 'SUITE_FILTER_EXCLUDES' in properties:
            self._filterExcludes = properties['SUITE_FILTER_EXCLUDES']
            assert isinstance(self._filterExcludes, (tuple, list))
        else:
            self._filterExcludes = None

        if 'SUITE_FILTER_INCLUDES' in properties:
            self._filterIncludes = properties['SUITE_FILTER_INCLUDES']
            assert isinstance(self._filterIncludes, (tuple, list))
        else:
            self._filterIncludes = None

        # 0: off, 1: module reloading, 2: skipped modules, 3: dependency graph
        self._debug = properties.get('SUITE_LOADER_DEBUG', 0)

        self._doNotReload = []
        if doNotReload is not None:
            if isinstance(doNotReload, (tuple, list)):
                self._doNotReload.extend(doNotReload)
            else:
                self._doNotReload.append(doNotReload)

    def _addModuleDependency(self, dependencies, child, parent):
        """Adds a module dependency, child depends on parent"""
        if child in dependencies:
            if parent not in dependencies[child]:
                dependencies[child].append(parent)
        else:
            dependencies[child] = [parent]

    def _createTestSuite(self):
        """Factory method to allow derived classes to create their own test suite."""
        return unittest.TestSuite()

    def _findModuleDependencies(self, dependencies, moduleNames):
        """Finds the module dependencies for currently loaded set of user modules"""

        for moduleName in moduleNames:
            # skip modules that have already been examined
            if moduleName in dependencies:
                continue

            # skip modules that aren't already loaded
            if moduleName not in sys.modules:
                continue

            # skip builtins, python libraries and tools libraries
            if not self._isTestModule(moduleName):
                continue

            for attr in sys.modules[moduleName].__dict__.values():
                # if attribute is an imported module
                if isinstance(attr, types.ModuleType):
                    importedName = attr.__name__
                    if self._isTestModule(importedName):
                        # if module is sub-module
                        if importedName.startswith(moduleName):
                            self._addModuleDependency(dependencies, importedName, moduleName)
                        else:
                            self._addModuleDependency(dependencies, moduleName, importedName)
                # if attribute is an imported class but not a builtin class
                if isinstance(attr, type) and hasattr(attr, '__module__'):
                    importedName = attr.__module__
                    # ignore classes in this module
                    if importedName != moduleName and self._isTestModule(importedName):
                        self._addModuleDependency(dependencies, moduleName, importedName)

            # if dependencies were found for this module
            if moduleName in dependencies:
                # recurse through module parents to check for further dependencies
                parents = dependencies[moduleName]
                if parents is not None:
                    self._findModuleDependencies(dependencies, parents)
            else:
                # mark this module has having no dependencies
                dependencies[moduleName] = None

        return dependencies

    def _getTests(self, classes):
        """ Get a list of the tests in the given classes.  If the classes parameter
            is defined then add any tests for those classes to the list. """

        testList = []
        for testClass in classes:
            self._logger.debug("Finding tests from class '{}'".format(testClass.__name__))
            class_tests = []

            # We'll add tests using the traditional method of stepping round all the methods
            # looking for ones that start test*, as long as they didn't have an
            # entry in testDescriptions
            for attribute in dir(testClass):
                testObject = getattr(testClass, attribute)
                if hasattr(testObject, "__call__") :
                    if re.match('test', attribute):
                        class_tests.append({'class':testClass, 'method':attribute})

            # Go through the list of tests, and generate specifiers for all the tests that
            # don't already have them.
            for test in class_tests:
                className = test['class'].__name__
                method_name = test['method']
                self._logger.debug("Added possible test '{}.{}'".format(className, method_name))
                if not hasattr(test, "specifier"):
                    specifier = "{}.{}".format(className, method_name)
                    test['specifier'] = specifier

            testList.extend(class_tests)

        return testList

    def _importModule(self, moduleName):
        """Import the requested module"""

        # check its not already been imported
        if moduleName not in sys.modules:
            importlib.import_module(moduleName)

        return sys.modules[moduleName]

    def _isTestModule(self, moduleName):
        """Returns true if the given module is a test module."""
        moduleObject = sys.modules[moduleName]
        # builtins do not have a file attribute
        # check module file is in the test directory
        return hasattr(moduleObject, '__file__') and moduleObject.__file__.startswith(self._testdir)

    def _loadClasses(self, classNames):
        """Load class references from list of class names"""
        #Use an ordered dictionary in case some order of tests has been imposed at project level e.g. via unittest.ini
        classTree = OrderedDict()
        # determine the class tree and the set of modules to reload
        for classPath in classNames:
            moduleName, className = classPath.rsplit('.', 1)
            if moduleName in classTree:
                classTree[moduleName].append(className)
            else:
                classTree[moduleName] = [className]

        # reload the required modules and their dependencies
        self._logger.debug('Reloading modules')
        self._reloadModules(classTree.keys())

        # import any remaining classes and generate the class list
        self._logger.debug('Importing classes')
        classes = []
        for moduleName, classNames in classTree.items():
            moduleObject = self._importModule(moduleName)
            for className in classNames:
                theClass = getattr(moduleObject, className)
                classes.append(theClass)

        return classes

    def _reloadModule(self, moduleName):
        """Reloads the given module"""
        if self._debug >= 1:
            self._logger.debug('Reloading {}'.format(moduleName))
        importlib.reload(sys.modules[moduleName])

    def _reloadModules(self, moduleNames):
        """Reloads the currently loaded set of user modules using dependency ordering"""

        # determine the dependency graph
        dependencies = {}
        self._findModuleDependencies(dependencies, moduleNames)

        self._showDependencies(dependencies)

        # first skip non-reloadable modules and their dependencies
        skipped = {}
        for moduleName in dependencies.keys():
            # some classes must not be reloaded
            if moduleName in self._doNotReload:
                self._skipModule(skipped, dependencies, moduleName)
        for child in skipped.keys():
            del dependencies[child]

        self._showDependencies(dependencies)

        # then reload modules that have no dependencies
        reloaded = {}
        for child in dependencies.keys():
            if dependencies[child] is None:
                self._reloadModule(child)
                reloaded[child] = True
        for child in reloaded.keys():
            del dependencies[child]

        # now walk the dependency graph to reload dependent modules in the correct order
        while len(dependencies.keys()) > 0:
            self._showDependencies(dependencies)
            # taking a sorted copy of the keys allows for more efficient reloading
            # and for deletion of dictionary keys during iteration
            for child in sorted(dependencies.keys()):
                for parent in dependencies[child]:
                    if parent not in reloaded:
                        if parent in dependencies:
                            break
                        elif parent not in skipped:
                            self._reloadModule(parent)
                            reloaded[parent] = True
                else:
                    self._reloadModule(child)
                    reloaded[child] = True
                    del dependencies[child]

    def _skipModule(self, skipped, dependencies, moduleName):
        """Removes the given module and its dependencies from the dependency list"""
        if moduleName not in skipped:
            if self._debug >= 2:
                self._logger.debug('Skipping {}'.format(moduleName))
            skipped[moduleName] = True
            parents = dependencies[moduleName]
            if parents is not None:
                for parent in parents:
                    self._skipModule(skipped, dependencies, parent)

    def _showDependencies(self, dependencies):
        if self._debug >= 3:
            self._logger.debug('==========================================================================================')
            for moduleName in sorted(dependencies.keys()):
                self._logger.debug('Dependency {} => {}'.format(moduleName, dependencies[moduleName]))

    def load(self, tests, classes):
        """ Run tests in this class. 'tests' is the test specifier, or list of test specifiers,
            or None for all tests.  In the latter case, the tests are determined by looking for
            methods that start "test", which are individual test cases."""

        if classes is None:
            classes = []

        if self._filterExcludes is not None:
            excludes = self._filterExcludes
        else:
            excludes = []

        if self._filterIncludes is not None:
            includes = self._filterIncludes
        else:
            includes = []

        # reload modules and translate class names into class references
        classList = self._loadClasses(classes)

        self._logger.debug("Generating Test Configuration")

        # If we're given a list of tests to run, these are either "specifiers" (ie class.name)
        # or just simple names, in which case we prepend the name of the current class
        # to make a specifier.
        if tests is not None:
            if type(tests) is str:
                tests = [tests]     # Make a single test name into a list

            newTests = []
            for test in tests:
                if re.search(r'\.', test):
                    newTests.append(test)
                else:
                    newTests.append('{}.{}'.format(classList[0].__name__,test))
            tests = newTests

        # Get the list of defined tests
        definedTests = self._getTests(classList)

        # Generate a list of all the specifiers
        specifiers = {}
        for test in definedTests:
            assert "specifier" in test
            specifiers[test['specifier']] = True

        requestedTests = tests

        # We loop round all the defined tests, looking for ones that are in the requested
        # list.  This means that (in theory) one test specifier could match more than one
        # test.  Any specifiers that haven't been used by the end of the test are an
        # error, so keep track of those that have.
        suite = unittest.TestSuite()
        lastClass = None
        if requestedTests is None:
            usedSpecifiers = {}
        else:
            usedSpecifiers = {spec:False for spec in requestedTests}

        for test in definedTests:
            thisClass = test['class']
            testGroups = thisClass.getTestMethodGroups(test['method'])

            # Check whether we need to skip the test.
            if requestedTests and not test['specifier'] in requestedTests:
                # The user requested some tests, and this isn't one of them
                continue
            elif hasattr(thisClass, '__pyfwtest_skip_tags__') and \
                    len([tag for tag in excludes if tag in thisClass.__pyfwtest_skip_tags__])>0:
                # This test is part of a class that we've been told to skip for this run
                continue
            elif len([reason for reason in excludes if reason in testGroups]) > 0:
                # This test has a prerequisite that we've been told to exclude for this run
                continue
            elif len(includes) > 0:
                if len([reason for reason in includes if reason in testGroups]) == len(includes):
                    # We're going to run this test - mark that we've used this specifier
                    usedSpecifiers[test['specifier']] = True
                else:
                    # This test has a prerequisite that was not included for this run
                    continue
            else:
                # We're going to run this test - mark that we've used this specifier
                usedSpecifiers[test['specifier']] = True

            # Before adding the new test, we check whether we've changed class.
            testClass = test['class']
            if testClass != lastClass:
                # Create a TestSuiteWithFixture to handle tests of that class.
                fixtureSuite = self._createTestSuite()
                suite.addTest(fixtureSuite)
                lastClass = testClass

            # Add the new test.
            self._logger.debug("Creating object for specifier {}".format(test['specifier']))
            newTest = testClass(test['method'])
            fixtureSuite.addTest(newTest)

        # Check that we used all the test specifiers
        for specifier, value in usedSpecifiers.items():
            if not value:
                raise Exception("Test '{}' could not be found in {}"
                    .format(specifier, [test['specifier'] for test in definedTests]))

        return suite

