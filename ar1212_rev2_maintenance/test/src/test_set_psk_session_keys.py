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
# Test SetPskSessionKeys correctly.
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
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_provision_asset import HostCommandProvisionAsset
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROIParams, HostCommandSetVideoAuthROIResponse
from nvm_record import NvmRecordPSKMasterSecret
from root_of_trust import RootOfTrust
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from crypto_helper import CryptoHelper
from patch_type import PatchType
from test_fixture import FixtureType
from reset_types import ResetType

class SymmetricAECipherMode(Enum):
    AES_CCM = 0
    AES_GCM = 1

class VideoAuthModeType(Enum):
    FULL_FRAME = 0
    ROI = 1

class PixelPackModeType(Enum):
    OPTIMIZED = 0
    NON_OPTIMIZED = 1
    INVALID = 2


class TestSetPskSessionKeys(BaseTestCase):
    """Test SetPskSessionKeys functionality"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'

        self.logger.debug('setUp complete')
        self.camera.reset(ResetType.SENSOR_SOFT)
        #camera.sensor_hard_reset()

    def get_status(self):
        """Issue the GetStatus command and return current lifecycle"""
        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute()
        return response

    def get_sensor_id(self):
        """ Issue GetSensorId command """
        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id

    def set_ephemeral_master_secret(self, masterSecret = bytes([0]*16)):
        """ Issue SetEthemeralMasterSecret command """
        expected = Error.SUCCESS
        params = HostCommandSetEthemeralMasterSecretParams()
        params.masterSecretKeyType = 0
        params.masterSecret = masterSecret
        response = HostCommandSetEthemeralMasterSecretResponse()
        self.logger.info('Sending SetEthemeralMasterSecret command')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received SetEthemeralMasterSecret response')

    def provision_assets(self, master_secret = bytes(range(16))):
        """ Issue the ProvisionAsset command """

        asset = NvmRecordPSKMasterSecret(master_secret)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 1234, clock_speed_hz = 50000000, root_of_trust = RootOfTrust.CM)


    def set_psk_session_keys(self, videoAuthCipherMode=0, controlChannelCipherMode=0, hostSalt=[0]*16, expectedResponse=Error.SUCCESS):
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

    @testIdentifier('10.1.1')
    def test_sanity_cm(self):
        """Check FW responds to SetPskSessionKeys correctly in CM"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)

        id = self.get_sensor_id()
        self.set_ephemeral_master_secret()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.SUCCESS)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.SESSION == status.current_phase)

    @testIdentifier('10.1.2')
    def test_sanity_dm(self):
        """Check FW responds to SetPskSessionKeys correctly in DM"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        id = self.get_sensor_id()
        self.set_ephemeral_master_secret()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.SUCCESS)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.SESSION == status.current_phase)

    @testIdentifier('10.1.3')
    def test_sanity_se(self):
        """Check FW responds to SetPskSessionKeys correctly in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEthemeralMasterSecret not supported in this LCS
        self.provision_assets()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.SUCCESS)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.SESSION == status.current_phase)

    @testIdentifier('10.2.1')
    def test_psk_can_only_be_applied_once_se(self):
        """Check FW rejects second attempt to apply SetPskSessionKeys"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        # In secure mode secrets must be in OTPM, SetEthemeralMasterSecret not supported in this LCS
        self.provision_assets()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0, hostSalt=[0]*16, expectedResponse=Error.SUCCESS)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.SESSION == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0, hostSalt=[0]*16, expectedResponse=Error.ACCESS)


        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.SESSION == status.current_phase)

    @testIdentifier('10.2.2')
    def test_psk_rejected_if_get_sensor_id_not_called_cm(self):
        """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)

        self.set_ephemeral_master_secret()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.AGAIN)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    @testIdentifier('10.2.3')
    def test_psk_rejected_if_get_sensor_id_not_called_dm(self):
        """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        self.set_ephemeral_master_secret()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.AGAIN)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    @testIdentifier('10.2.4')
    def test_psk_rejected_if_get_sensor_id_not_called_se(self):
        """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        # In secure mode secrets must be in OTPM, SetEthemeralMasterSecret not supported in this LCS
        self.provision_assets()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.AGAIN)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    @testIdentifier('10.2.5')
    def test_psk_rejected_if_no_secret_present_cm(self):
        """Check FW rejects SetPskSessionKeys if master secret not present"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.CM == status.current_lifecycle_state)

        self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.NOENT)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    @testIdentifier('10.2.6')
    def test_psk_rejected_if_no_secret_present_dm(self):
        """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.DM == status.current_lifecycle_state)

        self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.NOENT)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    @testIdentifier('10.2.7')
    def test_psk_rejected_if_no_secret_present_se(self):
        """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        self.set_psk_session_keys(videoAuthCipherMode=0, controlChannelCipherMode=0,  hostSalt=[0]*16, expectedResponse=Error.NOENT)

        status = self.get_status()
        # Check we're in the Session phase
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

    def _set_video_auth_roi_command_encrypted(self, controlKey, controlChannelNonce, encryptionType = SymmetricAECipherMode.AES_CCM,
            videoAuthMode = VideoAuthModeType.FULL_FRAME, expectedResponse = Error.SUCCESS, pixelPackMode = PixelPackModeType.OPTIMIZED):
        # Package and Encrypt

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
        0)                  # self.__frameCounter,

        print("cN:", bytes(controlChannelNonce).hex())
        print("in:", bytes(videoAuthConfigParams).hex())

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


    def _set_video_auth_roi_command(self, videoAuthMode = VideoAuthModeType.FULL_FRAME, expectedResponse = Error.SUCCESS):
        # Issue SetVideoAuthRoi command
        params = HostCommandSetVideoAuthROIParams()
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

        response = HostCommandSetVideoAuthROIResponse()

        self.logger.info('Sending command: SET_VIDEO_AUTH_ROI')
        result = self.command_helper.execute(HostCommandCode.SET_VIDEO_AUTH_ROI, params, response, timeout_usec=50000000, verbosity=logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response: {}'.format(expectedResponse.name))
        return response

    def _get_session_keys(self, master_secret, host_salt, sensor_salt, rom_version, patch_version, soc_id, video_key_size_bytes, control_key_size_bytes):
        salt = bytes(host_salt + sensor_salt)
        context = 'AR0820R2'.encode()
        context += rom_version.to_bytes(2, byteorder='little')
        context += patch_version.to_bytes(2, byteorder='little')
        context += bytes(list(soc_id))
        key_len = video_key_size_bytes + control_key_size_bytes
        keys = self.crypto.hkdf(master_secret, key_len, salt, context)
        video_key = keys[:video_key_size_bytes]
        control_key = keys[control_key_size_bytes:]
        return video_key, control_key

    @testIdentifier('10.3.1')
    def test_psk_master_secret_2_overrides_master_secret_1_se(self):
        """Check master 2 will override master secret 1"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue the GetStatus command to check system is now in the Configure phase
        status = self.get_status()
        assert(LifecycleState.SECURE == status.current_lifecycle_state)

        id = self.get_sensor_id()
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # In secure mode secrets must be in OTPM, SetEphemeralMasterSecret not supported in this LCS
        # Create PskMasterSecret asset encrypted with KPICV_DEFAULT
        masterSecret =  bytes(range(16))
        print(len(masterSecret))
        print(masterSecret)
        self.provision_assets(masterSecret)
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Create a second PskMasterSecret asset encrypted with KPICV_DEFAULT
        masterSecret1 =  bytes([1]*16)
        print(len(masterSecret1))
        print(masterSecret1)
        self.provision_assets(masterSecret1)
        status = self.get_status()
        assert(SystemMgrPhase.CONFIGURE == status.current_phase)

        # Issue SetPskSessionKeys command
        hostSalt=[0]*16
        response = self.set_psk_session_keys(videoAuthCipherMode=1, controlChannelCipherMode=0,  hostSalt=hostSalt, expectedResponse=Error.SUCCESS)
        status = self.get_status()
        assert(SystemMgrPhase.SESSION == status.current_phase)

        # Generate local session keys
        video_key, control_key = self._get_session_keys(masterSecret1, hostSalt, response.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

        # Configure video MAC generation
        self._set_video_auth_roi_command_encrypted(control_key, response.controlChannelNonce)

        status = self.get_status()
        assert(status.current_phase == SystemMgrPhase.SESSION)


    @testIdentifier('10.3.2')
    def test_psk_master_secret_overrides_ephemeral_master_secret_dm(self):
         """Check FW rejects SetPskSessionKeys if GetSensorId not executed"""

         self._common_setup(LifecycleState.DM, PatchType.PATCH)

         # Issue the GetStatus command to check system is now in the Configure phase
         status = self.get_status()
         assert(LifecycleState.DM == status.current_lifecycle_state)

         id = self.get_sensor_id()
         status = self.get_status()
         assert(SystemMgrPhase.CONFIGURE == status.current_phase)

         # Set ephemeral master secret
         masterSecret =  bytes(range(16))
         print(len(masterSecret))
         print(masterSecret)
         self.set_ephemeral_master_secret(masterSecret)
         status = self.get_status()
         assert(SystemMgrPhase.CONFIGURE == status.current_phase)

         # Create a PskMasterSecret asset encrypted with KPICV_DEFAULT
         masterSecret1 =  bytes([1]*16)
         print(len(masterSecret1))
         print(masterSecret1)
         self.provision_assets(masterSecret1)
         status = self.get_status()
         assert(SystemMgrPhase.CONFIGURE == status.current_phase)

         # Issue SetPskSessionKeys command
         hostSalt=[0]*16
         response = self.set_psk_session_keys(videoAuthCipherMode=1, controlChannelCipherMode=0,  hostSalt=hostSalt, expectedResponse=Error.SUCCESS)
         status = self.get_status()
         assert(SystemMgrPhase.SESSION == status.current_phase)

         # Generate local session keys
         video_key, control_key = self._get_session_keys(masterSecret1, hostSalt, response.sensorSalt, status.rom_version, status.patch_version, id, 16, 16)

         # Configure video MAC generation
         self._set_video_auth_roi_command_encrypted(control_key, response.controlChannelNonce)

         status = self.get_status()
         assert(status.current_phase == SystemMgrPhase.SESSION)




