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
# Firmware test framework test result notifier class
####################################################################################

import logging
import unittest

from .listeners import ITestResultListener

class TestResultNotifier(unittest.TestResult):
    """A test result class which sends notifications to a set of test reporters.

    The class is a custom TestResult class which listens to the test run and collects
    a set of results but also notifies an optional set of test reporter listeners
    thereby allowing more than one type of test report to be generated from a test
    run e.g. text, HTML, XML, email, etc.
    """

    def __init__(self, stopOnError=False, listeners=None):
        super().__init__()
        self._logger = logging.getLogger(self.__class__.__name__)
        self._successes = []
        self._stopOnError = stopOnError
        self._listeners = []
        if listeners is not None:
            if isinstance(listeners, (tuple, list)):
                self.addListeners(listeners)
            else:
                self.addListener(listeners)

    def _notifyListeners(self, handlerName, *args, **kwds):
        """Notify listeners of an event"""
        for listener in self._listeners:
            if hasattr(listener, handlerName):
                handler = getattr(listener, handlerName)
                handler(*args, **kwds)

    # override method
    def addError(self, test, err):
        """Called when an error has occurred. 'err' is a tuple of values as
        returned by sys.exc_info().
        """
        self._logger.debug('addError')
        super().addError(test, err)
        self._notifyListeners('addError', test, err)

    # override method
    def addExpectedFailure(self, test, err):
        """Called when an expected failure/error occurred."""
        self._logger.debug('addExpectedFailure')
        super().addExpectedFailure(test, err)
        self._notifyListeners('addExpectedFailure', test, err)

    # override method
    def addFailure(self, test, err):
        """Called when an error has occurred. 'err' is a tuple of values as
        returned by sys.exc_info()."""
        self._logger.debug('addFailure')
        super().addFailure(test, err)
        self._notifyListeners('addFailure', test, err)

    def addListener(self, listener):
        """Add a listener to the list of result listeners"""
        if not isinstance(listener, ITestResultListener):
            raise TypeError("Illegal test result listener type")
        self._listeners.append(listener)

    def addListeners(self, listeners):
        """Add a set of listeners to the list of result listeners"""
        for listener in listeners:
            self.addListener(listener)

    # override method
    def addSkip(self, test, reason):
        """Called when a test is skipped."""
        self._logger.debug('addSkip')
        super().addSkip(test, reason)
        self._notifyListeners('addSkip', test, reason)

    # override method
    def addSuccess(self, test):
        """Called when a test has completed successfully"""
        self._logger.debug('addSuccess')
        super().addSuccess(test)
        self._successes.append(test)
        self._notifyListeners('addSuccess', test)

    # override method
    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeed."""
        self._logger.debug('addUnexpectedSuccess')
        super().addUnexpectedSuccess(test)
        self._notifyListeners('addUnexpectedSuccess', test)

    def getErrors(self):
        """Returns the errors."""
        return self.errors

    def getExpectedFailures(self):
        """Returns the expected failures."""
        return self.expectedFailures

    def getFailures(self):
        """Returns the failures."""
        return self.failures

    def getSkipped(self):
        """Returns the skipped tests."""
        return self.skipped

    def getSuccesses(self):
        """Returns the successful tests."""
        return self._successes

    def getTestsRun(self):
        """Returns the number of tests executed."""
        return self.testsRun

    def getUnexpectedSuccesses(self):
        """Returns the unexpected successes."""
        return self.unexpectedSuccesses

    # override method
    def startTest(self, test):
        "Called when the given test is about to be run"
        self._logger.info('startTest: ' + str(test))
        super().startTest(test)
        self._notifyListeners('startTest', test)

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed.

        See start_test for a method called before each test.
        """
        self._logger.info('######## START OF TEST RUN ########')
        super().startTestRun()
        self._notifyListeners('startTestRun', epoch, fixture, suite)

    def stop(self):
        """Indicates that the tests should be aborted"""
        self._logger.info('###############  Aborting test run due to error ######################')
        super().stop()
        self._notifyListeners('stop')

    # override method
    def stopTest(self, test):
        """Called when the given test has been run"""
        self._logger.debug('stopTest')
        super().stopTest(test)
        self._notifyListeners('stopTest', test)
        if self._stopOnError and not self.wasSuccessful():
            self.stop()

    def stopTestRun(self, epoch):
        """Called once after all tests are executed.

        See stop_test for a method called after each test.
        """
        self._logger.info('######## END OF TEST RUN ########')
        super().stopTestRun()
        self._notifyListeners('stopTestRun', epoch, self)

    # override method
    def wasSuccessful(self):
        """Tells whether or not this result was a success."""
        return 0 == len(self.failures) and 0 == len(self.errors)

