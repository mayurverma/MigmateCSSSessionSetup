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
# Test SdpuApp.
####################################################################################
import logging
import os
import sys
import struct

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_depu_program_otpm import HostCommandDepuProgramOtpmParams, HostCommandDepuProgramOtpmResponse
from secure_boot_certificate import CC312LifeCycleState, HbkId
from crypto_helper import CryptoHelper
from patch_type import PatchType
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from root_of_trust import RootOfTrust
from host_command_get_sensor_id import HostCommandGetSensorIdParams, HostCommandGetSensorIdResponse, HostCommandGetSensorId
from host_command_get_status import HostCommandGetStatusParams, HostCommandGetStatusResponse, HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorIdParams, HostCommandGetSensorIdResponse
from host_command_apply_debug_entitlement import HostCommandApplyDebugEntitlement
from secure_boot_entitlement import SecureBootDebugEntitlement
from secure_boot_certificate import CC312LifeCycleState, HbkId
from host_session import HostSession, SessionContext, SessionMode, SymmetricAECipherMode, SymmetricAuthCipherMode, AsymmetricCipherMode, SymmetricKeyType
from host_command_provision_asset import HostCommandProvisionAssetParams, HostCommandProvisionAssetResponse, HostCommandProvisionAsset
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret
from key_name import KeyName
from host_command_set_video_auth_roi import HostCommandSetVideoAuthROI
from diag_helper import DiagHelper
from systemmgr_css_fault import SystemMgrCssFault
from crypto_services_client import CryptoServicesClient
from branch_properties import BranchProperties
from build_properties import BuildProperties
from reset_types import ResetType
from toolbox import Toolbox
from process_otpm import AssetType, ProcessOTPM

depu_app_output_dir = BuildProperties["DEPU"].output_path
if depu_app_output_dir not in sys.path:
    sys.path.append(depu_app_output_dir)
from importlib import import_module


