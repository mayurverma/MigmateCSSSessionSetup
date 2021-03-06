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
import sys

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from host_command_depu_program_otpm import HostCommandDepuProgramOtpmParams, HostCommandDepuProgramOtpmResponse
from secure_boot_certificate import CC312LifeCycleState, HbkId
from crypto_helper import CryptoHelper
from patch_type import PatchType
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_apply_debug_entitlement import HostCommandApplyDebugEntitlement
from secure_boot_entitlement import SecureBootDebugEntitlement
from host_session import HostSession, SessionContext, SessionMode, SymmetricAECipherMode, SymmetricAuthCipherMode, AsymmetricCipherMode, SymmetricKeyType
from host_command_provision_asset import HostCommandProvisionAssetParams, HostCommandProvisionAssetResponse
from provision_asset_helper import ProvisionAssetHelper
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from nvm_record import NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROI
from host_command_set_video_auth_roi import VideoAuthModeType, PixelPackModeType
from crypto_services_client import CryptoServicesClient
from branch_properties import BranchProperties
from build_properties import BuildProperties
from root_of_trust import RootOfTrust
from test_fixture import FixtureType
from diag_helper import DiagHelper
from diag_checkpoint import DiagCheckpoint
from host_command_execute_debug import HostCommandExecuteDebug
from reset_types import ResetType
from toolbox import Toolbox
from key_name import KeyName


depu_app_output_dir = BuildProperties["DEPU"].output_path
if depu_app_output_dir not in sys.path:
    sys.path.append(depu_app_output_dir)
from importlib import import_module

