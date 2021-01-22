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

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from branch_properties import BranchProperties, BranchType
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_load_patch_chunk import HostCommandLoadPatchChunkParams, HostCommandLoadPatchChunkResponse, HostCommandLoadPatchChunk
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_apply_debug_entitlement import HostCommandApplyDebugEntitlement
from secure_boot_entitlement import SecureBootDebugEntitlement
from secure_boot_certificate import CC312LifeCycleState, HbkId
from crypto_helper import CryptoHelper
from patch_helper import *
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from patch_type import PatchType
from crypto_services_client import CryptoServicesClient
from branch_properties import BranchProperties
from root_of_trust import RootOfTrust
from test_fixture import FixtureType
from reset_types import ResetType

class TestLoadPatchChunk(BaseTestCase):
    """Test the LoadPatchChunk host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.crypto = CryptoHelper()
        self.assetbuilder = AssetStructure()
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')

    def is_ar0820_rev2(self):
        """Check if the ROM is the RC1 version (917)
        """
        return BranchProperties.get_type() == BranchType.AR0820_REV2


    def wait_for_system_ready(self):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, Error.SUCCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

    def check_for_expected_status(self, expected_phase, expected_lcs, expected_patch_version=None, expected_rom_version=None):
        """ Check system is in the expected phase and lifecycle state
            - returns the GetStatus response object
        """
        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute()

        self.assertEqual(response.current_phase, expected_phase,
            'Expected {} phase but was {}'.format(expected_phase.name, response.current_phase.name))
        self.assertEqual(response.current_lifecycle_state, expected_lcs,
            'Expected {} LCS but was {}'.format(expected_lcs.name, response.current_lifecycle_state.name))
        if expected_patch_version:
            self.assertEqual(response.patch_version, expected_patch_version,
                'Expected {} patch version but was {}'.format(expected_patch_version, response.patch_version))
        if expected_rom_version:
            self.assertEqual(response.rom_version, expected_rom_version,
                'Expected {} rom version but was {}'.format(expected_rom_version, response.rom_version))

        return response

    def get_soc_id_and_reset(self):

        self._wait_for_system_ready()

        get_sensor_id = HostCommandGetSensorId(self.camera)

        soc_id = bytes(get_sensor_id.execute().unique_id)

        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self._wait_for_system_ready()

        return soc_id

    def apply_entitlement(self, lifecycle):

        soc_id = self.get_soc_id_and_reset()

        if LifecycleState.SECURE == lifecycle:
            crypto_lifecycle = CC312LifeCycleState.Secure
        elif LifecycleState.DM == lifecycle:
            crypto_lifecycle = CC312LifeCycleState.DM
        elif LifecycleState.CM == lifecycle:
            crypto_lifecycle = CC312LifeCycleState.CM
        else:
            self.assertTrue(False, "incorrect lifecycle")

        crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)

        blob = crypto.create_debug_entitlement(
                                               cert_version = 0x10000,
                                               sw_version = 5,
                                               lcs = crypto_lifecycle,
                                               debug_enabler_mask   = 0x00000000000000000000000000000010,
                                               debug_enabler_lock   = 0x00000000000000000000000000000010,
                                               debug_developer_mask = 0x00000000000000000000000000000010,
                                               soc_id = soc_id)

        response = HostCommandApplyDebugEntitlement(self.camera).execute(blob, 27000000)



    def load_patch_chunks(self, current_lcs, expected=Error.SUCCESS,
                              use_bad_key=False,
                              use_wrong_order=False,
                              use_wrong_loader=False,
                              use_wrong_rom=False,
                              use_wrong_patch_version = False):
        """Loads a patch via one or more 'LoadPatchChunk' commands
        """

        # Read the patch image data
        patch_image_file = self.patch_loader.get_patch_binary_path(PatchType.VERIFICATION)
        patch_map_file = self.patch_loader.get_patch_map_path(PatchType.VERIFICATION)

        if use_wrong_rom:
            rom_version = self.rom_version ^ 1
        else:
            rom_version = self.rom_version

        patch_data = get_chunks(rom_version, self.patch_version, patch_image_file, patch_map_file)

        saved_expected = expected
        for chunk in patch_data.chunks:
            params = HostCommandLoadPatchChunkParams()
            if use_wrong_patch_version:
                chunk.patch_version += chunk.this_chunk
                if 0 == chunk.this_chunk:
                    expected=Error.SUCCESS
                else:
                    expected=saved_expected

            if use_wrong_loader:
                chunk.loader_address = 0
                if chunk.last_chunk != chunk.this_chunk:
                    expected=Error.SUCCESS
                else:
                    expected=saved_expected

            if use_wrong_order:
                chunk.this_chunk += 1

            if (LifecycleState.CM == current_lcs):
                cmd = Command().generate(chunk.asset_id(), chunk.image())
                self.logger.debug(cmd)
                params.asset_payload = cmd.image()
                response = HostCommandLoadPatchChunkResponse()
                self.logger.debug('Sending (CM) LOAD_PATCH_CHUNK command with : {}'.format(params.asset_payload.hex()))
                result = self.command_helper.execute(HostCommandCode.LOAD_PATCH_CHUNK, params, response, verbosity=logging.INFO)
                self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
                # Abort if we're expecting to fail
                if expected.value != Error.SUCCESS: return
            else:
                asset = chunk.asset_id().to_bytes(4, 'little')
                if use_bad_key:
                    root_of_trust = RootOfTrust.DM
                else:
                    root_of_trust = RootOfTrust.CM

                crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = root_of_trust, key_set_name = self.key_set_name)
                asset += crypto.encrypt_asset(chunk.asset_id(), chunk.image())

                params.asset_payload = asset
                response = HostCommandLoadPatchChunkResponse()
                self.logger.debug('Sending Encrypted LOAD_PATCH_CHUNK command with : {}'.format(params.asset_payload.hex()))
                result = self.command_helper.execute(HostCommandCode.LOAD_PATCH_CHUNK, params, response, verbosity=logging.INFO)
                self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
                # Abort if we're expecting to fail
                if expected.value != Error.SUCCESS: return

    def _load_patch_chunk(self, chunk_number, current_lcs, expected=Error.SUCCESS):
        """Loads the nth chunk of a valid patch using a 'LoadPatchChunk' command """

        # Read the patch image data
        patch_image_file = self.patch_loader.get_patch_binary_path(PatchType.VERIFICATION)
        with open(patch_image_file, 'rb') as istream:
            patch_image = istream.read()

        # Max chunk size is 512 bytes minus the packaged asset header (20 bytes)
        # TODO: <PH>: need to retrieve load_address and loader_address from the create_patch.py script
        patch_data = Chunk(rom_version = self.rom_version,
            patch_version = self.patch_version, load_address = 0x20000000, loader_address = 0x20000019,
            max_chunk_size = (512 - 20), data = patch_image)

        chunk = patch_data.chunks[chunk_number]

        if (LifecycleState.CM == current_lcs):
            cmd = Command().generate(chunk.asset_id(), chunk.image())
            self.logger.debug(cmd)
            asset = cmd.image()
        else:
            asset = chunk.asset_id().to_bytes(4, 'little')
            crypto = CryptoServicesClient(BranchProperties.get_product_prefix(), root_of_trust = RootOfTrust.CM, key_set_name = self.key_set_name)
            asset += crypto.encrypt_asset(chunk.asset_id(), chunk.image())

        HostCommandLoadPatchChunk(self.camera).execute(asset, expected =expected)

    @testIdentifier('4.1.1')
    def test_load_patch_chunk_in_cm(self):
        """Check LoadPatchChunk command in CM"""

        self._common_setup(LifecycleState.CM, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.CM)

        self.assertEqual(self.rom_version, response.rom_version)

        self.patch_loader.load_patch(LifecycleState.CM, PatchType.VERIFICATION)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        self.assertEqual(0xCAFE, response.rom_version) # changed by the patch

    @testIdentifier('4.1.2')
    def test_load_patch_chunk_in_dm(self):
        """Check LoadPatchChunk command in DM"""

        self._common_setup(LifecycleState.DM, None)

        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)

        self.patch_loader.load_patch(LifecycleState.DM, PatchType.VERIFICATION)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)

        self.assertEqual(0xCAFE, response.rom_version) # changed by the patch

    @testIdentifier('4.1.3')
    def test_load_patch_chunk_in_se(self):
        """Check LoadPatchChunk command in SECURE"""

        self._common_setup(LifecycleState.SECURE, None)

        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.VERIFICATION)

        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)

        self.assertEqual(0xCAFE, response.rom_version) # changed by the patch

    @testIdentifier('4.1.4')
    def test_load_patch_chunk_in_secure_after_debug_entitlement(self):
        """Check LoadPatchChunk command in SECURE after loading a debug entitlement"""

        self._common_setup(LifecycleState.SECURE, None)

        lifecycle_state = LifecycleState.SECURE
        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, lifecycle_state)
        self.apply_entitlement(lifecycle_state)
        self.patch_loader.load_patch(lifecycle_state, PatchType.VERIFICATION)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, lifecycle_state)
        self.assertEqual(0xCAFE, response.rom_version) # changed by the patch

    def _check_load_patch_chunk_updates_version_on_success(self, lifecycle_state):
        """ Check version information is not updated if patch loading fails"""

        # Note patch changed ROM version returned by get status
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, lifecycle_state, self.patch_version, 0xCAFE)

    @testIdentifier('4.1.5')
    def test_load_patch_chunk_updates_version_on_success_cm(self):
        """ Check version information is not updated if patch loading succeeds"""

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        self._check_load_patch_chunk_updates_version_on_success(LifecycleState.CM)

    @testIdentifier('4.1.6')
    def test_load_patch_chunk_updates_version_on_success_dm(self):
        """ Check version information is not updated if patch loading succeeds"""

        self._common_setup(LifecycleState.DM, PatchType.VERIFICATION)

        self._check_load_patch_chunk_updates_version_on_success(LifecycleState.DM)

    @testIdentifier('4.1.7')
    def test_load_patch_chunk_updates_version_on_success_se(self):
        """ Check version information is updated if patch loading succeeds"""

        self._common_setup(LifecycleState.SECURE, PatchType.VERIFICATION)

        self._check_load_patch_chunk_updates_version_on_success(LifecycleState.SECURE)

    @testIdentifier('4.2.1')
    def test_load_patch_chunk_in_cm_fails_in_configure_phase(self):

        self._common_setup(LifecycleState.SECURE, None)

        # Force the System into the Configure phase, then try to load a patch
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.CM)

        # Issue GetSensorId command
        response = HostCommandGetSensorId(self.camera).execute()

        # Log response
        self.logger.info('Unique ID = {}'.format(str(response.unique_id)))

        # Check for expected side-effects
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

        # Now attempt to load a patch
        self.load_patch_chunks(LifecycleState.CM, expected=Error.ACCESS)

        # Check for no side-effects
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)

    @testIdentifier('4.2.2')
    def test_load_patch_chunk_in_cm_fails_if_patch_already_loaded(self):

        self._common_setup(LifecycleState.CM, None)

        # Load a patch, then try to load another patch
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.CM)
        self.load_patch_chunks(LifecycleState.CM)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.CM)
        self.assertEqual(0xCAFE, response.rom_version) # changed by the patch
        self.load_patch_chunks(LifecycleState.CM, expected=Error.ACCESS)

    @testIdentifier('4.2.3')
    def test_load_patch_chunk_in_dm_fails_if_patch_is_not_authentic(self):

        self._common_setup(LifecycleState.DM, None)

        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)
        self.load_patch_chunks(LifecycleState.DM, use_bad_key=True, expected=Error.BADMSG)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch

        # Further attempts to load a patch shall be rejected with ACCESS
        self.load_patch_chunks(LifecycleState.DM, use_bad_key=True, expected=Error.ACCESS)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch

        # and again, this time with a good key
        self.load_patch_chunks(LifecycleState.DM, use_bad_key=False, expected=Error.ACCESS)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch

    @testIdentifier('4.2.4')
    def test_load_patch_chunk_in_dm_fails_if_wrong_order(self):

        self._common_setup(LifecycleState.DM, None)

        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)
        self.load_patch_chunks(LifecycleState.DM, use_wrong_order=True, expected=Error.INVAL)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch


    @testIdentifier('4.2.5')
    def test_load_patch_chunk_in_dm_fails_if_wrong_rom_version(self):

        self._common_setup(LifecycleState.DM, None)

        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)
        self.load_patch_chunks(LifecycleState.DM, use_wrong_rom=True, expected=Error.RANGE)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch

    @testIdentifier('4.2.6')
    def test_load_patch_chunk_in_dm_fails_if_wrong_patch_version(self):

        self._common_setup(LifecycleState.DM, None)

        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)
        if self.is_ar0820_rev2():
            # More info in http://jira.onsemi.com/browse/AR0820FW-187
            expected_error = Error.INVAL
        else:
            expected_error = Error.RANGE
        self.load_patch_chunks(LifecycleState.DM, use_wrong_patch_version=True, expected=expected_error)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch


    @testIdentifier('4.2.7')
    def test_load_patch_chunk_in_dm_fails_if_zero_loader_address(self):

        self._common_setup(LifecycleState.DM, None)

        # Load a patch encrypted with the wrong key
        self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.DM)
        self.load_patch_chunks(LifecycleState.DM, use_wrong_loader=True, expected=Error.NOENT)
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.DM)
        self.assertEqual(self.rom_version, response.rom_version) # NOT changed by the patch


    def _check_load_patch_chunk_does_not_updated_version_on_failure(self, lifecycle_state):
        """ Check version information is not updated if patch loading fails"""

        # Check get status correctly reports system state
        self.check_for_expected_status(SystemMgrPhase.DEBUG, lifecycle_state, 0, self.rom_version)
        # get the target into patch phase by issuing a LoadPatchChunk command with a valid first chunk
        self._load_patch_chunk(0, lifecycle_state)    # chunk number 0, will succeed
        # issue get status, should be in PATCH phase
        self.check_for_expected_status(SystemMgrPhase.PATCH, lifecycle_state, 0, self.rom_version)
        # get the target into configure phase by issuing a second LoadPatchChunk command with an unexpected chunk so
        # that the patch will *NOT* be applied
        self._load_patch_chunk(2, lifecycle_state, Error.INVAL)     # chunk number 2, will not succeed - target is expecting 1
        # issue get status, should be in CONFIGURE phase, Check expected Patch Version
        self.check_for_expected_status(SystemMgrPhase.CONFIGURE, lifecycle_state, 0, self.rom_version)

    @testIdentifier('4.2.8')
    def test_load_patch_chunk_does_not_updated_version_on_failure_cm(self):
        """ Check version information is not updated if patch loading fails"""

        self._common_setup(LifecycleState.CM, None)

        self._check_load_patch_chunk_does_not_updated_version_on_failure(LifecycleState.CM)

    @testIdentifier('4.2.9')
    def test_load_patch_chunk_does_not_updated_version_on_failure_dm(self):
        """ Check version information is not updated if patch loading fails"""

        self._common_setup(LifecycleState.DM, None)

        self._check_load_patch_chunk_does_not_updated_version_on_failure(LifecycleState.DM)

    @testIdentifier('4.2.10')
    def test_load_patch_chunk_does_not_updated_version_on_failure_se(self):
        """ Check version information is not updated if patch loading fails"""

        self._common_setup(LifecycleState.SECURE, None)

        self._check_load_patch_chunk_does_not_updated_version_on_failure(LifecycleState.SECURE)

