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
# Test GetStatus command.
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase

from command_helper import CommandHelper
from patch_type import PatchType
from host_command_get_status import HostCommandGetStatusResponse, HostCommandGetStatus
from host_command_execute_debug import HostCommandExecuteDebug, DebugId

class TestPatchExecute(BaseTestCase):
    """Test patch execution after load, specifically support functions associated with patching"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.verbose = True
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.logger.debug('setUp complete')


    @testIdentifier('4.3.1')
    def test_patch_execute_remove_and_add_cm(self):
        """ Check we can remove patch then add it back in using provided support functions """
        # Check system is ready
        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Execute patch id 0
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.DISABLE_PATCH)
        status = HostCommandGetStatus(self.camera).execute()
        self.assertNotEqual(0xCAFE, status.rom_version)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.ENABLE_PATCH)
        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SWITCH_PATCH)
        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xBEEF, status.rom_version)

