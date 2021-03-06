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

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# DO NOT EDIT - This file is auto-generated by the generate_lifecycle_states.py script
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

from enum import IntEnum, unique

@unique
class LifecycleState(IntEnum):
    """Lifecycle states enumeration."""
    CM = 0       # Chip Manufacture
    DM = 1       # Device Manufacture
    SECURE = 2   # Secure
    RMA = 3      # RMA

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

