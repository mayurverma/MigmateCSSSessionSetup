####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
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
# Test response to NVM corruption issues
####################################################################################
import logging
import struct
from enum import Enum

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from branch_properties import BranchProperties, BranchType

from command_helper import CommandHelper
from patch_type import PatchType
from diag_helper import DiagHelper
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError
from host_command_get_status import HostCommandGetStatus
from host_command_provision_asset import HostCommandProvisionAsset
from nvm_record import NvmRecordPSKMasterSecret
from host_command_get_sensor_id import HostCommandGetSensorId
from reset_types import ResetType

if BranchProperties.get_type() == BranchType.TRUNK:
    from host_command_execute_debug import HostCommandExecuteDebug, DebugId

class TestNvmMgr(BaseTestCase):
    """Test NVM handling of various corruption scenarios """

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.rom_version = self.patch_loader.get_rom_version()
        self.camera.reset(ResetType.SENSOR_HARD)

    def startup(self, expected = Error.SUCCESS, LifecycleState = LifecycleState.SECURE,
        timeout_usec=5000, interval_usec=100):
        """ Check response from provision asset command """

        if Error.SUCCESS == expected:
            self._common_setup(LifecycleState, PatchType.VERIFICATION)
            self.camera.wait_usec(1000)
        else:
            response = self.command_helper.wait_for_command_done(timeout_usec=timeout_usec, interval_usec=interval_usec,
                verbosity=logging.INFO)
            self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
            self.camera.wait_usec(5000)

    @testIdentifier('3.6.1')
    def test_corrupt_length_dm(self):
        """Check FW responds correctly to non zero length field in 'free' location DM"""

        """INFO: <rw>: This test will fail on ARO820 REV2 - AR0820FW-209: NvmMgrFindFreeSpace does not detect corrupt OTPM record headers """
        self.startup(expected = Error.SYSERR, LifecycleState = LifecycleState.DM, timeout_usec = 50000, interval_usec = 50)

        fault, context, info = DiagHelper(self.camera).check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_FATAL, fault)
        self.assertEqual(6, DiagHelper(self.camera).get_info())
        self.logger.info('Check for error NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY ......')
        self.assertEqual(SystemMgrFatalError.NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY, context)

    @testIdentifier('3.4.1')
    def test_ecc_ded_dm(self):
        """Check FW responds correctly to ECC DED at OTPM location 0x30 i.e generates OTPM_ERROR fault"""

        self.startup(expected = Error.SYSERR, LifecycleState = LifecycleState.DM, timeout_usec = 50000, interval_usec = 50)

        self.logger.info('Check for OTPM ECC DED hard fault at 0x30 ......')
        fault, context, info = DiagHelper(self.camera).check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.OTPM_ERROR, fault)
        self.logger.info('Context of error  0x{:04x}'.format(context))
        self.assertEqual(0x30, context)
        self.assertEqual(0, DiagHelper(self.camera).get_info())

    @testIdentifier('3.4.2')
    def test_ecc_ded_0x10_cm(self):
        """Check FW responds correctly to ECC DED at OTPM location 0x10 i.e. generates a CRYPTOCELL_HW_ERROR fault"""

        self.startup(expected = Error.SYSERR, LifecycleState = LifecycleState.CM, timeout_usec = 50000, interval_usec = 50)

        self.logger.info('Check for OTPM ECC DED hard fault at 0x10 ......')
        fault, context, info = DiagHelper(self.camera).check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.CRYPTOCELL_HW_ERROR, fault)
        self.assertEqual(SystemMgrFatalError.LIFECYCLE_OTPM_FAULT, context)
        self.assertEqual(0, DiagHelper(self.camera).get_info())

    @testIdentifier('3.7.1')
    def test_nvm_get_free_space_empty_cm(self):
        """Check patch support function NvmMgrGetFreeSpaceAddress() returns correct value when user space is empty"""

        self.startup(LifecycleState = LifecycleState.CM)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Execute patch id 0
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NVM_FREE_SPACE_OFFSET)
        self.assertEqual(44, response.result)

    @testIdentifier('3.7.2')
    def test_nvm_get_free_space_dummy_record_cm(self):
        """Check patch support function NvmMgrGetFreeSpaceAddress() returns correct value with dummy record in user space"""

        self.startup(LifecycleState = LifecycleState.CM)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Execute patch id 0
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NVM_FREE_SPACE_OFFSET)
        self.assertEqual(44+3+1, response.result)
