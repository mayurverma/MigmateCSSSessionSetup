####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Test provisioning on a DEPU part, will write to OTPM. Intended to be used for
# one off manual testing
####################################################################################
import logging
import sys
import time

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from host_command_depu_program_otpm import HostCommandDepuProgramOtpmParams, HostCommandDepuProgramOtpmResponse
from host_command_provision_asset import HostCommandProvisionAsset
from patch_type import PatchType
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from host_command_get_sensor_id import HostCommandGetSensorId
from host_session import HostSession, SessionContext, SessionMode
from host_command_provision_asset import HostCommandProvisionAssetParams, HostCommandProvisionAssetResponse
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from nvm_record import NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret, NvmRecordTRNGCharacterization, NvmRecordOtpmConfig
from otpm_config import OtpmConfig
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROI
from crypto_services_client import CryptoServicesClient
from crypto_helper import CryptoHelper
from branch_properties import BranchProperties
from build_properties import BuildProperties
from root_of_trust import RootOfTrust
from test_fixture import FixtureType
from reset_types import ResetType
from toolbox import Toolbox
from key_name import KeyName

depu_app_output_dir = BuildProperties["DEPU"].output_path
if depu_app_output_dir not in sys.path:
    sys.path.append(depu_app_output_dir)
from importlib import import_module

