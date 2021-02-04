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
# Firmware test framework fixture classes
####################################################################################

import logging

from .listeners import ITestRunnerListener
from .properties import Properties

class TestFixture(ITestRunnerListener):
    """A test fixture comprised of one or more fixture elements."""

    def __init__(self, properties):
        """Object constructor."""
        self._logger = logging.getLogger(self.__class__.__name__)
        self._properties = properties
        self._fixtureElements = []

        # allow derived classes to add fixture elements
        self._createFixtureElements(properties)

    def _createFixtureElements(self, properties):
        """Factory method to allow derived classes to add fixture elements."""
        return

    def addElement(self, element):
        """Send platform information to logger"""
        assert isinstance(element, ITestRunnerListener)
        self._fixtureElements.append(element)

    def getProperties(self):
        """Accessor for the properties."""
        return self._properties

    def startTestRun(self):
        """Notification handler for the start of a test run."""
        self._logger.debug('######## FIXTURE SETUP ########')
        for element in self._fixtureElements:
            element.startTestRun()

    def stopTestRun(self):
        """Notification handler for the end of a test run."""
        self._logger.debug('######## FIXTURE TEARDOWN ########')
        for element in reversed(self._fixtureElements):
            element.stopTestRun()

