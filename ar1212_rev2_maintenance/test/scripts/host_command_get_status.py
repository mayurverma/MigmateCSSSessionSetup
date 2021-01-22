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
# Camera GetStatus host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from host_command import HostCommandParams, HostCommandResponse
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase

class HostCommandGetStatusParams(HostCommandParams):
    """Host command GetStatus parameters."""
    pass # No parameters

class HostCommandGetStatusResponse(HostCommandResponse):
    """Host command GetStatus response."""

    def __init__(self):
        super().__init__('<HHHHH')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__current_phase = SystemMgrPhase(members[0])
        self.__current_lifecycle_state = LifecycleState(members[1])
        self.__rom_version = members[2]
        self.__patch_version = members[3]
        self.__stack_used_long_words = members[4]

    @property
    def current_phase(self):
        return self.__current_phase

    @property
    def current_lifecycle_state(self):
        return self.__current_lifecycle_state

    @property
    def rom_version(self):
        return self.__rom_version

    @property
    def patch_version(self):
        return self.__patch_version

    @property
    def stack_used_long_words(self):
        return self.__stack_used_long_words

class HostCommandGetStatus():
    """Get status helper class."""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)

    def execute(self, expected = Error.SUCCESS, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """Issue the GetStatus command and return response"""

        params = HostCommandGetStatusParams()
        response = HostCommandGetStatusResponse()
        self.__logger.info('Sending command: GET_STATUS')
        result = self.__command_helper.execute(HostCommandCode.GET_STATUS, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response')

        # Log response
        self.__logger.info('Current Phase         = {}'.format(str(response.current_phase)))
        self.__logger.info('Current LCS           = {}'.format(str(response.current_lifecycle_state)))
        self.__logger.info('Patch Version         = 0x{:04x}'.format(response.patch_version))
        self.__logger.info('ROM version           = {} (0x{:04x})'.format(str(response.rom_version), response.rom_version))
        self.__logger.info('Stack Used Long Words = {}'.format(response.stack_used_long_words))

        return response
