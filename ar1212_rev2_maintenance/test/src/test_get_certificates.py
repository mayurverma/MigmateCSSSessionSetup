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
# Test GetStatus command.
####################################################################################
import logging
import struct

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from branch_properties import BranchProperties, BranchType
from command_helper import CommandHelper
from lifecycle_state import LifecycleState
from host_command_code import HostCommandCode
from host_command_provision_asset import HostCommandProvisionAsset
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_get_certificates import HostCommandGetCertificatesParams, HostCommandGetCertificatesResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from host_command_get_status import HostCommandGetStatus
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from root_of_trust import RootOfTrust
from patch_type import PatchType
from test_fixture import FixtureType

class TestGetCertificates(BaseTestCase):
    """Test the GetCertificates host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')

    def compare_value(self, a, expected, msg = ""):
        """ Compare two values """

        self.assertEqual(expected, a, 'Comparison {} failed : {} vs {} (expected)'.format(msg, expected, a))

    def set_public_certificates(self, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public certificate and send by ProvisionAsset command"""

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number = 100)
        asset = NvmRecordRSAPublicCert(pub_cert)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        return pub_cert

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

    @testIdentifier('7.1.1')
    def test_get_certificates_none_in_cm(self):
        """Check GetCertificates command when no certificates presents in CM"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.ACCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.1.2')
    def test_get_certificates_none_in_dm(self):
        """Check GetCertificates command when no certificates presents in DM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.NOENT   # Does not exist in NVM! or Error.INVAL?
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.1.3')
    def test_get_certificates_fail_in_cm(self):
        """Check GetCertificates command is not supported in CM"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        # Issue GetCertificates command and it should return ERROR_ACCESS
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.ACCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.2.1')
    def test_get_certificates_in_dm_succeeds_with_valid_certificates(self):
        """Check GetCertificates command in DM
           - use a valid certificate
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)


        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.DM == response.current_lifecycle_state)

        # Create and set RSA public certificate
        sensor_cert = self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_A.value;     # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        bytes_sensor_cert = bytes(sensor_cert)
        len_sensor_cert = len(bytes_sensor_cert)

        self.compare_value(response.sensor_certificate_size_words * 2, len_sensor_cert, "Certificate sensor size")
        self.compare_value(response.sensor_certificate, bytes_sensor_cert, "Certificate sensor contents")
        self.compare_value(response.vendor_certificate_size_words, 0, "Certificate vendor size")

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.2.2')
    def test_get_certificates_in_secure_succeeds_with_valid_certificates(self):
        """Check GetCertificates command in SECURE
           - use valid certificates for sensor and vendor
        """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.SECURE == response.current_lifecycle_state)

        # Create and set RSA public certificate
        sensor_cert = self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)
        print(sensor_cert)

        vendor_cert = self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.VENDOR)
        print(vendor_cert)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_B.value;  # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        bytes_sensor_cert = bytes(sensor_cert)
        len_sensor_cert = len(bytes_sensor_cert)
        bytes_vendor_cert = bytes(vendor_cert)
        len_vendor_cert = len(bytes_vendor_cert)

        self.compare_value(response.sensor_certificate_size_words * 2, len_sensor_cert, "Certificate sensor size")
        self.compare_value(response.sensor_certificate, bytes_sensor_cert, "Certificate sensor contents")
        self.compare_value(response.vendor_certificate_size_words * 2, len_vendor_cert, "Certificate vendor size")
        self.compare_value(response.vendor_certificate, bytes_vendor_cert, "Certificate vendor contents")

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.3.1')
    def test_get_certificates_twice_in_dm(self):
        """Check GetCertificates command twice in DM
           - use a valid certificate and Get Certificates twice
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.DM == response.current_lifecycle_state)

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        # Issue GetCertificates command AGAIN!
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.ACCESS    # Only allow one GetCertificates per session. Error 2nd time.
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.3.2')
    def test_set_session_key_before_get_certificates_in_dm(self):
        """Send SetSesessionKeys before GetCertificates command in DM
           - use a valid certificate and Get Certificates
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.DM == response.current_lifecycle_state)

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
        expected = Error.AGAIN  # Reject and ask for trying again! Can't set session keys before have the certificates.
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.3.3')
    def test_set_session_key_after_get_certificates_in_dm(self):
        """Send SetSesessionKeys after GetCertificates command in DM
           - use a valid certificate and Get Certificates
        """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.DM == response.current_lifecycle_state)

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetSensorId command
        HostCommandGetSensorId(self.camera).execute()

        self.set_ephemeral_master_secret()

        # Issue SetPskSessionKeys command
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = 0
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = 0
        params.controlChannelKeyType = 0
        params.hostSalt = bytes([0]*16)
        response = HostCommandSetPskSessionKeysResponse()
        expected = Error.SUCCESS       # We do need to send GetSensorId before this command.
        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

    @testIdentifier('7.3.4')
    def test_get_certificates_twice_in_secure(self):
        """Check GetCertificates command twice in SECURE
           - use a valid certificate and Get Certificates twice
        """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.SECURE == response.current_lifecycle_state)

        # Create and set RSA public certificate
        self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        # Issue GetCertificates command AGAIN!
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = 1;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.ACCESS    # Only allow one GetCertificates per session. Error 2nd time.
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

