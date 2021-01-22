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
# DO NOT EDIT - This file is auto-generated by the generate_commands.py script
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

from enum import IntEnum, unique

@unique
class HostCommandCode(IntEnum):
    """CommandHandler host command code enumeration."""

    NULL = 0x0000                         # (0x0000) Null command (default register value)

    GET_STATUS = 0x8000                   # (0x8000) Get CSS status (any phase)
    APPLY_DEBUG_ENTITLEMENT = 0x8001      # (0x8001) Enable a debug feature (Debug phase)
    AUTHORIZE_RMA = 0x8002                # (0x8002) Authorize life-cycle transition to RMA state (Debug phase)
    LOAD_PATCH_CHUNK = 0x8003             # (0x8003) Load a patch chunk (Debug/Patch phase)

    PROVISION_ASSET = 0x8010              # (0x8010) Provision an asset to the device and program to OTPM (Configure phase)
    SELF_TEST = 0x8011                    # (0x8011) Perform a self-test (Configure phase)
    GET_SENSOR_ID = 0x8012                # (0x8012) Retrieve the unique sensor identifier (Configure phase)

    GET_CERTIFICATES = 0x8020             # (0x8020) Retrieve public certificates for sensor authentication (Configure phase)
    SET_SESSION_KEYS = 0x8021             # (0x8021) Negotiate video authentication and control channel session keys using an ephemeral master secret (Configure phase)
    SET_PSK_SESSION_KEYS = 0x8022         # (0x8022) Negotiate video authentication and control channel session keys using a pre-shared master secret (Configure phase)
    SET_EPHEMERAL_MASTER_SECRET = 0x8023  # (0x8023) Set an ephemeral master secret (Configure phase)

    SET_VIDEO_AUTH_ROI = 0x8030           # (0x8030) Set the video authentication region-of-interest (Session phase)
    ENABLE_MONITORING = 0x8031            # (0x8031) Enable firmware monitoring (Session phase)

    DEPU_PROGRAM_OTPM = 0x8850            # (0x8850) Patched-in command to load and burn DEPU assets.

    DEBUG_GET_REGISTERS = 0x8860          # (0x8860) Patched-in debug command to read some registers.

    EXECUTE_DEBUG = 0x8F00                # (0x8F00) Verification Application general purpose debug command to extend test coverage

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)
