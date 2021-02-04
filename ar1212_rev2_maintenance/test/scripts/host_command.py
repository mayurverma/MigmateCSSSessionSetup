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
# Camera host command base class for firmware tests
####################################################################################
import struct

class HostCommandParams():
    """Host command parameters base class."""
    
    def __init__(self):
        pass

    def pack(self):
        """Packs the host command into a bytes array for transport."""
        return bytes()

class HostCommandResponse():
    """Host command response base class."""
    
    def __init__(self, format=''):
        """Object constructor
               format - Struct unpack format string e.g. '<H'
        """
        self._format = format

    def size_bytes(self):
        """Returns the length of the expected response."""
        return struct.calcsize(self._format)

    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        raise NotImplementedError('Not implemented by derived class')
