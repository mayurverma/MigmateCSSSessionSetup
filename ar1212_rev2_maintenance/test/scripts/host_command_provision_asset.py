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
# Camera Provision Asset host command
####################################################################################
import struct
import logging

from enum import Enum
from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from root_of_trust import RootOfTrust
from crypto_services_client import CryptoServicesClient
from branch_properties import BranchProperties

from host_command import HostCommandParams, HostCommandResponse

class HostCommandProvisionAssetParams(HostCommandParams):
    """Host command ProvisionAsset parameters."""

    def __init__(self):
        self.verbose = False

    # overridden method
    def pack(self):
        padding = 0
        members = struct.pack("<LLHH{}s".format(self.__assetSizeBytes),
                self.__assetId,
                self.__clockSpeedHz,
                self.__rootOfTrust,
                padding,
                self.__assetPayload)

        if self.verbose: print('packed members', members)
        return members

    @property
    def assetId(self):
        return self.__assetId

    @assetId.setter
    def assetId(self, value):
        self.__assetId = value

    @property
    def clockSpeedHz(self):
        return self.__clockSpeedHz

    @clockSpeedHz.setter
    def clockSpeedHz(self, value):
        self.__clockSpeedHz = value

    @property
    def rootOfTrust(self):
        return self.__rootOfTrust

    @rootOfTrust.setter
    def rootOfTrust(self, value):
        self.__rootOfTrust = value

    @property
    def assetPayload(self):
        return self.__assetPayload

    @assetPayload.setter
    def assetPayload(self, val):
        self.__assetSizeBytes = len(val)
        self.__assetPayload = val

        if self.verbose:
            print("AssetSizeBytes = {}".format(self.__assetSizeBytes))
            for i in range(len(self.__assetPayload)):
                print("Payload[{}] = 0x{:02x}".format(i, self.__assetPayload[i]))

class HostCommandProvisionAssetResponse(HostCommandResponse):
    """Host command ProvisionAsset response."""
    pass    # No response

class HostCommandProvisionAsset():
    """Host command ProvisionAsset class."""

    def __init__(self, camera, key_set_name):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__key_set_name = key_set_name
        self.__host_command_code = HostCommandCode.PROVISION_ASSET
        self.__product_name = BranchProperties.get_product_prefix()

    def execute(self,
                  nvm_record,
                  asset_id = 0x12345678,
                  root_of_trust = RootOfTrust.CM,
                  clock_speed_hz = 50000000,
                  expected = Error.SUCCESS, timeout_usec=500000000, interval_usec=100, verbosity=logging.INFO):

        """ Issue Provision Assets command """


        command_code = self.__host_command_code
        crypto = CryptoServicesClient(self.__product_name, root_of_trust = root_of_trust, key_set_name = self.__key_set_name)

        params = HostCommandProvisionAssetParams()
        response = HostCommandProvisionAssetResponse()

        # Configure input parameters
        params.assetId = asset_id
        params.clockSpeedHz = clock_speed_hz
        params.rootOfTrust = root_of_trust.value
        # Package it up
        params.assetPayload = crypto.encrypt_asset(asset_id, bytes(nvm_record))

        self.__logger.info('Sending command: {}'.format(command_code.name))
        result = self.__command_helper.execute(command_code, params, response, timeout_usec = timeout_usec, verbosity = verbosity)
        self.__logger.info('Received response: {}'.format(result.name))
        if expected != result:
            raise Exception('Operation failed with unexpected error {} (expected: {})'.format(result.name, expected.name))

        return response

