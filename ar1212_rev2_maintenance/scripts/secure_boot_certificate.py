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
# Library to generate ARM certificates for testing.
#
########################################################################################
from crypto_helper import CryptoHelper
from enum import Enum

class CC312LifeCycleState(Enum):
    CM = 0
    DM = 1
    Secure = 5
    RMA = 7

class HbkId(Enum):
    HBK0 = 0
    HBK1 = 1
    HBK  = 2
    HBKNone = 0xF  # match ARM value for an enabler certificate with 3 certificates

class SecureBootCertificate():

    crypto_helper = CryptoHelper()

    KEY_CERT_TOKEN = 0x53426b63
    DEBUG_ENABLER_TOKEN = 0x5364656E
    DEBUG_DEVELOPER_TOKEN = 0x53646465

    HBK_ID_FLAG_BIT_OFFSET = 0
    LCS_ID_FLAG_BIT_OFFSET = 4
    RMA_CERT_FLAG_BIT_OFFSET = 8

    def __init__(self, type, sign_key, cert_version, hbk_id = HbkId.HBK0, lcs = CC312LifeCycleState.Secure, rma = False):

        assert type in ["KEY_CERTIFICATE", "ENABLER_CERTIFICATE", "DEVELOPER_CERTIFICATE"], "Incorrect TYPE value"

        assert sign_key.size_in_bits() == 3072, "Only 3072 bits key are supported"

        self._hbk_id = hbk_id
        self._sign_key = sign_key
        self._type = type
        self._cert_version = cert_version
        self._key_n = sign_key.n
        self._key_np = (2 ** (3072+64+8-1)) // sign_key.n
        self._lcs = lcs
        self._rma = rma
        data_to_sign = self._generate_header()
        data_to_sign += self._generate_public_key()
        data_to_sign += self._generate_body()
        signature = self.crypto_helper.rsassa_pss_sign(self._sign_key, data_to_sign)
        self._signature = signature[::-1] # reverse to big endian

    def _generate_header(self):
        result = bytes(0)
        result += self._magic_number.to_bytes(4, byteorder='little')
        result += self._cert_version.to_bytes(4, byteorder='little')
        result += self._size.to_bytes(4, byteorder='little')
        flags = self._hbk_id.value << SecureBootCertificate.HBK_ID_FLAG_BIT_OFFSET
        flags += (self._lcs.value << SecureBootCertificate.LCS_ID_FLAG_BIT_OFFSET)
        flags += (1 << SecureBootCertificate.RMA_CERT_FLAG_BIT_OFFSET) if self._rma else 0
        result += flags.to_bytes(4, byteorder='little')
        return result

    def _generate_public_key(self):
        result = bytes(0)
        result += self._key_n.to_bytes(96*4, byteorder='big') # note it is big endian
        result += self._key_np.to_bytes(5*4, byteorder='big') # note it is big endian
        return result

    def __bytes__(self):
        result = self._generate_header()
        result += self._generate_public_key()
        result += self._generate_body()
        result += self._signature
        return result

    def __str__(self):
        result  = "Certificate type : {}\n".format(self._type)
        result += "Version   : {}\n".format(self._cert_version)
        result += "size      : {:#X}\n".format(self._size)
        result += self._str_body()
        result += "N         : 0x{:X}\n".format(self._key_n)
        result += "NP        : 0x{:040X}\n".format(self._key_np)
        return result

class SecureBootKeyCert(SecureBootCertificate):

    def __init__(self, sign_key, enabler_pub_key, cert_version = 0, hbk_id = HbkId.HBK0, sw_version = 0):
        assert enabler_pub_key.size_in_bits() == 3072, "Only 3072 bits key are supported"
        self._sw_version = sw_version
        self._magic_number = SecureBootCertificate.KEY_CERT_TOKEN
        self._size = 0x72
        self._enabler_pub_key = enabler_pub_key
        super().__init__("KEY_CERTIFICATE", sign_key, cert_version, hbk_id, lcs = CC312LifeCycleState.CM)

    def _str_body(self):
        result  = "HbkId     : {}\n".format(self._hbk_id.name)
        result += "SW Ver    : {}\n".format(self._sw_version)
        result += "PubKeyHash: 0x{}\n".format(self.crypto_helper.hbk_gen(self._enabler_pub_key).hex().upper())
        return result

    def _generate_body(self):
        result = bytes(0)
        result += self._sw_version.to_bytes(4, byteorder='little')
        result += self.crypto_helper.hbk_gen(self._enabler_pub_key)
        return result

class SecureBootEnablerCert(SecureBootCertificate):

    def __init__(self, sign_key, developer_pub_key, cert_version = 0, hbk_id = HbkId.HBKNone, debug_mask = 0, debug_lock = 0, lcs = CC312LifeCycleState.Secure, rma = False):
        assert developer_pub_key.size_in_bits() == 3072, "Only 3072 bits key are supported"
        self._debug_mask = debug_mask
        self._debug_lock = debug_lock
        self._magic_number = SecureBootCertificate.DEBUG_ENABLER_TOKEN
        self._size = 0x79
        self._developer_pub_key = developer_pub_key
        super().__init__("ENABLER_CERTIFICATE", sign_key, cert_version, hbk_id = hbk_id, lcs = lcs, rma = rma)

    def _generate_body(self):
        result = bytes(0)
        result += self._debug_mask.to_bytes(4*4, byteorder='little')
        result += self._debug_lock.to_bytes(4*4, byteorder='little')
        result += self.crypto_helper.hbk_gen(self._developer_pub_key)
        return result

    def _str_body(self):
        result = "LCS       : {}\n".format(self._lcs.name)
        if (self._hbk_id != HbkId.HBKNone):
            result += "HbkId     : {}\n".format(self._hbk_id.name)
        if self._rma:
            result += "RMA     : {}\n".format(self._rma)
        result += "Debug Mask: {:#034X}\n".format(self._debug_mask)
        result += "Debug Lock: {:#034X}\n".format(self._debug_lock)
        result += "PubKeyHash: 0x{}\n".format(self.crypto_helper.hbk_gen(self._developer_pub_key).hex().upper())
        return result

class SecureBootDeveloperCert(SecureBootCertificate):

    def __init__(self, sign_key, soc_id, cert_version = 0, debug_mask = 0):
        assert len(soc_id) == 32, "Only 32 bytes are supported"
        self._debug_mask = debug_mask
        self._magic_number = SecureBootCertificate.DEBUG_DEVELOPER_TOKEN
        self._size = 0x75
        self._soc_id = soc_id
        super().__init__("DEVELOPER_CERTIFICATE", sign_key, cert_version, lcs = CC312LifeCycleState.CM)

    def _generate_body(self):
        result = bytes(0)
        result += self._debug_mask.to_bytes(4*4, byteorder='little')
        result += self._soc_id
        return result

    def _str_body(self):
        result = "Debug Mask: {:#034X}\n".format(self._debug_mask)
        result += "SOC_ID    : 0x{}\n".format(self._soc_id.hex().upper())
        return result


if __name__ == "__main__":
    print("Just testing...")






