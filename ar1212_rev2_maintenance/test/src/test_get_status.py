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
import random

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from patch_helper import *
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from diag_helper import DiagHelper
from patch_type import PatchType

from host_command_code import HostCommandCode
from host_command_set_session_keys import HostCommandSetSessionKeysParams, HostCommandSetSessionKeysResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeys
from provision_asset_helper import ProvisionAssetHelper
from host_command_get_certificates import HostCommandGetCertificates
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from host_session import SessionContext
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType, SessionMode
from test_fixture import FixtureType

class SharedRamHelper():
    """Shared RAM helper class."""

    # ICORE Shared RAM sizes
    __SRAM_NUM_PAGES = 4
    __SRAM_PAGE_SIZE = 1024

    def __init__(self, camera):
        self.__diag_helper = DiagHelper(camera)
        self.__camera = camera
        self.__sensor = camera.sensor
        self.__reg = camera.sensor.reg
        self._sram_num_pages = self.__SRAM_NUM_PAGES
        self._sram_page_size = self.__SRAM_PAGE_SIZE

    def _read_shared_ram(self, page_number, offset_addr, number_of_reads = 1):
        """Read 16-bit wide location(s) in one page of shared RAM"""

        # test input params, not allowed to read over a page boundary
        assert (page_number < self.__SRAM_NUM_PAGES)
        assert ((offset_addr + number_of_reads) <= self._sram_page_size / 2)  # page size in bytes

        # read the shared RAM
        self.__reg.CSS_PAGE = page_number
        payload = bytearray()
        payload.extend(self.__reg.CSS_PARAMS_0.burst_read(number_of_reads * 2))       # 16 bit icb access, so x2 bytes
        return struct.unpack('<' + 'H'*number_of_reads, payload)

    def _write_shared_ram(self, page_number, write_data, number_of_writes = 1):
        """Write 16-bit wide locations in one page of shared RAM"""
        # test input params, not possible to write over a page boundary
        assert (page_number < self.__SRAM_NUM_PAGES)
        assert (number_of_writes <= self._sram_page_size / 2)           # _sram_page_size is in bytes

        self.__reg.CSS_PAGE = page_number
        start = int(0) # can only write from beginning of page
        stop = int(start + number_of_writes*2)
        self.__reg.CSS_PARAMS_0 = write_data[start:stop]


