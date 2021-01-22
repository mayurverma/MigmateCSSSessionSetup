#######################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Expose the Branch properties for use case testing.
#
########################################################################################
import configparser
import os
from enum import Enum

class BranchType(Enum):
    """Branch type enumeration."""
    TRUNK = 'trunk'
    AR0820_REV2 = 'branches/ar0820_rev2_maintenance'
    AR1212_REV1 = 'branches/ar1212_rev1_maintenance'
    AR1212_REV2 = 'branches/ar1212_rev2_maintenance'
    # add more here if needed

class BranchProperties():
    """Branch properties class"""

    __properties = None

    @classmethod
    def __get__properties(cls):
        """Read branch.properties file and populate the properties database"""
        if cls.__properties == None:            
            dirname = os.path.dirname(os.path.realpath(__file__))
            filename = os.path.join(dirname, 'branch.properties')
            cls.__properties = configparser.ConfigParser()
            cls.__properties.read(filename)
        return cls.__properties

    @classmethod
    def get_type(cls):
        return BranchType[cls.__get__properties()['DEFAULT']['BRANCH_TYPE']]

    @classmethod
    def get_product_prefix(cls):
        return cls.__get__properties()['DEFAULT']['PRODUCT_PREFIX']


if __name__ == "__main__":
    type = BranchProperties.get_type()
    print('Branch Type : {}'.format(type.name))
    print('Product Prefix : {}'.format(BranchProperties.get_product_prefix()))

