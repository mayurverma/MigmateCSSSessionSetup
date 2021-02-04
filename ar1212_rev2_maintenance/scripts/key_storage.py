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
# Key storage for Depu
#
########################################################################################
import os
import sys
import getpass

from enum import Enum
from crypto_helper import CryptoHelper
from key_set import KeySet

rootdir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

class KeyStorageId(Enum):
    """KeyStorage identifier enumeration"""
    test = {'key_dir' : os.path.join(rootdir, 'test', 'keysdb', 'test'),
            'password' : None}

    eval = {'key_dir' : os.path.join(rootdir, 'src', 'DepuApp', 'scripts'),
            'password' : os.path.join(rootdir, 'src', 'DepuApp', 'scripts', 'key_storage.txt')}
    # ... add more here

# TODO:<SC> move it to another file? For the moment stick the finder here
def get_key_storage(name):
    name_enum = KeyStorageId[name.lower()]
    key_storage = KeyStorage()
    if name_enum == KeyStorageId.eval:
        key_storage.load_files(name_enum.value['key_dir'], password = name_enum.value['password'])
    elif name_enum == KeyStorageId.test:
        key_storage.load_files(name_enum.value['key_dir'], password = name_enum.value['password'])
    else:
        assert False, "get_key_storage: {} not implemented".format(name)
    return key_storage

class KeyStorage(KeySet):
    def __init__(self):
        self.__crypto = CryptoHelper()
        super().__init__(None, None, None, None, None, None, None)
        self.__salt = 'KeyStorage'.encode()

    def load_files(self, keys_dir, password = None):

        password_correct = False

        assert os.path.isdir(keys_dir), "error loading directory {}".format(keys_dir)

        password = self.__get_raw_password(password)

        try:
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_arm.txt')):
                self._rot_cm_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_arm.txt'), passphrase=password)
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_hbk1.txt')):
                self._rot_dm_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_hbk1.txt'), passphrase=password)
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_sensor.txt')):
                self._sensor_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_sensor.txt'), passphrase=password)
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_vendor.txt')):
                self._vendor_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_vendor.txt'), passphrase=password)
            if os.path.isfile(os.path.join(keys_dir, 'krtl.txt')):
                self._krtl =       self.__crypto.aes_import_key(os.path.join(keys_dir, 'krtl.txt'), password=password)
            if os.path.isfile(os.path.join(keys_dir, 'kpicv.txt')):
                self._kpicv =       self.__crypto.aes_import_key(os.path.join(keys_dir, 'kpicv.txt'), password=password)
            if os.path.isfile(os.path.join(keys_dir, 'kcp.txt')):
                self._kcp =         self.__crypto.aes_import_key(os.path.join(keys_dir, 'kcp.txt'), password=password)
            password_correct = True
        except:
            print(">>>> Incorrect password given. Only public information available")

            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_arm_pub.txt')):
                self._rot_cm_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_arm_pub.txt'))
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_hbk1_pub.txt')):
                self._rot_dm_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_hbk1_pub.txt'))
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_sensor_pub.txt')):
                self._sensor_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_sensor_pub.txt'))
            if os.path.isfile(os.path.join(keys_dir, 'rsa_key_vendor_pub.txt')):
                self._vendor_pair = self.__crypto.rsa_import_key(os.path.join(keys_dir, 'rsa_key_vendor_pub.txt'))
        return password_correct

    def save_files(self, keys_dir, password = None):

        password = self.__get_raw_password(password)

        if self._rot_cm_pair and self._rot_cm_pair.has_private():
            self.__crypto.rsa_export_key(self._rot_cm_pair, os.path.join(keys_dir, 'rsa_key_arm.txt'), password = password)
            self.__crypto.rsa_export_key(self._rot_cm_pair.publickey(), os.path.join(keys_dir, 'rsa_key_arm_pub.txt'))

        if self._rot_dm_pair and self._rot_dm_pair.has_private():
            self.__crypto.rsa_export_key(self._rot_dm_pair, os.path.join(keys_dir, 'rsa_key_hbk1.txt'), password = password)
            self.__crypto.rsa_export_key(self._rot_dm_pair.publickey(), os.path.join(keys_dir, 'rsa_key_hbk1_pub.txt'))

        if self._sensor_pair and self._sensor_pair.has_private():
            self.__crypto.rsa_export_key(self._sensor_pair, os.path.join(keys_dir, 'rsa_key_sensor.txt'), password = password)
            self.__crypto.rsa_export_key(self._sensor_pair.publickey(), os.path.join(keys_dir, 'rsa_key_sensor_pub.txt'))

        if self._vendor_pair and self._vendor_pair.has_private():
            self.__crypto.rsa_export_key(self._vendor_pair, os.path.join(keys_dir, 'rsa_key_vendor.txt'), password = password)
            self.__crypto.rsa_export_key(self._vendor_pair.publickey(), os.path.join(keys_dir, 'rsa_key_vendor_pub.txt'))

        if self._krtl:
            self.__crypto.aes_export_key(self._krtl,       os.path.join(keys_dir, 'krtl.txt'), password = password)
        if self._kpicv:
            self.__crypto.aes_export_key(self._kpicv,       os.path.join(keys_dir, 'kpicv.txt'), password = password)
        if self._kcp:
            self.__crypto.aes_export_key(self._kcp,         os.path.join(keys_dir, 'kcp.txt'), password = password)

    def generate_random_keys(self):

        self._rot_cm_pair = self.__crypto.generate_random_rsa_key(3072)
        self._sensor_pair = self.__crypto.generate_random_rsa_key(2048)
        self._vendor_pair = self.__crypto.generate_random_rsa_key(2048)
        self._krtl        = self.__crypto.generate_random_aes_key(16)
        self._kpicv       = self.__crypto.generate_random_aes_key(16)
        self._kcp         = self.__crypto.generate_random_aes_key(16)

    def get_default_test_keys(self):

        self._rot_cm_pair = self.__crypto.generate_rsa_key_from_components(self.__crypto.HBK0_KEY_MODULUS_DEFAULT, d=self.__crypto.HBK0_KEY_PRIV_EXP_DEFAULT)
        self._rot_dm_pair = self.__crypto.generate_rsa_key_from_components(self.__crypto.HBK1_KEY_MODULUS_DEFAULT, d=self.__crypto.HBK1_KEY_PRIV_EXP_DEFAULT)
        self._kpicv       = self.__crypto.KPICV_DEFAULT
        self._kcp         = self.__crypto.KCP_DEFAULT

    def save_password_in_file(self, password_file, password):
        if isinstance(password, str):
            password = password.encode()
        password2 = self.__salt + getpass.getuser().encode()
        self.__crypto.aes_export_key(password, password_file, password = password2)

    def __load_password_from_file(self,        password_file):
        password = self.__salt + getpass.getuser().encode()
        return self.__crypto.aes_import_key(password_file, password = password)

    def __get_raw_password(self, password):
        if password is not None:
            if os.path.isfile(password):
                password = self.__load_password_from_file(password)
        return password


