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
# Test Firmware processes the SetVideoAuthRoi command correctly.
####################################################################################
import logging
import struct
from enum import Enum

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from common_error import Error
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_set_session_keys import HostCommandSetSessionKeysParams, HostCommandSetSessionKeysResponse
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROIParams, HostCommandSetVideoAuthROIResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from provision_asset_helper import ProvisionAssetHelper
from host_command_get_certificates import HostCommandGetCertificatesParams, HostCommandGetCertificatesResponse
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from patch_type import PatchType
from host_session import SessionContext
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType
from test_fixture import FixtureType
from reset_types import ResetType

class VideoAuthModeType(Enum):
    FULL_FRAME = 0
    ROI = 1

class PixelPackModeType(Enum):
    OPTIMIZED = 0
    NON_OPTIMIZED = 1
    INVALID = 2


class TestSetVideoAuthRoi(BaseTestCase):
    """Test SetVideoAuthROI functionality"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')
        self.camera.reset(ResetType.SENSOR_HARD)

    def get_status(self):
        """Issue the GetStatus command and return current lifecycle"""
        response = HostCommandGetStatus(self.camera).execute()
        return response

    def get_sensor_id(self):
        """ Issue GetSensorId command """
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id


    def set_ephemeral_master_secret(self, masterSecret = bytes([0]*16), masterSecretKeyType = SymmetricKeyType.AES_128):
        """ Issue SetEphemeralMasterSecret command """
        expected = Error.SUCCESS
        params = HostCommandSetEthemeralMasterSecretParams()
        params.masterSecretKeyType = masterSecretKeyType.value
        params.masterSecret = masterSecret
        response = HostCommandSetEthemeralMasterSecretResponse()
        self.logger.info('Sending command: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def provision_assets(self):
        """ Issue the ProvisionAsset command """

        # Create a PSKMasterSecret128AssetT
        master_secret = bytes([1]*16)
        ProvisionAssetHelper(self.camera, self.key_set_name).provision_psk_master_secret(master_secret, asset_id = 6789)
        return master_secret

    def set_psk_session_keys(self, videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_CCM, videoAuthKeyType = SymmetricKeyType.AES_128, controlChannelKeyType = SymmetricKeyType.AES_128):
        """ Issue SetPskSessionKeys command """

        hostSalt = [0]*16
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = videoAuthCipherMode.value
        params.videoAuthKeyType = videoAuthKeyType.value
        params.controlChannelCipherMode = controlChannelCipherMode.value
        params.controlChannelKeyType = controlChannelKeyType.value
        params.hostSalt = bytes(hostSalt)
        response = HostCommandSetPskSessionKeysResponse()

        expected = Error.SUCCESS
        self.logger.info('Sending SetPskSessionKeys command:')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received SetPskSessionKeys response: {}'.format(expected.name))

        # Log response
        self.logger.info('hostSalt = {}'.format(hostSalt))
        self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
        self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
        self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))

        return response, hostSalt

    def provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""

        ProvisionAssetHelper(self.camera, self.key_set_name).provision_rsa_certificates(rsa_key, auth_id = auth_id, purpose = purpose, number=100)

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
        self.assertTrue(False, "incorrect key size {}".format(key))

    def set_session_keys(self, rsa_key, context, expected = Error.SUCCESS):
        """Run SetSessionKeys command"""

        hostSalt = [0]*16

        params = HostCommandSetSessionKeysParams()
        params.videoAuthCipherMode = context.video_auth_cipher_mode.value
        params.videoAuthKeyType = context.video_auth_key_type.value
        params.controlChannelCipherMode = context.control_channel_cipher_mode.value
        params.controlChannelKeyType = context.control_channel_key_type.value
        params.hostSalt = bytes(hostSalt)

        if SymmetricKeyType.AES_128 == context.master_key_type:
            master_key_size_bytes = 16
        elif SymmetricKeyType.AES_192 == context.master_key_type:
            master_key_size_bytes = 24
        elif SymmetricKeyType.AES_256 == context.master_key_type:
            master_key_size_bytes = 32
        else:
            self.assertTrue(False, "Incorrect master key type")

        masterSecret = bytes([0]*master_key_size_bytes)
        msg = self.build_session_master_secret(masterSecret)
        params.sessionParamsCipher = context.session_params_cipher.value
        params.encryptedParamsSizeWords = len(msg)//2
        params.encryptedParams = self.crypto.rsa_oaep_encrypt(rsa_key, msg)
        response = HostCommandSetSessionKeysResponse()

        self.logger.info('Sending command: SET_SESSION_KEYS')
        result = self.command_helper.execute(HostCommandCode.SET_SESSION_KEYS, params, response, timeout_usec=5000000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        # Log response
        if Error.SUCCESS == expected:
            self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
            self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
            self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))

        return response, masterSecret, hostSalt

    def standard_test(self, rsa_key, session_context,
                      auth_id = CertificateAuthority.VENDOR_B,
                      purpose = CertificatePurpose.SENSOR,
                      expected_lifecycle = LifecycleState.DM):
        """Check FW responds to SetSessionKeys correctly in DM"""

        # Check system is ready
        expected = Error.SUCCESS
        response = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(response, expected.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

        # Issue the GetStatus command to check system is now in the correct lifecycle
        response = self.get_status()
        assert(expected_lifecycle == response.current_lifecycle_state)
        id = self.get_sensor_id()

        # Issue Provision asset command
        if CertificatePurpose.VENDOR == purpose:
            self.provision_rsa_key(rsa_key, auth_id, CertificatePurpose.VENDOR)
        self.provision_rsa_key(rsa_key, auth_id, CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        self.get_certificates(auth_id)

        # Issue SetSessionKeys command
        responseParams, masterSecret, hostSalt = self.set_session_keys(rsa_key, session_context)

        response = self.get_status()
        # Check we're in the Session phase
        assert(response.current_phase == SystemMgrPhase.SESSION)

        return responseParams, masterSecret, hostSalt

    def get_session_keys(self, master_secret, host_salt, sensor_salt, rom_version, patch_version, soc_id, video_key_size_bytes, control_key_size_bytes):
        salt = bytes(host_salt + sensor_salt)
        context = 'AR0820R2'.encode()
        context += rom_version.to_bytes(2, byteorder='little')
        context += patch_version.to_bytes(2, byteorder='little')
        context += bytes(list(soc_id))
        key_len = video_key_size_bytes + control_key_size_bytes
        keys = self.crypto.hkdf(master_secret, key_len, salt, context)
        video_key = keys[:video_key_size_bytes]
        control_key = keys[video_key_size_bytes:]
        return video_key, control_key

    def _set_video_auth_roi_command(self, videoAuthMode = VideoAuthModeType.FULL_FRAME, expectedResponse = Error.SUCCESS, injectBadParams = False):
        # Issue SetVideoAuthRoi command
        params = HostCommandSetVideoAuthROIParams()
        if injectBadParams == False:
            params.videoAuthMode = videoAuthMode.value
            params.pixelPackMode = 0
            params.pixelPackValue = 0
            params.firstRow = 0
            params.lastRow = 0
            params.rowSkip = 0
            params.firstColumn = 0
            params.lastColumn = 0
            params.columnSkip = 0
            params.frameCounter = 0
            params.mac = bytes([0]*16)
        else:
            # out-of-range values
            params.videoAuthMode = VideoAuthModeType.ROI.value # ROI mode, means that most of the following field are 'valid'
            params.pixelPackMode = PixelPackModeType.OPTIMIZED.value
            params.pixelPackValue = 0x1234
            params.firstRow = 2000
            params.lastRow = 1999
            params.rowSkip = 2000
            params.firstColumn = 3000
            params.lastColumn = 2999
            params.columnSkip = 3000
            params.frameCounter = 0xffffffffffffffff  # 64-bit
            params.mac = bytes([0]*16)

        response = HostCommandSetVideoAuthROIResponse()

        self.logger.info('Sending command: SET_VIDEO_AUTH_ROI')
        result = self.command_helper.execute(HostCommandCode.SET_VIDEO_AUTH_ROI, params, response, timeout_usec=50000000, verbosity=logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expectedResponse.name))
        return response

    def _set_video_auth_roi_command_encrypted(self, controlKey, controlChannelNonce, encryptionType = SymmetricAECipherMode.AES_CCM,
            videoAuthMode = VideoAuthModeType.FULL_FRAME, expectedResponse = Error.SUCCESS, injectBadParams = False,
            pixelPackMode = PixelPackModeType.OPTIMIZED, frameCounter = 0):
        # Package and Encrypt

        if injectBadParams == False:
            videoAuthConfigParams = struct.pack("<HHHHHHHHHHHHQ",
            videoAuthMode.value,      # self.__videoAuthMode,
            pixelPackMode.value,                  # self.__pixelPackMode,
            0,                  # self.__pixelPackValue,
            0,                  # self.__firstRow,
            0,                  # self.__lastRow,
            0,                  # self.__rowSkip,
            0,                  # self.__firstColumn,
            0,                  # self.__lastColumn,
            0,                  # self.__columnSkip,
            0,0,0,              # Padding
            frameCounter)       # self.__frameCounter,
        else:
            videoAuthConfigParams = struct.pack("<HHHHHHHHHHHHQ",
            videoAuthMode.value,
            PixelPackModeType.OPTIMIZED.value,
            0x1234,             # self.__pixelPackValue,
            2000,               # self.__firstRow,
            1999,               # self.__lastRow,
            2001,               # self.__rowSkip,
            3000,               # self.__firstColumn,
            2999,               # self.__lastColumn,
            3000,               # self.__columnSkip,
            0,0,0,              # Padding
            frameCounter)       # self.__frameCounter,

        print("controlChannelNonce:", bytes(controlChannelNonce).hex())
        print("videoAuthConfigParams:", bytes(videoAuthConfigParams).hex())

        if SymmetricAECipherMode.AES_GCM == encryptionType:
            print('GCM')
            payload, mac = self.crypto.gcm_encrypt(controlKey, bytes(controlChannelNonce), bytes(videoAuthConfigParams), HostCommandCode.SET_VIDEO_AUTH_ROI.value.to_bytes(2, 'little'))
        else:
            print('CCM')
            payload, mac = self.crypto.ccm_encrypt(controlKey, bytes(controlChannelNonce), bytes(videoAuthConfigParams), HostCommandCode.SET_VIDEO_AUTH_ROI.value.to_bytes(2, 'little'))

        print("payload:", payload.hex())
        print("mac:", mac.hex())

        # Issue ApplyDebugEntitlement command
        params = HostCommandSetVideoAuthROIParams()
        params.encryptedParams = payload
        params.mac = mac
        response = HostCommandSetVideoAuthROIResponse()

        self.logger.info('Sending SetVideoAuthRoi command')
        result = self.command_helper.execute(HostCommandCode.SET_VIDEO_AUTH_ROI, params, response, timeout_usec=50000000, verbosity=logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received SetVideoAuthRoi response: {}'.format(expectedResponse.name))
        return response

    def _increase_nonce(self, nonce):
        """ Increments the nonce, modulo len(nonce)
            - note least-significant byte of the nonce is incremented
        """
        nonce_list = list(nonce)
        for index in range(len(nonce)-1, -1, -1):
            nonce_list[index] += 1
            nonce_list[index] &= 0xFF
            if nonce_list[index] != 0:
                break
        return bytes(nonce_list)

    @testIdentifier('12.1.1')
    def test_sanity_cm(self):
        """Check FW responds to SetVideoAuthROI correctly in CM"""

        self._common_setup(LifecycleState.CM, None)

        # Check system is ready i.e. in CM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)
        assert(SystemMgrPhase.DEBUG == status.current_phase)

        # SetVideoAuthRoi command not valid in DEBUG phase
        self._set_video_auth_roi_command(expectedResponse = Error.ACCESS)

        self.camera.reset(ResetType.SENSOR_HARD)
        self.camera.wait_usec(1000)
        self._wait_for_system_ready()

        self.patch_loader.load_patch(LifecycleState.CM, PatchType.PATCH)

        # get into CONFIGURE phase
        id = self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # SetVideoAuthRoi command not valid in CONFIGURE phase
        self._set_video_auth_roi_command(expectedResponse = Error.ACCESS)
        self.command_helper.wait_for_command_done(verbosity=logging.INFO)

        # get into SESSION phase
        master_secret = bytes([0]*16)
        self.set_ephemeral_master_secret(master_secret)
        responseParams, hostSalt = self.set_psk_session_keys()
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce))

        # Check we're still in SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.2')
    def test_sanity_dm(self):
        """Check FW responds to SetVideoAuthROI correctly in DM"""

        self._common_setup(LifecycleState.DM, None)

        # Check system is ready i.e. in CM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)
        assert(SystemMgrPhase.DEBUG == status.current_phase)

        # SetVideoAuthRoi command not valid in DEBUG phase
        self._set_video_auth_roi_command(expectedResponse = Error.ACCESS)

        self.camera.reset(ResetType.SENSOR_HARD)
        self.camera.wait_usec(1000)
        self._wait_for_system_ready()

        self.patch_loader.load_patch(LifecycleState.DM, PatchType.PATCH)

        # Get phase into CONFIGURE
        id = self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # SetVideoAuthRoi command not valid in CONFIGURE phase
        self._set_video_auth_roi_command(expectedResponse = Error.ACCESS)
        self.command_helper.wait_for_command_done(verbosity=logging.INFO)

        # Get phase into SESSION
        master_secret = bytes([0]*16)
        self.set_ephemeral_master_secret(master_secret)
        responseParams, hostSalt = self.set_psk_session_keys()
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce))

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.3')
    def test_sanity_psk_se(self):
        """Check FW responds to SetVideoAuthROI correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Retrieve patch version which is used by get_session_keys()
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEphemeralMasterSecret not supported in this LCS
        master_secret = self.provision_assets()
        responseParams, hostSalt = self.set_psk_session_keys()

        # Check we're in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce))

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.4')
    def test_sanity_rsa_se(self):
        """Check FW responds to SetVideoAuthROI correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContext()

        responseParams, masterSecret, hostSalt = self.standard_test(rsa_key, session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

        video_key, control_key = self.get_session_keys(masterSecret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.SUCCESS)

        # Check we're in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.5')
    def test_sanity_psk_gcm_se(self):
        """Check FW responds to SetVideoAuthROI correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEphemeralMasterSecret not supported in this LCS
        master_secret = self.provision_assets()
        responseParams, hostSalt = self.set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_GCM)
        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self.logger.info('video_key = {}'.format(bytes(video_key).hex()))
        self.logger.info('control_key = {}'.format(bytes(control_key).hex()))

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_GCM, videoAuthMode=VideoAuthModeType.FULL_FRAME)

        # Check we're in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.7')
    def test_sanity_dm_ccm_256(self):
        """Check FW responds to SetVideoAuthROI correctly in DM, using 256-bit CCM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check system is ready i.e. DM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()

        # Check phase is CONFIGURE
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Get phase into SESSION
        master_secret = bytes([0xAA]*32)
        self.set_ephemeral_master_secret(master_secret, SymmetricKeyType.AES_256)
        responseParams, hostSalt = self.set_psk_session_keys(
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC,
            controlChannelCipherMode = SymmetricAECipherMode.AES_CCM,
            videoAuthKeyType = SymmetricKeyType.AES_128,
            controlChannelKeyType = SymmetricKeyType.AES_256)
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 32)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce),
            encryptionType = SymmetricAECipherMode.AES_CCM,
            videoAuthMode = VideoAuthModeType.ROI)

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.8')
    def test_sanity_dm_gcm_256(self):
        """Check FW responds to SetVideoAuthROI correctly in DM, using 256-bit GCM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check system is ready i.e. DM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()

        # Check phase is CONFIGURE
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Get phase into SESSION
        master_secret = bytes([0xA5]*24)
        self.set_ephemeral_master_secret(master_secret, SymmetricKeyType.AES_192)
        responseParams, hostSalt = self.set_psk_session_keys(
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC,
            controlChannelCipherMode = SymmetricAECipherMode.AES_GCM,
            videoAuthKeyType = SymmetricKeyType.AES_192,
            controlChannelKeyType = SymmetricKeyType.AES_256)
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 24, 32)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce),
            encryptionType = SymmetricAECipherMode.AES_GCM,
            videoAuthMode = VideoAuthModeType.ROI)

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.9')
    def test_sanity_dm_ccm_192(self):
        """Check FW responds to SetVideoAuthROI correctly in DM, using 192-bit CCM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check system is ready i.e. DM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()

        # Check phase is CONFIGURE
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Get phase into SESSION
        master_secret = bytes([0x55]*16)
        self.set_ephemeral_master_secret(master_secret, SymmetricKeyType.AES_128)
        responseParams, hostSalt = self.set_psk_session_keys(
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC,
            controlChannelCipherMode = SymmetricAECipherMode.AES_CCM,
            videoAuthKeyType = SymmetricKeyType.AES_192,
            controlChannelKeyType = SymmetricKeyType.AES_192)
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 24, 24)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce),
            encryptionType = SymmetricAECipherMode.AES_CCM,
            videoAuthMode = VideoAuthModeType.ROI)

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.1.10')
    def test_sanity_dm_gcm_192(self):
        """Check FW responds to SetVideoAuthROI correctly in DM, using 192-bit GCM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Check system is ready i.e. DM and in DEBUG phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()

        # Check phase is CONFIGURE
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Get phase into SESSION
        master_secret = bytes([0xA5]*24)
        self.set_ephemeral_master_secret(master_secret, SymmetricKeyType.AES_192)
        responseParams, hostSalt = self.set_psk_session_keys(
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC,
            controlChannelCipherMode = SymmetricAECipherMode.AES_GCM,
            videoAuthKeyType = SymmetricKeyType.AES_256,
            controlChannelKeyType = SymmetricKeyType.AES_192)
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 32, 24)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce),
            encryptionType = SymmetricAECipherMode.AES_GCM,
            videoAuthMode = VideoAuthModeType.FULL_FRAME)

        # Check we're still in the Session phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.1')
    def test_reject_with_access_no_secrets_se(self):
        """Check FW responds to SetVideoAuthROI with ACCESS error in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEphemeralMasterSecret not supported in this LCS
        #self.set_ephemeral_master_secret()
        #sensorSalt, videoAuthGMACIV, controlChannelNonce = self.set_psk_session_keys()

        # Issue ApplyDebugEntitlement command
        params = HostCommandSetVideoAuthROIParams()
        params.videoAuthMode = 0
        params.pixelPackMode = 0
        params.pixelPackValue = 0
        params.firstRow = 0
        params.lastRow = 0
        params.rowSkip = 0
        params.firstColumn = 0
        params.lastColumn = 0
        params.columnSkip = 0
        params.frameCounter = 0
        params.mac = bytes([0]*16)
        response = HostCommandSetVideoAuthROIResponse()

        expected = Error.ACCESS
        self.logger.info('Sending command: SET_VIDEO_AUTH_ROI')
        result = self.command_helper.execute(HostCommandCode.SET_VIDEO_AUTH_ROI, params, response, timeout_usec=50000, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expected.name))

        status = self.get_status()
        # Check we're in the CONFIGURE phase
        assert(status.current_phase == SystemMgrPhase.CONFIGURE)

    @testIdentifier('12.2.2')
    def test_roi_psk_gcm_video_auth_mode_incorrect_cm(self):
        """Check FW raises error when videoAuthMode (ROI) is incompatible with gcm usage"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # Get phase into SESSION
        master_secret = bytes([0]*16)
        self.set_ephemeral_master_secret(master_secret)
        responseParams, hostSalt = self.set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_GCM)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_GCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.INVAL)

        status = self.get_status()
        # Check we're in the SESSION phase
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.3')
    def test_roi_psk_gcm_video_auth_mode_incorrect_dm(self):
        """Check FW raises error when videoAuthMode (ROI) is incompatible with gcm usage"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # Get phase into SESSION
        master_secret = bytes([0]*16)
        self.set_ephemeral_master_secret(master_secret)
        responseParams, hostSalt = self.set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_GCM)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_GCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.INVAL)

        status = self.get_status()
        # Check we're in the SESSION phase
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.4')
    def test_roi_psk_gcm_video_auth_mode_incorrect_se(self):
        """Check FW raises error when videoAuthMode (ROI) is incompatible with gcm usage"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEphemeralMasterSecret not supported in this LCS
        master_secret = self.provision_assets()
        responseParams, hostSalt = self.set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_GCM)
        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_GCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.BADMSG)

        status = self.get_status()
        # Check we're in the SESSION phase
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.5')
    def test_out_of_range_roi_params_are_not_checked_cm(self):
        """Check FW does not test parameters in SetVideoAuthROI command in CM"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)

        # get into CONFIGURE phase
        id = self.get_sensor_id()
        self.command_helper.wait_for_command_done(verbosity=logging.INFO)

        # get into SESSION phase
        master_secret = bytes([0]*16)
        self.set_ephemeral_master_secret(master_secret)
        responseParams, hostSalt = self.set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        video_key, control_key = self.get_session_keys(master_secret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)
        # Issue SetVideoAuthRoi command
        self._set_video_auth_roi_command_encrypted(control_key, bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_CCM,
            videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.SUCCESS, injectBadParams = True)

        # Check we're still in SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.6')
    def test_out_of_range_roi_params_are_not_checked_se(self):
        """Check FW does not test parameters in SetVideoAuthROI command in SE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContext()

        responseParams, masterSecret, hostSalt = self.standard_test(rsa_key, session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

        video_key, control_key = self.get_session_keys(masterSecret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.SUCCESS, injectBadParams = True)

        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.7')
    def test_invalid_pixel_packing_mode_se(self):
        """Check command reject with BADMSG"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContext()

        responseParams, masterSecret, hostSalt = self.standard_test(rsa_key, session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

        video_key, control_key = self.get_session_keys(masterSecret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.BADMSG, pixelPackMode = PixelPackModeType.INVALID)

        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

    @testIdentifier('12.2.8')
    def test_multiple_roi_updates_se(self):
        """Check that the command can be sent multiple times"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContext()

        responseParams, masterSecret, hostSalt = self.standard_test(rsa_key, session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

        video_key, control_key = self.get_session_keys(masterSecret, hostSalt, responseParams.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=bytes(responseParams.controlChannelNonce), encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, frameCounter=0x12345678)
        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        controlChannelNonce = self._increase_nonce(bytes(responseParams.controlChannelNonce))
        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=controlChannelNonce, encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, frameCounter=0x87654321)
        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        controlChannelNonce = self._increase_nonce(controlChannelNonce)
        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=controlChannelNonce, encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, frameCounter=0xAABBCCDD)
        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

        # Specification not clear but pixel packing mode is only checked on first call to set ROI so subsequent invalid setting is ignored
        controlChannelNonce = self._increase_nonce(controlChannelNonce)
        self._set_video_auth_roi_command_encrypted(controlKey=control_key, controlChannelNonce=controlChannelNonce, encryptionType=SymmetricAECipherMode.AES_CCM, videoAuthMode=VideoAuthModeType.ROI, expectedResponse=Error.SUCCESS, pixelPackMode = PixelPackModeType.INVALID, frameCounter=0xDDCCBBAA)
        # Check we're still in the SESSION phase
        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)

