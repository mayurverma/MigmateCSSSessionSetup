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
# Expose the ROM build properties for use case testing.
#
########################################################################################
import configparser
import os

from enum import Enum
from patch_type import PatchType

class BuildProperties(Enum):
    """ROM build properties class"""
    ROM         = 'Rom'
    # Patches follow the same naming as PatchType
    CMPU            = PatchType.CMPU.value
    DEPU            = PatchType.DEPU.value
    DMPU            = PatchType.DMPU.value
    PATCH           = PatchType.PATCH.value
    PROD_TEST       = PatchType.PROD_TEST.value
    SDPU            = PatchType.SDPU.value
    TRNG            = PatchType.TRNG.value
    VALIDATION      = PatchType.VALIDATION.value
    VERIFICATION    = PatchType.VERIFICATION.value

    def __init__(self, dirname):
        """Parse the build properties file."""
        root_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
        self.__output_path = os.path.join(root_dir, 'src', self.value, 'output', 'asic.armcc')
        filename = os.path.join(self.__output_path, 'build.properties')
        self.__properties = configparser.ConfigParser()
        self.__properties.read(filename)

    def __get__properties(self, property):
        """Check for properties and return if present"""
        if property in self.__properties['DEFAULT']:
            return self.__properties['DEFAULT'][property]
        else:
            raise NotImplementedError()

    @property
    def rom_version(self):
        return int(self.__get__properties('BUILD_ROM_VERSION'), 0)

    @property
    def patch_version(self):
        return int(self.__get__properties('BUILD_PATCH_VERSION'), 0)

    @property
    def output_path(self):
        return self.__output_path


if __name__ == "__main__":
    print('ROM Version   : 0x{:04X}'.format(BuildProperties.ROM.rom_version))
    print('PATCH Version : 0x{:04X}'.format(BuildProperties.PATCH.patch_version))
    print('output path   : {}'.format(BuildProperties.PATCH.output_path))