class TestGetStatus(BaseTestCase):
    """Test the GetStatus host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.PATCH)
        self.crypto = CryptoHelper()
        self.shared_ram = SharedRamHelper(self.camera)
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')

    def _wait_until_target_ready_for_command(self, expectedResponse = Error.SUCCESS):
        """ Check system is ready by polling the doorbell bit """
        response = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(response, expectedResponse.value, 'Operation failed with error {}'.format(response.name))
        self.logger.info('System is ready')

    def _get_sensor_id(self):
        """ Issue GetSensorId command """
        response = HostCommandGetSensorId(self.camera).execute()
        return response.unique_id

    def _set_ephemeral_master_secret(self):
        """ Issue SetEthemeralMasterSecret command """
        expected = Error.SUCCESS
        params = HostCommandSetEthemeralMasterSecretParams()
        params.masterSecretKeyType = 0
        params.masterSecret = bytes([0]*16)
        response = HostCommandSetEthemeralMasterSecretResponse()
        self.logger.info('Sending command: SET_EPHEMERAL_MASTER_SECRET')
        result = self.command_helper.execute(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response, verbosity=logging.INFO)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

    def _set_psk_session_keys(self):
        """Issue SetPskSessionKeys command """

        response = HostCommandSetPskSessionKeys(self.camera).execute(
            videoAuthCipherMode = SymmetricAuthCipherMode.AES_CMAC,
            videoAuthKeyType = SymmetricKeyType.AES_128,
            controlChannelCipherMode = SymmetricAECipherMode.AES_CCM,
            controlChannelKeyType = SymmetricKeyType.AES_128,
            hostSalt = bytes([0]*16))

        # Log response
        self.logger.info('sensorSalt = {}'.format(response.sensorSalt))
        self.logger.info('videoAuthGMACIV = {}'.format(response.videoAuthGMACIV))
        self.logger.info('controlChannelNonce = {}'.format(response.controlChannelNonce))

    def _provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""

        ProvisionAssetHelper(self.camera, self.key_set_name).provision_rsa_certificates(rsa_key, auth_id = auth_id, purpose = purpose, number=100)

    def _get_certificates(self, auth_id):
        """Run GetCertificates command"""
        # Issue GetCertificates command
        HostCommandGetCertificates(self.camera).execute(auth_id)

    def _build_session_master_secret(self, key):
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

    def _set_session_keys(self, rsa_key, context, expected = Error.SUCCESS):
        """Run SetSessionKeys command"""

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

        msg = self._build_session_master_secret(bytes([0]*master_key_size_bytes))
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

    def _secure_session_test(self, rsa_key, session_context,
                      auth_id = CertificateAuthority.VENDOR_B,
                      purpose = CertificatePurpose.SENSOR,
                      expected_lifecycle = LifecycleState.DM,
                      expected_session_response = Error.SUCCESS):
        """Check FW responds to SetSessionKeys correctly in DM"""

        # Check system is ready and in CONFIGURE phase
        self._wait_until_target_ready_for_command()
        id = self._get_sensor_id()

        self._wait_until_target_ready_for_command()
        self._get_status(LifecycleState.SECURE, SystemMgrPhase.CONFIGURE)

        # Issue Provision asset command
        if CertificatePurpose.VENDOR == purpose:
            self._provision_rsa_key(rsa_key, auth_id, CertificatePurpose.VENDOR)
        self._provision_rsa_key(rsa_key, auth_id, CertificatePurpose.SENSOR)

        # Issue GetCertificates command
        self._get_certificates(auth_id)

        # Issue SetSessionKeys command
        self._set_session_keys(rsa_key, session_context, expected_session_response)

    def _write_read_shared_mem_areas(self):
        # write every location in each shared-ram page, read all back and test
        samples16_per_page = int(self.shared_ram._sram_page_size / 2)    # size is in bytes, reads are 16-bit
        ram_offset_addr = 0

        # All locations in each page should be zero
        for ram_page in range(0, self.shared_ram._sram_num_pages):
            self.logger.info('Testing Shared RAM for zero, all locations, Page {}'.format(ram_page))
            read_values = self.shared_ram._read_shared_ram(ram_page, ram_offset_addr, number_of_reads = samples16_per_page)
            for index in range (0, samples16_per_page):
                self.assertEqual(0, read_values[index], 'Error Shared Ram Non-Zero: Page={}, Offset={}, Value=0x{:04x}'.format(ram_page, ram_offset_addr, read_values[index]))

        # generate 4 pages worth of random data
        test_write_data = bytes([random.randint(0,255) for x in range(self.shared_ram._sram_num_pages * samples16_per_page*2)]) # test_write_data is uint8[4096]

        # write the test data to all four pages
        for ram_page in range(0, self.shared_ram._sram_num_pages):
            self.logger.info('Writing Test Data to Shared RAM, Page {}'.format(ram_page))
            start = int(ram_page * samples16_per_page*2)
            stop = int((ram_page+1) * samples16_per_page * 2)
            self.shared_ram._write_shared_ram(ram_page, write_data = test_write_data[start:stop], number_of_writes = samples16_per_page)

        # read the data back from each page and test it
        for ram_page in range(0, self.shared_ram._sram_num_pages):
            self.logger.info('Reading Test Data from Shared RAM, Page {}'.format(ram_page))
            test_read_data = self.shared_ram._read_shared_ram(ram_page, 0, number_of_reads = samples16_per_page)     # test_read_data is uint16[512]
            for index in range(0, samples16_per_page):
                data_offset = (ram_page * samples16_per_page * 2) + (index*2)
                write_data16 = test_write_data[data_offset] + (test_write_data[data_offset + 1] << 8)   # LoByte + HiByte
                self.assertEqual(write_data16, test_read_data[index],
                    'Error Shared Ram at Page={} addr={}: Expected=0x{:04x}, Actual=0x{:04x}'.format(ram_page, ram_offset_addr, write_data16, test_read_data[index]))


    def _get_status(self, expectedLCS, expectedPhase = SystemMgrPhase.DEBUG, expectedPatchVersion = 0):
        """ Issue GetStatus command and test response against expected values """
        response = HostCommandGetStatus(self.camera).execute()

        # test some responses
        expected_rom_version = self.patch_loader.get_rom_version()
        self.assertEqual(expected_rom_version, response.rom_version,
            'Unexpected ROM version, expected={}, actual={}'.format(expected_rom_version, response.rom_version))

        self.assertEqual(expectedLCS.value, response.current_lifecycle_state.value,
            'Wrong LC state, expected={}, actual={}'.format(expectedLCS.name, response.current_lifecycle_state.name))

        self.assertEqual(expectedPhase.value, response.current_phase.value,
            'Wrong Phase, expected={}, actual={}'.format(expectedPhase.name, response.current_phase.name))

        self.assertEqual(expectedPatchVersion, response.patch_version,
            'Wrong Patch Version, expected=0x{:04x}, actual=0x{:04x}'.format(expectedPatchVersion, response.patch_version))

        # Target should report stack use as non-zero
        self.assertNotEqual(0, response.stack_used_long_words, 'Stack Use incorrectly reported as Zero')

    @testIdentifier('3.1.1')
    def test_get_status_sanity_cm(self):
        """ Sanity check GetStatus command in CM """
        # Check system is ready and call the getStatus command in the DEBUG phase
        # After each call, test that the target is in the expected LCS and expectedPhase
        self._common_setup(LifecycleState.CM, None)
        self._get_status(LifecycleState.CM)

    @testIdentifier('3.1.2')
    def test_get_status_cm(self):
        """ Check GetStatus command in DM, through DEBUG, CONFIGURE and SESSION system phases"""

        # Check system is ready and call the getStatus command in each phase: DEBUG, PATCH, CONFIGURE, SESSION
        # After each call, test that the target is in the expected LCS and expectedPhase
        self._common_setup(LifecycleState.CM, None)

        self._get_status(LifecycleState.CM, SystemMgrPhase.DEBUG, expectedPatchVersion = 0)
        self.patch_loader.load_patch(LifecycleState.CM, PatchType.PATCH)
        self._get_status(LifecycleState.CM, SystemMgrPhase.CONFIGURE, expectedPatchVersion = self.patch_version)

        # get the target into session phase
        self._get_sensor_id()
        self._set_ephemeral_master_secret()
        self._set_psk_session_keys()

        self._wait_until_target_ready_for_command()
        self._get_status(LifecycleState.CM, SystemMgrPhase.SESSION, expectedPatchVersion = self.patch_version)


    @testIdentifier('3.1.3')
    def test_get_status_dm(self):
        """ Check GetStatus command in DM, through DEBUG, CONFIGURE and SESSION system phases"""

        # Check system is ready and call the getStatus command in each phase: DEBUG, PATCH, CONFIGURE, SESSION
        # After each call, test that the target is in the expected LCS and expectedPhase
        self._common_setup(LifecycleState.DM, None)

        self._get_status(LifecycleState.DM, SystemMgrPhase.DEBUG, expectedPatchVersion = 0)
        self.patch_loader.load_patch(LifecycleState.DM, PatchType.PATCH)
        self._get_status(LifecycleState.DM, SystemMgrPhase.CONFIGURE, expectedPatchVersion = self.patch_version)

        # get the target into session phase
        self._get_sensor_id()
        self._set_ephemeral_master_secret()
        self._set_psk_session_keys()

        self._wait_until_target_ready_for_command()
        self._get_status(LifecycleState.DM, SystemMgrPhase.SESSION, expectedPatchVersion = self.patch_version)

    @testIdentifier('3.1.4')
    def test_get_status_se(self):
        """ Check GetStatus command in SE, in all phases with no patch """

        # After each call, test that the target is in the expected LCS and expectedPhase
        self._common_setup(LifecycleState.SECURE, None)
        self._get_status(LifecycleState.SECURE, SystemMgrPhase.DEBUG)

        # get the target into session phase
        rsa_key = self.crypto.generate_random_rsa_key(2048)
        session_context = SessionContext()
        self._secure_session_test(rsa_key, session_context, CertificateAuthority.VENDOR_B, CertificatePurpose.SENSOR, LifecycleState.SECURE)

        # Check we're in the Session phase
        self._wait_until_target_ready_for_command()
        self._get_status(LifecycleState.SECURE, SystemMgrPhase.SESSION)

    @testIdentifier('3.1.5')
    def test_host_read_shared_ram_cm(self):
        """ Check Host can write-read shared-RAM after doorbell is cleared"""

        # wait until target has cleared the doorbell bit, then read the shared RAM
        self._common_setup(LifecycleState.CM, None)
        self._write_read_shared_mem_areas()

        # ensure test is being run in correct lifecycle
        self._get_status(expectedLCS = LifecycleState.CM)

    @testIdentifier('3.1.6')
    def test_host_read_shared_ram_dm(self):
        """ Check Host can write-read shared-RAM after doorbell is cleared"""

        # wait until target has cleared the doorbell bit, then read the shared RAM
        self._common_setup(LifecycleState.DM, None)
        self._write_read_shared_mem_areas()

        # ensure test is being run in correct lifecycle
        self._get_status(expectedLCS = LifecycleState.DM)

    @testIdentifier('3.1.7')
    def test_host_read_shared_ram_se(self):
        """ Check Host can write-read shared-RAM after doorbell is cleared"""

        # wait until target has cleared the doorbell bit, then read the shared RAM
        self._common_setup(LifecycleState.SECURE, None)
        self._write_read_shared_mem_areas()

        # ensure test is being run in correct lifecycle
        self._get_status(expectedLCS = LifecycleState.SECURE)
