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
# Camera SetVideoAuthROI host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper

from host_command import HostCommandParams, HostCommandResponse
from verification_app_debug_id import DebugId

class HostCommandExecuteDebugParams(HostCommandParams):
    """Host command ExecuteDebug parameters."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):

        members = struct.pack("<LLL{}H".format(self.__packetSizeWords),
            self.__id, 
            self.__parameter,
            self.__packetSizeWords, 
            *self.__packet
            )
        return members

    @property
    def id(self):
        return self.__id

    @id.setter
    def id(self, id):
        self.__id = id.value

    @property
    def parameter(self):
        return self.__parameter

    @parameter.setter
    def parameter(self, parameter):
        self.__parameter = parameter

    @property
    def packet(self):
        return self.__packet

    @packet.setter
    def packet(self, value):
        self.__packetSizeWords = len(value)
        self.__packet = value
        print("Packet size: {}, Contents:[{}]".format(self.__packetSizeWords, value))

class HostCommandExecuteDebugResponse(HostCommandResponse):
    """Host command ExecuteDebug response."""

    def __init__(self):
        super().__init__('<L5L')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__result = members[0]
        self.__data = members[1:5]

    @property
    def result(self):
        return self.__result

    @property
    def data(self):
        return self.__data

class HostCommandExecuteDebug():
    """ ExecuteDebug execution helper."""

    def __init__(self, camera):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__host_command_code = HostCommandCode.EXECUTE_DEBUG
        self.__command_helper = CommandHelper(camera)

    def execute(self,
                  id = DebugId.NO_ACTION, parameter = 0, packet = [],
                  expected = Error.SUCCESS, timeout_usec=500000, interval_usec=100, verbosity=logging.INFO):

        """Issue the ExecuteDebug command and return response"""

        params = HostCommandExecuteDebugParams()
        params.id = id
        params.parameter = parameter
        params.packet = packet
        response = HostCommandExecuteDebugResponse()
        self.__logger.info('Sending command: EXECUTE_DEBUG: {}'.format(str(id)))
        result = self.__command_helper.execute(HostCommandCode.EXECUTE_DEBUG, params, response, force_get_response=True, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response')

        return response

