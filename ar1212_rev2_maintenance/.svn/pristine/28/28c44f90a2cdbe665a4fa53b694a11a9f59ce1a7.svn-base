####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Test toolbox functions
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from lifecycle_state import LifecycleState

from patch_type import PatchType
from diag_helper import DiagHelper

from host_command_get_status import HostCommandGetStatus
from host_command_execute_debug import HostCommandExecuteDebug
from verification_app_debug_id import DebugId

def calculate_checksum(dataBlock):
    """ Computes the RFC1071 checksum on the supplied response parameters """
    checksum = 0
    for data in dataBlock:
        checksum += data
        print("Checksum: 0x{:04x}, Data: 0x{:04x}".format(checksum, data))
    # Fold the carry bits back into the lower 16-bits, then invert
    while checksum > 0xFFFF:
        checksum = (checksum & 0xFFFF) + (checksum >> 16)

    return ~checksum & 0xFFFF

class TestToolbox(BaseTestCase):
    """Test fault paths"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.verbose = True
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.diag_helper = DiagHelper(self.camera)
        self.logger.debug('setUp complete')

    @testIdentifier('23.1.1')
    def test_calculate_checksum(self):
        """ Check checksum calculation """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        packet=[0,1]
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CALCULATE_CHECKSUM, packet=packet)

        expected = calculate_checksum(packet)
        self.assertEqual(expected, response.result, "Expected: 0x{:04x} !=  0x{:04x}".format(expected, response.result))

    @testIdentifier('23.1.2')
    def test_calculate_checksum_vector(self):
        """ Check checksum calculation using vector from https://tools.ietf.org/html/rfc1071 """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        packet=[0x0100, 0x03f2, 0xf5f4, 0xf7f6]
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CALCULATE_CHECKSUM, packet=packet)

        expected = calculate_checksum(packet)
        self.assertEqual(~0xf2dd & 0xffff, response.result, "Result incorrect: 0x{:04x}".format(response.result))
        self.assertEqual(expected, response.result, "Expected: 0x{:04x} !=  0x{:04x}".format(expected, response.result))


 