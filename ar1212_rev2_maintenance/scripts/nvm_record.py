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
# Library to generate NvmRecords for testing.
#
########################################################################################
from rsa_public_certificate import CertificateAuthority
from enum import Enum

class NvmRecord():

    class Type(Enum):
        RSA_PUBLIC_CERTIFICATE  = 1
        RSA_PRIVATE_CERTIFICATE = 2
        PSK_MASTER_SECRET       = 3
        TRNG_CHARACTERIZATION   = 4
        OTPM_CONFIGURATION      = 5

    def __init__(self, type):
        self.__asset_type = type

    def _get_header_bytes(self):
        result = self.__asset_type.value.to_bytes(2, byteorder='little')
        payload_size = self.asset_length_long_words
        result += payload_size.to_bytes(2, byteorder='little')
        return result

    def _get_payload_bytes(self):
        raise NotImplementedError("Please Implement this method in the subclass")

    def __bytes__(self):
        return self._get_header_bytes() + self._get_payload_bytes()

    def __str__(self):
        result = "NvmRecord Type : {}\n".format(self.__asset_type.name)
        return result

    @property
    def asset_type(self):
        return self.__asset_type

    @property
    def asset_length_long_words(self):
        return len(self._get_payload_bytes()) // 4

class NvmRecordRSAPublicCert(NvmRecord):

    def __init__(self, rsa_public_certificate):
        super().__init__(NvmRecord.Type.RSA_PUBLIC_CERTIFICATE)
        self.__rsa_public_certificate = rsa_public_certificate

    def _get_payload_bytes(self):
        return bytes(self.__rsa_public_certificate)

    def __str__(self):
        result = super().__str__()
        result += str(self.__rsa_public_certificate)
        return result

    @property
    def certificate(self):
        return self.__rsa_public_certificate

class NvmRecordRSAPrivateKey(NvmRecord):

    class RSAType(Enum):
        #private key types
        RSA_BASIC_2048 = 1
        RSA_BASIC_3072 = 2
        RSA_BASIC_4096 = 3

    def __init__(self, rsa_key, auth_id = CertificateAuthority.VENDOR_A):
        super().__init__(NvmRecord.Type.RSA_PRIVATE_CERTIFICATE)
        self.__rsa_key = rsa_key
        assert self.__rsa_key.has_private(), "The RSA key is not valid"

        self._key_size_bits = self.__rsa_key.size_in_bits()

        if 2048 == self._key_size_bits:
            self.__key_type = self.RSAType.RSA_BASIC_2048
        elif 3072 == self._key_size_bits:
            self.__key_type = self.RSAType.RSA_BASIC_3072
        elif 4096 == self._key_size_bits:
            self.__key_type = self.RSAType.RSA_BASIC_4096
        else:
            assert False, "Key Size not supported"
        self.__certificate_auth_id = auth_id

    def _get_payload_bytes(self):
        result = self.__certificate_auth_id.value.to_bytes(2, byteorder='little')
        result += self.__key_type.value.to_bytes(2, byteorder='little')
        result += self.__rsa_key.d.to_bytes(self._key_size_bits // 8, byteorder='big')
        return result

    def __str__(self):
        result = super().__str__()
        result += "Certificate key size     : {}\n".format(self._key_size_bits)
        result += "Certificate Authority ID : {}\n".format(self.__certificate_auth_id.name)
        result += "Certificate Private Key  : {}\n".format(hex(self.__rsa_key.d))
        return result

    @property
    def certificate_auth_id(self):
        return self.__certificate_auth_id

    @property
    def key_type(self):
        return self.__key_type

    @property
    def private_key(self):
        return self.__rsa_key.d

class NvmRecordPSKMasterSecret(NvmRecord):

    class AESType(Enum):
        #private key types
        AES_128 = 0
        AES_192 = 1
        AES_256 = 2

    def __init__(self, aes_key):
        super().__init__(NvmRecord.Type.PSK_MASTER_SECRET)
        self.__key = aes_key
        self.__key_size_bits = len(self.__key) * 8

        if 128 == self.__key_size_bits:
            self._priv_key_type = self.AESType.AES_128
        elif 192 == self.__key_size_bits:
            self._priv_key_type = self.AESType.AES_192
        elif 256 == self.__key_size_bits:
            self._priv_key_type = self.AESType.AES_256
        else:
            assert False, "Key Size not supported"

    def _get_payload_bytes(self):
        result = self._priv_key_type.value.to_bytes(2, byteorder='little')
        result += (0).to_bytes(2, byteorder='little')
        result += self.__key
        return result

    def __str__(self):
        result = super().__str__()
        result += "Certificate key size     : {}\n".format(self.__key_size_bits)
        result += "Certificate Private Key  : {}\n".format(self.__key.hex())
        return result

    @property
    def master_secret(self):
        return self.__key

class NvmRecordTRNGCharacterization(NvmRecord):

    def __init__(self, r1, r2, r3, r4):
        super().__init__(NvmRecord.Type.TRNG_CHARACTERIZATION)
        self._values = []
        self._values.append(r1)
        self._values.append(r2)
        self._values.append(r3)
        self._values.append(r4)

    def _get_payload_bytes(self):
        result = bytes(0)
        for value in self._values:
            result += value.to_bytes(4, byteorder='little')
        return result

    def __str__(self):
        result = super().__str__()
        result += "TRNG values               : {}\n".format(self._values)
        return result

class NvmRecordOtpmConfig(NvmRecord):

    def __init__(self, otpm_config):
        super().__init__(NvmRecord.Type.OTPM_CONFIGURATION)
        self._config = otpm_config

    def _get_payload_bytes(self):
        return bytes(self._config)

    def __str__(self):
        result = super().__str__()
        result += "OTPM value                : {}\n".format(self._config)
        return result

if __name__ == "__main__":
    print("Just testing...")

