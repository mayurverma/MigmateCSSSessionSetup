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
# Test SetSessionKeys correctly.
####################################################################################
import logging
from enum import Enum

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from common_error import Error
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_set_session_keys import HostCommandSetSessionKeysParams, HostCommandSetSessionKeysResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_get_certificates import HostCommandGetCertificatesParams, HostCommandGetCertificatesResponse
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROI
from provision_asset_helper import ProvisionAssetHelper
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from root_of_trust import RootOfTrust
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from host_session import HostSession, SessionContext
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType, SessionMode
from patch_type import PatchType
from test_fixture import FixtureType
from reset_types import ResetType

class SessionContextRSA(SessionContext):

    def __init__(self, rsa_key, **kwds):
        super().__init__(**kwds)
        self.__rsa_key = rsa_key

    @property
    def rsa_key(self):
        return self.__rsa_key;
    @rsa_key.setter
    def rsa_key(self, value):
        self.__rsa_key = value


class TestSetSessionKeys(BaseTestCase):
    """Test SetSessionKeys functionality"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')
        self.camera.reset(ResetType.SENSOR_SOFT)

    def get_status(self):
        """Issue the GetStatus command and return current lifecycle"""
        response = HostCommandGetStatus(self.camera).execute()
        return response.current_phase, response.current_lifecycle_state

    def get_sensor_id(self):
        """ Issue GetSensorId command """
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id

    def provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""

        ProvisionAssetHelper(self.camera, self.key_set_name).provision_rsa_certificates(rsa_key, auth_id = auth_id, purpose = purpose, number=100)

    def provision_master_secret(self, master_secret):
        """Create RSA public/private key and send by ProvisionAsset command"""

        ProvisionAssetHelper(self.camera, self.key_set_name).provision_psk_master_secret(master_secret, asset_id = 6789)

    def get_certificates(self, auth_id):
        """Run GetCertificates command"""

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = auth_id.value;
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        self.assertEqual(result, Error.SUCCESS.value, 'Operation failed with error {}'.format(result.name))

    def build_session_master_secret(self, key):
        """ Build the message before encryption for the set session key command """
        pad = bytes([0,0])
        if len(key) == 16:
            return bytes([0,0]) + pad + key
        elif len(key) == 24:
            return bytes([1,0]) + pad + key
        elif len(key) == 32:
            return bytes([2,0]) + pad + key

        # failure condition
        return bytes([3,0]) + pad + key


    def set_session_keys(self, context, expected = Error.SUCCESS):
        """Run SetSessionKeys command"""

        rsa_key = context.rsa_key
        params = HostCommandSetSessionKeysParams()
        params.videoAuthCipherMode = context.video_auth_cipher_mode.value
        params.videoAuthKeyType = context.video_auth_key_type.value
        params.controlChannelCipherMode = context.control_channel_cipher_mode.value
        params.controlChannelKeyType = context.control_channel_key_type.value
        params.hostSalt = bytes([0]*16)

        if SymmetricKeyType.AES_128 == context.master_key_type:
            master_key_size_bytes = 16
        elif SymmetricKeyType.AES_192 == context.master_key_type:
            master_key_size_bytes = 24
        elif SymmetricKeyType.AES_256 == context.master_key_type:
            master_key_size_bytes = 32
        else:
            self.assertTrue(expected != Error.SUCCESS, "Incorrect master key type")
            self.logger.info('Failure condition : 18 bytes key for master secret')
            master_key_size_bytes = 18

        msg = self.build_session_master_secret(bytes([0]*master_key_size_bytes))
        params.sessionParamsCipher = context.session_params_cipher.value
        params.encryptedParamsSizeWords = len(msg)//2
        params.encryptedParams = self.crypto.rsa_oaep_encrypt(rsa_key, msg)
        response = HostCommandSetSessionKeysResponse()

        self.logger.info('Sending command')
        result = self.command_helper.execute(HostCommandCode.SET_SESSION_KEYS, params, response, timeout_usec=500000000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Log response
        if Error.SUCCESS == expected:
            self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
            self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
            self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))


    def standard_test(self, session_context,
                      auth_id = CertificateAuthority.VENDOR_B,
                      purpose = CertificatePurpose.SENSOR,
                      expected_lifecycle = LifecycleState.DM,
                      expected_session_response = Error.SUCCESS,
                      force_wrong_rsa_key = False):
        """Check FW responds to SetSessionKeys"""

        # Issue the GetStatus command to check system is now in the Configure phase
        phase, lifecycle = self.get_status()
        self.assertEqual(expected_lifecycle, lifecycle)
        id = self.get_sensor_id()

        # Issue Provision asset command
        rsa_key = session_context.rsa_key
        if CertificatePurpose.VENDOR == purpose:
            # a vendor random certificate should not break the AR0820FW as it is not used
            rsa_key_vendor = self.crypto.generate_random_rsa_key()
            self.provision_rsa_key(rsa_key_vendor, auth_id, CertificatePurpose.VENDOR)
        self.provision_rsa_key(rsa_key, auth_id, CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        self.get_certificates(auth_id)

        # Issue SetSessionKeys command
        if force_wrong_rsa_key:
            size = session_context.rsa_key.size_in_bits()
            #change the key
            session_context.rsa_key = self.crypto.generate_random_rsa_key(size)
        self.set_session_keys(session_context, expected_session_response)

        phase, lifecycle = self.get_status()
        if Error.SUCCESS == expected_session_response:
            # Check we're in the Session phase
            self.assertEqual(phase, SystemMgrPhase.SESSION)
        else:
            self.assertEqual(phase, SystemMgrPhase.CONFIGURE)


    ########################### 9.0.x : Sanity ############################

    @testIdentifier('9.1.1')
    def test_sanity_dm(self):
        """Check FW responds to SetSessionKeys correctly in DM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.DM)

    @testIdentifier('9.1.2')
    def test_sanity_dm_3072(self):
        """Check FW responds to SetSessionKeys correctly in DM with a 3072 rsa key"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(3072)
        session_context = SessionContextRSA(rsa_key)
        self.standard_test(session_context, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR, LifecycleState.DM)

    @testIdentifier('9.1.3')
    def test_sanity_dm_4096(self):
        """Check FW responds to SetSessionKeys correctly in DM with a 4096 rsa key"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(4096)
        session_context = SessionContextRSA(rsa_key)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.VENDOR, LifecycleState.DM)

    @testIdentifier('9.1.4')
    def test_sanity_secure(self):
        """Check FW responds to SetSessionKeys correctly in SECURE"""
        # Check system is ready

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue Provision asset command
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self.provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)

        for index in range(2):
            self.logger.info(session)
            set_video_auth_roi.execute()

    ########################### 9.1.x : Failure Cases ############################

    @testIdentifier('9.2.1')
    def test_dm_fail_no_soc_id(self):
        """Check SetSessionKeys responds to error when not SOC id command is run before"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        phase, lifecycle = self.get_status()
        assert(LifecycleState.DM == lifecycle)

        # Issue Provision asset command
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self.provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_B)

        # Issue GetCertificates command
        self.get_certificates(CertificateAuthority.VENDOR_B)

        # Issue SetSessionKeys command
        session_context = SessionContextRSA(rsa_key)
        self.set_session_keys(session_context, Error.AGAIN)

        phase, lifecycle = self.get_status()
        # Check we're in the Session phase
        assert(phase == SystemMgrPhase.CONFIGURE)

    def fail_with_no_certificates(self, expected_lifecycle):

        self._common_setup(expected_lifecycle, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        phase, lifecycle = self.get_status()
        assert(expected_lifecycle == lifecycle)
        id = self.get_sensor_id()

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        if LifecycleState.CM != expected_lifecycle:
            # Issue Provision asset command
            self.provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_B)

        if LifecycleState.CM != expected_lifecycle:
            expected_failure = Error.AGAIN
        else:
            expected_failure = Error.ACCESS

        # Issue SetSessionKeys command
        session_context = SessionContextRSA(rsa_key)
        self.set_session_keys(session_context, expected_failure)

        phase, lifecycle = self.get_status()
        # Check we're in the Session phase
        assert(phase == SystemMgrPhase.CONFIGURE)

    @testIdentifier('9.2.2')
    def test_dm_fail_no_certificates(self):
        """Check SetSessionKeys responds to error when not Get Certificates command is run before in DM"""

        self.fail_with_no_certificates(LifecycleState.DM)

    @testIdentifier('9.2.3')
    def test_cm_fail_no_certificates(self):
        """Check SetSessionKeys responds to error when not Get Certificates command is run before in CM"""

        self.fail_with_no_certificates(LifecycleState.CM)

    @testIdentifier('9.2.4')
    def test_secure_fail_no_certificates(self):
        """Check SetSessionKeys responds to error when not Get Certificates command is run before in SECURE"""

        self.fail_with_no_certificates(LifecycleState.SECURE)

    @testIdentifier('9.2.5')
    def test_two_sessions(self):
        """Check SetSessionKeys responds to error when a second session is attempeted in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)
        self.set_session_keys(session_context, expected = Error.ACCESS)

    @testIdentifier('9.2.6')
    def test_bad_video_mode(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         video_auth_cipher_mode = SymmetricAuthCipherMode.RESERVED,
                                         )
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.RANGE)

    @testIdentifier('9.2.7')
    def test_bad_control_mode(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         control_channel_cipher_mode = SymmetricAECipherMode.RESERVED
                                         )
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.RANGE)

    @testIdentifier('9.2.8')
    def test_bad_rsa_mode(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         session_params_cipher = AsymmetricCipherMode.RESERVED)
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.INVAL)

    @testIdentifier('9.2.9')
    def test_bad_master_secret_type(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         master_key_type = SymmetricKeyType.RESERVED)
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.BADMSG)

    @testIdentifier('9.2.10')
    def test_bad_video_key_type(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         video_auth_key_type = SymmetricKeyType.RESERVED,
                                         )
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.RANGE)

    @testIdentifier('9.2.11')
    def test_bad_control_key_type(self):
        """Check SetSessionKeys responds to error when wrong configuration"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         control_channel_key_type = SymmetricKeyType.RESERVED
                                         )
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.RANGE)

    @testIdentifier('9.2.12')
    def test_session_after_failure(self):
        """Check SetSessionKeys responds to error when a second call is attempeted in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         control_channel_key_type = SymmetricKeyType.RESERVED
                                         )
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.RANGE)

        # run now a correct session
        session_context = SessionContextRSA(rsa_key)
        self.set_session_keys(session_context, expected = Error.ACCESS)

    @testIdentifier('9.2.13')
    def test_rsa_session_after_psk_session_ccm(self):
        """Check FW responds to SetSessionKeys correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue Provision asset command
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self.provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR)

        aes_key = self.crypto.generate_random_aes_key()
        self.provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)

        # Issue SetSessionKeys command
        session_context = SessionContextRSA(rsa_key)
        self.set_session_keys(session_context, Error.ACCESS)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)

        for index in range(2):
            self.logger.info(session)
            set_video_auth_roi = HostCommandSetVideoAuthROI(session)


    @testIdentifier('9.2.14')
    def test_rsa_session_after_psk_session_gcm(self):
        """Check FW responds to SetSessionKeys correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue Provision asset command
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        self.provision_rsa_key(rsa_key, CertificateAuthority.VENDOR_A, CertificatePurpose.SENSOR)

        aes_key = self.crypto.generate_random_aes_key()
        self.provision_master_secret(aes_key)

        context = SessionContext(control_channel_cipher_mode = SymmetricAECipherMode.AES_GCM)
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)

        # Issue SetSessionKeys command
        session_context = SessionContextRSA(rsa_key)
        self.set_session_keys(session_context, Error.ACCESS)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)

        for index in range(2):
            self.logger.info(session)
            set_video_auth_roi = HostCommandSetVideoAuthROI(session)


    @testIdentifier('9.2.15')
    def test_session_with_wrong_rsa_key_se(self):
        """Check SetSessionKeys responds to error when a call is attempeted with the wrong key in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key)
        self.standard_test(session_context,
                           CertificateAuthority.VENDOR_B,
                           CertificatePurpose.SENSOR,
                           LifecycleState.SECURE,
                           expected_session_response = Error.BADMSG,
                           force_wrong_rsa_key = True)


    ########################### 9.2.x : different configurations ############################

    @testIdentifier('9.3.1')
    def test_symmetric_key_192(self):
        """Check SetSessionKeys when using 192 bits master key"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key, master_key_type = SymmetricKeyType.AES_192)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

    @testIdentifier('9.3.2')
    def test_symmetric_key_256(self):
        """Check SetSessionKeys when using 256 bits master key"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key, master_key_type = SymmetricKeyType.AES_256)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

    @testIdentifier('9.3.3')
    def test_gmac_gcm(self):
        """Check SetSessionKeys when using GMAC / GCM modes"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         video_auth_cipher_mode = SymmetricAuthCipherMode.AES_GCM_GMAC,
                                         control_channel_cipher_mode = SymmetricAECipherMode.AES_GCM
                                         )
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

    @testIdentifier('9.3.4')
    def test_aes_keys_192_128(self):
        """Check SetSessionKeys when using 192 control and 128 video key sizes"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key, control_channel_key_type = SymmetricKeyType.AES_192)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

    @testIdentifier('9.3.5')
    def test_aes_keys_256_192(self):
        """Check SetSessionKeys when using 256 control and 192 video key sizes"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                                video_auth_key_type = SymmetricKeyType.AES_192,
                                                control_channel_key_type = SymmetricKeyType.AES_256)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

    @testIdentifier('9.3.6')
    def test_aes_keys_256_256(self):
        """Check SetSessionKeys when using 256 control and 256 video key sizes"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContextRSA(rsa_key,
                                         control_channel_key_type = SymmetricKeyType.AES_256,
                                         video_auth_key_type = SymmetricKeyType.AES_256)
        self.standard_test(session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

