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
# Test Authorize RMA command.
####################################################################################
import logging
import struct
import datetime

from enum import Enum
from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from patch_helper import *
from patch_type import PatchType
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from diag_helper import DiagHelper
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from host_command_authorize_rma import HostCommandAuthorizeRmaParams, HostCommandAuthorizeRmaResponse, HostCommandAuthorizeRmaParams

class TestAuthorizeRma(BaseTestCase):
    """Test the Authorize RMA host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.patch_version = 1
        self.get_status = HostCommandGetStatus(self.camera)
        self.diag_helper = DiagHelper(self.camera)
        self.patch_version = self.patch_loader.get_patch_version(PatchType.PATCH)
        self.logger.debug('setUp complete')

    def _wait_until_target_ready_for_command(self, expectedResponse = Error.SUCCESS):
        """ Check system is ready by polling the doorbell bit """
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expectedResponse.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

    def _authorize_rma(self, commandParams, expectedResponse = Error.SUCCESS):
        """ Issue the AuthorizeRMA command and test the response"""

        response = HostCommandAuthorizeRmaResponse()
        self.logger.info('Sending command: AUTHORIZE RMA')
        result = self.command_helper.execute(HostCommandCode.AUTHORIZE_RMA, commandParams, response, verbosity = logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def _authorize_rma_not_implemented(self, lifecyleState):
        self._common_setup(lifecyleState, PatchType.PATCH)
        status = self.get_status.execute()
        self.assertEqual(lifecyleState, status.current_lifecycle_state)

        commandParams = HostCommandAuthorizeRmaParams()
        commandParams.encryptedParams = bytes([0] * 8)
        commandParams.mac = bytes([0] * 16)

        self._authorize_rma(commandParams, expectedResponse = Error.ACCESS)     # error because the command is not implemented
        
    @testIdentifier('25.1.1')
    def test_authorize_rma_not_implemented_cm(self):
        """ Authorize RMA command is not implemented cm """

        self._authorize_rma_not_implemented(LifecycleState.CM)
        
    @testIdentifier('25.1.2')
    def test_authorize_rma_not_implemented_dm(self):
        """ Authorize RMA command is not implemented dm """

        self._authorize_rma_not_implemented(LifecycleState.DM)
  
    @testIdentifier('25.1.3')
    def test_authorize_rma_not_implemented_se(self):
        """ Authorize RMA command is not implemented se """

        self._authorize_rma_not_implemented(LifecycleState.SECURE)
