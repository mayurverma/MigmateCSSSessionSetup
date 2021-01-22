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
# Test Enable Health Monitor command.
####################################################################################
import logging
import struct
import datetime

from enum import Enum
from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from patch_helper import *
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from diag_helper import DiagHelper
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError
from host_command_load_patch_chunk import HostCommandLoadPatchChunkParams, HostCommandLoadPatchChunkResponse, HostCommandLoadPatchChunk

from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse
from provision_asset_helper import ProvisionAssetHelper
from host_command_enable_monitoring import HostCommandEnableMonitoringParams, HostCommandEnableMonitoringResponse, HostCommandEnableMonitoring
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROI
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret
from host_session import HostSession, SessionContext, SessionMode
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType
from patch_type import PatchType
from host_command_execute_debug import HostCommandExecuteDebug, HostCommandExecuteDebugResponse
from verification_app_debug_id import DebugId
from test_fixture import FixtureType

class TestEnableMonitoring(BaseTestCase):
    """Test the Enable Monitoring host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.patch_version = 1
        self.get_status = HostCommandGetStatus(self.camera)
        self.get_sensor_id = HostCommandGetSensorId(self.camera)
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.logger.debug('setUp complete')

    def _wait_until_target_ready_for_command(self, expectedResponse = Error.SUCCESS):
        """ Check system is ready by polling the doorbell bit """
        response = self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        self.assertEqual(response, expectedResponse.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

    def _get_session_keys(self, master_secret, host_salt, sensor_salt, rom_version, patch_version, soc_id, video_key_size_bytes, control_key_size_bytes):
        salt = bytes(host_salt + sensor_salt)
        context = 'AR0820R2'.encode()
        context += rom_version.to_bytes(2, byteorder = 'little')
        context += patch_version.to_bytes(2, byteorder = 'little')
        context += bytes(list(soc_id))
        key_len = video_key_size_bytes + control_key_size_bytes
        keys = CryptoHelper().hkdf(master_secret, key_len, salt, context)
        video_key = keys[:video_key_size_bytes]
        control_key = keys[control_key_size_bytes:]
        return video_key, control_key

    def _provision_master_secret(self, master_secret):
        """Create RSA public/private key and send by ProvisionAsset command"""
        ProvisionAssetHelper(self.camera, self.key_set_name).provision_psk_master_secret(master_secret, asset_id = 6789)

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

    def _set_psk_session_keys(self, videoAuthCipherMode = SymmetricAuthCipherMode.AES_GCM_GMAC, controlChannelCipherMode = SymmetricAECipherMode.AES_CCM):
        """ Issue SetPskSessionKeys command """

        hostSalt = [0] * 16
        params = HostCommandSetPskSessionKeysParams()
        params.videoAuthCipherMode = videoAuthCipherMode.value
        params.videoAuthKeyType = 0
        params.controlChannelCipherMode = controlChannelCipherMode.value
        params.controlChannelKeyType = 0
        params.hostSalt = bytes(hostSalt)
        response = HostCommandSetPskSessionKeysResponse()

        expected = Error.SUCCESS
        self.logger.info('Sending SetPskSessionKeys command:')
        result = self.command_helper.execute(HostCommandCode.SET_PSK_SESSION_KEYS, params, response, timeout_usec = 50000, verbosity = logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received SetPskSessionKeys response: {}'.format(expected.name))

        # Log response
        self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
        self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
        self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))

        return response.sensorSalt, response.videoAuthGMACIV, response.controlChannelNonce, hostSalt

    def _enable_monitoring(self, commandParams, expectedResponse = Error.SUCCESS):
        """ Issue the EnableMonitoring command and test the response"""

        response = HostCommandEnableMonitoringResponse()
        self.logger.info('Sending command: ENABLE_MONITORING')
        result = self.command_helper.execute(HostCommandCode.ENABLE_MONITORING, commandParams, response, verbosity = logging.INFO)
        self.assertEqual(result, expectedResponse.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def _load_patch_chunk_zero(self):
        """Loads the first chunk of a valid patch using a 'LoadPatchChunk' command """

        # Read the patch image data
        patch_image_file = self.patch_loader.get_patch_binary_path(PatchType.VERIFICATION)
        with open(patch_image_file, 'rb') as istream:
            patch_image = istream.read()

        # Max chunk size is 512 bytes minus the packaged asset header (20 bytes)
        # TODO: <PH>: need to retrieve load_address and loader_address from the create_patch.py script
        patch_data = Chunk(rom_version = self.rom_version,
            patch_version = self.patch_version, load_address = 0x20000000, loader_address = 0x20000019,
            max_chunk_size = (512 - 20), data = patch_image)

        chunk = patch_data.chunks[0]

        cmd = Command().generate(chunk.asset_id(), chunk.image())
        self.logger.debug(cmd)
        asset = cmd.image()

        HostCommandLoadPatchChunk(self.camera).execute(asset, Error.SUCCESS)

    @testIdentifier('15.1.1')
    def test_enable_monitoring_bad_phase(self):
        """ Enable Monitoring command in debug phase will fail """

        self._common_setup(LifecycleState.CM, None)

        # Call the Enable Monitoring command in the DEBUG phase - this will fail
        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = bytes([0] * 8)
        params.mac = bytes([0] * 16)

        self._enable_monitoring(params, expectedResponse = Error.ACCESS)

    @testIdentifier('15.1.2')
    def test_enable_monitoring_cm(self):
        """ Get the target into SESSION phase and Enable Monitoring in CM """

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)

        # get into CONFIGURE phase
        id = self.get_sensor_id.execute()
        self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Enable the Health Monitor
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        extClockFreqHz = 6000000    # 6MHz
        pllFreqHz = 100000000       # 100MHz
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)

        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))

        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # let the sim run long enough to prove that WDOG is operating correctly
        self.fixture.wait_usec(1200000)     # 1.2 secs

    @testIdentifier('15.1.3')
    def test_enable_monitoring_se(self):
        """ Get the target into SESSION phase and Enable Monitoring in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(6000000, 20000000)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

        # wait a while then check status. Repeat for longer than a WDOG Reset could appear (if not being patted correctly)
        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

    @testIdentifier('15.1.4')
    def test_enable_monitoring_dm(self):
        """ Get the target into SESSION phase and Enable Monitoring in DM """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(6000000, 20000000)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

        # wait a while then check status. Repeat for longer than a WDOG Reset could appear (if not being patted correctly)
        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

    @testIdentifier('15.1.5')
    def test_enable_monitoring_reject_dm(self):
        """ Check second enable monitoring command is rejected dm """

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(6000000, 20000000)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

        # wait a while then check status. Repeat for longer than a WDOG Reset could appear (if not being patted correctly)
        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        enable_monitoring.execute(6000000, 20000000, expected = Error.ALREADY)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

    @testIdentifier('15.1.6')
    def test_enable_monitoring_reject_se(self):
        """ Check second enable monitoring command is rejected in secure"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(6000000, 20000000)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

        # wait a while then check status. Repeat for longer than a WDOG Reset could appear (if not being patted correctly)
        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        self.logger.info('Wait for 100ms')
        self.fixture.wait_usec(100000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        enable_monitoring.execute(6000000, 20000000, expected = Error.ALREADY)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

    @testIdentifier('15.1.7')
    def test_enable_monitoring_invalid_ext_clock_dm(self):
        """ Check invalid parameters are rejected"""

        self._common_setup(LifecycleState.DM, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(5000000, 20000000, expected = Error.RANGE)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

    @testIdentifier('15.1.8')
    def test_enable_monitoring_invalid_ext_clock_se(self):
        """ Check invalid parameters are rejected with BADMSG in secure"""

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.SECURE, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(5000000, 20000000, expected = Error.BADMSG)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

    @testIdentifier('15.1.9')
    def test_enable_monitoring_bad_phase_patch_cm(self):
        """ Enable Monitoring command in patch phase will fail in cm"""

        self._common_setup(LifecycleState.CM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        # get the target into patch phase
        self._load_patch_chunk_zero()
        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.PATCH, status.current_phase)

        # Call the Enable Monitoring command in the PATCH phase - this will fail
        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = bytes([0] * 8)
        params.mac = bytes([0] * 16)

        self._enable_monitoring(params, expectedResponse = Error.ACCESS)

    @testIdentifier('15.1.10')
    def test_enable_monitoring_bad_phase_configure_cm(self):
        """ Enable Monitoring command in patch phase will fail in cm """

        self._common_setup(LifecycleState.CM, None)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.DEBUG, status.current_phase)

        # get into CONFIGURE phase
        id = self.get_sensor_id.execute()
        self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # Call the Enable Monitoring command in the CONFIGURE phase - this will fail
        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = bytes([0] * 8)
        params.mac = bytes([0] * 16)
        self._enable_monitoring(params, expectedResponse = Error.ACCESS)

    @testIdentifier('15.1.11')
    def test_enable_monitoring_reject_cm(self):
        """ Check second enable monitoring command is rejected cm"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)

        # get into CONFIGURE phase
        id = self.get_sensor_id.execute()
        self.command_helper.wait_for_command_done(verbosity = logging.INFO)
        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Enable the Health Monitor
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        extClockFreqHz = 6000000    # ExtClkHz = 6MHz
        pllFreqHz = 20000000        # PllClkHz = 20MHz (actually it's 100MHz)
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)
        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))

        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # wait until target is ready for next command then try it a second time
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params, Error.ALREADY)

    @testIdentifier('15.2.1')
    def test_enable_monitoring_check_stack_full_cm(self):
        """ Call patch function which exhausts stack and check monitor worked as expected """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # Execute patch id 0
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NO_ACTION)

        id = self.get_sensor_id.execute()
        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Call the stack overflow test function in the verification patch.
        # Since the health monitor is not yet running, this should not cause an error
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.FILL_STACK)
        self.logger.info('Fill the stack. Target should not FATAL error since Health Monitor not yet running')
        status = self.get_status.execute()
        self.logger.info('Wait for 1 second')
        self.fixture.wait_usec(62500)     # 62.5ms
        self.logger.info('Check there\'s no FATAL error')
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

        # Enable the Health Monitor
        self.logger.info('Enable the Health Monitor')
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        # trick the sim into running sixteen times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        extClockFreqHz = 6000000    # 6MHz
        pllFreqHz = 6250000         # PllClkHz = 6.25MHz (actually it's 100MHz, 16x faster)
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)
        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))
        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # let the sim run long enough to allow Health Monitor to detect stack overrun
        self.logger.info('Watchdog must detect the stack overrun in less than 100ms - so pause for 96ms and check')
        self.fixture.wait_usec(6000)     # 6ms, which is 96ms in real time
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_FATAL, fault)
        self.assertEqual(SystemMgrFatalError.HEALTH_MONITOR_STACK_OVERRUN, context)
        self.assertEqual(1024, info)

    @testIdentifier('15.2.2')
    def test_enable_monitoring_fw_lockup_cm(self):
        """ Call patch function which executes an infinite loop and check monitor worked as expected """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        # Execute patch id 0
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NO_ACTION)

        id = self.get_sensor_id.execute()
        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Enable the Health Monitor
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        # trick the sim into running sixteen times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        extClockFreqHz = 6000000    # ExtClkHz = 6MHz
        pllFreqHz = 6250000         # PllClkHz = 6.25MHz (actually it's 100MHz, 16x faster)
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)

        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))

        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # let the sim run a short while enough to prove that WDOG is operating correctly
        self.logger.info('Pause to ensure WatchDog is operating correctly')
        self.fixture.wait_usec(7000)     # 7ms, which is two Watchdog Interrupts, which is 100ms at normal speed
        self.logger.info('Pause complete')
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

        # call the lockup test function in the verification patch
        self.logger.info('Execute the debug code to put the CPU in an infinite loop')
        try:
            response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYS_EXIT, expected = Error.SUCCESS, timeout_usec = 7000)
        except TimeoutError:
            self.logger.info("Debug command timeout received, as expected. CPU is stuck in a loop. Watchdog should kick in")
        else:
            self.assertTrue(False, "Debug Command failed to lock up the CPU")

        # wait until the watchdog has expired. That should be in 62.5ms because it's running 16x faster than usual
        self.fixture.wait_usec(63500)     # 63.5ms.  Allow 1ms extra time

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.WATCHDOG, fault)
        # There is no requirement to read/verify what the CSS has written to its info and context registers

    @testIdentifier('15.3.1')
    def test_enable_monitoring_set_short_timer_interval_cm(self):
        """ Call patch function which sets timer interval which is shorter than pause interval so fatal error triggered """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        id = self.get_sensor_id.execute()
        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Enable the Health Monitor
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        extClockFreqHz = 6000000    # 6MHz
        pllFreqHz = 100000000       # 100MHz
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)

        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))

        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # Set new timer interval (50 mSec )
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SET_WATCHDOG_TIMER_INTERVAL,
            parameter = 50000)

        self.assertEqual(50000, response.result)

        # Let the device run long enough to prove that WDOG is operating correctly
        self.logger.info('Pause to ensure WatchDog is operating correctly')
        self.fixture.wait_usec(60000)
        self.logger.info('Pause complete')

        status = self.get_status.execute()

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.FILL_STACK)
        status = self.get_status.execute()
        self.logger.info('Pause to ensure WatchDog has time to trigger')
        self.fixture.wait_usec(60000)
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_FATAL, fault)
        self.assertEqual(SystemMgrFatalError.HEALTH_MONITOR_STACK_OVERRUN, context)
        self.assertEqual(1024, info)

    @testIdentifier('15.3.2')
    def test_enable_monitoring_set_long_timer_interval_cm(self):
        """ Call patch function which sets timer interval which is longer than pause interval so fatal error not triggered """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        id = self.get_sensor_id.execute()
        # get into SESSION phase
        self._set_ephemeral_master_secret()
        sensorSalt, videoAuthGMACIV, controlChannelNonce, hostSalt = self._set_psk_session_keys(videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC)
        status = self.get_status.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        # Enable the Health Monitor
        master_secret = bytes([0] * 16)
        video_key, control_key = self._get_session_keys(master_secret, hostSalt, sensorSalt, status.rom_version, status.patch_version, id.unique_id, 16, 16)

        extClockFreqHz = 6000000    # 6MHz
        pllFreqHz = 100000000       # 100MHz
        enableHealthMonFreqParams = struct.pack("<LL", extClockFreqHz, pllFreqHz)

        payload, mac = CryptoHelper().ccm_encrypt(control_key, bytes(controlChannelNonce), bytes(enableHealthMonFreqParams), HostCommandCode.ENABLE_MONITORING.value.to_bytes(2, 'little'))

        params = HostCommandEnableMonitoringParams()
        params.encryptedParams = payload
        params.mac = mac
        self._wait_until_target_ready_for_command()
        self._enable_monitoring(params)

        # Set new timer interval (250 mSec (maximum practical value))
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SET_WATCHDOG_TIMER_INTERVAL,
            parameter = 250000)

        self.assertEqual(250000, response.result)

        # Let the device run long enough to prove that WDOG is operating correctly
        self.logger.info('Pause to ensure WatchDog is operating correctly (300 mSec)')
        self.logger.info(datetime.datetime.now())
        self.fixture.wait_usec(300000)
        self.logger.info(datetime.datetime.now())
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()

        status = self.get_status.execute()

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.FILL_STACK)
        status = self.get_status.execute()

        self.logger.info('Pause for same period as in test test_enable_monitoring_set_short_timer_interval_cm (60 mSec),')
        self.logger.info('however in this case watchdog should not trigger (no way to sync so could fail?)')
        self.fixture.wait_usec(60000)
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

    @testIdentifier('15.3.3')
    def test_enable_monitoring_set_danger_zone_size_cm(self):
        """ Call patch function which sets danger zone size """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.CM, status.current_lifecycle_state)
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SET_WATCHDOG_DANGER_ZONE_SIZE,
            parameter = 64)
        self.assertEqual(64, response.result)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SET_WATCHDOG_DANGER_ZONE_SIZE,
            parameter = 32)
        self.assertEqual(32, response.result)

    @testIdentifier('15.3.4')
    def test_enable_monitoring_vcore_error_dm(self):
        """ Force a vcore error, check monitor identifies and handles it correctly """

        self._common_setup(LifecycleState.DM, PatchType.VERIFICATION)

        status = self.get_status.execute()
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        aes_key = bytes([0] * 16)
        self._provision_master_secret(aes_key)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = aes_key)
        set_video_auth_roi = HostCommandSetVideoAuthROI(session)
        set_video_auth_roi.execute()

        enable_monitoring = HostCommandEnableMonitoring(session)

        # trick the sim into running five times faster by lying about the PLL clock frequency. It's actually 100MHz in secure_tb
        enable_monitoring.execute(6000000, 20000000)   # ExtClkHz = 6MHz, PllClkHz = 20MHz (actually it's 100MHz)

        # wait a while then check status. Repeat for longer than a WDOG Reset could appear (if not being patted correctly)
        self.logger.info('Wait for 300ms')
        self.fixture.wait_usec(300000)  # 100ms
        status = self.get_status.execute()
        self.assertEqual(SystemMgrPhase.SESSION, status.current_phase)
        self.assertEqual(LifecycleState.DM, status.current_lifecycle_state)

        # Set new timer interval (250 mSec (maximum practical value))
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.AESCMAC_STATUS,
            parameter = 0)
        self.assertEqual(0, response.result)

        # Let the device run long enough to prove that WDOG is operating correctly
        self.logger.info('Pause to ensure WatchDog is operating correctly (300 mSec)')
        self.logger.info(datetime.datetime.now())
        self.fixture.wait_usec(300000)
        self.logger.info(datetime.datetime.now())
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()

        status = self.get_status.execute()

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.NONE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.AESCMAC_STATUS,
            parameter = 0xffffffff)
        self.assertEqual(0xffffffff, response.result)

        status = self.get_status.execute()

        self.fixture.wait_usec(60000)
        self.logger.info('Pause complete')

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.FIRMWARE_FATAL, fault)
        self.assertEqual(SystemMgrFatalError.HEALTH_MONITOR_VCORE_ERROR , context)