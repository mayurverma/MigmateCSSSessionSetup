####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Root-of-Trust identifier
####################################################################################
from enum import IntEnum, unique

class RootOfTrust(IntEnum):
    """Root-of-trust enumeration
    """
    CM = 0      # Chip Manufacturer root-of-trust
    DM = 1      # Device Manufacturer root-of-trust
    CD = 2      # Chip Designer root-of-trust
    UNKNOWN = 3 # Unknown/unitialised

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

