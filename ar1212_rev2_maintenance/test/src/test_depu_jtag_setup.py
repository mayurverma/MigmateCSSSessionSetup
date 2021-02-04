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
# Test to validate JTAG configuration on DEPU enable part.
####################################################################################
import logging
import sys
from enum import IntEnum, unique

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from secure_boot_certificate import CC312LifeCycleState
from patch_type import PatchType
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_apply_debug_entitlement import HostCommandApplyDebugEntitlement
from crypto_services_client import CryptoServicesClient
from branch_properties import BranchProperties
from build_properties import BuildProperties
from root_of_trust import RootOfTrust
from test_fixture import FixtureType
from diag_helper import DiagHelper
from reset_types import ResetType

depu_app_output_dir = BuildProperties["DEPU"].output_path
if depu_app_output_dir not in sys.path:
    sys.path.append(depu_app_output_dir)
from importlib import import_module

@unique
class DcuConfig(IntEnum):
    """DCU configuration enumeration."""

    DCU_RESERVED = 0x01                     # Reserved by ARM
    DCU_DEBUG_WITH_CC312_RESET = 0x02       # debug_enable_with_cc312_reset
    DCU_DEBUG_WITHOUT_CC312_RESET = 0x4     # debug_enable_without_cc312_reset
    DCU_ICORE_REGISTER_ENABLE = 0x08        # icore_registers_enable
    DCU_INFO_ENABLE = 0x10                  # Enables INFO register usage by software

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

@unique
class SensorGpioConfig(IntEnum):
    """DCU configuration enumeration."""

    SP_TEST = 0x01
    SP_GPIO2_TRSTL = 0x08
    SP_TEMPFLAG_TDO = 0x20

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

