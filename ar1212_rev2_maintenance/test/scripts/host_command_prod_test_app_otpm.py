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
# OtpmBlankCheck and OtpmVerify host command parameters and response
####################################################################################
import struct

from host_command import HostCommandParams, HostCommandResponse

class HostCommandOtpmParams(HostCommandParams):
    """Parent of OtpmBlankCheckParams
             and OtpmVerifyParams."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):
        members = struct.pack('<H', self.__mode.value)
        return members

    @property
    def mode(self):
        return self.__mode

    @mode.setter
    def mode(self, value):
        self.__mode = value


class HostCommandOtpmResponse(HostCommandResponse):
    """Parent of OtpmBlankCheckResponse
             and OtpmVerifyResponse."""

    def __init__(self):
        _MESSAGE_BUFFER_SIZE_LONGWORDS = 1024
        super().__init__('<{}H'.format(str( 2*(_MESSAGE_BUFFER_SIZE_LONGWORDS))))

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__blockLengthWords = members[0]
        self.__checksum = members[1]
        self.__dataBlock = []
        for index in range(2, (self.blockLengthWords+2)):
            self.__dataBlock.append( members[index] )

    @property
    def blockLengthWords(self):
        return self.__blockLengthWords

    @property
    def checksum(self):
        return self.__checksum

    @property
    def dataBlock(self):
        return self.__dataBlock


class HostCommandOtpmBlankCheckParams(HostCommandOtpmParams):
    """Host command OtpmBlankCheck parameters."""
    pass

class HostCommandOtpmBlankCheckResponse(HostCommandOtpmResponse):
    """Host command OtpmBlankCheck response."""
    pass

class HostCommandOtpmVerifyParams(HostCommandOtpmParams):
    """Host command OtpmVerify parameters."""
    pass

class HostCommandOtpmVerifyResponse(HostCommandOtpmResponse):
    """Host command OtpmVerify response."""
    pass

