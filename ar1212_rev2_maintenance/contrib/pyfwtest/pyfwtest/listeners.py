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
# Firmware test framework listener interface classes
####################################################################################

class ITestRunnerListener(object):
    """An interface class for classes that listen for test runner notifications."""

    def startTestRun(self):
        """Notification handler for the start of a test run."""
        raise NotImplementedError()

    def stopTestRun(self):
        """Notification handler for the end of a test run."""
        raise NotImplementedError()


class ITestResultListener(object):
    """A base class for classes that listen for test result notifications."""

    def addError(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info().
        """
        pass

    def addExpectedFailure(self, test, err):
        """Called when an expected failure/error occured."""
        pass

    def addFailure(self, test, err):
        """Called when an error has occurred.
        'err' is a tuple of values as returned by sys.exc_info()."""
        pass

    def addSkip(self, test, reason):
        """Called when a test is skipped."""
        pass

    def addSuccess(self, test):
        """Called when a test has completed successfully"""
        pass

    def addUnexpectedSuccess(self, test):
        """Called when a test was expected to fail, but succeed."""
        pass

    def startTest(self, test):
        """Called when the given test is about to be run"""
        pass

    def startTestRun(self, epoch, fixture, suite):
        """Called once before any tests are executed."""
        pass

    def stop(self):
        """Indicates that the tests should be aborted"""
        pass

    def stopTest(self, test):
        """Called when the given test has been run"""
        pass

    def stopTestRun(self, epoch, result):
        """Called once after all tests are executed."""
        pass

