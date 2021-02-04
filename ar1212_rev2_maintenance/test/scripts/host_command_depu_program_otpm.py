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
# Camera DEPU_PROGRAM_OTPM host command
####################################################################################
import struct

from host_command import HostCommandParams, HostCommandResponse

class HostCommandDepuProgramOtpmParams(HostCommandParams):
    """Host command SdpuProgramOtpm parameters."""

    def __init__(self):
        self.verbose = False

    # overridden method
    def pack(self):
        members = struct.pack("<LHH",
                self.__clockSpeedHz,
                self.__blockLengthLongWords,
                self.__checksum)
        members += self.__dataBlob

        if self.verbose: print('packed members', members)
        return members

    @property
    def clockSpeedHz(self):
        return self.__clockSpeedHz

    @clockSpeedHz.setter
    def clockSpeedHz(self, value):
        self.__clockSpeedHz = value

    @property
    def blockLengthLongWords(self):
        return self.__blockLengthLongWords

    @blockLengthLongWords.setter
    def blockLengthLongWords(self, value):
        self.__blockLengthLongWords = value

    @property
    def checksum(self):
        return self.__checksum

    @checksum.setter
    def checksum(self, value):
        self.__checksum = value

    @property
    def dataBlob(self):
        return self.__dataBlob

    @dataBlob.setter
    def dataBlob(self, value):
        self.__dataBlob = value

class HostCommandDepuProgramOtpmResponse(HostCommandResponse):
    """Host command SdpuProgramOtpm response."""

    def __init__(self):
        # Returns the 2 16-bit responses
        super().__init__('<HH')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__error_address = members[0]
        self.__number_soft_errors = members[1]

    @property
    def error_address(self):
        return self.__error_address

    @property
    def number_soft_errors(self):
        return self.__number_soft_errors


