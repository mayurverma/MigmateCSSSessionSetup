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
# Enumeration of loadable components e.g. applications and maintenance patch
#
########################################################################################
from enum import Enum

class PatchType(Enum):
    """Patch type enumeration."""
    CMPU            = 'CmpuApp'
    DEPU            = 'DepuApp'
    DMPU            = 'DmpuApp'
    PATCH           = 'Patch'
    PROD_TEST       = 'ProdTestApp'
    SDPU            = 'SdpuApp'
    TRNG            = 'TrngApp'
    VALIDATION      = 'ValidationApp'
    VERIFICATION    = 'VerificationApp'


if __name__ == "__main__":
    patch = PatchType.VALIDATION
    print('{} : {}'.format(patch.name, patch.value))

