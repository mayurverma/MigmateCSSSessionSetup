####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Asset provisioning helper for firmware tests
####################################################################################
import logging

from common_error import Error
from root_of_trust import RootOfTrust
from host_command_provision_asset import HostCommandProvisionAsset
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey, NvmRecordPSKMasterSecret, NvmRecordTRNGCharacterization
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority

class ProvisionAssetHelper():
    """Get status helper class."""

    def __init__(self, camera, key_set_name):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__provision_asset_cmd = HostCommandProvisionAsset(camera, key_set_name)


    def provision_psk_master_secret(self, aes_key = bytes(range(16)), asset_id = 1234, clock_speed_hz = 50000000, expected = Error.SUCCESS,
            root_of_trust = RootOfTrust.CM, verbosity=logging.INFO, timeout_usec=50000):
        """ Create and provision NvmRecordPSKMasterSecret """

        # Create a PSK Master Secret Asset
        asset = NvmRecordPSKMasterSecret(aes_key)

        response = self.__provision_asset_cmd.execute(asset, asset_id = asset_id, clock_speed_hz = clock_speed_hz, root_of_trust = root_of_trust,
                                           expected = expected, verbosity=verbosity, timeout_usec=timeout_usec)


    def provision_trng_characterization(self, r1 = 0, r2 = 0, r3 = 0, r4 = 0, asset_id = 1234, clock_speed_hz = 50000000, expected = Error.SUCCESS,
            root_of_trust = RootOfTrust.CM, verbosity=logging.INFO, timeout_usec=50000):
        """ Create and provision NvmRecordTRNGCharacterization """

        # Create TRNG Asset
        asset = NvmRecordTRNGCharacterization(r1, r2, r3, r4)

        response = self.__provision_asset_cmd.execute(asset, asset_id = asset_id, clock_speed_hz = clock_speed_hz, root_of_trust = root_of_trust,
                                           expected = expected, verbosity=verbosity, timeout_usec=timeout_usec)


    def provision_rsa_certificates(self, rsa_key, sign_key_pair = None, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR,
                                          version = 0, number = 0, root_of_trust = RootOfTrust.CM):
        """Create RSA public/private key (when purpose == SENSOR) and send by ProvisionAsset command"""

        pub_cert = RSAPublicCertificate(rsa_key, sign_key_pair = sign_key_pair, auth_id = auth_id, purpose = purpose, version = version, number = number)
        asset = NvmRecordRSAPublicCert(pub_cert)
        self.__provision_asset_cmd.execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = root_of_trust)

        if CertificatePurpose.SENSOR == purpose:
            asset = NvmRecordRSAPrivateKey(rsa_key, auth_id = auth_id)
            self.__provision_asset_cmd.execute(asset, asset_id = 1234, clock_speed_hz = 150000000, root_of_trust = root_of_trust)


