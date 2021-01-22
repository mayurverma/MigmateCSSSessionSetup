####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Camera SelTest host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper

from host_command import HostCommandParams, HostCommandResponse
from verification_app_debug_id import DebugId

class HostCommandSelfTestParams(HostCommandParams):
    """Host command ExecuteDebug parameters."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):

        members = struct.pack("<L{}H".format(self.__packetSizeWords),
            self.__packetSizeWords,
            *self.__packet
            )
        return members

    @property
    def packet(self):
        return self.__packet

    @packet.setter
    def packet(self, value):
        self.__packetSizeWords = len(value)
        self.__packet = value
        print("Packet size: {}, Contents:[{}]".format(self.__packetSizeWords, value))

class HostCommandSelfTestResponse(HostCommandResponse):
    """Host command ExecuteDebug response."""

    def __init__(self):
        super().__init__('<L')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__result = members[0]

    @property
    def result(self):
        return self.__result

    @property
    def data(self):
        return self.__data

class HostCommandSelfTest():
    """ ExecuteDebug execution helper."""

    def __init__(self, camera):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__host_command_code = HostCommandCode.SELF_TEST
        self.__command_helper = CommandHelper(camera)

    def execute(self, packet = [], expected = Error.SUCCESS, timeout_usec = 500000, interval_usec = 100, verbosity = logging.INFO):

        """Issue the command and return response"""
        params = HostCommandSelfTestParams()
        params.packet = packet
        response = HostCommandSelfTestResponse()
        self.__logger.info('Sending command: SELF_TEST:')
        result = self.__command_helper.execute(self.__host_command_code, params, response, timeout_usec = timeout_usec, interval_usec = interval_usec, verbosity = verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response')

        return response

