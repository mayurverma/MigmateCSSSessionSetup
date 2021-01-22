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
# Camera GetSensorId host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from host_command import HostCommandParams, HostCommandResponse

class HostCommandGetSensorIdParams(HostCommandParams):
    """Host command GetSensorId parameters."""
    pass # No parameters

class HostCommandGetSensorIdResponse(HostCommandResponse):
    """Host command GetSensorId response."""

    def __init__(self):
        # Returns the 32-byte uniqueId
        super().__init__('<BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__unique_id = members

    @property
    def unique_id(self):
        return self.__unique_id

class HostCommandGetSensorId():
    """Get status helper class."""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)

    def execute(self, expected = Error.SUCCESS, timeout_usec=500000000, interval_usec=100, verbosity=logging.INFO):
        """ Issue GetSensorId command """

        params = HostCommandGetSensorIdParams()
        response = HostCommandGetSensorIdResponse()
        self.__logger.info('Sending command: GET_SENSOR_ID')
        result = self.__command_helper.execute(HostCommandCode.GET_SENSOR_ID, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        elif Error.SUCCESS == result:
            # Log response
            self.__logger.info('Unique ID = {}'.format(str(response.unique_id)))

        self.__logger.info('Received response')

        return response
