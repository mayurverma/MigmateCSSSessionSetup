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
# Firmware test framework utilities
####################################################################################

class CompareTestIds(object):
    """Key class that compares two test identifiers using numerical field comparison."""
    def __init__(self, obj, *args):
        self.obj = obj

    def __compare(self, other):
        aElements = self.obj.split('.')
        bElements = other.obj.split('.')
        aLength = len(aElements)
        bLength = len(bElements)
        numElems = min(aLength, bLength)
        for index in range(numElems):
            aValue = int(aElements[index])
            bValue = int(bElements[index])
            if aValue > bValue:
                return 1
            elif aValue < bValue:
                return -1
        if aLength > bLength:
            return 1
        elif aLength < bLength:
            return -1
        return 0

    def __lt__(self, other):
        return self.__compare(other) < 0

    def __gt__(self, other):
        return self.__compare(other) > 0

    def __eq__(self, other):
        return self.__compare(other) == 0

    def __le__(self, other):
        return self.__compare(other) <= 0

    def __ge__(self, other):
        return self.__compare(other) >= 0

    def __ne__(self, other):
        return self.__compare(other) != 0

