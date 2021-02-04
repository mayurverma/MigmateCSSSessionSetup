#######################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Softwar").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
########################################################################################
#
# Patching Loader class
#
########################################################################################
import os
import logging
import configparser
import glob

from common_error import Error
from crypto_helper import CryptoHelper
from process_map import ProcessMap
from patch_helper import get_chunks, Command
from patch_type import PatchType
from build_properties import BuildProperties
from host_command_code import HostCommandCode
from host_command_load_patch_chunk import HostCommandLoadPatchChunkParams, HostCommandLoadPatchChunkResponse
from lifecycle_state import LifecycleState
from branch_properties import BranchProperties
from test_fixture import FixtureType

class PatchLoaderBase(object):
    """Patch loader abstract base class."""
    def __init__(self, fixture_type, command_helper, verbosity):
        self._fixture_type = fixture_type
        self._command_helper = command_helper
        self._verbosity = verbosity
        self.__logger = logging.getLogger('PatchLoaderBase')

    def _execute_command(self, asset, expected_result = Error.SUCCESS):
        """Executes a single load patch chunk command and checks the response."""
        params = HostCommandLoadPatchChunkParams()
        response = HostCommandLoadPatchChunkResponse()
        params.asset_payload = asset

        result = self._command_helper.execute(HostCommandCode.LOAD_PATCH_CHUNK,
            params, response, verbosity = self._verbosity)

        assert expected_result == result, 'LOAD_PATCH_CHUNK returned error code {}'.format(result.name)
        return result

    def get_dist_basename(self, patch_type, patch_version = None):
        """Returns the distribution basename of the patch/application files."""
        product_prefix = BranchProperties.get_product_prefix()
        if patch_version is None:
            patch_version = self.get_patch_version(patch_type)
        return '{}_{}_v{:04X}'.format(product_prefix, patch_type.value, patch_version)

    def get_patch_chunk_basename(self, current_lcs, patch_type, patch_version = None, key_set_name = None):
        """Returns the basename of the patch file chunks."""
        dist_basename = self.get_dist_basename(patch_type, patch_version)
        if key_set_name is None:
            if (LifecycleState.CM == current_lcs):
                key_set_name = 'clear'
            elif (self._fixture_type in (FixtureType.MODEL, FixtureType.SIM)):
                key_set_name = 'test'
            else:
                key_set_name = 'eval'
        return '{}_{}'.format(dist_basename, key_set_name)

    def get_patch_dirname(self, patch_type):
        """Returns the dirname prefix of any patch file path."""
        return BuildProperties[patch_type.name].output_path

    def get_patch_binary_path(self, patch_type):
        """Returns the path to the patch binary file."""
        return os.path.join(self.get_patch_dirname(patch_type), '{}.bin'.format(patch_type.value))

    def get_patch_map_path(self, patch_type):
        """Returns the path to the patch map file."""
        return os.path.join(self.get_patch_dirname(patch_type), '{}.map'.format(patch_type.value))

    def get_patch_version(self, patch_type):
        """Returns the patch/application version."""
        return BuildProperties[patch_type.name].patch_version

    def get_rom_version(self):
        """Returns the ROM version."""
        return BuildProperties.ROM.rom_version

    def load_patch_assets(self, patch_assets, expected_result = Error.SUCCESS):
        """Loads an ordered set of patch assets into the device."""
        for description, asset in patch_assets:
            if self._verbosity == logging.INFO:
                self.__logger.log(self._verbosity, 'Loading {}'.format(description))
            result = self._execute_command(asset, expected_result)
            if Error.SUCCESS != result:
                break

        return result


class PatchBinaryLoader(PatchLoaderBase):
    """Patch loader that finds the patch binary file, converts it to chunks and then loads it into the device."""
    def __init__(self, fixture_type, command_helper, verbosity=logging.INFO):
        super().__init__(fixture_type, command_helper, verbosity)
        self.__crypto_helper = CryptoHelper()

    def get_patch_data(self, patch_type, patch_version = None):
        """Returns a sorted list of unencrypted patch chunks ready for loading."""

        # Get the version information
        rom_version = self.get_rom_version()
        if patch_version is None:
            patch_version = self.get_patch_version(patch_type)

        # Get the binary and map file paths
        patch_binary_path = self.get_patch_binary_path(patch_type)
        patch_map_path = self.get_patch_map_path(patch_type)

        # Use PatchHelper.get_chunks() to get the chunks from the binary file
        return get_chunks(rom_version, patch_version, patch_binary_path, patch_map_path)

    def get_patch_assets(self, current_lcs, patch_type, patch_version = None, key = None):
        """Returns a sorted list of patch assets ready for loading."""
        patch_data = self.get_patch_data(patch_type, patch_version)
        rom_version = patch_data.rom_version
        asset_list = []
        for chunk in patch_data.chunks:
            chunk_num = chunk.this_chunk
            description = '{} Chunk {:3} Version {:6}({}) to Rom {}'.format(
                patch_name, chunk_num, patch_version, hex(patch_version), rom_version)
            if (LifecycleState.CM == current_lcs):
                cmd = Command().generate(chunk.asset_id(), chunk.image())
                asset = cmd.image()
            else:
                asset = chunk.asset_id().to_bytes(4, 'little')
                if None == key:
                    key = self.__crypto_helper.KPICV_DEFAULT
                asset += self.__crypto_helper.asset_encrypter(key, chunk.asset_id(), chunk.image())
            asset_list.append((description, asset))

        return asset_list

    def load_patch(self, current_lcs, patch_type, patch_version = None, key = None, expected_result = Error.SUCCESS):
        """Finds the requested patch binary file, converts it to chunks and loads it into the device."""

        patch_assets = self.get_patch_assets(current_lcs, patch_type, patch_version, key)
        self.load_patch_assets(patch_assets, expected_result)


class PatchLoader(PatchLoaderBase):
    """Default patch loader that uses loads pre-built patch chunk files."""
    def __init__(self, fixture_type, command_helper, verbosity=logging.INFO):
        super().__init__(fixture_type, command_helper, verbosity)

    def get_patch_assets(self, current_lcs, patch_type, patch_version = None, key_set_name = None):
        """Returns a sorted list of patch assets ready for loading."""
        patch_dirname = self.get_patch_dirname(patch_type)
        patch_basename = self.get_patch_chunk_basename(current_lcs, patch_type, patch_version, key_set_name)
        patch_filter = os.path.join(patch_dirname, '{}_c??.bin'.format(patch_basename))
        patch_chunk_paths = glob.glob(patch_filter)
        assert len(patch_chunk_paths) > 0, "Error : No files matching {} (Try building the appropriate patch)".format(patch_filter)
        asset_list = []
        for patch_chunk_path in sorted(patch_chunk_paths):
            with open(patch_chunk_path, 'rb') as istream:
                asset = istream.read()
                asset_list.append((patch_chunk_path, asset))

        return asset_list

    def load_patch(self, current_lcs, patch_type, patch_version = None, key_set_name = None, expected_result = Error.SUCCESS):
        """Finds the relevant patch and loads it using one or more LoadPatchChunk commands"""
        patch_assets = self.get_patch_assets(current_lcs, patch_type, patch_version, key_set_name)
        self.load_patch_assets(patch_assets, expected_result)


if __name__ == "__main__":
    print("Just testing...")

