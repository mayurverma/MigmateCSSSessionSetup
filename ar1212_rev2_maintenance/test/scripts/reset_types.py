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
# Enumeration of supported reset types
#
########################################################################################
from enum import Enum, unique


@unique
class ResetType(Enum):
    """Reset type enumeration."""
    CAMERA = 0          # Currently not used but reserved for reset of camera board
    SENSOR_HARD = 1     # Hard sensor reset
    SENSOR_SOFT = 2     # Soft sensor reset

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)