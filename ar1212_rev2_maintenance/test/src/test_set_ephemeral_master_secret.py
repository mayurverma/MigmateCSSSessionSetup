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
# Test SetEphemeralMasterSecret correctly.
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from common_error import Error
from crypto_helper import CryptoHelper
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_provision_asset import HostCommandProvisionAsset
from root_of_trust import RootOfTrust
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from patch_type import PatchType
from test_fixture import FixtureType
from reset_types import ResetType

class TestSetEphemeralMasterSecret(BaseTestCase):
    """Test SetEphemeralMasterSecret functionality"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.params = HostCommandSetEthemeralMasterSecretParams()
        self.response = HostCommandSetEthemeralMasterSecretResponse()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'

        self.camera.reset(ResetType.SENSOR_HARD)
        self.logger.debug('setUp complete')

    def _wait_for_doorbell_clear(self):
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

    def _get_sensor_id(self):
        """ Issue GetSensorId command """
        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id

    def _provision_master_secret(self, master_secret):
        """Create RSA public/private key and send by ProvisionAsset command"""

        asset = NvmRecordPSKMasterSecret(master_secret)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)


    def _provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        if CertificatePurpose.SENSOR == purpose:
            asset = NvmRecordRSAPrivateKey(rsa_key, auth_id = auth_id)
            HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 1234, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)


    def _get_status(self):
        """Issue the GetStatus command and return current lifecycle"""

        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute()

        # Log response
        self.logger.info('Current Phase         = {}'.format(str(response.current_phase)))
        self.logger.info('Current LCS           = {}'.format(str(response.current_lifecycle_state)))

        return response.current_phase, response.current_lifecycle_state

    def _do_sanity(self, expectedLCS, expectedPhaseBefore, expectedPhaseAfter, expectedResponse = Error.SUCCESS):

        # Issue the GetStatus command to check system is now in the Expected pre-command phase
        phase, lifecycle = self._get_status()
        self.assertEqual(expectedLCS, lifecycle), 'Bad LCS: Expected={} Actual=()'.format(expectedLCS, lifecycle)
        assert(expectedPhaseBefore == phase)

        # Issue SetEthemeralMasterSecret command
        self.params.masterSecretKeyType = 0
        self.params.masterSecret = bytes([0]*16)

        expected = expectedResponse
        self.logger.info('Sending command: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, self.params, self.response, timeout_usec=5000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Check we're in the desired post-command phase
        phase, lifecycle = self._get_status()
        assert(expectedLCS == lifecycle)
        assert(expectedPhaseAfter == phase)

    @testIdentifier('11.1.1')
    def test_sanity_cm(self):
        """Check FW responds to SetEphemeralMasterSecret correctly in CM"""

        self._common_setup(LifecycleState.CM, None)

        # send setEphemeralMasterSecret command in the DEBUG phase
        self._do_sanity(LifecycleState.CM, SystemMgrPhase.DEBUG, SystemMgrPhase.CONFIGURE)

        # now ensure we can't set ephemeral key twice
        self._wait_for_doorbell_clear()
        expected = Error.ALREADY
        self.logger.info('Sending command again: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, self.params, self.response, timeout_usec=5000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

    @testIdentifier('11.1.2')
    def test_sanity_dm(self):
        """Check FW responds to SetEphemeralMasterSecret correctly in DM"""

        # get the target into configure phase - just to be different from the CM test
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # send setEphemeralMasterSecret command in the CONFIGURE phase
        self._do_sanity(LifecycleState.DM, SystemMgrPhase.CONFIGURE, SystemMgrPhase.CONFIGURE)

        # now ensure we can't set ephemeral key twice
        self._wait_for_doorbell_clear()
        expected = Error.ALREADY
        self.logger.info('Sending command again: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, self.params, self.response, timeout_usec=5000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

    @testIdentifier('11.1.3')
    def test_sanity_se(self):
        """Check FW responds to SetEphemeralMasterSecret correctly in SE"""

        # get the target into configure phase - just to be different from the CM test
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # send setEphemeralMasterSecret command. This command is *NOT* supported in secure mode
        self._do_sanity(LifecycleState.SECURE, SystemMgrPhase.CONFIGURE, SystemMgrPhase.CONFIGURE, Error.ACCESS)

    ##########   Error cases   ##################

    @testIdentifier('11.2.1')
    def test_set_ephemeral_master_secret_with_psk_master_secret_already_in_nvm_dm(self):
        """Check FW responds to SetEphemeralMasterSecret correctly when a PSK Master secret already in NVM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Expected pre-command phase
        phase, lifecycle = self._get_status()
        self.assertEqual(LifecycleState.DM, lifecycle), 'Bad LCS: Expected=DM Actual={}'.format(lifecycle)

        # Issue Provision asset command with 2048 RSA key
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self._provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR)

        aes_key = self.crypto.generate_random_aes_key()
        self._provision_master_secret(aes_key)

        phase, lifecycle = self._get_status()
        self.assertEqual(LifecycleState.DM, lifecycle), 'Bad LCS: Expected=DM Actual={}'.format(lifecycle)
        self.assertEqual(SystemMgrPhase.CONFIGURE, phase), 'Bad Phase: Expected=CONFIGURE Actual={}'.format(phase)

        # Master secret is now in NVM, now issue SetEphemeralMasterSecret, will be ignored
        self._do_sanity(LifecycleState.DM, SystemMgrPhase.CONFIGURE, SystemMgrPhase.CONFIGURE, Error.SUCCESS)

    @testIdentifier('11.2.2')
    def test_set_ephemeral_master_secret_with_sensor_auth_private_key_already_in_nvm(self):
        """Check FW responds to SetEphemeralMasterSecret correctly when a SensorAuthPrivKey already in NVM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Expected pre-command phase
        phase, lifecycle = self._get_status()
        self.assertEqual(LifecycleState.DM, lifecycle), 'Bad LCS: Expected=DM Actual={}'.format(lifecycle)

        # Issue Provision asset command with 2048 RSA key
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self._provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR)

        # Private key is now in NVM, now issue SetEphemeralMasterSecret, will be ignored
        self._do_sanity(LifecycleState.DM, SystemMgrPhase.CONFIGURE, SystemMgrPhase.CONFIGURE, Error.SUCCESS)

