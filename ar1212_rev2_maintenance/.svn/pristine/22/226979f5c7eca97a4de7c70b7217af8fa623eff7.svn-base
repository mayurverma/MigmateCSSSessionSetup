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
# Camera SetPskSessionKeys host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from host_command import HostCommandParams, HostCommandResponse
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType

class HostCommandSetPskSessionKeysParams(HostCommandParams):
    """Host command SetPskSessionKeys parameters."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):

        members = struct.pack("<HHHH16s",
                self.__videoAuthCipherMode,
                self.__videoAuthKeyType,
                self.__controlChannelCipherMode,
                self.__controlChannelKeyType,
                self.__hostSalt)
        return members

    @property
    def videoAuthCipherMode(self):
        return self.__videoAuthCipherMode

    @videoAuthCipherMode.setter
    def videoAuthCipherMode(self, value):
        self.__videoAuthCipherMode = value

    @property
    def videoAuthKeyType(self):
        return self.__videoAuthKeyType

    @videoAuthKeyType.setter
    def videoAuthKeyType(self, value):
        self.__videoAuthKeyType = value

    @property
    def controlChannelCipherMode(self):
        return self.__controlChannelCipherMode

    @controlChannelCipherMode.setter
    def controlChannelCipherMode(self, value):
        self.__controlChannelCipherMode = value

    @property
    def controlChannelKeyType(self):
        return self.__controlChannelKeyType

    @controlChannelKeyType.setter
    def controlChannelKeyType(self, value):
        self.__controlChannelKeyType = value

    @property
    def hostSalt(self):
        return self.__hostSalt

    @hostSalt.setter
    def hostSalt(self, value):
        self.__hostSalt = value

class HostCommandSetPskSessionKeysResponse(HostCommandResponse):
    """Host command SetPskSessionKeys response."""

    def __init__(self):
        # Returns the 32-byte uniqueId
        super().__init__('<16s12s12s')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__sensorSalt = list(members[0])
        self.__videoAuthGMACIV = list(members[1])
        self.__controlChannelNonce = list(members[2])

    @property
    def sensorSalt(self):
        return self.__sensorSalt

    @property
    def videoAuthGMACIV(self):
        return self.__videoAuthGMACIV

    @property
    def controlChannelNonce(self):
        return self.__controlChannelNonce

class HostCommandSetPskSessionKeys():
    """Command execution class"""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)

    def execute(self,
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC, videoAuthKeyType = SymmetricKeyType.AES_128,
            controlChannelCipherMode = SymmetricAECipherMode.AES_CCM, controlChannelKeyType = SymmetricKeyType.AES_128,
            hostSalt = bytes([0]*16),
            expected = Error.SUCCESS, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """Issue the GetStatus command and return response"""

        params = HostCommandSetPskSessionKeysParams()
        response = HostCommandSetPskSessionKeysResponse()

        params.videoAuthCipherMode = videoAuthCipherMode.value
        params.videoAuthKeyType = videoAuthKeyType.value
        params.controlChannelCipherMode = controlChannelCipherMode.value
        params.controlChannelKeyType = controlChannelKeyType.value
        params.hostSalt = hostSalt

        self.__logger.info('Sending command: SET_PSK_SESSION_KEYS')
        result = self.__command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response: {}'.format(expected.name))

        return response
