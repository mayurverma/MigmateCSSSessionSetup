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
# Test Firmware can boot correctly.
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from diag_checkpoint import DiagCheckpoint
from common_error import Error
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from lifecycle_state import LifecycleState
from host_command_get_status import HostCommandGetStatus
from reset_types import ResetType
from systemmgr_phase import SystemMgrPhase
from host_command_get_sensor_id import HostCommandGetSensorId

class TestBoot(BaseTestCase):
    """Test firmware can boot correctly"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.get_status = HostCommandGetStatus(self.camera)
        self.logger.debug('setUp complete')

    @testIdentifier('1.1.1')
    def test_sanity(self):

        """Check FW boots to wait for interrupt checkpoint."""
        for retry in range(50):
            checkpoint = self.diag_helper.get_checkpoint()
            self.logger.info('CHECKPOINT = {}'.format(str(checkpoint)))
            if checkpoint == DiagCheckpoint.SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT:
                break
            self.fixture.wait_usec(100)

        # Check system reaches ready state
        expected = DiagCheckpoint.SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT
        self.assertEqual(checkpoint, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

    @testIdentifier('1.1.2')
    def test_sanity_se(self):

        self._common_setup(LifecycleState.SECURE, None)

        # Issue the GetStatus command to check system is now in the debug phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.DEBUG, response.current_phase)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()
        response = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, response.current_phase)

        self.camera.reset(ResetType.SENSOR_HARD)
        self.fixture.wait_usec(100)

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the debug phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.DEBUG, response.current_phase)

    @testIdentifier('1.1.3')
    def test_boot_with_pause_cm(self):

        self._common_setup(LifecycleState.CM, None)

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, response.current_lifecycle_state)

        # Set the pausepoint during boot sequence
        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        self.diag_helper.set_pausepoint(expected)
        self.camera.reset(ResetType.SENSOR_SOFT)

        # Check we can stop at the first pausepoint
        self.diag_helper.wait_for_pausepoint(verbosity = logging.INFO)
        actual = self.diag_helper.get_checkpoint()
        self.logger.info('Paused at checkpoint {}'.format(str(actual)))

        # Verify expected checkpoint was reached
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

        self.fixture.wait_usec(100)

        # Check again after delay to ensure still at pause point
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))
        self.diag_helper.release_pausepoint()

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, response.current_lifecycle_state)

    @testIdentifier('1.1.4')
    def test_boot_with_pause_dm(self):

        self._common_setup(LifecycleState.DM, None)

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, response.current_lifecycle_state)

        # Set the pausepoint during boot sequence
        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        self.diag_helper.set_pausepoint(expected)
        self.camera.reset(ResetType.SENSOR_SOFT)

        # Check we can stop at the first pausepoint
        self.diag_helper.wait_for_pausepoint(verbosity = logging.INFO)
        actual = self.diag_helper.get_checkpoint()
        self.logger.info('Paused at checkpoint {}'.format(str(actual)))

        # Verify expected checkpoint was reached
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))

        self.fixture.wait_usec(100)

        # Check again after delay to ensure still at pause point
        self.assertEqual(actual, expected.value, 'Failed to reach checkpoint {}'.format(expected.name))
        self.diag_helper.release_pausepoint()

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, response.current_lifecycle_state)

    @testIdentifier('1.1.5')
    def test_boot_with_pause_hard_reset_cm(self):

        self._common_setup(LifecycleState.CM, None)

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, response.current_lifecycle_state)

        # Set the pausepoint during boot sequence
        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        self.diag_helper.set_pausepoint(expected)

        #Hard reset will clear pause register
        self.camera.reset(ResetType.SENSOR_HARD)

        self.fixture.wait_usec(500)

        self.assertEqual(False, self.diag_helper.is_paused())

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, response.current_lifecycle_state)


    @testIdentifier('1.1.6')
    def test_boot_with_pause_hard_reset_dm(self):

        self._common_setup(LifecycleState.DM, None)

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, response.current_lifecycle_state)

        # Set the pausepoint during boot sequence
        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        self.diag_helper.set_pausepoint(expected)

        #Hard reset will clear pause register
        self.camera.reset(ResetType.SENSOR_HARD)

        self.fixture.wait_usec(500)

        self.assertEqual(False, self.diag_helper.is_paused())

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, response.current_lifecycle_state)

    @testIdentifier('1.1.7')
    def test_boot_with_pause_failed_se(self):

        self._common_setup(LifecycleState.SECURE, None)

        # Issue the GetStatus command to check system is now in the Configure phase
        response = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)

        # Set the pausepoint during boot sequence
        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        self.diag_helper.set_pausepoint(expected)
        pausepoint = self.diag_helper.get_pausepoint()

        self.assertEqual(0, pausepoint, 'Operation failed pausepoint is {}, not ZERO'.format(pausepoint))

        # Soft reset will pause if pauspoint value valid, write should not have changed register in Secure LCS
        self.logger.info('Soft Reset')
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.fixture.wait_usec(1000)

        try:
            response = self.get_status.execute()
        except TimeoutError as e:
            self.assertTrue(False, "ERROR: Pause point should not work in SECURE, so timeout not expected")

        # Pause not expected!!
        self.assertEqual(False, self.diag_helper.is_paused())

