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
# Firmware test framework campaign support
####################################################################################

from .properties import Properties

class TestCampaign(object):
    """Provides the test campaign capability"""

    def __init__(self):
        self._properties = Properties()
        self._tests = {}

    def addTest(self, moduleName, className, methodName):
        """Adds a test to the campaign"""
        if not isinstance(moduleName, str):
            raise TypeError('Invalid module name')
        if not isinstance(className, str):
            raise TypeError('Invalid class name')
        if not isinstance(methodName, str):
            raise TypeError('Invalid method name')
        if moduleName not in self._tests:
            self._tests[moduleName] = {className : [methodName]}
        elif className not in self._tests[moduleName]:
            self._tests[moduleName][className] = [methodName]
        elif methodName in self._tests[moduleName][className]:
            raise ValueError('Duplicate test methods are not allowed')
        else:
            self._tests[moduleName][className].append(methodName)

    def addProperty(self, name, value):
        """Adds a property key/value pair."""
        self._properties[name] = value

    def getClasses(self):
        """Returns a list of classes in the format used by TestRunner."""
        classes = []
        for moduleName in self._tests.keys():
            for className in self._tests[moduleName].keys():
                classes.append(moduleName + '.' + className)
        return classes

    def getTests(self):
        """Returns a list of tests in the format used by TestRunner."""
        tests = []
        for moduleName in self._tests.keys():
            for className in self._tests[moduleName].keys():
                for methodName in self._tests[moduleName][className]:
                    tests.append('{}.{}'.format(className, methodName))
        return tests

    def getProperties(self):
        """Returns the campaign properties."""
        return self._properties

