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
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError

from diag_helper import DiagHelper
from command_helper import CommandHelper
from patch_type import PatchType
from host_command_get_status import HostCommandGetStatus
from host_command_execute_debug import HostCommandExecuteDebug, DebugId
from irq_types import IRQnType
from reset_types import ResetType

class TestInterruptHandling(BaseTestCase):
    """Test interrupt handling by injection using the Test Patch"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.verbose = True
        self.diag_helper = DiagHelper(self.camera)
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.rom_version = self.patch_loader.get_rom_version()
        self.logger.debug('setUp complete')


    @testIdentifier('1.2.1')
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

    @testIdentifier('1.2.2')
    def test_patch_execute_add_irq_handler_cm(self):
        """ Check default handler causes fatal error then reset and check new handler can be added and executed """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Copy vector table to RAM
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.RELOCATE_IRQ_VECTOR_TABLE,
            expected = Error.SUCCESS)
        # Generate fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter=IRQnType.CCPowerdownReady_IRQn.value, expected = Error.SYSERR)

        # Wait for system to go fatal
        self.fixture.wait_usec(10000)        #10 mSec

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION, fault)
        self.assertEqual(IRQnType.CCPowerdownReady_IRQn.value, context.value)

        # Reset device and change IRQ handler
        self.camera.reset(ResetType.SENSOR_HARD)
        self.logger.info('Resetting device......')
        self.camera.wait_usec(100)

        # Check system is ready
        response = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(response, Error.SUCCESS, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        status = HostCommandGetStatus(self.camera).execute()

        self.patch_loader.load_patch(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Copy vector table to RAM
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.RELOCATE_IRQ_VECTOR_TABLE,
            expected = Error.SUCCESS)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NEW_IRQ_HANDER,
            parameter=IRQnType.CCPowerdownReady_IRQn.value, expected = Error.SUCCESS)
        self.assertEqual(IRQnType.CCPowerdownReady_IRQn.value - 1, response.result)
        # Trigger interrupt
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter=IRQnType.CCPowerdownReady_IRQn.value, expected = Error.SUCCESS)

        info = self.diag_helper.get_info()
        self.assertEqual(1, info)

        # Trigger interrupt
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter=IRQnType.CCPowerdownReady_IRQn.value, expected = Error.SUCCESS)

        info = self.diag_helper.get_info()
        self.assertEqual(2, info)
