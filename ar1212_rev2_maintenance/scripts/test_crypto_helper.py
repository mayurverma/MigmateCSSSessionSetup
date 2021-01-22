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
# Unittest for All crypto
#
########################################################################################
import unittest
from crypto_helper import CryptoHelper

class AssetTestExample1():
    # asset precomputed in the command line with the following parameters
    # key-filename =  icv_key_enc.bin
    # keypwd-filename = icv_key_pwd.txt
    # asset-id   = 0x12345678
    # asset-filename = asset.bin
    # asset-pkg = asset_pkg.bin
    icv_key = bytes([0x63, 0x61, 0x66, 0x65, 0x62, 0x61, 0x62, 0x65, 0x64, 0x65, 0x61, 0x64, 0x62, 0x65, 0x65, 0x66])
    asset_id = 0x12345678
    asset_bin = bytes(16)
    nonce = bytes([0xD9,  0x25,  0x9E,  0x5C,  0xD2,  0xE7,  0x09,  0x1D,  0xA3,  0x15,  0x92,  0x09])
    asset_pkg = bytes([0x74, 0x65, 0x73, 0x41, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD9, 0x25, 0x9E, 0x5C, 0xD2, 0xE7, 0x09, 0x1D, 0xA3, 0x15, 0x92, 0x09, 0xC1, 0xFF, 0xEB, 0xDF, 0x1D, 0xB1, 0xCC, 0x99, 0x84, 0x12, 0x7A, 0xC9, 0x1D, 0xB0, 0xB8, 0x92, 0x1F, 0xD7, 0x13, 0xFF, 0x7C, 0x6D, 0x72, 0xA3, 0xBD, 0x63, 0x0C, 0x12, 0xCA, 0xE0, 0x3F, 0xD1])


class TestCryptoHelper(unittest.TestCase):


    def setUp(self):
        self.crypto_helper = CryptoHelper()

    def test_basic_ccm(self):
        self.crypto_helper.rand_func = lambda x : bytes(x) # deterministic

        nonce = bytes([0]*12)
        session_key = bytes([0]*16)
        msg = bytes([0]*42)
        ad = bytes([0]*16)
        ciphertext, tag = self.crypto_helper.ccm_encrypt(session_key, nonce, msg, ad)
        #print("Tag           :", len(tag), tag)
        #print("ciphertext    :", len(ciphertext), ciphertext.hex())
        self.assertEqual(len(ciphertext), len(msg))
        self.assertEqual(len(tag), 16)
        self.assertEqual(self.crypto_helper.get_number_zero_bits(nonce), (12*8))

    def test_basic_rsa(self):
        rsa_key = self.crypto_helper.generate_random_rsa_key()
        msg = bytes([0]*42)
        ciphertext = self.crypto_helper.rsa_oaep_encrypt(rsa_key, msg)
        self.assertEqual(msg, self.crypto_helper.rsa_oaep_decrypt(rsa_key, ciphertext))
        self.assertEqual(len(ciphertext), rsa_key.size_in_bytes())

        signature = self.crypto_helper.rsassa_pss_sign(rsa_key, msg)
        self.assertTrue(self.crypto_helper.rsassa_pss_verify(rsa_key, msg, signature))

    def test_basic_kdf(self):
        session_key = bytes([0]*16)
        nonce = bytes([0]*12)
        keyd = self.crypto_helper.hkdf(session_key, 32, nonce)
        self.assertEqual(len(keyd), (32))
        #print("Key derived   :", len(keyd), keyd.hex())

    def test_basic_key_management(self):
        key = 'cafebabedeadbeef'.encode()
        self.crypto_helper.aes_export_key(key, 'icv_key_2.bin', 'hi', mode = CryptoHelper.AES_128_CBC)
        key2 = self.crypto_helper.aes_import_key('icv_key_2.bin', 'hi', mode = CryptoHelper.AES_128_CBC)
        self.assertEqual(key, key2)

    def test_basic_asset_encrypter(self):
        a = self.crypto_helper.asset_encrypter(AssetTestExample1.icv_key, AssetTestExample1.asset_id, AssetTestExample1.asset_bin, nonce = AssetTestExample1.nonce)
        self.assertEqual(bytes(a), AssetTestExample1.asset_pkg)
        associated_data, nonce, msg, tag = self.crypto_helper.asset_decrypter(AssetTestExample1.icv_key, AssetTestExample1.asset_id, a)

        self.assertEqual(msg, AssetTestExample1.asset_bin)
        self.assertEqual(nonce, AssetTestExample1.nonce)

if __name__ == "__main__":
    unittest.main()