class TestDepuProvisioning(BaseTestCase):
    """Test provisioning on a real part using DevWare"""

    # Hard coded RSA key values
    RSA_KEY_MODULUS = 0xbf5dc7afd4228245b9eca696d1fcaab9a81991a76f12bf451623ba66867b1a40e9205aa36097d7789f17b9b828c834eeabda83f2413ab47f202f35506e6f4c445f40eff920576a31c02496b6246cf974fee683691c3cd2342009f3676ce77ab170b640375057a29509806e3498d09ed273a2aa11890be39463b256ca6af4e15e4038c5e459e70f182423111111d91a148e9363edb3c107de264d734333ae76235e49f0fd41e06ad078cadf9382d5bb27c1688a48bd84fb4932a8932e55b8336d1e3f6cefed84de4ce86e07a232c8a238549dcfadc15e08b5d997eb65d3daf0c58db03ed5e7f750eb8f546e75b3f48b836bf6a4107f740a8b362d9c69310edb678ecc78a10f78408ab7a25bc02d03ed020725fd79c3bb559719697fe1e3149961abe88bf62cf518065186bc94319ca1ed41de1ed03763caa4b461143488481b6e10f6e1114f2a1bf61c0913fa89104fc0a5be762534480bd23e84479d82f0eeb4360b0deef89120c911e5b486dadaec9b20ba99291c8c8b120dc4ee3ec75e0231
    RSA_KEY_PRIV_EXP = 0x395bc71c2d25f0cd2c76c1d207fbc742619fd4580ec35c403d6c151560eeaa0655ba0460def57438abdae5284d5501049ecf1ee87063c3b12cf8e96e83c369653ef5cc57ec393753d527426332d4d50a9bd28505eed84e1b20954af72f9dcc593e12ea7989e3d290006a95ddbf061cecb08b4b9578f1eed34df4a8ee0014092f0269e1ead98597b12137bf8851c4b47bffe62eb8133e62f8d39b3856a6b97ea08844fa6e369e28f5f696bc5fcf01f81868f0c82e185c3123960310267dbbf9f190703c4226b63eb6fe0c195ecbbce94a88d3a62834752d7592ef06403a07bccf6425961b19b8aa1c3249b25249da0dfcb50cd29dfcb0703b7279f84455fc0590212f55c872273c1c3d42bd7615f57c6ff08ce5fbd8c1c46ce44aad7963d0c64ced195a80a3b4cb3f310e77d549d7e504ebc1f794b1f5a09d03fcd149f67fee94a040d273818756270f5bb7b6b70ab98d4683e53f8c3cdf2253b7a51d31b78eb38d2ffaab76ed360f6cb1294703949bc577258a2932c771ef62e05bb23ec1f8f7

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.rom_version = self.patch_loader.get_rom_version()
        self.verbose = True
        # These tests should be running in DevWare only but it might be useful sometimes to debug it in simulation:
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.crypto_cm = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)
        self.logger.debug('setUp complete')

    def run_depu_program_otpm(self, clockSpeedHz = 50000000, expected_result = Error.SUCCESS):

        if (self.fixture.get_type() in (FixtureType.MIGMATE, FixtureType.DEVWARE)):
            self.fail("Depu programming should NOT be attempted on DEVWARE or MIGMATE")

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

    def provision_with_depu(self):
        """ provision part using DEPU"""

        if (self.fixture.get_type() in (FixtureType.MIGMATE, FixtureType.DEVWARE)):
            self.fail("Depu programming should NOT be attempted on DEVWARE or MIGMATE")

        self.sensor_soft_reset()

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        response = self.run_depu_program_otpm()

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, response.current_phase)
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)

        patch_version = self.patch_loader.get_patch_version(PatchType.DEPU)
        self.assertEqual(patch_version, response.patch_version)
        self.sensor_soft_reset()

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(SystemMgrPhase.DEBUG, response.current_phase)
        self.assertEqual(LifecycleState.SECURE, response.current_lifecycle_state)
        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

    def wait_for_system_ready(self, expected_result=Error.SUCCESS):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

    def get_sensor_id(self):
        """ Issue GetSensorId command """

        response = HostCommandGetSensorId(self.camera).execute()
        # Log response
        self.logger.info('Unique ID = {}'.format(str(response.unique_id)))

        return bytes(response.unique_id)

    def sensor_hard_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_HARD)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self.wait_for_system_ready()

    def sensor_soft_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
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
        params.assetPayload = crypto.encrypt_asset(params.assetId, bytes(asset)) # kpicv
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
        # Package it up, encrypt using Kcp (for DM) or Kpicv (for CM)
        params.assetPayload = crypto.encrypt_asset(params.assetId, bytes(asset))
        response = HostCommandProvisionAssetResponse()

        # Issue the command and get the response
        self.logger.info('Sending command PROVISION_ASSET')
        result = self.command_helper.execute(HostCommandCode.PROVISION_ASSET, params, response, timeout_usec=500000000, verbosity=logging.DEBUG)

        self.logger.info("Result: {}".format(result.name))
        self.assertTrue(result in [Error.SUCCESS, Error.ALREADY], 'Operation failed with error {}'.format(result.name))

    @testIdentifier('22.5.1')
    def test_depu_provision_psk_with_kcp(self):
        """Provision kcp and setup session using PSK on DEPU provisioned device"""

        if (self.fixture.get_type() in (FixtureType.SIM, FixtureType.MODEL)):
            self.provision_with_depu()

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Psk key 0-15
        master_secret = bytes(range(16))
        self.logger.info('Psk Key = {}'.format(master_secret.hex()))
        self.provision_master_secret(RootOfTrust.DM, master_secret)

    @testIdentifier('22.5.2')
    def test_depu_provision_3K_rsa(self):
        """Provision 3k key and setup session on DEPU provisioned device"""

        if (self.fixture.get_type() in (FixtureType.SIM, FixtureType.MODEL)):
            self.provision_with_depu()

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Generate RSA key
        rsa_key = CryptoHelper().generate_rsa_key_from_components(self.RSA_KEY_MODULUS, d=self.RSA_KEY_PRIV_EXP)

        # Issue Provision asset command
        self.provision_rsa_key(rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

    @testIdentifier('22.5.3')
    def test_depu_provision_trng_characterization(self):
        """Check ProvisionAsset command in Secure
           - Use an valid TRNG characterization asset package, with a valid payload
           - Try to load a second asset (ENOSPC). One programmed by DEPU so this would be the third to be loaded (2 max)
        """

        if (self.fixture.get_type() in (FixtureType.SIM, FixtureType.MODEL)):
            self.provision_with_depu()

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        asset = NvmRecordTRNGCharacterization(5002, 1002, 502, 2)
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM)

        # Attempt to write a second set of characterization data, will fail as two already exist. One just programmed, one programmed by depu
        asset = NvmRecordTRNGCharacterization(5004, 1004, 504, 4)
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM, expected=Error.NOSPC)

    @testIdentifier('22.5.4')
    def test_depu_provision_otpm_config(self):
        """Check ProvisionAsset command in Secure
           - Try to load the same asset again (EALREADY)
           - Try to load a second asset
           - Try to load a third asset (ENOSPC)
        """

        if (self.fixture.get_type() in (FixtureType.SIM, FixtureType.MODEL)):
            self.provision_with_depu()

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        otpm_config = OtpmConfig(tvpps_ns = 10000)
        asset = NvmRecordOtpmConfig(otpm_config)
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM)
        # Should already exist
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM, expected=Error.ALREADY)

        # Second asset
        otpm_config = OtpmConfig(tvpps_ns = 10002)
        asset = NvmRecordOtpmConfig(otpm_config)
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM)
        # Should already exist
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM, expected=Error.NOSPC)

        # Third asset
        otpm_config = OtpmConfig(tvpps_ns = 10004)
        asset = NvmRecordOtpmConfig(otpm_config)
        response = HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 27000000, root_of_trust = RootOfTrust.DM, expected=Error.NOSPC)


    @testIdentifier('22.6.1')
    def test_depu_session_psk(self):
        """Setup session using programmed psk key"""

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Psk key 0-15
        master_secret = bytes(range(16))

        context = SessionContext()
        session = HostSession(context, self.camera)
        time_start = time.perf_counter()
        command_exec_time = session.connect(mode = SessionMode.PSK_MODE, master_secret = master_secret)
        time_end = time.perf_counter()
        self.logger.info("Session start-up time: {}".format(time_end - time_start))
        self.logger.info("Command execution time: {}".format(command_exec_time))

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()

    @testIdentifier('22.6.2')
    def test_depu_session_2K_rsa_vendor_a(self):
        """Setup session using depu programmed rsa key (original vendor A)"""

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        rsa_key_vendor = self.crypto_cm.get_public_key(KeyName.VENDOR_PAIR)

        context = SessionContext()
        session = HostSession(context, self.camera)
        time_start = time.perf_counter()
        command_exec_time = session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor, auth_id = CertificateAuthority.VENDOR_A)
        time_end = time.perf_counter()
        self.logger.info("Session start-up time: {}".format(time_end - time_start))
        self.logger.info("Command execution time: {}".format(command_exec_time))

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()

    @testIdentifier('22.6.3')
    def test_depu_session_3K_rsa_vendor_b(self):
        """Setup session using post DEPU programmed rsa key (new vendor B)"""

        self.sensor_soft_reset()
        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Generate RSA key
        rsa_key = CryptoHelper().generate_rsa_key_from_components(self.RSA_KEY_MODULUS, d=self.RSA_KEY_PRIV_EXP)

        context = SessionContext()
        session = HostSession(context, self.camera)
        time_start = time.perf_counter()
        command_exec_time = session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key, auth_id = CertificateAuthority.VENDOR_B)
        time_end = time.perf_counter()
        self.logger.info("Session start-up time: {}".format(time_end - time_start))
        self.logger.info("Command execution time: {}".format(command_exec_time))

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()



