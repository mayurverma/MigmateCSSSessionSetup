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
# Camera EnableMonitoring host command
####################################################################################
import logging
import struct

from enum import Enum
from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper

from host_command import HostCommandParams, HostCommandResponse

class HostCommandEnableMonitoringParams(HostCommandParams):
    """Host command Enable Monitoring parameters."""

    def __init__(self):
        self.__encryptedParams = None
        pass

    # overridden method
    def pack(self):

        # encrypted params
        members = struct.pack("8s16s",
                self.__encryptedParams,
                self.__mac)

        return members

    @property
    def encryptedParams(self):
        return self.__encryptedParams

    @encryptedParams.setter
    def encryptedParams(self, value):
        self.__encryptedParams = value

    @property
    def mac(self):
        return self.__mac

    @mac.setter
    def mac(self, value):
        self.__mac = value


class HostCommandEnableMonitoringResponse(HostCommandResponse):
    """Host command Enable Monitoring response."""
    pass

class HostCommandEnableMonitoring():
    """Enable Monitoring command class."""

    def __init__(self, session):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__session = session
        self.__host_command_code = HostCommandCode.ENABLE_MONITORING

    def execute(self,
                  extClockHz,
                  pllClockHz,
                  expected = Error.SUCCESS, timeout_usec=500000, interval_usec=100, verbosity=logging.INFO):

        # Issue EnableMonitoring command
        enableMonitoringParams = struct.pack("<LL",
                             extClockHz,
                             pllClockHz,
                            )


        params = HostCommandEnableMonitoringParams()
        response = HostCommandEnableMonitoringResponse()

        params.encryptedParams = enableMonitoringParams

        self.__session.encrypt_and_execute_command(self.__host_command_code, params, response,
                                                   expected = expected, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)

        return response



