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
import struct

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from command_helper import CommandHelper
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from host_command_code import HostCommandCode
from host_command_provision_asset import HostCommandProvisionAssetParams, HostCommandProvisionAssetResponse, HostCommandProvisionAsset
from host_command_get_status import HostCommandGetStatus
from host_command_get_certificates import HostCommandGetCertificatesParams, HostCommandGetCertificatesResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_get_sensor_id import HostCommandGetSensorId
from host_session import HostSession, SessionContext
from host_session_types import SessionMode
from crypto_helper import CryptoHelper
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from provision_asset_helper import ProvisionAssetHelper
from nvm_record import NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret, NvmRecordTRNGCharacterization, NvmRecordOtpmConfig
from root_of_trust import RootOfTrust
from otpm_config import OtpmConfig
from patch_type import PatchType
from test_fixture import FixtureType

class TestProvisionAsset(BaseTestCase):
    """Test the ProvisionAsset host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.get_status = HostCommandGetStatus(self.camera)
        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.provision_asset_cmd = HostCommandProvisionAsset(self.camera, self.key_set_name)
        self.set_ethemeral_params = HostCommandSetEthemeralMasterSecretParams()
        self.response = HostCommandSetEthemeralMasterSecretResponse()
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

    def _get_sensor_id(self):
        """ Issue GetSensorId command """
        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id

    def _set_psk_session_keys(self, videoAuthCipherMode=0, controlChannelCipherMode=0, hostSalt=[0]*16, expectedResponse=Error.SUCCESS):
        """ Issue SetPskSessionKeys command """

        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = videoAuthCipherMode
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = controlChannelCipherMode
        params.controlChannelKeyType = 0
        params.hostSalt = bytes(hostSalt)
        response = HostCommandSetPskSessionKeysResponse()

        self.logger.info('Sending SetPskSessionKeys command:')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=500000, verbosity=logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received SetPskSessionKeys response: {}'.format(expectedResponse.name))

        # Log response
        if Error.SUCCESS == result.value:
            self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
            self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
            self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))

        return response

    def _set_ephemeral_master_secret(self):
        """ Issue SetEthemeralMasterSecret command """
        expected = Error.SUCCESS
        params = HostCommandSetEthemeralMasterSecretParams()
        params.masterSecretKeyType = 0
        params.masterSecret = bytes([0] * 16)
        response = HostCommandSetEthemeralMasterSecretResponse()
        self.logger.info('Sending command: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response, verbosity = logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def _set_public_certificates(self, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR, key_size = 2048, CertificateNumber = 100):
        """Create RSA public certificate and send by ProvisionAsset command"""

        rsa_key = self.crypto.generate_random_rsa_key(key_size)
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number = CertificateNumber)
        asset = NvmRecordRSAPublicCert(pub_cert)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        return pub_cert

    def _set_private_key(self, auth_id = CertificateAuthority.VENDOR_B):
        
        rsa_key = self.crypto.generate_random_rsa_key(2048)

        asset = NvmRecordRSAPrivateKey(rsa_key, auth_id = auth_id)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 1234, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)
        return rsa_key

    def _compare_value(self, a, expected, msg = ""):
        """ Compare two values """
        self.assertEqual(expected, a, 'Comparison {} failed : {} vs {} (expected)'.format(msg, expected, a))

    @testIdentifier('8.1.1')
    def test_provision_asset_in_cm_fails(self):
        """Check ProvisionAsset command in CM
           - will be rejected with ERROR_ACCESS
        """
        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Check we're in CM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        # Issue the ProvisionAsset command
        params = HostCommandProvisionAssetParams()
        params.assetId = 1234
        params.clockSpeedHz = 50000000
        params.rootOfTrust = 0  # RoT CM
        params.assetPayload = b'\00'
        response = HostCommandProvisionAssetResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, Error.ACCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Check we're in CM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

    @testIdentifier('8.1.2')
    def test_provision_asset_in_dm_fails_if_root_of_trust_is_bad(self):
        """Check ProvisionAsset command in DM
           - use an invalid asset package with an invalid root-of-trust
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Issue the ProvisionAsset command
        params = HostCommandProvisionAssetParams()
        params.assetId = 1234
        params.clockSpeedHz = 50000000
        params.rootOfTrust = RootOfTrust.UNKNOWN
        params.assetPayload = b'\00\01\02\03\04\05\06\07\08\09'
        response = HostCommandProvisionAssetResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, Error.BADMSG, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.3')
    def test_provision_asset_in_dm_fails_if_asset_is_garbage(self):
        """Check ProvisionAsset command in DM
           - use an invalid asset package
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Issue the ProvisionAsset command
        params = HostCommandProvisionAssetParams()
        params.assetId = 1234
        params.clockSpeedHz = 50000000
        params.rootOfTrust = RootOfTrust.CM
        params.assetPayload = b'\00\01\02\03\04\05\06\07\08\09'
        response = HostCommandProvisionAssetResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, Error.BADMSG, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.4')
    def test_provision_asset_in_dm_fails_if_asset_is_provisioned_twice(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, with a valid payload, and try to provision it twice
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))

        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Issue the command and get the response
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM, expected = Error.ALREADY)

    @testIdentifier('8.1.5')
    def test_provision_asset_in_dm_fails_if_asset_payload_is_garbage(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, but the payload is garbage
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Create a bogus asset
        asset_type = 0x0009 # invalid
        asset_length_long_words = 0x0005 # 20 bytes
        garbage = 0x0024

        asset_payload = struct.pack('<HHH',
            asset_type, asset_length_long_words, garbage)

        self.provision_asset_cmd.execute(asset_payload, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM, expected=Error.BADMSG)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.6')
    def test_provision_asset_in_dm_fails_if_asset_is_not_authentic(self):
        """Check ProvisionAsset command in DM
           - use an asset package with the wrong provisioning key
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        params = HostCommandProvisionAssetParams()
        params.assetId = 1234
        params.clockSpeedHz = 50000000
        params.rootOfTrust = RootOfTrust.CM

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))

        # Package it up using the wrong key
        key = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\xff'

        params.assetPayload = self.crypto.asset_encrypter(key, params.assetId, bytes(master_secret_asset))
        response = HostCommandProvisionAssetResponse()

        # Issue the command and get the response
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, Error.BADMSG, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.7')
    def test_provision_asset_in_dm_fails_if_clock_speed_is_out_of_range(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, with a valid payload, but with a bad clock speed
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))

        self.provision_asset_cmd.execute(bytes(master_secret_asset), asset_id = 1234, clock_speed_hz = 160000000, root_of_trust = RootOfTrust.CM, expected=Error.RANGE)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.8')
    def test_provision_asset_in_dm_fails_if_provisioning_key_is_dm(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, with a valid payload, but use the DM root-of-trust key
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        params = HostCommandProvisionAssetParams()
        params.assetId = 1234
        params.clockSpeedHz = 160000000
        params.rootOfTrust = RootOfTrust.DM.value

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,25)))

        # Package it up
        params.assetPayload = self.crypto.asset_encrypter(bytes(16), params.assetId, bytes(master_secret_asset))
        response = HostCommandProvisionAssetResponse()

        # Issue the command and get the response
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, Error.ACCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.1.9')
    def test_provision_asset_in_se_fails_for_three_master_secrets(self):
        """Check ProvisionAsset command in Secure
           - use an valid asset package, with a valid payload, but try to provision three of them
        """
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Check we're in SE:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,25)))

        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1234, clock_speed_hz = 60000000, root_of_trust = RootOfTrust.CM)

        # Check we're in SE:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1235, clock_speed_hz = 80000000, root_of_trust = RootOfTrust.CM)

        # Check we're in SE:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,33)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1236, clock_speed_hz = 90000000, root_of_trust = RootOfTrust.CM,
                                  expected = Error.NOSPC)

        # Check we're in SE:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

    @testIdentifier('8.1.10')
    def test_provision_asset_in_dm_fails_if_session_phase(self):
        """Check ProvisionAsset command is rejected in SESSION phase
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status.execute()
        assert(LifecycleState.DM == status.current_lifecycle_state)
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        id = self._get_sensor_id()
        self._set_ephemeral_master_secret()
        status = self.get_status.execute()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command and check system now in Session phase
        self._set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.SUCCESS)
        status = self.get_status.execute()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        # Create a PSK Master Secret Asset and attempt to provision the target
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM,
                                         expected = Error.ACCESS)

    @testIdentifier('8.1.11')
    def test_provision_private_key_dm_attempt_to_start_session_with_replaced_key_fails(self):
        """Check that a subsequently replaced sensor authorisation private key may no longer be used to start a session
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a private key from vendor A
        self.logger.info('Provision first Sensor Private Key from VENDOR_A')
        rsa_key_original = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a replacement private key from vendor A
        self.logger.info('Provision a replacement Sensor Private Key from VENDOR_A')
        rsa_key_replaced = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a Public Certificate from VENDOR_A
        self.logger.info('Provision a Certificate from VENDOR_A')
        pub_cert = RSAPublicCertificate(rsa_key_replaced, auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)
        self.provision_asset_cmd.execute(asset, asset_id = 2345, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Try to start a session using the original private key - it will fail
        self.logger.info('Attempt to start a session using the original private Key from VENDOR_A. It will fail')
        context_orig = SessionContext()
        session = HostSession(context_orig, self.camera)
        try:
            session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_original)
        except AssertionError:
            self.logger.info("Failed to connect using the original private key as expected")
        else:
            assert False, "Unexpected successful connection using supposedly replaced key"
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.2.1')
    def test_provision_psk_master_secret_in_dm_succeeds_with_valid_asset(self):
        """Check ProvisionAsset command in DM with a 128 PSK
           - use an valid asset package, with a valid payload
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,17)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

    @testIdentifier('8.2.2')
    def test_provision_psk_master_secret_in_se_succeeds_with_valid_asset(self):
        """Check ProvisionAsset command in Secure with a 256 PSK
           - use an valid asset package, with a valid payload
        """
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Check we're in SE:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,33)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 5678, clock_speed_hz = 6000000, root_of_trust = RootOfTrust.CM)

        # Check we're in SE:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

    @testIdentifier('8.2.3')
    def test_provision_psk_master_secret_in_se_succeeds_with_valid_asset_192(self):
        """Check ProvisionAsset command in Secure with a 192 PSK
           - use an valid asset package, with a valid payload
        """
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Check we're in SE:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Create a PSK Master Secret Asset
        master_secret_asset = NvmRecordPSKMasterSecret(bytes(range(1,25)))
        self.provision_asset_cmd.execute(master_secret_asset, asset_id = 3456, clock_speed_hz = 6000000, root_of_trust = RootOfTrust.CM)

        # Check we're in SE:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

    def hex_dump(self, name, blob):
        print("Dumping", name)
        for i in range(len(blob)):
            print("{}[0x{:x}] = 0x{:02x}".format(name, i, blob[i]))

    @testIdentifier('8.3.1')
    def test_provision_public_certificate_in_dm_succeeds_with_valid_asset(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, with a valid payload
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        rsa_key = self.crypto.generate_random_rsa_key(3072)
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Issue the GetCertificates command to retrieve the new certificate
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_B.value
        response = HostCommandGetCertificatesResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, Error.SUCCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        self.assertEqual(0, response.vendor_certificate_size_words)

        # unpack the received certificate
        rx_cert = RSAPublicCertificate.get_and_validate_certificate_from_bytes(rsa_key, response.sensor_certificate)

        # Check the received certificate matches the original
        self.assertEqual(bytes(rx_cert), bytes(pub_cert))

    @testIdentifier('8.3.2')
    def test_provision_public_certificate_in_dm_succeeds_with_valid_asset_4096(self):
        """Check ProvisionAsset command in DM
           - use an valid asset package, with a valid 4096 payload
        """
        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check we're in DM:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        rsa_key = self.crypto.generate_random_rsa_key(4096)
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        self.provision_asset_cmd.execute(asset, asset_id = 2345, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        # Check we're in DM:CONFIGURE
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Issue the GetCertificates command to retrieve the new certificate
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_B.value
        response = HostCommandGetCertificatesResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, Error.SUCCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        self.assertEqual(0, response.vendor_certificate_size_words)

        # unpack the received certificate
        rx_cert = RSAPublicCertificate.get_and_validate_certificate_from_bytes(rsa_key, response.sensor_certificate)

        # Check the received certificate matches the original
        self.assertEqual(bytes(rx_cert), bytes(pub_cert))

    @testIdentifier('8.3.3')
    def test_provision_public_certificates_in_dm_succeeds_with_two_valid_certificates_4096(self):
        """Check Provision Certificate command in DM
           - use two valid 4096 certificates
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.DM == response.current_lifecycle_state)

        # Create and set two RSA public certificates
        self.logger.info('Provision first 4096 Certificate')
        sensor_cert_1 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, key_size = 4096)
        self.logger.info('Provision second 4096 Certificate')
        sensor_cert_2 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, key_size = 4096)

        # Issue GetCertificates command for the first certificate
        self.logger.info('Get the first Certificate and check it')
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_A.value;     # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        bytes_sensor_cert_1 = bytes(sensor_cert_1)
        len_sensor_cert_1 = len(bytes_sensor_cert_1)

        # Check the received certificate matches the original
        self._compare_value(response.sensor_certificate_size_words * 2, len_sensor_cert_1, "Certificate 1 sensor size")
        self._compare_value(response.sensor_certificate, bytes_sensor_cert_1, "Certificate 1 sensor contents")
        self._compare_value(response.vendor_certificate_size_words, 0, "Certificate 1 vendor size")

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('8.3.4')
    def test_provision_public_certificates_in_secure_succeeds_with_three_valid_certificates_3072(self):
        """Check Provision Certificate command in SECURE
           - use three valid 3072 certificates for sensor and vendor
        """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()
        assert(LifecycleState.SECURE == response.current_lifecycle_state)

        # Create and set three RSA 3072 public certificates
        self.logger.info('Provision first 3072 Certificate')
        sensor_cert_1 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, key_size = 3072)

        self.logger.info('Provision second 3072 Certificate')
        vendor_cert_1 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.VENDOR, key_size = 3072)

        self.logger.info('Provision third 3072 Certificate')
        sensor_cert_2 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, key_size = 3072)

        # Issue GetCertificates command
        self.logger.info('Get the first Certificate and check it')
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_B.value;  # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        bytes_sensor_cert = bytes(sensor_cert_1)
        len_sensor_cert = len(bytes_sensor_cert)
        bytes_vendor_cert = bytes(vendor_cert_1)
        len_vendor_cert = len(bytes_vendor_cert)

        self._compare_value(response.sensor_certificate_size_words * 2, len_sensor_cert, "Certificate1 sensor size")
        self._compare_value(response.sensor_certificate, bytes_sensor_cert, "Certificate1 sensor contents")
        self._compare_value(response.vendor_certificate_size_words * 2, len_vendor_cert, "Certificate1 vendor size")
        self._compare_value(response.vendor_certificate, bytes_vendor_cert, "Certificate1 vendor contents")

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('8.3.5')
    def test_provision_public_certificates_in_secure_succeeds_replacing_certificates(self):
        """Check that a public certificate may be replaced in SECURE
        """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()
        assert(LifecycleState.SECURE == response.current_lifecycle_state)

        # Provision a sensor certificate from VENDOR_A. This one will be superceded
        self.logger.info('Provision first Sensor Certificate from VENDOR_A')
        vendor_a_sensor_cert_1 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, key_size= 3072)

        # Provision a vendor certificate from VENDOR_A
        self.logger.info('Provision first Vendor Certificate from VENDOR_A')
        vendor_a_vendor_cert_1 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.VENDOR)

        # Provision a sensor and vendor certificates from VENDOR_B - these must not affect the VENDOR_A certificates
        self.logger.info('Provision Sensor Certificate from VENDOR_B')
        vendor_b_sensor_cert_1 = self._set_public_certificates()
        self.logger.info('Provision Vendor Certificate from VENDOR_B')
        vendor_b_vendor_cert_1 = self._set_public_certificates(purpose = CertificatePurpose.VENDOR)

        # Provision another sensor certificate from VENDOR_A, this will supercede the first. Use a smaller size than the first just for variation
        self.logger.info('Provision another Sensor Certificate from VENDOR_A')
        vendor_a_sensor_cert_2 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, CertificateNumber = 200)
        bytes_vendor_a_sensor_cert_2 = bytes(vendor_a_sensor_cert_2)    # for checking later
        len_vendor_a_sensor_cert_2 = len(bytes_vendor_a_sensor_cert_2)    # for checking later

        # Provision another vendor certificate from VENDOR_A, this will supercede the first
        self.logger.info('Provision another Vendor Certificate from VENDOR_A')
        vendor_a_vendor_cert_2 = self._set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.VENDOR, CertificateNumber = 207)
        bytes_vendor_a_vendor_cert_2 = bytes(vendor_a_vendor_cert_2)    # for checking later
        len_vendor_a_vendor_cert_2 = len(bytes_vendor_a_vendor_cert_2)    # for checking later

        # Retrieve and check the replacement vendor and sensor certificates from VENDOR_A
        self.logger.info('Get the VENDOR_A Certificates and check them')
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_A.value;  # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Check fetched certificates against what we provisioned the second time
        self._compare_value(response.sensor_certificate_size_words * 2, len_vendor_a_sensor_cert_2, "Certificate1 sensor size")
        self._compare_value(response.sensor_certificate, bytes_vendor_a_sensor_cert_2, "Certificate1 sensor contents")
        self._compare_value(response.vendor_certificate_size_words * 2, len_vendor_a_vendor_cert_2, "Certificate1 vendor size")
        self._compare_value(response.vendor_certificate, bytes_vendor_a_vendor_cert_2, "Certificate1 vendor contents")

    @testIdentifier('8.3.6')
    def test_provision_multiple_private_keys_in_dm_succeeds(self):
        """Check that multiple sensor authorisation private keys may be provisioned
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a private key from vendor A
        self.logger.info('Provision first Sensor Private Key from VENDOR_A')
        rsa_key_1 = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A) #A)

        # Provision second private key from vendor A
        self.logger.info('Provision second Sensor Private Key from VENDOR_A')
        rsa_key_2 = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A) #A)

        # Provision third private key from vendor A
        self.logger.info('Provision third Sensor Private Key from VENDOR_A')
        rsa_key_3 = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A)
        
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Ensure the three keys are different
        assert (rsa_key_1 != rsa_key_2)
        assert (rsa_key_1 != rsa_key_3)
        assert (rsa_key_2 != rsa_key_3)

    @testIdentifier('8.3.7')
    def test_provision_private_key_in_dm_succeeds_replacing_key(self):
        """Check that sensor authorisation private key may be replaced
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a private key from vendor A
        self.logger.info('Provision first Sensor Private Key from VENDOR_A')
        rsa_key_original = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a Public Certificate from VENDOR_A
        self.logger.info('Provision a Certificate from VENDOR_A')
        pub_cert = RSAPublicCertificate(rsa_key_original, auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        self.provision_asset_cmd.execute(asset, asset_id = 2345, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a replacement private key from vendor A
        self.logger.info('Provision a replacement Sensor Private Key from VENDOR_A')
        rsa_key_replaced = self._set_private_key(auth_id = CertificateAuthority.VENDOR_A)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Provision a replacement Public Certificate from VENDOR_A
        self.logger.info('Provision a replacement Certificate from VENDOR_A')
        pub_cert = RSAPublicCertificate(rsa_key_replaced, auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR, number=101)
        asset = NvmRecordRSAPublicCert(pub_cert)
        print(pub_cert)
        self.provision_asset_cmd.execute(asset, asset_id = 4567, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # start a session using the new replacement private key
        self.logger.info('Start a session using the replacement private Key from VENDOR_A')
        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_replaced)
        self.check_for_expected_status(SystemMgrPhase.SESSION, LifecycleState.DM)

    @testIdentifier('8.4.1')
    def test_provision_trng_characterization_with_kce_in_se(self):
        """Check ProvisionAsset command in Secure
           - use an valid Trng characterization asset package, with a valid payload
           - Try to load the same asset again (EALREADY)
           - Try to load a second asset
           - Try to load a third asset (ENOSPC)
        """
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Check we're in DM:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        asset = NvmRecordTRNGCharacterization(1,2,3,4)

        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM)

        # Check we're in SECURE:CONFIGURE
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Check now trying to load the same one again
        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM, expected=Error.ALREADY)

        # Check now trying to load another asset
        asset = NvmRecordTRNGCharacterization(5,6,7,8)
        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM)

        # Check now trying to load another asset (3rd one should fail)
        asset = NvmRecordTRNGCharacterization(9,10,11,12)
        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM, expected=Error.NOSPC)

        # Check now trying to load a new otpm dummy asset
        otpm_config = OtpmConfig(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)
        asset = NvmRecordOtpmConfig(otpm_config)
        self.provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM)

        self.logger.info('-----------------------------------------------------------------------------')
        self.logger.info('If running on the model you should double check manually the contents of OTPM')
        self.logger.info('Address 0x2C should contain the header for the first nvm_record              ')
        self.logger.info('OTPM should have 3 nvm_records (2 TRNG - 1 OTPM)                             ')
        self.logger.info('-----------------------------------------------------------------------------')

    @testIdentifier('8.5.1')
    def test_provision_otpm_config_with_kce_in_se(self):
        """Check ProvisionAsset command in Secure
           - use an valid OTPM config asset package, with a valid payload
           - Try to load the same asset again (EALREADY)
           - Try to load a second asset
           - Try to load a third asset (ENOSPC)
        """
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Check we're in DM:DEBUG
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        otpm_config = OtpmConfig()
        asset = NvmRecordOtpmConfig(otpm_config)

        self.provision_asset_cmd.execute(bytes(asset), asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.DM)

        # Check we're in SECURE:CONFIGURE
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Check now trying to load the same one again
        # clockSpeedHz = 140000000 -> change clock to force a retrieve of OTPM
        self.provision_asset_cmd.execute(bytes(asset), asset_id = 6789, clock_speed_hz = 140000000, root_of_trust = RootOfTrust.DM, expected=Error.ALREADY)

        # Check now trying to load another asset
        otpm_config = OtpmConfig(tvpps_ns = 10001)
        asset = NvmRecordOtpmConfig(otpm_config)
        self.provision_asset_cmd.execute(bytes(asset), asset_id = 6789, clock_speed_hz = 140000000, root_of_trust = RootOfTrust.DM)

        # Check now trying to load another asset (3rd one should fail)
        otpm_config = OtpmConfig(tvpps_ns = 10002)
        asset = NvmRecordOtpmConfig(otpm_config)

        self.provision_asset_cmd.execute(bytes(asset), asset_id = 6789, clock_speed_hz = 140000000, root_of_trust = RootOfTrust.DM, expected=Error.NOSPC)

        self.logger.info('-----------------------------------------------------------------------------')
        self.logger.info('If running on the model you should double check manually the contents of OTPM')
        self.logger.info('Address 0x2C should contain the header for the nvm_record                    ')
        self.logger.info('OTPM should have 2 nvm_records (2 OTPM)                                      ')
        self.logger.info('-----------------------------------------------------------------------------')


