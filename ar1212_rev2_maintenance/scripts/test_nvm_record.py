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
# Unittest for NVM record
#
########################################################################################
import unittest
from crypto_helper import CryptoHelper
from otpm_config import OtpmConfig
from rsa_public_certificate import RSAPublicCertificate, CertificateAuthority
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret, NvmRecordTRNGCharacterization, NvmRecordOtpmConfig

class TestNvmRecord(unittest.TestCase):


    def setUp(self):
        self.crypto_helper = CryptoHelper()

    def test_basic_rsa_public_cert(self):
        rsa_key = self.crypto_helper.generate_random_rsa_key()
        cert = RSAPublicCertificate(rsa_key, number = 1, version = 3)
        c = NvmRecordRSAPublicCert(cert)
        c1 = RSAPublicCertificate.get_and_validate_certificate_from_bytes(rsa_key, bytes(c)[4:])
        self.assertEqual(bytes(c)[0:2], NvmRecord.Type.RSA_PUBLIC_CERTIFICATE.value.to_bytes(2, byteorder='little'))
        self.assertEqual(bytes(c)[4:], bytes(c1))

    def test_basic_rsa_private_cert(self):
        rsa_key = self.crypto_helper.generate_random_rsa_key()
        c = NvmRecordRSAPrivateKey(rsa_key, CertificateAuthority.VENDOR_A)
        self.assertEqual(bytes(c)[0:2], NvmRecord.Type.RSA_PRIVATE_CERTIFICATE.value.to_bytes(2, byteorder='little'))

    def test_basic_psk_master_secret(self):
        aes_key = bytes([0,1,2,3,4,5,6,7,8,9,0xa,0xb,0xc,0xd,0xe,0xf])
        c = NvmRecordPSKMasterSecret(aes_key)
        self.assertEqual(bytes(c)[0:2], NvmRecord.Type.PSK_MASTER_SECRET.value.to_bytes(2, byteorder='little'))

    def test_basic_trng(self):
        c = NvmRecordTRNGCharacterization(1,2,3,4)
        self.assertEqual(bytes(c)[0:2], NvmRecord.Type.TRNG_CHARACTERIZATION.value.to_bytes(2, byteorder='little'))

    def test_basic_otpm(self):
        otpm_config = OtpmConfig(1,2,3,4,5,6,7,8,9,0,11,12,13,14,15,16,17,18,19,20,21)
        c = NvmRecordOtpmConfig(otpm_config)
        self.assertEqual(bytes(c)[0:2], NvmRecord.Type.OTPM_CONFIGURATION.value.to_bytes(2, byteorder='little'))


if __name__ == "__main__":
    unittest.main()





