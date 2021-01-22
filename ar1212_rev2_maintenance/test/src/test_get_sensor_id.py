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
# Test GetSensorId command.
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from branch_properties import BranchProperties, BranchType
from command_helper import CommandHelper
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from host_command_code import HostCommandCode
from host_command_provision_asset import HostCommandProvisionAsset
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_get_status import HostCommandGetStatus
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from root_of_trust import RootOfTrust
from patch_type import PatchType
from test_fixture import FixtureType

class TestGetSensorId(BaseTestCase):
    """Test the GetSensorId host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')

    def wait_for_system_ready(self):
        """Waits for the system to be ready
        """
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

    def check_for_expected_status(self, expected_phase, expected_lcs):
        """ Check system is in the expected phase and lifecycle state
            - returns the GetStatus response object
        """
        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute()

        self.assertEqual(response.current_phase, expected_phase,
            'Expected {} phase but was {}'.format(expected_phase.name, response.current_phase.name))
        self.assertEqual(response.current_lifecycle_state, expected_lcs,
            'Expected {} LCS but was {}'.format(expected_lcs.name, response.current_lifecycle_state.name))

        return response

    def set_ephemeral_master_secret(self):
        """ Issue SetEthemeralMasterSecret command """
        expected = Error.SUCCESS
        params = HostCommandSetEthemeralMasterSecretParams()
        params.masterSecretKeyType = 0
        params.masterSecret = bytes([0]*16)
        response = HostCommandSetEthemeralMasterSecretResponse()
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def set_public_certificates(self, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public certificate and send by ProvisionAsset command"""

        rsa_key = self.crypto.generate_random_rsa_key(2048)

        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)


    @testIdentifier('6.1.1')
    def test_get_sensor_id_in_cm(self):
        """Check GetSensorId command in CM with no patch"""

        self._common_setup(LifecycleState.CM, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.CM)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be zero in CM LCS
        for i in range(len(response.unique_id)):
            self.assertEqual(response.unique_id[i], 0)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

    @testIdentifier('6.1.2')
    def test_get_sensor_id_in_dm(self):
        """Check GetSensorId command in DM with no patch"""

        self._common_setup(LifecycleState.DM, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be zero in DM LCS
        for i in range(len(response.unique_id)):
            self.assertEqual(response.unique_id[i], 0)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('6.1.3')
    def test_get_sensor_id_in_se(self):
        """Check GetSensorId command in Secure with no patch"""

        self._common_setup(LifecycleState.SECURE, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be non-zero in DM LCS
        is_non_zero = False
        for i in range(len(response.unique_id)):
            if response.unique_id[i] != 0:
                is_non_zero = True
        self.assertTrue(is_non_zero)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

    @testIdentifier('6.1.4')
    def test_get_sensor_id_twice_in_se(self):
        """Check GetSensorId command twice in Secure with no patch"""

        self._common_setup(LifecycleState.SECURE, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be non-zero in DM LCS
        is_non_zero = False
        for i in range(len(response.unique_id)):
            if response.unique_id[i] != 0:
                is_non_zero = True
        self.assertTrue(is_non_zero)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)


    @testIdentifier('6.2.1')
    def test_set_session_key_before_get_sensor_id_in_se(self):
        """Send SetSesessionKeys before GetSensorId command in Secure"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)
        # Log response
        self.logger.info('Stack Used Long Words = {}'.format(response.stack_used_long_words))

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue SetPskSessionKeys command
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = 0
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = 0
        params.controlChannelKeyType = 0
        params.hostSalt = bytes([0]*16)
        response = HostCommandSetPskSessionKeysResponse()
        # Should this be an Error.ACCESS?
        expected = Error.AGAIN  # Reject and ask for trying again! Can't set session keys before have the sensor ID.
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be non-zero in SECURE LCS
        is_non_zero = False
        for i in range(len(response.unique_id)):
            if response.unique_id[i] != 0:
                is_non_zero = True
        self.assertTrue(is_non_zero)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)


    @testIdentifier('6.2.2')
    def test_set_session_key_after_get_sensor_id_in_se(self):
        """Send SetSesessionKeys after GetSensorId command in SECURE """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)
        # Log response
        self.logger.info('Stack Used Long Words = {}'.format(response.stack_used_long_words))

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be non-zero in SECURE LCS
        is_non_zero = False
        for i in range(len(response.unique_id)):
            if response.unique_id[i] != 0:
                is_non_zero = True
        self.assertTrue(is_non_zero)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Issue SetPskSessionKeys command
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = 0
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = 0
        params.controlChannelKeyType = 0
        params.hostSalt = bytes([0]*16)
        response = HostCommandSetPskSessionKeysResponse()
        expected = Error.NOENT     # This command is NOT supported in secure mode!
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('6.2.3')
    def test_set_session_key_after_get_sensor_id_in_dm(self):
        """Send SetSesessionKeys after GetSensorId command in DM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        # Log response
        self.logger.info('Stack Used Long Words = {}'.format(response.stack_used_long_words))

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Unique ID should be zero in DM LCS
        for i in range(len(response.unique_id)):
            self.assertEqual(response.unique_id[i], 0)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        self.set_ephemeral_master_secret()

        # Issue SetPskSessionKeys command
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = 0
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = 0
        params.controlChannelKeyType = 0
        params.hostSalt = bytes([0]*16)
        response = HostCommandSetPskSessionKeysResponse()
        expected = Error.SUCCESS
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

