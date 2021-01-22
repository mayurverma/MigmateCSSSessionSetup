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
# Symmetric key type identifier
####################################################################################
from enum import IntEnum, unique

class SymmetricKeyType(IntEnum):
    """Symmetric Key Type enumeration
    """
    AES_128 = 0         # 128-bit AES key
    AES_192 = 1         # 192-bit AES key
    AES_256 = 2         # 256-bit AES key
    AES_RESERVED = 3    # Reserved for future use

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