class TestDepuValidation(BaseTestCase):
    """Test the SdpuProgramOtpm host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.crypto_helper = CryptoHelper()
        self.rom_version = self.patch_loader.get_rom_version()
        self.verbose = True
        # These tests should be running in devware only but it might be useful sometimes to debug it in simulation:
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.crypto_cm = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)
        self.logger.debug('setUp complete')

    def wait_for_system_ready(self, expected_result=Error.SUCCESS):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))
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

        if self.verbose:
            self.logger.info('Current Phase         = {}'.format(str(response.current_phase)))
            self.logger.info('Current LCS           = {}'.format(str(response.current_lifecycle_state)))
            self.logger.info('ROM Version           = {}'.format(response.rom_version))
            self.logger.info('Patch Version         = {}'.format(response.patch_version))
            self.logger.info('Stack Used Long Words = {}'.format(response.stack_used_long_words))

        return response

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

    def run_depu_program_otpm(self, clockSpeedHz = 50000000, expected_result = Error.SUCCESS):

        depu_app_content = import_module('depu_app_content_{}'.format(self.key_set_name)).DepuAppContent

        # Issue ProgramOtpm command
        params = HostCommandDepuProgramOtpmParams()
        params.clockSpeedHz = clockSpeedHz
        params.blockLengthLongWords = len(depu_app_content)
        params.checksum = Toolbox().calculate_checksum(depu_app_content)
        params.dataBlob = depu_app_content
        response = HostCommandDepuProgramOtpmResponse()
        self.logger.info('Sending command: DEPU_PROGRAM_OTPM')
        result = self.command_helper.execute(HostCommandCode.DEPU_PROGRAM_OTPM, params, response, timeout_usec=500000000, verbosity=logging.INFO, force_get_response = True)

        # Log response
        self.logger.info('Received response ({})'.format(result.name))
        self.logger.info('Error Address = {}'.format(response.error_address))
        self.logger.info('Number of soft Errors = {}'.format(response.number_soft_errors))
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))

        return response

    def get_sensor_id(self):
        """ Issue GetSensorId command """

        response = HostCommandGetSensorId(self.camera).execute()
        # Log response
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

    def provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""
        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)

        params = HostCommandProvisionAssetParams()
        params.assetId = 6789
        params.clockSpeedHz = 27000000
        params.rootOfTrust = 0  # RoT CM

        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number=100)
        asset = NvmRecordRSAPublicCert(pub_cert)

        # Package it up
        params.assetPayload = crypto.encrypt_asset(params.assetId, bytes(asset))
        response = HostCommandProvisionAssetResponse()

        # Issue the command and get the response
        self.logger.info('Sending command PROVISION_ASSET NvmRecordRSAPublicCert')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=500000000, verbosity=logging.INFO)
        self.logger.info('Received response {}'.format(result.name))
        self.assertTrue(result in [Error.SUCCESS, Error.ALREADY], 'Operation failed with error {}'.format(result.name))

        if CertificatePurpose.SENSOR == purpose:
            asset = NvmRecordRSAPrivateKey(rsa_key, auth_id = auth_id)

            # Package it up
            params.assetId = 1234
            params.assetPayload = crypto.encrypt_asset(params.assetId, bytes(asset))
            response = HostCommandProvisionAssetResponse()

            # Issue the command and get the response
            self.logger.info('Sending command PROVISION_ASSET NvmRecordRSAPrivateKey')
            result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=500000000, verbosity=logging.INFO)
            self.logger.info('Received response {}'.format(result.name))
            self.assertTrue(result in [Error.SUCCESS, Error.ALREADY], 'Operation failed with error {}'.format(result.name))


    def provision_master_secret(self, root_of_trust, master_secret):
        """Create RSA public/private key and send by ProvisionAsset command"""

        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = root_of_trust, key_set_name = self.key_set_name)

        params = HostCommandProvisionAssetParams()
        params.assetId = 6789
        params.clockSpeedHz = 27000000
        params.rootOfTrust = root_of_trust.value

        asset = NvmRecordPSKMasterSecret(master_secret)
        # Package it up
        params.assetPayload = crypto.encrypt_asset(params.assetId, bytes(asset))
        response = HostCommandProvisionAssetResponse()

        # Issue the command and get the response
        self.logger.info('Sending command PROVISION_ASSET')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=500000000, verbosity=logging.INFO)
        self.logger.info('Received response PROVISION_ASSET {}'.format(result.name))
        self.assertTrue(result in [Error.SUCCESS, Error.ALREADY], 'Operation failed with error {}'.format(result.name))

    @testIdentifier('22.1.1')
    def test_depu_app_programing_cm(self):
        """Use DepuApp to provision part and check part end up in secure lifecycle state"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        response = self.run_depu_program_otpm()

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)
        patch_version = self.patch_loader.get_patch_version(PatchType.DEPU)
        self.assertEqual(patch_version, response.patch_version)
        self.sensor_soft_reset()

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)
        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)
        soc_id = self.get_sensor_id()

    @testIdentifier('22.2.1')
    def test_depu_loading_patch(self):
        """Check patch can be loaded on a DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

    @testIdentifier('22.2.2')
    def test_depu_loading_debug_entitlement(self):
        """Check application of debug entitlement on a DEPU provisioned device"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, None)

        soc_id = self.get_sensor_id()

        self.sensor_soft_reset()

        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, 0x0010, 0x0010, 0x0010)

    @testIdentifier('22.2.3')
    def test_depu_starting_session(self):
        """Check session initialisation on a DEPU provisioned device"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()

    @testIdentifier('22.2.4')
    def test_depu_loading_debug_entitlement_check_pause(self):
        """Check application of debug entitlement on a DEPU provisioned device and that pause register is accessible"""
        self.sensor_hard_reset()

        self._common_setup(LifecycleState.SECURE, None)

        soc_id = self.get_sensor_id()

        self.sensor_soft_reset()

        HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0, DiagHelper(self.camera).get_info())

        expected = DiagCheckpoint.LIFECYCLE_MGR_CC312_INIT_ENTRY
        DiagHelper(self.camera).set_pausepoint(expected)
        self.assertEqual(0, DiagHelper(self.camera).get_pausepoint())

        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, 0x0008, 0x0008, 0x0008)

        HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0, DiagHelper(self.camera).get_info())

        DiagHelper(self.camera).set_pausepoint(expected)
        self.assertEqual(expected.value, DiagHelper(self.camera).get_pausepoint())

    @testIdentifier('22.2.5')
    def test_depu_loading_debug_entitlement_check_info(self):
        """Check application of debug entitlement on a DEPU provisioned device and that info register is update"""
        self.sensor_hard_reset()

        self._common_setup(LifecycleState.SECURE, None)

        soc_id = self.get_sensor_id()

        self.sensor_soft_reset()

        HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0, DiagHelper(self.camera).get_info())

        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, 0x0010, 0x0010, 0x0010)

        HostCommandGetStatus(self.camera).execute()
        self.assertEqual(HostCommandCode.GET_STATUS, DiagHelper(self.camera).get_info())


    @testIdentifier('22.4.1')
    def test_depu_session_gmac(self):
        """Start gmac session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_GCM_GMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.FULL_FRAME)
        
    def test_depu_session_gmac_16bit_pack42(self):
        """Start gmac session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_GCM_GMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.FULL_FRAME,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x42)
            
    def test_depu_session_gmac_16bit_pack0(self):
        """Start gmac session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_GCM_GMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.FULL_FRAME,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x00)
            
    def test_depu_session_cmac_16bit_pack0(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x00,
            firstRow = 10,
            lastRow = 16,
            rowSkip = 2,
            firstColumn = 22,
            lastColumn = 974,
            columnSkip = 8,
            frameCounter = 0)
            
    def test_depu_session_cmac1_16bit_pack0(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x00,
            firstRow = 52,
            lastRow = 1576,
            rowSkip = 4,
            firstColumn = 50,
            lastColumn = 688,
            columnSkip = 2,
            frameCounter = 0)
            
    def test_depu_session_cmac2_16bit_pack0(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x00,
            firstRow = 4,
            lastRow = 3098,
            rowSkip = 26,
            firstColumn = 22,
            lastColumn = 974,
            columnSkip = 8,
            frameCounter = 0)
    

    @testIdentifier('22.4.2')
    def test_depu_session_cmac_12bit(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x42,
            firstRow = 24,
            lastRow = 1558,
            rowSkip = 3,
            firstColumn = 50,
            lastColumn = 938,
            columnSkip = 2,
            frameCounter = 0)

    @testIdentifier('22.4.3')
    def test_depu_session_cmac_20bit(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.NON_OPTIMIZED,
            pixelPackValue = 0,
            firstRow = 608,
            lastRow = 1318,
            rowSkip = 4,
            firstColumn = 162,
            lastColumn = 496,
            columnSkip = 2,
            frameCounter = 0)
            
    def test_depu_session_cmac_20bit_pack42(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x42,
            firstRow = 10,
            lastRow = 16,
            rowSkip = 2,
            firstColumn = 68,
            lastColumn = 1020,
            columnSkip = 8,
            frameCounter = 0)
            
    def test_depu_session_cmac1_20bit_pack42(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x42,
            firstRow = 922,
            lastRow = 928,
            rowSkip = 2,
            firstColumn = 68,
            lastColumn = 1020,
            columnSkip = 8,
            frameCounter = 0)
            
    def test_depu_session_cmac2_20bit_pack42(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x42,
            firstRow = 10,
            lastRow = 16,
            rowSkip = 2,
            firstColumn = 22,
            lastColumn = 974,
            columnSkip = 8,
            frameCounter = 0)
            
    def test_depu_session_cmac_20bit_pack0(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.OPTIMIZED,
            pixelPackValue = 0x00,
            firstRow = 10,
            lastRow = 16,
            rowSkip = 2,
            firstColumn = 68,
            lastColumn = 1020,
            columnSkip = 8,
            frameCounter = 0)

    @testIdentifier('22.4.4')
    def test_depu_session_cmac_24bit(self):
        """Start session on DEPU provisioned part"""
        self.sensor_soft_reset()

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext(master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP)

        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute(videoAuthMode = VideoAuthModeType.ROI,
            pixelPackMode = PixelPackModeType.NON_OPTIMIZED,
            pixelPackValue = 0,
            firstRow = 946,
            lastRow = 1728,
            rowSkip = 4,
            firstColumn = 2,
            lastColumn = 998,
            columnSkip = 4,
            frameCounter = 0)

