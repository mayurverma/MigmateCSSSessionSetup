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
# Public Certificate with OnSemi format
#
########################################################################################
from crypto_helper import CryptoHelper
from enum import Enum

class CertificateAuthority(Enum):
    """Certificate Authority identifier enumeration
    """
    VENDOR_A = 0    # Identifies certificates signed by the Vendor A certificate authority
    VENDOR_B = 1    # Identifies certificates signed by the (optional) Vendor B certificate authority
    UNKNOWN  = 2    # Unknown/unitialised

class CertificatePurpose(Enum):
    """Certificate purpose enumeration
    """
    VENDOR   = 0    # The Vendor certificate signed by the Vendor certificate authority
    SENSOR   = 1    # The Sensor certificate signed by the Vendor certificate authority
    UNKNOWN  = 2    # Unknown/unitialised

class RSAPublicCertificate():

    crypto_helper = CryptoHelper()

    #certificate headers
    TYPE_2048_BIT = 0x4332
    TYPE_3072_BIT = 0x4333
    TYPE_4096_BIT = 0x4334
    # Flags
    PURPOSE_FIELD_BIT_OFFSET = 0
    AUTH_ID_FIELD_BIT_OFFSET = 2
    VERSION_FIELD_BIT_OFFSET = 4
    NUMBER_FIELD_BIT_OFFSET  = 6

    PURPOSE_FIELD_BIT_MASK = 0x3
    AUTH_ID_FIELD_BIT_MASK = 0x3
    VERSION_FIELD_BIT_MASK = 0x3
    NUMBER_FIELD_BIT_MASK  = 0x3FFFFFF

    #TODO big or little endian
    RSA_ENDIANNESS = 'big'

    def __init__(self, public_key, sign_key_pair = None, auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR, version = 0, number = 0, autosign = True):
        self.__rsa_public_key = public_key.publickey()

        if sign_key_pair:
            self._sign_key = sign_key_pair
        else:
            self._sign_key = public_key

        assert self._sign_key.size_in_bits() == self.__rsa_public_key.size_in_bits(), "No support for different size keys in the certificate"

        self._key_size_bits = self._sign_key.size_in_bits()

        if 2048 == self._key_size_bits:
            self._cert_type = self.TYPE_2048_BIT
        elif 3072 == self._key_size_bits:
            self._cert_type = self.TYPE_3072_BIT
        elif 4096 == self._key_size_bits:
            self._cert_type = self.TYPE_4096_BIT
        else:
            assert False, "Key Size not supported"

        self.__certificate_auth_id = auth_id
        self.__certificate_version = version
        self.__certificate_number = number
        self.__certificate_purpose = purpose

        if autosign:
            assert self._sign_key.has_private(), "The signing key is not valid"
            self.__rsa_signature = self.crypto_helper.rsassa_pss_sign(self._sign_key, self._generate_signed_info())
            if self.RSA_ENDIANNESS == 'big':
                self.__rsa_signature = self.__rsa_signature[::-1]

    def _generate_signed_info(self):
        signed_info = bytes(0)
        signed_info +=  self._cert_type.to_bytes(2, byteorder='little')
        offset = 2 + (self._key_size_bits // 32)
        signed_info += offset.to_bytes(2, byteorder='little')
        info =  ((self.__certificate_purpose.value & self.PURPOSE_FIELD_BIT_MASK) << self.PURPOSE_FIELD_BIT_OFFSET)
        info += ((self.__certificate_auth_id.value & self.AUTH_ID_FIELD_BIT_MASK) << self.AUTH_ID_FIELD_BIT_OFFSET)
        info += ((self.__certificate_version & self.VERSION_FIELD_BIT_MASK) << self.VERSION_FIELD_BIT_OFFSET)
        info += ((self.__certificate_number  & self.NUMBER_FIELD_BIT_MASK) << self.NUMBER_FIELD_BIT_OFFSET)
        signed_info += info.to_bytes(4, byteorder='little')
        signed_info += self.__rsa_public_key.n.to_bytes(self._key_size_bits // 8, byteorder=self.RSA_ENDIANNESS)

        return signed_info

    def __bytes__(self):
        return self._generate_signed_info() + self.__rsa_signature

    @classmethod #Deserializer
    def get_and_validate_certificate_from_bytes(cls, validation_key, raw_data):

        #process header
        info = int.from_bytes(raw_data[4:8], byteorder='little')
        purpose = (info >> cls.PURPOSE_FIELD_BIT_OFFSET) & cls.PURPOSE_FIELD_BIT_MASK
        auth_id = (info >> cls.AUTH_ID_FIELD_BIT_OFFSET) & cls.AUTH_ID_FIELD_BIT_MASK
        version = (info >> cls.VERSION_FIELD_BIT_OFFSET) & cls.VERSION_FIELD_BIT_MASK
        number  = (info >> cls.NUMBER_FIELD_BIT_OFFSET)  & cls.NUMBER_FIELD_BIT_MASK

        offset = int.from_bytes(raw_data[2:4], byteorder='little')
        key_size_bytes = (offset - 2) * 4
        modulus = int.from_bytes(raw_data[8:8 + (key_size_bytes)], byteorder=cls.RSA_ENDIANNESS)
        e = 65537
        public_key = cls.crypto_helper.generate_rsa_key_from_components(modulus, e)

        if validation_key is not None:
            sign_key = validation_key
            key_size_bits = sign_key.size_in_bits()
            #cert_type = int.from_bytes(raw_data[0:2], byteorder='little')
            assert key_size_bytes == key_size_bits // 8, "Validation key size not expected {} vs {}".format(key_size_bytes, key_size_bits // 8)
        else:
            #assuming that if validation_key is not given then is autosign
            sign_key = public_key

        signature = raw_data[(offset*4):]
        data_sign = raw_data[:(offset*4)]
        signature_little = signature[::-1] if cls.RSA_ENDIANNESS == 'big' else signature
        assert cls.crypto_helper.rsassa_pss_verify(sign_key, data_sign, signature_little), "Signature incorrect"

        cert = RSAPublicCertificate(public_key, sign_key, auth_id = CertificateAuthority(auth_id), purpose = CertificatePurpose(purpose), version = version, number = number, autosign = False)
        cert.__rsa_signature = signature
        assert data_sign == cert._generate_signed_info() , "The generated Certificate doesn't generate the same body contents"
        return cert

    def __str__(self):
        result  = "Certificate key size     : {}\n".format(self._key_size_bits)
        result += "Certificate Type         : {}\n".format(hex(self._cert_type))
        result += "Certificate Authority ID : {}\n".format(self.__certificate_auth_id.name)
        result += "Certificate Version      : {}\n".format(self.__certificate_version)
        result += "Certificate Number       : {}\n".format(hex(self.__certificate_number))
        result += "Certificate Purpose      : {}\n".format(self.__certificate_purpose.name)
        result += "Certificate Public Key   : {}\n".format(hex(self.__rsa_public_key.n))
        result += "Certificate Signature    : {}\n".format(self.__rsa_signature.hex())
        return result


    @property
    def certificate_purpose(self):
        return self.__certificate_purpose

    @property
    def certificate_auth_id(self):
        return self.__certificate_auth_id

    @property
    def certificate_version(self):
        return self.__certificate_version

    @property
    def certificate_number(self):
        return self.__certificate_number

    @property
    def rsa_public_key(self):
        return self.__rsa_public_key

    @property
    def rsa_signature(self):
        return self.__rsa_signature


if __name__ == "__main__":
    print("Just testing...")

