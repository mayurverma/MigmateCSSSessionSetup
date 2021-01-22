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
# Firmware test framework test case decorators
####################################################################################

from .case import TestCase

__all__ = ['testIdentifier', 'testGroups']

# Decorator that adds a test identifier string
def testIdentifier(identifier):
    def decorator(testMethod):
        testMethod.__pyfwtest_identifier__ = identifier
        return testMethod

    return decorator

# Decorator that adds a list of test group label(s)
def testGroups(*groups):
    def decorator(testMethod):
        testMethod.__pyfwtest_groups__ = tuple(groups)
        return testMethod

    return decorator

# Decorator that adds the writeln method to stream objects
class WritelnDecorator(object):
    def __init__(self,stream):
        self._stream = stream

    def __getattr__(self, attr):
        return getattr(self._stream,attr)

    def writeln(self, arg=None):
        if arg:
            self.write(arg)
        self.write('\n') # text-mode streams translate to \r\n if needed