class TestDepuJtagSetup(BaseTestCase):
    """Test the SdpuProgramOtpm host command operation"""

    def setUp(self):

        if FixtureType.DEVWARE != self.fixture.get_type():
            self.assertFalse("Requires fixture type DEVWARE")

        # Run base class setup method
        super().setUp()

        self.rom_version = self.patch_loader.get_rom_version()
        self.verbose = True
        # These tests should be running in devware only
        self.key_set_name = 'eval'
        self.crypto_cm = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)
        self.logger.debug('setUp complete')

    def wait_for_system_ready(self, expected_result=Error.SUCCESS):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

    def apply_debug_entitlement(self, cryptoLifeCycle, socId,
        debug_enabler_mask   = 0x00000000000000000000000000000000,
        debug_enabler_lock   = 0x00000000000000000000000000000000,
        debug_developer_mask = 0x00000000000000000000000000000000,
        clockSpeedHz=27000000,
        expectedResponse = Error.SUCCESS):
        """Execute ApplyDebugEntitlement command"""


        blob = self.crypto_cm.create_debug_entitlement(
                                               cert_version = 0x10000,
                                               sw_version = 5,
                                               lcs = cryptoLifeCycle,
                                               debug_enabler_mask   = debug_enabler_mask,
                                               debug_enabler_lock   = debug_enabler_lock,
                                               debug_developer_mask = debug_developer_mask,
                                               soc_id = socId)

        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clockSpeedHz)

        return response

    def get_sensor_id(self):
        """ Issue GetSensorId command """

        response = HostCommandGetSensorId(self.camera).execute()
        self.logger.info('Unique ID = {}'.format(str(response.unique_id)))

        return bytes(response.unique_id)

    def sensor_soft_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self.wait_for_system_ready()

    def sensor_hard_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_HARD)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self.wait_for_system_ready()

    def enable_jtag_debug(self, enable):

        reg = self.camera.sensor.reg
        if enable:
            reg.CSS_JTAG_DEBUG.value = 0x01
        else:
            reg.CSS_JTAG_DEBUG.value = 0x00

    def is_jtag_debug_enabled(self):

        reg = self.camera.sensor.reg
        return (1 == reg.CSS_JTAG_DEBUG.uncached_value)

    def SetSensorGpio(self, enable, value):

        self.camera.WriteSensorGpio((enable << 8) | value)

    @testIdentifier('22.7.1')
    def test_depu_setup_jtag_for_debug(self):
        """Check application of debug entitlement on a DEPU provisioned device to enable JTAG debugging

        Board setup
            1) P42 - SADDR2 should be set to ZERO (2-3 linked)
            2) P43 - TEST remove link (TEST should be high) (check as DEMO3 could drive it low via SensorGPIOControl, bit 0)
        Required steps to enable JTAG
            1) Apply debug configuration setting bit 2 (0x04)
            2) Set CSS_JTAG_DEBUG = 1
            3) Set TEST pin high.
        NOTES:
        - Removing link P43 should pull TEST high, however the DEMO3 board may drive this signal
          low so you may also need to set Demo3 register "SensorGPIOControl[0]" to zero
        - On this device SADDR2 is shared with JTAG TCK signal so when TEST is assert SADDR2 will be driven to logic
          ZERO internally so the serial address of this device could change i.e. if this signal was configured as a logic
          ONE externally.
        - When setting the device into ARM JTAG mode, an internal TEST signal is asserted. In this mode serial accesses to AR0820 device no longer functions (known issue).

        The DEMO3 SensorGPIOControl has 16 bits, the top 8 bits are enable signals the bottom 8 bits are
        the corresponding signal state, there are only 6 valid signals i.e. SP0 to SP5. The active signals are:
            SP0 (S_TEST)
            SP3 (S_GPIO2/TRST_L)
            SP5 (S_TEMP_FLAG/TDO)

        Setting SensorGPIOControl to 0x0000 implies signal levels are based on sensor board signal states only
        and associated pull ups/pull downs, so
            SP0 (S_TEST) = High with P43 removed
            SP3 (S_GPIO2/TRST_L) = High (Pullup)
            SP5 (S_TEMP_FLAG/TDO) = X (Depends on AR0825 output pin state)
        """

        self.SetSensorGpio(SensorGpioConfig.SP_TEST, 0)

        self.sensor_hard_reset()
        self.enable_jtag_debug(False)
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)
        soc_id = self.get_sensor_id()

        self.logger.info(">>>>>>>>>>>>>>>>>>> RESTART <<<<<<<<<<<<<<<<<<<<<<<")
        self.sensor_soft_reset()
        self.wait_for_system_ready()

        response = HostCommandGetStatus(self.camera).execute()
        # By default access to info register is disabled in secure mode
        self.assertEqual(0, DiagHelper(self.camera).get_info())
        self.assertEqual(SystemMgrPhase.DEBUG, response.current_phase)
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)

        self.enable_jtag_debug(True)
        self.assertTrue(self.is_jtag_debug_enabled())

        # Apply debug entitlement enabling debug mode, access to icore debug information and firmware access to info
        # register
        dcu_config = (DcuConfig.DCU_DEBUG_WITHOUT_CC312_RESET | DcuConfig.DCU_ICORE_REGISTER_ENABLE |
                      DcuConfig.DCU_INFO_ENABLE)
        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, dcu_config, dcu_config, dcu_config)
        HostCommandGetStatus(self.camera).execute()
        # Check info register is read correctly
        self.assertEqual(HostCommandCode.GET_STATUS, DiagHelper(self.camera).get_info())
        self.assertEqual(SystemMgrPhase.DEBUG, response.current_phase)
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)

        self._load_patch(LifecycleState.SECURE, PatchType.PATCH)

        response = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(HostCommandCode.GET_STATUS, DiagHelper(self.camera).get_info())
        self.assertEqual(SystemMgrPhase.CONFIGURE, response.current_phase)
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)

        # GIO allocation
        # 0 - S_TEST
        # 3 - S_GPIO2/TRST_L
        # 5 - S_TEMP_FLAG/TDO
        self.SetSensorGpio(SensorGpioConfig.SP_TEST, 1)

        self.logger.info(">>>>>>>>>>>>>>>>>>> Connect JTAG cable to target <<<<<<<<<<<<<<<<<<<<<<<")

        # NOTE to get serial access to device a hardware reset is required and JTAG cable should be removed

