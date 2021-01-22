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
# Test Firmware diagnostics support.
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from diag_checkpoint import DiagCheckpoint

from command_helper import CommandHelper
from diag_helper import DiagHelper
from patch_type import PatchType

from host_command_get_status import HostCommandGetStatus
from host_command_execute_debug import HostCommandExecuteDebug, HostCommandExecuteDebugResponse
from verification_app_debug_id import DebugId
from reset_types import ResetType

class TestDiag(BaseTestCase):
    """Test firmware diagnostics support."""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.logger.debug('setUp complete')

    @testIdentifier('2.1.1')
    def test_pausepoint(self):
        """Check FW boots to first pausepoint, then release and pause at
           entry to main, then release and run to wait for interrupt state."""

        self._common_setup(LifecycleState.CM, None)

        # Set the pausepoint to first possible pausepoint
        expected = DiagCheckpoint.INIT_START
        self.diag_helper.set_pausepoint(expected)
        self.camera.reset(ResetType.SENSOR_SOFT)

        # Check we can stop at the first pausepoint
        self.diag_helper.wait_for_pausepoint(verbosity=logging.INFO)
        actual = self.diag_helper.get_checkpoint()
        self.logger.info('Paused at checkpoint {}'.format(str(actual)))

        # Verify expected checkpoint was reached
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

        # Set pausepoint on entry to C main function
        expected = DiagCheckpoint.MAIN
        self.diag_helper.set_pausepoint(expected)
        self.diag_helper.release_pausepoint()

        # Check we can stop at a later pausepoint
        self.diag_helper.wait_for_pausepoint(verbosity=logging.INFO)
        actual = self.diag_helper.get_checkpoint()
        self.logger.info('Paused at checkpoint {}'.format(str(actual)))

        # Verify expected checkpoint was reached
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

        # Release the system witn no pausepoint
        self.diag_helper.release_pausepoint()
        for retry in range(50):
            actual = self.diag_helper.get_checkpoint()
            self.logger.info('CHECKPOINT = {}'.format(str(actual)))
            if actual == DiagCheckpoint.SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT:
                break
            self.fixture.wait_usec(100)

        # Check system reaches ready state
        expected = DiagCheckpoint.SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

    @testIdentifier('2.2.1')
    def test_get_sys_info(self):
        """ Test call to low level function SystemDrvGetInfo() using test patch """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYS_INFO,
            parameter=0x5555)

        self.assertEqual(0x5555+1, response.result)




















