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

class TestBasic(BaseTestCase):
    """Test firmware can boot correctly"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.logger.debug('setUp complete')

    @testIdentifier('20.1.1')
    def test_boot(self):
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

    @testIdentifier('20.1.2')
    def test_get_status(self):

        self._common_setup(LifecycleState.CM, None)

        # Issue the GetStatus command to check system
        response = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(LifecycleState.CM, response.current_lifecycle_state)