class TestDepuProgramOtpm(BaseTestCase):
    """Test the SdpuProgramOtpm host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.logger.debug('setUp complete')
        self.crypto_helper = CryptoHelper()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.DEPU)
        self.rom_version = self.patch_loader.get_rom_version()
        self.verbose = True
        self.key_set_name = 'test'


    def wait_for_system_ready(self, expected_result=Error.SUCCESS):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

    def check_for_expected_status(self, expected_phase, expected_lcs, expected = Error.SUCCESS, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """ Check system is in the expected phase and lifecycle state
            - returns the GetStatus response object
        """

        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute(expected, timeout_usec, interval_usec, verbosity)

        self.assertEqual(response.current_phase, expected_phase,
            'Expected {} phase but was {}'.format(expected_phase.name, response.current_phase.name))
        self.assertEqual(response.current_lifecycle_state, expected_lcs,
            'Expected {} LCS but was {}'.format(expected_lcs.name, response.current_lifecycle_state.name))

        return response

    def provision_rsa_key(self, rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public/private key and send by ProvisionAsset command"""

        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number=100)
        nvm_record = NvmRecordRSAPublicCert(pub_cert)

        provision_asset_cmd = HostCommandProvisionAsset(self.camera, self.key_set_name)
        provision_asset_cmd.execute(nvm_record, asset_id = 6789, root_of_trust = RootOfTrust.CM, clock_speed_hz = 150000000)

        if CertificatePurpose.SENSOR == purpose:
            nvm_record = NvmRecordRSAPrivateKey(rsa_key, auth_id = auth_id)
            provision_asset_cmd.execute(nvm_record, asset_id = 1234, root_of_trust = RootOfTrust.CM, clock_speed_hz = 150000000)


    def apply_debug_entitlement(self, cryptoLifeCycle, socId,
        debug_enabler_mask   = 0x00000000000000000000000000000000,
        debug_enabler_lock   = 0x00000000000000000000000000000000,
        debug_developer_mask = 0x00000000000000000000000000000000,
        clockSpeedHz=27000000,
        sw_version = 5,
        cert_version = 0x10000,
        root_of_trust = RootOfTrust.CM,
        expectedResponse = Error.SUCCESS):
        """Execute ApplyDebugEntitlement command"""

        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = root_of_trust, key_set_name = self.key_set_name)

        blob = crypto.create_debug_entitlement(
                                               cert_version = cert_version,
                                               sw_version = sw_version,
                                               lcs = cryptoLifeCycle,
                                               debug_enabler_mask   = debug_enabler_mask,
                                               debug_enabler_lock   = debug_enabler_lock,
                                               debug_developer_mask = debug_developer_mask,
                                               soc_id = socId)

        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, clockSpeedHz)

        return response

    def run_depu_program_otpm(self, clockSpeedHz = 50000000, expected_result = Error.SUCCESS, force_error = False):

        depu_app_content = import_module('depu_app_content_{}'.format(self.key_set_name)).DepuAppContent

        # Issue ProgramOtpm command
        params = HostCommandDepuProgramOtpmParams()
        params.clockSpeedHz = clockSpeedHz
        params.blockLengthLongWords = len(depu_app_content)
        params.checksum = Toolbox().calculate_checksum(depu_app_content)
        if force_error:
            params.checksum += 1
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

    def sensor_soft_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self.wait_for_system_ready()

    def run_depu_to_secure_device(self):

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()

        response = self.run_depu_program_otpm()
        self.assertEqual(0, response.error_address, 'Error Address incorrect: {}'.format(response.error_address))
        self.assertEqual(0, response.number_soft_errors, 'Num soft errors is incorrect: {}'.format(response.number_soft_errors))

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)
        self.assertEqual(self.patch_version, response.patch_version)

        HostCommandGetSensorId(self.camera).execute()
        self.sensor_soft_reset()

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

        print("\n>>>>>>>>>>>> run_depu_to_secure_device function finished\n")

    def dump_secure_otpm(self):
        """ Returns secure OTPM data (32-bit values) """

        otpm_bytes = self.camera.dump_otpm(start = 0, length = 0x2c*4)

        return struct.unpack("<{}L".format(len(otpm_bytes)//4), otpm_bytes)


    def dump_user_otpm(self):
        """ Returns user OTPM data (16-bit values) """
        start =  0x2c*4
        otpm_bytes = self.camera.dump_otpm(start = start, length = (4*1024) - start)

        return struct.unpack("<{}L".format(len(otpm_bytes)//4), otpm_bytes)


    @testIdentifier('21.1.1')
    def test_load_depu_cm(self):
        """Check DEPU program load in CM"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.SUCCESS)
        self.assertEqual(0x00, response.error_address)
        self.assertEqual(0x00, response.number_soft_errors)

    @testIdentifier('21.1.2')
    def test_load_depu_dm(self):
        """Check DEPU program load in DM"""

        self._common_setup(LifecycleState.DM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        # Patch was encrypted during loading so loading succedded but should not execute in this LCS
        response = self.run_depu_program_otpm(expected_result=Error.NOEXEC)

    @testIdentifier('21.1.3')
    def test_load_depu_se(self):
        """Check DEPU program load in SECURE"""

        self._common_setup(LifecycleState.SECURE, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        # Patch was encrypted during loading so loading succeeded but should not execute in this LCS
        response = self.run_depu_program_otpm(expected_result=Error.NOEXEC)

    @testIdentifier('21.1.4')
    def test_execute_command_depu_not_loaded_cm(self):
        """Check system responds with error when patch not loaded"""

        self._common_setup(LifecycleState.CM, PatchType.PATCH)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        # Depu not loaded expect ACCESS error
        response = self.run_depu_program_otpm(expected_result=Error.ACCESS)

    ##############################################################################################################################
    # Various types of OTPM corruption in user space
    ##############################################################################################################################
    @testIdentifier('21.2.1')
    def test_depu_otpm_sbe_0x2c_cm(self):
        """Check DEPU response to a single bit error in user space (0x2c)"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.SUCCESS)
        self.assertEqual(0x00, response.error_address)
        self.assertEqual(0x02, response.number_soft_errors)

    @testIdentifier('21.2.2')
    def test_depu_otpm_dbe_0x2d_cm(self):
        """Check DEPU response to a double bit error in user space (0x2d)"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        # INFO: <rw>: The OTPM write first reads to check the location is ZERO, a double bit error will
        # cause an exception, in the DEPU case the fatal error handler returns back to the original point
        # without any further processing. The register holding the expected read value is not updated so
        # depends on previous code flow and stack contents. We have observed that in our tests the register
        # holds a non-zero value so an IO error is generated.
        response = self.run_depu_program_otpm(expected_result=Error.IO)
        self.assertEqual(0x2d, response.error_address)
        self.assertEqual(0x00, response.number_soft_errors)

    @testIdentifier('21.2.3')
    def test_depu_otpm_not_blank_0x2c_cm(self):
        """Check DEPU response to a non blank location at address 0x2c, interpreted as record header"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.RANGE)
        self.assertEqual(0x00, response.error_address)
        self.assertEqual(0x00, response.number_soft_errors)

    @testIdentifier('21.2.4')
    def test_depu_otpm_not_blank_0x2d_cm(self):
        """Check DEPU response to a non blank location at address 0x2d"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.IO)
        self.assertEqual(0x2d, response.error_address)
        self.assertEqual(0x00, response.number_soft_errors)

    @testIdentifier('21.2.5')
    def test_depu_otpm_sbe_0x33_maps_to_new_value_cm(self):
        """Check DEPU respond correctly to a write verify error"""

        # Code of 0x208083 for upper 16-bits decodes as 0 with one soft error. This location will be written with code 0x200484 (value=0x41)
        # by DEPU, the value in OTPM is now 0x208487 (data value 0x2041 with one error). This will cause a read verify error

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.NOMSG)
        self.assertEqual(0x33, response.error_address)
        self.assertEqual(0x01, response.number_soft_errors)

    ##############################################################################################################################
    # Various types of OTPM corruption in C312 space
    ##############################################################################################################################
    @testIdentifier('21.3.1')
    def test_depu_otpm_not_blank_0x2a_cm(self):
        """Check DEPU response correctly to a non blank location in crypto space (0x2a)"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.IO)
        self.assertEqual(0x2a, response.error_address)
        self.assertEqual(0x00, response.number_soft_errors)

    @testIdentifier('21.3.2')
    def test_depu_otpm_sbe_0x2a_cm(self):
        """Check DEPU response correctly to a single bit error in crypto space (0x2a)"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        response = self.run_depu_program_otpm(expected_result=Error.SUCCESS)
        self.assertEqual(0x00, response.error_address)
        self.assertEqual(0x02, response.number_soft_errors)

    @testIdentifier('21.3.3')
    def test_depu_otpm_dbe_0x2a_cm(self):
        """Check DEPU response correctly to a double bit error in crypto space (0x2a)"""

        self._common_setup(LifecycleState.CM, None)

        self.patch_loader.load_patch(LifecycleState.CM, PatchType.DEPU, expected_result = Error.SYSERR)
        self.assertTrue(DiagHelper(self.camera).is_fatal_error())
        fault, context = DiagHelper(self.camera).get_css_error()
        self.assertEqual(SystemMgrCssFault.OTPM_ERROR, fault)
        self.assertEqual(0x2a, context)

    ##############################################################################################################################
    # Various of general application test after DEPU success run
    ##############################################################################################################################

    @testIdentifier('21.4.1')
    def test_depu_application_cm(self):
        """Check keys are working after DepuProgramOtpm command"""
        self.run_depu_to_secure_device()

        response = HostCommandGetSensorId(self.camera).execute()
        soc_id = bytes(response.unique_id)

        self.sensor_soft_reset()

        # HBK0
        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, 0x0010, 0x0010, 0x0010)
        # KPICV
        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PATCH, key_set_name = self.key_set_name)
        # KPICV
        master_secret = bytes(16)
        provision_asset_cmd = HostCommandProvisionAsset(self.camera, self.key_set_name)
        nvm_record = NvmRecordPSKMasterSecret(master_secret)
        provision_asset_cmd.execute(nvm_record, asset_id = 0x12345678, root_of_trust = RootOfTrust.CM)
        # Sensor and Vendor key
        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)
        rsa_key_vendor = crypto.get_public_key(KeyName.VENDOR_PAIR)
        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key_vendor)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()
        roi.execute()

    @testIdentifier('21.4.2')
    def test_depu_program_psk_with_kcp(self):
        """Check keys are working after DepuProgramOtpm command (PSK) """
        self.run_depu_to_secure_device()

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PATCH, key_set_name = self.key_set_name)
        # KCP
        master_secret = bytes(16)
        provision_asset_cmd = HostCommandProvisionAsset(self.camera, self.key_set_name)
        nvm_record = NvmRecordPSKMasterSecret(master_secret)
        provision_asset_cmd.execute(nvm_record, asset_id = 0x12345678, root_of_trust = RootOfTrust.DM)
        # Sensor and Vendor key
        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.PSK_MODE, master_secret = master_secret)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()

    @testIdentifier('21.4.3')
    def test_depu_program_3K_rsa(self):
        """Check keys are working after DepuProgramOtpm command (RSA)"""
        self.run_depu_to_secure_device()

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PATCH, key_set_name = self.key_set_name)

        rsa_key = self.crypto_helper.generate_random_rsa_key(size = 3072)

        # Issue Provision asset command
        self.provision_rsa_key(rsa_key, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR)

        context = SessionContext()
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = rsa_key.publickey(), auth_id = CertificateAuthority.VENDOR_B)

        self.logger.info(session)
        roi = HostCommandSetVideoAuthROI(session)
        roi.execute()

    @testIdentifier('21.4.4')
    def test_depu_program_apply_debug_entitlement(self):
        """Check application of debug entitlement after DEPU programming (minimum version should be 0)"""
        self.run_depu_to_secure_device()

        self._common_setup(LifecycleState.SECURE, None)

        response = HostCommandGetSensorId(self.camera).execute()
        soc_id =  bytes(response.unique_id)

        self.logger.info(">>>>>>>>>>>>>>>>>>> RESTART <<<<<<<<<<<<<<<<<<<<<<<")
        self.sensor_soft_reset()
        self._wait_for_system_ready()

        HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0, DiagHelper(self.camera).get_info())

        # Force sw_version to zero, if minimum version in ICV flags greater than this vcommand will not be aaccepted
        self.apply_debug_entitlement(CC312LifeCycleState.Secure, soc_id, 0x001c, 0x001c, 0x001c, sw_version=0)

    @testIdentifier('21.4.5')
    def test_depu_otpm_not_in_use_bit_cm(self):
        """Check that the Kceicv and KCE "not in use" bits in OTPM is set"""

        self.run_depu_to_secure_device()

        otpm = self.dump_secure_otpm()
        # Check bit 23 in Manufacturers programmable flags (Kciev not in use)
        self.assertTrue(0 != otpm[0x10] & (1 << 23))

        # Check bit 23 in OEM programmable flags (Kce not in use)
        self.assertTrue(0 != otpm[0x21] & (1 << 23))

    @testIdentifier('21.4.6')
    def test_depu_otpm_dcu_lock_bits_cm(self):
        """Check ICV DCU lock bits are set to 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFFF 0xFFFFFFE0 (all unused bits locked)"""

        self.run_depu_to_secure_device()

        otpm = self.dump_secure_otpm()
        self.assertEqual(0xFFFFFFE0, otpm[0x28])
        self.assertEqual(0xFFFFFFFF, otpm[0x29])
        self.assertEqual(0xFFFFFFFF, otpm[0x2a])
        self.assertEqual(0xFFFFFFFF, otpm[0x2b])

    @testIdentifier('21.4.7')
    def test_depu_otpm_check_general_configuration_bits_cm(self):
        """Check general configuration bits are 0x0"""

        self.run_depu_to_secure_device()

        otpm = self.dump_secure_otpm()
        self.assertEqual(0x00000000, otpm[0x27])

    @testIdentifier('21.4.8')
    def test_depu_otpm_check_minimum_version_cm(self):
        """Check minimum version values (anti-rollback counters)"""

        self.run_depu_to_secure_device()

        otpm = self.dump_secure_otpm()
        # Hbk0
        self.assertEqual(0x00000000, otpm[0x22])
        self.assertEqual(0x00000000, otpm[0x23])
        # Hbk1
        self.assertEqual(0x00000000, otpm[0x24])
        self.assertEqual(0x00000000, otpm[0x25])
        self.assertEqual(0x00000000, otpm[0x26])

    @testIdentifier('21.4.9')
    def test_depu_otpm_check_for_provisioning_header_cm(self):
        """Check provisioning header is located in first location of user data"""

        self.run_depu_to_secure_device()

        otpm = self.dump_user_otpm()
        self.assertEqual(0x00010006, otpm[0x00])
        patch_version = self.patch_loader.get_patch_version(PatchType.DEPU)
        self.assertEqual(patch_version, otpm[0x01], "Expected 0x{:08x} != 0x{:08x}".format(patch_version+1, otpm[0x01]))

    @testIdentifier('21.4.10')
    def test_depu_otpm_corrupt_blob_cm(self):
        """Corrupt checksum and check blob rejected"""

        self._common_setup(LifecycleState.CM, PatchType.DEPU)

        HostCommandGetSensorId(self.camera).execute()

        response = self.run_depu_program_otpm(expected_result = Error.INVAL, force_error = True)

    @testIdentifier('21.4.11')
    def test_depu_otpm_trng_record_cm(self):
        """Check for trng record with correct values"""

        self.run_depu_to_secure_device()

        user = self.dump_user_otpm()
        p = ProcessOTPM(user)
        index = p.find_asset(AssetType.TRNG_CHARACTERIZATION)

        self.assertEqual(1, len(index),"Only expected one record")
        asset = p.extract_asset(index[-1])
        self.assertEqual(5000, asset[1])
        self.assertEqual(1000, asset[2])
        self.assertEqual(500, asset[3])
        self.assertEqual(0, asset[4])



