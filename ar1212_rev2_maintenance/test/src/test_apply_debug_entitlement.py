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
import struct

from pyfwtest import *
from base_test_case import BaseTestCase
from branch_properties import BranchProperties, BranchType
from diag_helper import DiagHelper
from diag_checkpoint import DiagCheckpoint
from common_error import Error
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_apply_debug_entitlement import HostCommandApplyDebugEntitlementParams, HostCommandApplyDebugEntitlementResponse, HostCommandApplyDebugEntitlement
from secure_boot_entitlement import SecureBootDebugEntitlement, SecureBootRMAEntitlement
from crypto_helper import CryptoHelper
from secure_boot_certificate import CC312LifeCycleState, HbkId
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from root_of_trust import RootOfTrust
from crypto_services_client import CryptoServicesClient
from test_fixture import FixtureType
from reset_types import ResetType

class TestApplyDebugEntitlement(BaseTestCase):
    """Test ApplyDebugEntitlement functionality"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto_helper = CryptoHelper()
        self.get_status = HostCommandGetStatus(self.camera)
        self.get_sensor_id = HostCommandGetSensorId(self.camera)
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.camera.reset(ResetType.SENSOR_HARD)

        self.logger.debug('setUp complete')

    def apply_debug_entitlement(self,
                                       lcs, # CC312LifeCycleState
                                       socId,
                                       debug_enabler_mask = 0x00000000000000000000000000000000,
                                       debug_enabler_lock = 0x00000000000000000000000000000000,
                                       debug_developer_mask = 0x00000000000000000000000000000000,
                                       key_cert = False,
                                       root_of_trust = RootOfTrust.CM,
                                       clockSpeedHz = 6000000,
                                       expected = Error.SUCCESS):
        """Execute ApplyDebugEntitlement command"""

        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = root_of_trust, key_set_name = self.key_set_name)

        blob = crypto.create_debug_entitlement(key_cert = key_cert,
                                               cert_version = 0x10000,
                                               sw_version = 5,
                                               lcs = lcs,
                                               debug_enabler_mask = debug_enabler_mask,
                                               debug_enabler_lock = debug_enabler_lock,
                                               debug_developer_mask = debug_developer_mask,
                                               soc_id = socId)


        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clock_speed = clockSpeedHz, expected = expected)

        return response

    def run_apply_debug_entitlement_command(self, rsa_key, crypto_lifecycle, soc_id, expected_response = Error.SUCCESS):

        c = SecureBootDebugEntitlement(
                     enabler_cert_key = rsa_key,
                     developer_cert_key = rsa_key,
                     cert_version = 0x10000,
                     hbk_id = HbkId.HBK0,
                     sw_version = 5,
                     lcs = crypto_lifecycle,
                     debug_enabler_mask = 0x00000000000000000000000000000010,
                     debug_enabler_lock = 0x00000000000000000000000000000010,
                     debug_developer_mask = 0x00000000000000000000000000000010,
                     soc_id = soc_id)
        blob = bytes(c)

        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clock_speed = 6000000, expected = expected_response)

    def get_soc_id_and_reset(self):

        self._wait_for_system_ready()

        soc_id = bytes(self.get_sensor_id.execute().unique_id)

        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self._wait_for_system_ready()

        return soc_id


    @testIdentifier('5.1.1')
    def test_2_certificates_cm(self):
        """Check FW rejects command with ERROR_ACCESS in CM"""

        self._common_setup(LifecycleState.CM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)

        soc_id = bytes([0x00] * 32)
        initial_info = self.diag_helper.get_info()
        expected_info = HostCommandCode.GET_STATUS.value
        expected_apply_debug_entitlement_response = Error.ACCESS
        crypto_lifecycle = CC312LifeCycleState.CM
        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.1.2')
    def test_2_certificates_dm(self):
        """Check FW accepts command and return ERRORR_SUCCESS in DM"""

        self._common_setup(LifecycleState.DM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        soc_id = bytes([0x00] * 32)
        initial_info = self.diag_helper.get_info()
        expected_info = HostCommandCode.GET_STATUS.value
        expected_apply_debug_entitlement_response = Error.SUCCESS
        crypto_lifecycle = CC312LifeCycleState.DM
        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.1.3')
    def test_2_certificates_se(self):
        """Check FW accepts command and return ERRORR_SUCCESS in SECURE"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        soc_id = self.get_soc_id_and_reset()

        initial_info = self.diag_helper.get_info()
        expected_apply_debug_entitlement_response = Error.SUCCESS
        expected_info = initial_info
        crypto_lifecycle = CC312LifeCycleState.Secure

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, expected = expected_apply_debug_entitlement_response)

        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.2.1')
    def test_3_certificates_dm(self):
        """FW accepts command in DM life cycles using 3 certificates"""

        self._common_setup(LifecycleState.DM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        soc_id = bytes([0x00] * 32)
        initial_info = self.diag_helper.get_info()
        expected_info = HostCommandCode.GET_STATUS.value
        expected_apply_debug_entitlement_response = Error.SUCCESS
        crypto_lifecycle = CC312LifeCycleState.DM
        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.2.2')
    def test_3_certificates_se(self):
        """FW accepts command in SECURE life cycles using 3 certificates and enables info output"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        initial_info = self.diag_helper.get_info()
        expected_info = HostCommandCode.GET_STATUS.value

        soc_id = self.get_soc_id_and_reset()

        expected_apply_debug_entitlement_response = Error.SUCCESS
        expected_info = initial_info
        crypto_lifecycle = CC312LifeCycleState.Secure

        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.2.3')
    def test_2_commands_3_certificates_se(self):
        """FW accepts second command  using 3 certificates and enable then disables info output"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        # By default in secure mode info output should be disabled
        initial_info = self.diag_helper.get_info()
        self.logger.info('Initial info value read: {}'.format(initial_info))
        expected_info = HostCommandCode.GET_STATUS.value

        soc_id = self.get_soc_id_and_reset()

        expected_apply_debug_entitlement_response = Error.SUCCESS
        expected_info = initial_info
        crypto_lifecycle = CC312LifeCycleState.Secure

        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0000, 0x0010, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        # Entitlement should enable info output
        info_value = self.diag_helper.get_info()
        self.logger.info('Info value read after application of entitlement: {:x}'.format(info_value))
        self.assertEqual(HostCommandCode.GET_STATUS, info_value)
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        # Now send second entitlement this time it should disable info output

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0000, 0x0000, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        # Entitlement should disable info access, so info will contain last value written
        info_value = self.diag_helper.get_info()
        self.logger.info('Info value read after application of 2nd entitlement: {:x}'.format(info_value))
        self.assertEqual(HostCommandCode.APPLY_DEBUG_ENTITLEMENT, info_value)
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.2.4')
    def test_2_commands_3_certificates_with_lock_se(self):
        """Send 2 commands  first will enable info with lock, second will try to disable info but lock should prevent this"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        # By default in secure mode info output should be disabled
        initial_info = self.diag_helper.get_info()
        self.logger.info('Initial info value read: {}'.format(initial_info))
        expected_info = HostCommandCode.GET_STATUS.value


        expected_info = initial_info
        crypto_lifecycle = CC312LifeCycleState.Secure
        self.assertEqual(expected_info, initial_info, "expected_info({:x}) != initial_info({:x})".format(expected_info, initial_info))

        soc_id = self.get_soc_id_and_reset()

        expected_apply_debug_entitlement_response = Error.SUCCESS
        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        # Entitlement should enable info output
        info_value = self.diag_helper.get_info()
        self.logger.info('Info value read after application of entitlement: {:x}'.format(info_value))
        self.assertEqual(HostCommandCode.GET_STATUS, info_value)
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        expected_apply_debug_entitlement_response = Error.BADMSG
        # Now send second entitlement this time it should try to disable info output but fail due to lock
        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0000, 0x0000, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        # Entitlement should disable info access, so info will contain last value written
        info_value = self.diag_helper.get_info()
        self.logger.info('Info value read after application of 2nd entitlement: {:x}'.format(info_value))
        self.assertEqual(HostCommandCode.GET_STATUS, info_value, "expected_info({:x}) != info_value({:x})".format(HostCommandCode.GET_STATUS, info_value))
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.2.5')
    def test_command_rejected_in_configure_phase_se(self):
        """Send 2 commands  first in DEBUG phase, second in CONFIGURE, check second is rejected."""

        self._common_setup(LifecycleState.SECURE, None)

        soc_id = self.get_soc_id_and_reset()

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        expected_apply_debug_entitlement_response = Error.SUCCESS
        crypto_lifecycle = CC312LifeCycleState.Secure

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0000, 0x0010, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        # Now send get sensor Id which should cause phase change

        soc_id = bytes(self.get_sensor_id.execute().unique_id)
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # Now send second entitlement should be rejected as we are in CONFIGURE phase

        expected_apply_debug_entitlement_response = Error.ACCESS

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0000, 0x0000, key_cert = True, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        # Entitlement should disable info access, so info will contain last value written
        info_value = self.diag_helper.get_info()
        self.logger.info('Info value read after application of 2nd entitlement: {:x}'.format(info_value))
        self.assertEqual(HostCommandCode.GET_STATUS, info_value, "expected_info({:x}) != info_value({:x})".format(HostCommandCode.GET_STATUS, info_value))
        # Check we're in the Debug phase
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)


    @testIdentifier('5.3.1')
    def test_2_certificates_with_wrong_rsa_key_in_dm(self):
        """Check FW rejects command with ERROR_BADMSG in DM"""

        self._common_setup(LifecycleState.DM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        rsa_key = self.crypto_helper.generate_random_rsa_key()
        soc_id = bytes([0x00] * 32)

        self.run_apply_debug_entitlement_command(rsa_key, CC312LifeCycleState.DM, soc_id, expected_response = Error.BADMSG)

        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)


    @testIdentifier('5.3.2')
    def test_2_certificates_with_wrong_rsa_key_in_se(self):
        """Check FW rejects command with ERROR_BADMSG in SE"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)
        soc_id = self.get_soc_id_and_reset()

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        rsa_key = self.crypto_helper.generate_random_rsa_key()
        self.run_apply_debug_entitlement_command(rsa_key, CC312LifeCycleState.Secure, soc_id, expected_response = Error.BADMSG)

        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.3.3')
    def test_2_certificates_with_RMA_in_se(self):
        """Check FW rejects command with Error in SE"""

        self._common_setup(LifecycleState.SECURE, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        hbk0_rsa_key = self.crypto_helper.generate_rsa_key_from_components(self.crypto_helper.HBK0_KEY_MODULUS_DEFAULT, d = self.crypto_helper.HBK0_KEY_PRIV_EXP_DEFAULT)
        hbk1_rsa_key = self.crypto_helper.generate_rsa_key_from_components(self.crypto_helper.HBK1_KEY_MODULUS_DEFAULT, d = self.crypto_helper.HBK1_KEY_PRIV_EXP_DEFAULT)
        hbk0 = self.crypto_helper.hbk_gen(hbk0_rsa_key, True)
        hbk1 = self.crypto_helper.hbk_gen(hbk1_rsa_key, True)
        soc_id = self.crypto_helper.soc_id_gen(self.crypto_helper.HUK_DEFAULT, hbk0, hbk1)

        crypto_lifecycle = CC312LifeCycleState.Secure

        c = SecureBootRMAEntitlement(
                     enabler_cert_key = hbk0_rsa_key,
                     developer_cert_key = hbk0_rsa_key,
                     cert_version = 0x10000,
                     hbk_id = HbkId.HBK0,
                     sw_version = 5,
                     lcs = crypto_lifecycle,
                     soc_id = soc_id)

        print(c)
        blob = bytes(c)
        expected_response = Error.BADMSG
        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clock_speed = 6000000, expected = expected_response)

        c = SecureBootRMAEntitlement(
                     enabler_cert_key = hbk1_rsa_key,
                     developer_cert_key = hbk1_rsa_key,
                     cert_version = 0x10000,
                     hbk_id = HbkId.HBK1,
                     sw_version = 5,
                     lcs = crypto_lifecycle,
                     soc_id = soc_id)

        print(c)
        blob = bytes(c)

        if BranchProperties.get_type() == BranchType.AR0820_REV2:
            # More info in http://jira.onsemi.com/browse/AR0820FW-258
            expected_response = Error.INVAL
        else:
            expected_response = Error.SYSERR

        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clock_speed = 6000000, expected = expected_response)

        if BranchProperties.get_type() == BranchType.AR0820_REV2:
            # As per http://jira.onsemi.com/browse/AR0820FW-258 check it is a fatal error
            print("fatal", self.diag_helper.is_fatal_error())


    @testIdentifier('5.4.1')
    def test_loading_two_debug_entitlement_se(self):
        """Check FW accepts two debug entitlements changing two DCUs separately and return ERRORR_SUCCESS in SECURE"""

        self._common_setup(LifecycleState.SECURE, None)
        soc_id = self.get_soc_id_and_reset()

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        expected_apply_debug_entitlement_response = Error.SUCCESS

        crypto_lifecycle = CC312LifeCycleState.Secure

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0008, 0x0018, 0x0008, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

    @testIdentifier('5.4.2')
    def test_loading_debug_entitlement_hbk1_se(self):
        """Check FW accepts HBK1 (oem) debug entitlement changing DCU owned by ICV and return ERRORR_SUCCESS in SECURE"""

        self._common_setup(LifecycleState.SECURE, None)
        soc_id = self.get_soc_id_and_reset()

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        expected_apply_debug_entitlement_response = Error.SUCCESS

        crypto_lifecycle = CC312LifeCycleState.Secure

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, root_of_trust = RootOfTrust.DM, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(0, self.diag_helper.get_info())                     # DCU should not be working
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)


    @testIdentifier('5.4.3')
    def test_loading_two_debug_entitlement_mixed_se(self):
        """Check FW accepts two debug entitlements from OEM and ICV and return ERRORR_SUCCESS in SECURE"""

        self._common_setup(LifecycleState.SECURE, None)
        soc_id = self.get_soc_id_and_reset()

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        expected_apply_debug_entitlement_response = Error.SUCCESS

        crypto_lifecycle = CC312LifeCycleState.Secure

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, root_of_trust = RootOfTrust.DM, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(0, self.diag_helper.get_info())                     # DCU should not be working
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        self.apply_debug_entitlement(crypto_lifecycle, soc_id, 0x0010, 0x0010, 0x0010, root_of_trust = RootOfTrust.CM, expected = expected_apply_debug_entitlement_response)

        status = self.get_status.execute()
        self.assertEqual(HostCommandCode.GET_STATUS, self.diag_helper.get_info())
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

