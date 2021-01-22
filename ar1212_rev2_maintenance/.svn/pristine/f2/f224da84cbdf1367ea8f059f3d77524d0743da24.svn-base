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
# Types associated with sessions
#
########################################################################################
from enum import Enum

class SymmetricAuthCipherMode(Enum):
    AES_CMAC = 0
    AES_GCM_GMAC = 1
    RESERVED = 2

class SymmetricAECipherMode(Enum):
    AES_CCM = 0
    AES_GCM = 1
    RESERVED = 2

class AsymmetricCipherMode(Enum):
    RSAES_OAEP = 0
    RESERVED = 1

class SymmetricKeyType(Enum):
    AES_128 = 0
    AES_192 = 1
    AES_256 = 2
    RESERVED = 3

class SessionMode(Enum):
    EPHEMERAL_MODE = 0            # Using RSA encryption to transfer the ephemeral secret
    PSK_MODE = 1                  # The master secret is in OTPM
    UNSECURE_EPHEMERAL_MODE = 2   # Using no encryption to transfer the ephemeral secret (test)

