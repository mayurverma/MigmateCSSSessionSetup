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
# Library for crypto algorithms used in the comunications beetween the Host and the sensor (AR0820)
#
########################################################################################
from crypto_helper import CryptoHelper
from secure_boot_entitlement import SecureBootDebugEntitlement, CC312LifeCycleState, HbkId
from root_of_trust import RootOfTrust
from nvm_record import NvmRecordRSAPrivateKey
from key_name import KeyName

class CryptoServices():

    def __init__(self, key_set, root_of_trust = RootOfTrust.CM):
        self.key_set = key_set
        self.crypto_helper = CryptoHelper()
        self.root_of_trust = root_of_trust
        if self.root_of_trust not in [RootOfTrust.CM, RootOfTrust.DM, RootOfTrust.CD]:
            raise Exception('Operation failed with wrong ROT {}'.format(root_of_trust))

    def encrypt_asset(self, asset_id, asset_bin):
        if self.root_of_trust == RootOfTrust.CM:
            key = self.key_set.kpicv
        elif self.root_of_trust == RootOfTrust.DM:
            key = self.key_set.kcp
        elif self.root_of_trust == RootOfTrust.CD:
            key = self.key_set.krtl

        return self.crypto_helper.asset_encrypter(key, asset_id, asset_bin)

    def create_debug_entitlement(self,
                                 key_cert = False,
                                 sw_version = 0,
                                 cert_version = 0,
                                 soc_id = bytes(32),
                                 debug_enabler_mask = 0,
                                 debug_enabler_lock = 0,
                                 debug_developer_mask = 0,
                                 lcs = CC312LifeCycleState.Secure):

        if self.root_of_trust in [RootOfTrust.CM, RootOfTrust.CD]:
            # INFO: <SC> note that there is not Chip Designer root of trust
            hbk_id = HbkId.HBK0
            rsa_key = self.key_set.rsa_key_arm
        else:
            hbk_id = HbkId.HBK1
            rsa_key = self.key_set.rsa_key_hbk1

        key_cert_key = rsa_key if key_cert else None

        cert = SecureBootDebugEntitlement(
                     enabler_cert_key = rsa_key,
                     developer_cert_key = rsa_key,
                     key_cert_key = key_cert_key,
                     cert_version = cert_version,
                     hbk_id = hbk_id,
                     sw_version = sw_version,
                     lcs = lcs,
                     debug_enabler_mask   = debug_enabler_mask,
                     debug_enabler_lock   = debug_enabler_lock,
                     debug_developer_mask = debug_developer_mask,
                     soc_id = soc_id)

        return bytes(cert)

    def sign_rsassa_pss(self, key_name, blob_bin):

        if key_name == KeyName.SENSOR_PAIR:
            rsa_key = self.key_set.rsa_key_sensor
        elif key_name == KeyName.VENDOR_PAIR:
            rsa_key = self.key_set.rsa_key_vendor
        else:
            raise Exception('Operation failed with wrong id {}'.format(key_name))
        return self.crypto_helper.rsassa_pss_sign(rsa_key, blob_bin)

    def get_public_key(self, key_name):

        if key_name == KeyName.SENSOR_PAIR:
            rsa_key = self.key_set.rsa_key_sensor
        elif key_name == KeyName.VENDOR_PAIR:
            rsa_key = self.key_set.rsa_key_vendor
        elif key_name == KeyName.ROT_CM_PAIR:
            rsa_key = self.key_set.rsa_key_arm
        elif key_name == KeyName.ROT_DM_PAIR:
            rsa_key = self.key_set.rsa_key_hbk1
        else:
            raise Exception('Operation failed with wrong id {}'.format(key_name))
        return rsa_key.publickey()

    def encrypt_private_key(self, key_name, asset_id):
        """encrypt the key_name using it's default wrapping key"""
        if key_name == KeyName.SENSOR_PAIR:
            cert = NvmRecordRSAPrivateKey(self.key_set.rsa_key_sensor)
            asset_bin = bytes(cert)
        elif key_name == KeyName.KPICV:
            asset_bin = self.key_set.kpicv
        elif key_name == KeyName.KCP:
            asset_bin = self.key_set.kcp
        else:
            raise Exception('Operation failed with wrong id {}'.format(key_name))

        if self.root_of_trust == RootOfTrust.CM:
            key = self.key_set.kpicv
            return self.crypto_helper.asset_encrypter(key, asset_id, asset_bin)
        elif self.root_of_trust == RootOfTrust.DM:
            key = self.key_set.kcp
            return self.crypto_helper.asset_encrypter(key, asset_id, asset_bin)
        elif self.root_of_trust == RootOfTrust.CD:
            # special encryption for kpicv following arm cmpu_asset_pkg_util
            # The asset_id should be the value of hbk0
            key = self.key_set.krtl
            hbk0 = asset_id.to_bytes(16, byteorder='little')
            return self.crypto_helper.asset_kpicv_encrypter(key, hbk0, asset_bin)



if __name__ == "__main__":
    pass


