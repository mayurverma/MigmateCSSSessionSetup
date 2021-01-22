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
# Set of miscellaneous test to exercise functions intended for patching or part of
# unused library interfaces
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from lifecycle_state import LifecycleState

from patch_type import PatchType
from diag_helper import DiagHelper

from host_command_execute_debug import HostCommandExecuteDebug, HostCommandExecuteDebugResponse
from verification_app_debug_id import DebugId
from host_command_get_status import HostCommandGetStatus
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError
from host_command_self_test import HostCommandSelfTest

class TestCoverage(BaseTestCase):
    """Test fault paths"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.verbose = True
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.diag_helper = DiagHelper(self.camera)
        self.logger.debug('setUp complete')

    @testIdentifier('24.1.1')
    def test_cc312_hal_pal_empty(self):
        """ Execute CC312HalPal empty functions """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CC312_HAL_PAL_EMPTY, expected = Error.SUCCESS)
        self.logger.info("Executed {} functions".format(response.result))

    @testIdentifier('24.1.2')
    def test_cc312_hal_pal_memory(self):
        """ Execute CC312HalPal memory allocation/deallocation functions """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CC312_HAL_PAL_MEMORY, expected = Error.SUCCESS)
        self.logger.info("Executed {} functions".format(response.result))
        self.logger.info("Associated Data: 0x{:08x}".format(int(response.data[0])))


    @testIdentifier('24.1.3')
    def test_cc312_hal_pal_otpm_write(self):
        """ Execute CC312HalPal CC_BsvOTPWordWrite()"""
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CC312_HAL_PAL_OTPM_WRITE, parameter = 0x42, expected = Error.SUCCESS)
        self.assertEqual(0, int(response.data[0]))
        self.assertEqual(0, int(response.data[1]))
        self.assertEqual(0xCAFEBABE, int(response.data[2]))

    @testIdentifier('24.1.4')
    def test_cc312_hal_pal_abort(self):
        """ Execute CC312HalPal CC_PalAbort() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CC312_HAL_PAL_ABORT, expected = Error.SYSERR)


    @testIdentifier('24.1.5')
    def test_cc312_hal_pal_compare(self):
        """ Execute CC312HalPal CC_PalSecMemCmp() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.CC312_HAL_PAL_COMPARE, parameter = 0x55555555, packet = [0x0201, 0x0403], expected = Error.SUCCESS)


    @testIdentifier('24.2.1')
    def test_lifecycle_mgr_authorize_rma_cm(self):
        """ Execute LifecycleMgrOnAuthorizeRMA() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        # command is not implemented so expect ERROR_ACCESS
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.LIFECYCLE_MGR_AUTHORIZE_RMA, expected = Error.ACCESS)


    @testIdentifier('24.3.1')
    def test_system_drv_get_boot_options_cm(self):
        """ Execute SystemDrvGetBootOptionFlags() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYSTEM_DRV_GET_BOOT_OPTIONS, expected = Error.SUCCESS)
        self.assertEqual(0, int(response.result))

    @testIdentifier('24.4.1')
    def test_system_mgr_patch_support_cm(self):
        """ Execute functions intended to support patching """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYSTEM_MGR_PATCH_FUNCTIONS, expected = Error.SUCCESS)
        self.assertEqual(2, int(response.result))
        self.assertEqual(27000000, int(response.data[0]))

    @testIdentifier('24.4.2')
    def test_system_mgr_request_shutdown_cm(self):
        """ Execute SystemMgrRequestShutdown() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYSTEM_MGR_REQUEST_SHUTDOWN, expected = Error.SUCCESS)
        self.assertEqual(2, int(response.result))
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_TERMINATED, fault)
        self.assertEqual(0x55, context)
        self.assertEqual(0xCC, info)

    @testIdentifier('24.4.3')
    def test_system_mgr_unhandled_event_cm(self):
        """ Execute SystemMgrOnUnhandledEvent() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYSTEM_MGR_UNHANDLED_EVENT, expected = Error.NOENT)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_FATAL, fault)
        self.assertEqual(SystemMgrFatalError.UNHANDLED_EVENT, context)
        self.assertEqual(32, info)

    @testIdentifier('24.5.1')
    def test_crypto_mgr_self_test_cm(self):
        """ Execute self test, dummy function """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandSelfTest(self.camera).execute(expected = Error.ACCESS)


