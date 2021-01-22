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
from secure_boot_certificate import SecureBootKeyCert, SecureBootEnablerCert, SecureBootDeveloperCert
from secure_boot_certificate import CC312LifeCycleState, HbkId

class SecureBootDebugEntitlement():

    #This class encapsulates the 3 scripts used by ARM :
    # cert_key_util.py
    # cert_dbg_enabler_util.py
    # cert_dbg_developer_util.py

    def __init__(self, enabler_cert_key, developer_cert_key, key_cert_key = None, sw_version = 0,
                   cert_version = 0, lcs = CC312LifeCycleState.Secure, hbk_id = HbkId.HBK0,
                   debug_enabler_mask = 0, debug_enabler_lock = 0,
                   debug_developer_mask = 0, soc_id = 0):

        self.three_certificates = True if key_cert_key else False

        if self.three_certificates:
            self.key_cert = SecureBootKeyCert(key_cert_key, enabler_cert_key.publickey(), cert_version, hbk_id, sw_version)
            self.enabler_cert = SecureBootEnablerCert(enabler_cert_key, developer_cert_key.publickey(), cert_version, hbk_id = HbkId.HBKNone,
                                debug_mask = debug_enabler_mask, debug_lock = debug_enabler_lock, lcs = lcs, rma = False)
        else:
            self.enabler_cert = SecureBootEnablerCert(enabler_cert_key, developer_cert_key.publickey(), cert_version, hbk_id = hbk_id,
                                debug_mask = debug_enabler_mask, debug_lock = debug_enabler_lock, lcs = lcs, rma = False)
        self.developer_cert = SecureBootDeveloperCert(developer_cert_key, soc_id, cert_version, debug_mask = debug_developer_mask)

    def __bytes__(self):
        result = bytes(0)
        if self.three_certificates:
            result += bytes(self.key_cert)
        result += bytes(self.enabler_cert)
        result += bytes(self.developer_cert)
        return result

    def __str__(self):
        result = ""
        if self.three_certificates:
            result += str(self.key_cert)
        result += str(self.enabler_cert)
        result += str(self.developer_cert)
        return result

class SecureBootRMAEntitlement(SecureBootDebugEntitlement):

    def __init__(self, enabler_cert_key, developer_cert_key, key_cert_key = None, sw_version = 0,
                   cert_version = 0, lcs = CC312LifeCycleState.Secure, hbk_id = HbkId.HBK0, soc_id = 0):

        self.three_certificates = True if key_cert_key else False

        if self.three_certificates:
            self.key_cert = SecureBootKeyCert(key_cert_key, enabler_cert_key.publickey(), cert_version, hbk_id, sw_version)
            self.enabler_cert = SecureBootEnablerCert(enabler_cert_key, developer_cert_key.publickey(), cert_version, hbk_id = HbkId.HBKNone,
                                lcs = lcs, rma = True)
        else:
            self.enabler_cert = SecureBootEnablerCert(enabler_cert_key, developer_cert_key.publickey(), cert_version, hbk_id = hbk_id,
                                lcs = lcs, rma = True)
        self.developer_cert = SecureBootDeveloperCert(developer_cert_key, soc_id, cert_version)


if __name__ == "__main__":
    print("Just testing...")





