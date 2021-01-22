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
# Firmware test framework test case base class
####################################################################################

import logging
import sys
import unittest

__all__ = ['TestCase']

class TestCase(unittest.TestCase):
    """Common base class for FW test cases"""
    
    def __init__(self, methodName):
        super().__init__(methodName)
        self.__logger = logging.getLogger(self.__class__.__name__)

    # Returns the name of the test for the logs
    def __str__(self):
        testIdentifier = self.getTestIdentifier()
        if testIdentifier is not None:
            return 'Test {}: {}.{}.{}'.format(testIdentifier, self.__class__.__module__,
                self.__class__.__name__, self._testMethodName)
        else:
            return '{}.{}.{}'.format(self.__class__.__module__,
                self.__class__.__name__, self._testMethodName)

    # Returns the test description
    def getTestDescription(self):
        testIdentifier = self.getTestIdentifier()
        if testIdentifier is None:
            return '{}'.format(self.getTestSpecifier())
        else:
            return 'Test {}: {}'.format(testIdentifier, self.getTestSpecifier())

    # Returns the list of test case group labels
    def getTestGroups(self):
        return getattr(getattr(self, self._testMethodName), '__pyfwtest_groups__', [])

    # Returns the test case identifier
    def getTestIdentifier(self):
        return getattr(getattr(self, self._testMethodName), '__pyfwtest_identifier__', None)

    # Returns the list of test case group labels
    @classmethod
    def getTestMethodGroups(cls, methodName):
        return getattr(getattr(cls, methodName), '__pyfwtest_groups__', [])

    # Returns the test method name
    def getTestMethodName(self):
        return self._testMethodName

    # Returns the test specifier string
    def getTestSpecifier(self):
        return '{}.{}'.format(self.__class__.__name__, self._testMethodName)

    # override method
    def shortDescription(self):
        """Returns a one-line description of the test, or None if no
        description has been provided.

        The default implementation of this method returns the first line of
        the specified test method's docstring.
        """
        brief = super().shortDescription()
        testSpecifier = self.getTestSpecifier()
        testIdentifier = self.getTestIdentifier()
        if testIdentifier is None:
            return 'Test {} {}'.format(testSpecifier, brief)
        else:
            return 'Test {} {} {}'.format(testIdentifier, testSpecifier, brief)

    @property
    def logger(self):
        return self.__logger  

    @property
    def fixture(self):
        # Class variable that gets injected by the runner at startTestRun time
        return self.__pyfwtest_fixture__

