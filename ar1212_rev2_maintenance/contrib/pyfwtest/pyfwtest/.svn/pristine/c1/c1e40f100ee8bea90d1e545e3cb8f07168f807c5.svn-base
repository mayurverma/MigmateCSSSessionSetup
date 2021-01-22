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
# Firmware test framework test runner class
####################################################################################

import datetime
import logging
import os
import queue
import sys
import threading
import traceback
import unittest

from .case import TestCase
from .listeners import ITestRunnerListener
from .properties import Properties
from .reporter import HtmlTestSummaryReporter, HtmlTestDetailsReporter, \
    JUnitTestReporter, LogTestReporter, TextTestReporter, XmlTestReporter
from .result import TestResultNotifier
from .suite_loader import TestSuiteLoader

threadIsRunning = False

class TestRunner(object):
    """A custom test runner base class which notifies test reporters."""

    def __init__(self, properties):
        self._rootDirectory = properties.get('PROJECT_ROOT_DIR', '.')
        assert self._rootDirectory is not None and os.path.exists(self._rootDirectory)

        self._outputDirectory = properties.get('PROJECT_OUTPUT_DIR', 'output')
        assert self._outputDirectory is not None and os.path.exists(self._outputDirectory)

        self._stopOnError = properties.get('RUNNER_MODE_STOP_ON_ERROR', False)
        self._descriptions = properties.get('RUNNER_MODE_DESCRIPTIONS', 1)
        self._verbosity = properties.get('RUNNER_MODE_VERBOSITY', 1)

        self._localTimeZone = datetime.datetime.now(datetime.timezone.utc).astimezone().tzinfo

    @classmethod
    def _createFixture(cls, properties):
        """Factory method to allow derived classes to create their own fixture."""
        # extract the classpath
        classpath = properties.get('FIXTURE_CLASSPATH', 'pyfwtest.TestFixture')

        # import the module and get the class attribute
        moduleName, className = classpath.rsplit('.', 1)
        module = __import__(moduleName, globals(), locals(), [className])
        theClass = getattr(module, className)

        # check it is an allowed class
        assert issubclass(theClass, ITestRunnerListener)

        return theClass(properties)

    def _createReporters(self, result, stream=sys.stdout):
        """Factory method to allow derived classes to create their own reporters."""
        # create the JUnit test reporter
        filename = os.path.join(self._outputDirectory, 'junit.xml')
        reporter = JUnitTestReporter(filename)
        result.addListener(reporter)

        # create the XML test reporter
        filename = os.path.join(self._outputDirectory, 'results.xml')
        reporter = XmlTestReporter(filename)
        result.addListener(reporter)

        # create the HTML test summary reporter
        filename = os.path.join(self._outputDirectory, 'summary.html')
        reporter = HtmlTestSummaryReporter(filename)
        result.addListener(reporter)

        # create the HTML test details reporter
        filename = os.path.join(self._outputDirectory, 'details.html')
        reporter = HtmlTestDetailsReporter(filename)
        result.addListener(reporter)

        # create the logger reporter
        reporter = LogTestReporter()
        result.addListener(reporter)

        # create the text reporter (if needed)
        if stream is not None:
            reporter = TextTestReporter(stream, self._descriptions, self._verbosity)
            result.addListener(reporter)

    def _destroyReporters(self):
        """Allow derived classes to destroy their own reporters."""
        pass
        
    def _createResult(self):
        """Factory method to allow derived classes to create their own test result notifier."""
        return TestResultNotifier(self._stopOnError)

    @classmethod
    def _createSuite(cls, tests, classes, properties):
        """Creat the test suite."""
        try:
            return cls._createSuiteLoader(properties).load(tests, classes)
        except Exception as err:
            cls._logException(err, 'Failed to create the test suite')
            raise err

    @classmethod
    def _createSuiteLoader(cls, properties):
        """Factory method to allow derived classes to create their own test suite loader."""
        return TestSuiteLoader(properties)

    @classmethod
    def _logException(cls, err, msg):
        """Sends exceptions to the logger."""
        excType, excValue, excTraceback = sys.exc_info()
        logger = logging.getLogger(cls.__name__)
        logger.warning(msg)
        logger.warning(''.join(traceback.format_exception(excType, excValue, excTraceback)))

    def _execute(self, fixture, suite):
        """Execute the test suite on the given test fixture."""
        # Create the test result
        result = self._createResult()

        try:
            # Create the reporters
            self._createReporters(result)

            # Inject the fixture reference into the test case base class
            TestCase.__pyfwtest_fixture__ = fixture
            
            try:
                # Notify listeners that test run is starting
                epoch = datetime.datetime.now(self._localTimeZone)
                result.startTestRun(epoch, fixture, suite)
            
                try:
                    # Setup the fixture
                    fixture.startTestRun()

                except Exception as err:
                    # Create an error holder object and notify result
                    className = unittest.util.strclass(fixture.__class__)
                    errorName = 'startTestRun (%s)' % className
                    error = unittest.suite._ErrorHolder(errorName)
                    result.addError(error, sys.exc_info())

                else:
                    # Execute the test suite
                    suite(result)

                finally:
                    # Tear down the fixture
                    fixture.stopTestRun()

            finally:
                # Notify listeners that test run has stopped
                epoch = datetime.datetime.now(self._localTimeZone)
                result.stopTestRun(epoch)
        
        finally:
            # Destroy reporter resources
            self._destroyReporters()

        return result

    @classmethod
    def _start(cls, tests, classes, properties):
        """Start to execute the test suite."""
        # Configure the fixture first so that the project can alter its
        # capbalities prior to the suite loader creating the test suite
        fixture = cls._createFixture(properties)
        
        # Attempt to create the test suite (can fail for test case constructor errors)
        suite = cls._createSuite(tests, classes, properties)
        
        # Create the test runner
        runner = cls(properties)

        # Execute the test suite
        return runner._execute(fixture, suite)

    @classmethod
    def run(cls, tests, classes, properties):
        """Convenience method for invoking a test run.

           Run tests in this class. 'tests' is the test, or list of named tests,
           or None for all tests.  In the latter case, the tests are determined
           by looking for methods that start "test", which are individual
           test cases.
        """
        try:
            return cls._start(tests, classes, properties)
        except Exception as err:
            msg = 'Failed to execute the test suite'
            cls._logException(err, msg)
            return None

