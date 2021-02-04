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
# Camera SetEthemeralMasterSecret host command
####################################################################################
import struct

from host_command import HostCommandParams, HostCommandResponse

class HostCommandSetEthemeralMasterSecretParams(HostCommandParams):
    """Host command SetEthemeralMasterSecret parameters."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):

        members = struct.pack("<HH{}s".format(len(self.__masterSecret)),
                self.__masterSecretKeyType,
                0,
                self.__masterSecret)
        return members

    @property
    def masterSecretKeyType(self):
        return self.__masterSecretKeyType

    @masterSecretKeyType.setter
    def masterSecretKeyType(self, value):
        self.__masterSecretKeyType = value

    @property
    def masterSecret(self):
        return self.__masterSecret

    @masterSecret.setter
    def masterSecret(self, value):
        self.__masterSecret = value

class HostCommandSetEthemeralMasterSecretResponse(HostCommandResponse):
    """Host command SetEthemeralMasterSecret response."""
    pass
