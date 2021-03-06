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
# DO NOT EDIT - This file is auto-generated by the generate_system_phases.py script
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

from enum import IntEnum, unique

@unique
class SystemMgrPhase(IntEnum):
    """System Manager phases enumeration."""
    BOOT = 0          # Default phase at boot
    INITIALIZE = 1    # Initialize phase
    DEBUG = 2         # Debug phase
    PATCH = 3         # Patch phase
    CONFIGURE = 4     # Configure phase
    SESSION = 5       # Session phase
    SHUTDOWN = 6      # Shutdown phase
    NUM_PHASES = 7    # Used for unit testing only

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

