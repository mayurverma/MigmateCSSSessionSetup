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
# Library for crypto algorithms used in the comunications beetween the Host and the sensor (AR0820)
#
########################################################################################
import os
import sys

# Add Python libraries to the import search path
rootdir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if os.name == 'nt': # TODO check is 64bits here?
    pycryptodome_dir = os.path.join(rootdir, 'contrib', 'pycryptodome', 'win64')
else: #assuming is linux
    pycryptodome_dir = os.path.join(rootdir, 'contrib', 'pycryptodome', 'linux')

if pycryptodome_dir not in sys.path:
    sys.path.append(pycryptodome_dir)

from Crypto.PublicKey import RSA
from Crypto.Signature import pss
from Crypto.Random import get_random_bytes
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Hash import SHA256, CMAC, MD5
from Crypto.Util import Padding
from Crypto.Protocol.KDF import HKDF
from Crypto.IO import PKCS8


class CryptoHelper():

    #encryption modes to save AES key files:
    PKCS8 = 0
    AES_128_CBC = 1 # used when excuting openssl enc -d/e -nosalt -aes-128-cbc

    # The default huk supported by the default OTPM images
    HUK_DEFAULT = b'\x08\x04\x02\x01\x01\x01\x01\x01\x08\x04\x02\x01\x02\x02\x02\x02\x08\x04\x02\x01\x04\x04\x04\x04\x08\x04\x02\x01\x08\x08\x08\x08'

    # The default Kpicv provisioning key: supported by the default OTPM images
    # dm_otpm and secure_boot_otpm (in test/src)
    KPICV_DEFAULT = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00'

    KCP_DEFAULT = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x0f'

    # The default Hbk0 minimum version
    HBK0_MIN_VERSION = b'\x01\x00\x00\x00\x00\x00\x00\x00'

    HBK0_KEY_MODULUS_DEFAULT = 0xbf5dc7afd4228245b9eca696d1fcaab9a81991a76f12bf451623ba66867b1a40e9205aa36097d7789f17b9b828c834eeabda83f2413ab47f202f35506e6f4c445f40eff920576a31c02496b6246cf974fee683691c3cd2342009f3676ce77ab170b640375057a29509806e3498d09ed273a2aa11890be39463b256ca6af4e15e4038c5e459e70f182423111111d91a148e9363edb3c107de264d734333ae76235e49f0fd41e06ad078cadf9382d5bb27c1688a48bd84fb4932a8932e55b8336d1e3f6cefed84de4ce86e07a232c8a238549dcfadc15e08b5d997eb65d3daf0c58db03ed5e7f750eb8f546e75b3f48b836bf6a4107f740a8b362d9c69310edb678ecc78a10f78408ab7a25bc02d03ed020725fd79c3bb559719697fe1e3149961abe88bf62cf518065186bc94319ca1ed41de1ed03763caa4b461143488481b6e10f6e1114f2a1bf61c0913fa89104fc0a5be762534480bd23e84479d82f0eeb4360b0deef89120c911e5b486dadaec9b20ba99291c8c8b120dc4ee3ec75e0231
    HBK0_KEY_PRIV_EXP_DEFAULT = 0x395bc71c2d25f0cd2c76c1d207fbc742619fd4580ec35c403d6c151560eeaa0655ba0460def57438abdae5284d5501049ecf1ee87063c3b12cf8e96e83c369653ef5cc57ec393753d527426332d4d50a9bd28505eed84e1b20954af72f9dcc593e12ea7989e3d290006a95ddbf061cecb08b4b9578f1eed34df4a8ee0014092f0269e1ead98597b12137bf8851c4b47bffe62eb8133e62f8d39b3856a6b97ea08844fa6e369e28f5f696bc5fcf01f81868f0c82e185c3123960310267dbbf9f190703c4226b63eb6fe0c195ecbbce94a88d3a62834752d7592ef06403a07bccf6425961b19b8aa1c3249b25249da0dfcb50cd29dfcb0703b7279f84455fc0590212f55c872273c1c3d42bd7615f57c6ff08ce5fbd8c1c46ce44aad7963d0c64ced195a80a3b4cb3f310e77d549d7e504ebc1f794b1f5a09d03fcd149f67fee94a040d273818756270f5bb7b6b70ab98d4683e53f8c3cdf2253b7a51d31b78eb38d2ffaab76ed360f6cb1294703949bc577258a2932c771ef62e05bb23ec1f8f7

    HBK1_KEY_MODULUS_DEFAULT = 0xb50bfe84639a5406b0a80331ca689a540d9fbbdabe935689124aac7ecb1172e08b9bcfb9ffefd78e8f3f13038da35d3bea83e6a049c23383a79b8a46e7c351c631a5fc53a2851567617a911ef00ffb54b361bdd1df005c4f2ca9c00eefc3cb7d7d34f7d550883ca62a0297766b5aaceee3de9928d290a2fc0075f2c4fea5b3e91d79aa9cf2ec38b34e24ec7897c4eac52dcaa8e5bdc484e2f58dfc6860f8d2e781b9c40a2eef7c634c579f625187781af338adcc3e6008bd0e5ba67bb5df70dc8de17669552088cf11b362eeba4063ad9e5a57b7232913df1da6f546f86851766a2ebe3f33e21c2303b552a1711f4764edcdc062cd13998d072c3ee3a1121efc7c612746d56fd54007312e19d9dda9547e4531f822deed87c7e180e9684e29d8d7983f3d1bfaaabb51eefb2c1e752ebf1dcf2f9d81008f368f0e2ae383915a2fd229b2f239ca184664b850779d271b19b5b93cc20b2e979feade749953b796fd2b25f9b94f09181cc46ecf9f901576f05a89fcca3c12118d93044e48cb6b187b
    HBK1_KEY_PRIV_EXP_DEFAULT = 0x81a3a50c8785eac51f9a2f2f68109ef1b10fc02452728e747874a02fea9903f965fe9c3043e42a261accbf80bbdb4ed7a885ea8ebc42234112b37dc06814a4699703943b0f14dcceba2e59dc1d6a543dc2752ac72d37c4e1acc9a08d1182d223799484226b1822d5991f61f599c9c42b0b6049abc19b7019a2fad26bdd2c2a37b2b37c48066abfef24f5ab9e6e8ec2cd8e8c9f8462ee65795efbdcd1a11e8b66daad7a831038cc890e11703fa6c1aa6f21da6418ec5137f0a867583f1d6c7213472185a4bf31d20b84a2b20ea6a8e253833bdabceb148687c8101ad774f3f758c74842f9c7a4f207ff5a263ef8fe10027e140d6ad4aa2c4a6533a13f9439f69b72da82401af7f6b59f779eede9cc31f85b0efd95fa00fc26ee9ea88921bed42a445f0536176998dee5d2c5047588a8cbc55bfd108711143b26051b0b62935a93ea882590385ae131ff02152bdaf69a3a083499e86616049fb926a60827cb3bfb6c26f5d7e888ececc768bd5f1fd9a6ea8e73dd5cc16ec4279b0b93010114391

    def __init__(self):
        # This are the recommended defaults.
        # Do not change them unless you know what you are doing (mainly for testing)
        self.hash_algo = SHA256
        # This can be changed to make it deterministic - > no random values (i.e. rand_func = lambda x : bytes(x))
        self.rand_func = get_random_bytes
        self.protection = 'PBKDF2WithHMAC-SHA1AndAES256-CBC'

    def _md5_key_iv_derivation(self, password):
        # Generate the derived key and IV (https://www.openssl.org/docs/man1.0.2/crypto/EVP_BytesToKey.html)
        if type(password) == str:
            password = password.encode()

        h = MD5.new(password)
        key_derived = h.digest()
        h = MD5.new(key_derived + password)
        iv_derived = h.digest()
        return key_derived, iv_derived

    ##################### key derivation #####################

    def hkdf(self, master, key_len, salt, context=None, num_keys=1):
        return HKDF(master, key_len, salt, hashmod = self.hash_algo, context = context, num_keys = num_keys)


    ##################### AES encryption #####################

    def _aes_aead_mode_encrypt(self, mode, key, nonce, msg, associated_data = None):
        cipher_aes = AES.new(key, mode, nonce=nonce)
        if associated_data != None :
            cipher_aes.update(associated_data)
        ciphertext, tag = cipher_aes.encrypt_and_digest(msg)
        return ciphertext, tag

    def _aes_aead_mode_decrypt(self, mode, key, nonce, ciphertext, tag, associated_data = None):
        cipher_aes = AES.new(key, mode, nonce=nonce)
        if associated_data != None :
            cipher_aes.update(associated_data)
        msg = cipher_aes.decrypt_and_verify(ciphertext, tag)
        return msg

    # TODO some aes keys (i.e. kpicv) cannot be zero!
    def generate_random_aes_key(self, size = 16):
        key = get_random_bytes(size)
        return key

    def ccm_encrypt(self, key, nonce, msg, associated_data = None):
        return self._aes_aead_mode_encrypt(AES.MODE_CCM, key, nonce, msg, associated_data)

    def ccm_decrypt(self, key, nonce, ciphertext, tag, associated_data = None):
        return self._aes_aead_mode_decrypt(AES.MODE_CCM, key, nonce, ciphertext, tag, associated_data)

    def gcm_encrypt(self, key, nonce, msg, associated_data = None):
        return self._aes_aead_mode_encrypt(AES.MODE_GCM, key, nonce, msg, associated_data)

    def gcm_decrypt(self, key, nonce, ciphertext, tag, associated_data = None):
        return self._aes_aead_mode_decrypt(AES.MODE_GCM, key, nonce, ciphertext, tag, associated_data)

    def aes_import_key(self, filename, password = None, mode = PKCS8):
        with open(filename,'rb') as f:
            key_encrypted = f.read()
            if self.PKCS8 == mode:
                key = PKCS8.unwrap(key_encrypted, passphrase = password)[1]
            elif self.AES_128_CBC == mode:
                # This matches the format used by: openssl enc -d -nosalt -aes-128-cbc -in <encrypted_key.bin> -out <key.bin>
                key_derived, iv_derived = self._md5_key_iv_derivation(password)
                cipher_aes = AES.new(key_derived, AES.MODE_CBC, iv=iv_derived)
                key = cipher_aes.decrypt(key_encrypted)
                key = Padding.unpad(key, 16)  # remove padding pkcs7
            else:
                assert False, "mode {} not supported".format(mode)
        return key

    # This is for testing purposes, a private key should not be exported without a password and the
    # algorithm needs to be chosen carefully.
    def aes_export_key(self, key, filename, password = None, oid = '1.1.1', mode = PKCS8):
        if self.PKCS8 == mode:
            key_encrypted = PKCS8.wrap(key, oid, protection = self.protection, passphrase = password)
        elif self.AES_128_CBC == mode:
            # This matches the format used by: openssl enc -e -nosalt -aes-128-cbc -in <key.bin> -out <encrypted_key.bin>
            key_derived, iv_derived = self._md5_key_iv_derivation(password)
            cipher_aes = AES.new(key_derived, AES.MODE_CBC, iv=iv_derived)
            key_padded = Padding.pad(key, 16)
            key_encrypted = cipher_aes.encrypt(key_padded) # add padding pkcs7
        else:
            assert False, "mode {} not supported".format(mode)

        with open(filename,'wb') as f:
            f.write(key_encrypted)

    ##################### RSA encryption #####################

    def generate_random_rsa_key(self, size = 3072):
        key = RSA.generate(size)
        return key

    # generate a RSA key that pyCryptoDome understand. If d is not given then it would be a public key.
    def generate_rsa_key_from_components(self, modulus, e = 65537, d=None):
        if None != d:
            key = RSA.construct((modulus, e, d), consistency_check = True)
        else:
            key = RSA.construct((modulus, e), consistency_check = True)
        return key

    def rsa_import_key_from_contents(self, data, passphrase = None): # pem format
        key = RSA.import_key(data, passphrase)
        return key

    def rsa_import_key(self, filename, passphrase = None): # pem format
        with open(filename,'r') as f:
            key = self.rsa_import_key_from_contents(f.read(), passphrase)
        return key

    # This is for testing purposes, a private key should not be exported without a password and the
    # algorithm needs to be chosen carefully.
    def rsa_export_key(self, key, filename, password = None): # pem format
        with open(filename,'w') as f:
            if None != password:
                f.write(key.export_key('PEM', pkcs=8, protection = self.protection, passphrase = password ).decode())
            else:
                f.write(key.export_key('PEM').decode())

    def rsa_oaep_encrypt(self, key, msg):
        cipher_rsa = PKCS1_OAEP.new(key=key, hashAlgo=self.hash_algo, randfunc = self.rand_func)
        # Encrypt msg with the public RSA key
        ciphertext = cipher_rsa.encrypt(msg)
        return ciphertext

    def rsa_oaep_decrypt(self, key, ciphertext):
        cipher_rsa = PKCS1_OAEP.new(key=key, hashAlgo=self.hash_algo, randfunc = self.rand_func)
        # decrypt msg with the private RSA key
        msg = cipher_rsa.decrypt(ciphertext)
        return msg

    def rsassa_pss_sign(self, key, msg):
        h = self.hash_algo.new(msg)
        signer = pss.new(key, rand_func = self.rand_func)
        signature = signer.sign(h)
        return signature

    def rsassa_pss_verify(self, key, msg, signature):
        h = self.hash_algo.new(msg)
        verifier = pss.new(key, rand_func = self.rand_func)
        try:
            verifier.verify(h, signature)
            return True
        except (ValueError, TypeError):
            return False

    ##################### ARM specific algos #####################
    # key_len in bytes
    def kdf_aes_cmac(self, master, key_len, label, context, num_keys=1):
        # This matches the derivation formula used in CC312 python scripts (i.e asset_provisioning_util.py)
        derived_output = bytes()
        output_len = key_len * num_keys
        assert (output_len < 0x1FFF)

        # KDF in Counter Mode NIST SP 800-108
        # [ i || Label || 0x00 || Context || L ]
        l_size = 1 if output_len <= 31 else 2
        l = (output_len * 8).to_bytes(l_size, byteorder='big')
        for i in range((output_len + 15) // 16):
            msg = bytes([i+1]) + label + bytes([0]) + context + l
            cmac_obj = CMAC.new(master, ciphermod=AES)
            cmac_obj.update(msg)
            key_local = cmac_obj.digest()
            derived_output = derived_output + key_local

        if num_keys == 1:
            return derived_output[:key_len]
        kol = [derived_output[idx:idx + key_len] for idx in range(0, output_len, key_len)]
        return list(kol[:num_keys])

    def soc_id_gen(self, huk, hbk0, hbk1):
        # This matches the derivation formula used in CC312 library (hbk case not covered)
        temp = self.kdf_aes_cmac(huk, 128//8, bytes([0x49]), bytes(32))
        h = SHA256.new()
        h.update(hbk0 + hbk1 + temp)
        return h.digest()

    def hbk_gen(self, key, truncated = False):
        # This matches the format used by ARM to calculate the Hashes (hbk_gen_util.py)
        assert key.size_in_bits() == 3072, "Only 3072 bits key are supported"
        assert key.e == 65537, "Only 65537 values for public exponent supported"
        n = key.n
        # calcuate Barrett n' value
        np = (2 ** (3072+64+8-1)) // n
        # Note The next operation seem to be in big endian. It seems an standard way of OpenSSL
        # https://stackoverflow.com/questions/6460845/standard-way-to-hash-an-rsa-key (BN_bn2bin)
        n_bytes = n.to_bytes(key.size_in_bytes(), byteorder='big')
        np_bytes = np.to_bytes(20, byteorder='big')

        h = SHA256.new(n_bytes + np_bytes)
        hbk = h.digest()
        if truncated:
            # Note this is bytes not bits!
            hbk = hbk[:16]
        return hbk

    def get_sha256(self, data):
        # This runs sha256 on the data
        h = SHA256.new(data)
        return h.digest()

    def get_number_zero_bits(self, value):
        # One of the outputs in hbk_gen_util.py is the number of zeros in the hash number. This function achieves the same functionality
        len_bits = len(value) * 8
        value_int = int.from_bytes(value, byteorder='little')
        num_ones = bin(value_int).count('1')
        return len_bits - num_ones

    def pkcs7_padding(self, data, block_size = 16):
        return Padding.pad(data, block_size)

    def _asset_encrypter(self, aes_key, context, asset_bin, token, version, label, reserved, nonce = None):

        if 0 != (len(asset_bin) % 16):
            asset_bin = self.pkcs7_padding(asset_bin)
        if None == nonce:
            nonce = self.generate_random_aes_key(12)
        assert len(nonce) == 12, "Invalid nonce"

        derived_key = self.kdf_aes_cmac(aes_key, 16, label = label, context = context)

        associated_data = bytes(0)
        associated_data += token.to_bytes(4, byteorder='little')
        associated_data += version.to_bytes(4, byteorder='little')
        associated_data += len(asset_bin).to_bytes(4, byteorder='little')
        associated_data += reserved
        encrypted_asset, tag = self.ccm_encrypt(derived_key, nonce, asset_bin, associated_data = associated_data)
        result = associated_data
        result += nonce
        result += encrypted_asset
        result += tag
        return result

    def asset_kpicv_encrypter(self, aes_key, hbk0, kpicv, nonce = None):
        # This matches the encryption operations used in cmpu_asset_pkg_util.py script
        label = b'KEY ICV'
        context = hbk0

        derived_key = self.kdf_aes_cmac(aes_key, 16, label = label, context = context)

        asset_bin = kpicv
        asset_id = b'PICV'
        label = b'P'
        token = 0x50726F64
        version = 0x10000
        reserved = (0x52657631).to_bytes(4, byteorder='little')
        reserved += (0x52657632).to_bytes(4, byteorder='little')

        return self._asset_encrypter(derived_key, asset_id, asset_bin, token, version, label, reserved, nonce)


    def asset_encrypter(self, aes_key, asset_id, asset_bin, nonce = None):
        # This matches the encryption operations used in asset_provisioning_util.py script
        token = 0x41736574
        version = 0x10000
        label = b'P'
        reserved = (0).to_bytes(4*2, byteorder='little')
        context = asset_id.to_bytes(4, byteorder='little')
        return self._asset_encrypter(aes_key, context, asset_bin, token, version, label, reserved, nonce)

    def asset_decrypter(self, aes_key, asset_id, asset_encrypted_bin):
        # This should decrypt files from asset_provisioning_util.py script
        TOKEN = 0x41736574
        VERSION = 0x10000
        LABEL = b'P'

        token = int.from_bytes(asset_encrypted_bin[0:4], 'little')
        assert token == TOKEN, 'Unexpected Token'
        version = int.from_bytes(asset_encrypted_bin[4:8], 'little')
        assert version == VERSION, 'Unexpected Version'
        length = int.from_bytes(asset_encrypted_bin[8:12], 'little')
        nonce = asset_encrypted_bin[20:20+12]

        context = asset_id.to_bytes(4, byteorder='little')
        label = LABEL
        derived_key = self.kdf_aes_cmac(aes_key, 16, label = label, context = context)

        associated_data = asset_encrypted_bin[0:20]
        encrypted_asset = asset_encrypted_bin[32:32+length]
        tag = asset_encrypted_bin[32 + length:]

        msg = self.ccm_decrypt(derived_key, nonce, encrypted_asset, tag, associated_data = associated_data)
        return associated_data, nonce, msg, tag

if __name__ == "__main__":
    print("Just testing...")



