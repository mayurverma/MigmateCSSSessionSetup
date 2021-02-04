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
# Camera ApplyDebugEntitlement host command
####################################################################################
import struct
import logging

from common_error import Error
from host_command import HostCommandParams, HostCommandResponse
from command_helper import CommandHelper
from systemmgr_phase import SystemMgrPhase
from host_command_code import HostCommandCode


class HostCommandApplyDebugEntitlementParams(HostCommandParams):
    """Host command ApplyDebugEntitlement parameters."""

    def __init__(self):
        pass
    # overridden method
    def pack(self):

        members = struct.pack("<LL{}s".format(self.__blobSizeWords*2), self.__clockSpeedHz, self.__blobSizeWords, self.__blob)
        return members

    @property
    def clockSpeedHz(self):
        return self.__clockSpeedHz

    @clockSpeedHz.setter
    def clockSpeedHz(self, value):
        self.__clockSpeedHz = value

    @property
    def blob(self):
        return self.__blob

    @blob.setter
    def blob(self, value):
        self.__blobSizeWords = len(value)//2
        self.__blob = value
        #print(self.__blobSizeWords, (bytes(value).hex()))

class HostCommandApplyDebugEntitlementResponse(HostCommandResponse):
    """Host command ApplyDebugEntitlement response."""
    pass # No response

class HostCommandApplyDebugEntitlement():
    """Host command ApplyDebugEntitlement class."""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__host_command_code = HostCommandCode.APPLY_DEBUG_ENTITLEMENT

    def execute(self,
                  debug_entitlement_blob,
                  clock_speed=27000000,
                  expected = Error.SUCCESS, timeout_usec=500000000, interval_usec=100, verbosity=logging.INFO):

        """ Issue ApplyDebugEntitlement command """


        command_code = self.__host_command_code

        params = HostCommandApplyDebugEntitlementParams()
        response = HostCommandApplyDebugEntitlementResponse()

        # Configure input parameters
        params.clockSpeedHz = clock_speed
        params.blob = debug_entitlement_blob

        self.__logger.info('Sending command: {}'.format(command_code.name))
        result = self.__command_helper.execute(command_code, params, response, timeout_usec = timeout_usec, verbosity = verbosity)
        self.__logger.info('Received response: {}'.format(result.name))
        if expected != result:
            raise Exception('Operation failed with unexpected error {} (expected: {})'.format(result.name, expected.name))

        return response


