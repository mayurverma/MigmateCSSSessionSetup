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
# Class that represents a set of keys used by cryptographic operations.
#
########################################################################################

class KeySet():
    """Class that represents a set of keys used by cryptographic operations."""
    def __init__(self, krtl, kcp, kpicv, rot_cm_pair, rot_dm_pair, sensor_pair, vendor_pair):
        self._krtl = krtl
        self._kcp = kcp
        self._kpicv = kpicv
        self._rot_cm_pair = rot_cm_pair
        self._rot_dm_pair = rot_dm_pair
        self._sensor_pair = sensor_pair
        self._vendor_pair = vendor_pair

    @property
    def krtl(self):
        """Returns the RTL provisioning key."""
        return self._krtl

    @property
    def kcp(self):
        """Returns the customer provisioning key."""
        return self._kcp

    @property
    def kpicv(self):
        """Returns the IC vendor provisioning key."""
        return self._kpicv

    @property
    def rsa_key_arm(self):
        """Returns the Chip Manufacturer Root-Of-Trust assymetric key pair (public and private)."""
        return self._rot_cm_pair

    @property
    def rsa_key_hbk1(self):
        """Returns the Device Manufacturer Root-Of-Trust assymetric key pair (public and private)."""
        return self._rot_dm_pair

    @property
    def rsa_key_sensor(self):
        """Returns the sensor session/signing assymetric key pair (public and private)."""
        return self._sensor_pair

    @property
    def rsa_key_vendor(self):
        """Returns the vendor signing assymetric key pair (public and private)."""
        return self._vendor_pair


