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
# LoadPatchChunk command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from host_command import HostCommandParams, HostCommandResponse

class HostCommandLoadPatchChunkParams(HostCommandParams):
    """Host command LoadPatchChunk parameters."""

    def pack(self):
        """Packs the host command into a bytes for transport."""
        result = self.__asset_payload
        return result

    @property
    def asset_payload(self):
        return self.__asset_payload;

    @asset_payload.setter
    def asset_payload(self, val):
        self.__asset_payload = val;


class HostCommandLoadPatchChunkResponse(HostCommandResponse):
    """Host command LoadPatchChunk response."""

    # overridden method
    def unpack(self, payload):
        pass # No response

class HostCommandLoadPatchChunk():
    """Execute load patch chunk command."""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)

    def execute(self, asset, expected = Error.SUCCESS, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """Issue the GetStatus command and return response"""

        params = HostCommandLoadPatchChunkParams()
        response = HostCommandLoadPatchChunkResponse()

        params.asset_payload = asset
        self.__logger.info('Sending command: LOAD_PATCH_CHUNK')
        result = self.__command_helper.execute(HostCommandCode.LOAD_PATCH_CHUNK, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response')

        return response