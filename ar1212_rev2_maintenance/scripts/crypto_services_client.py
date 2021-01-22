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
import subprocess
import os
import getpass

from root_of_trust import RootOfTrust
from secure_boot_certificate import CC312LifeCycleState
from key_name import KeyName
from crypto_helper import CryptoHelper
##Mayur
from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from branch_properties import BranchProperties, BranchType
from command_helper import CommandHelper
from lifecycle_state import LifecycleState
from host_command_code import HostCommandCode
from host_command_get_status import HostCommandGetStatus
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from root_of_trust import RootOfTrust
from patch_type import PatchType
from test_fixture import FixtureType



class GetCertificates():
    """Test the GetCertificates host command operation"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.crypto = CryptoHelper()
        self.key_set_name = 'test' if (self.fixture.get_type() in (FixtureType.MODEL, FixtureType.SIM)) else 'eval'
        self.logger.debug('setUp complete')

    def compare_value(self, a, expected, msg = ""):
        """ Compare two values """

        self.assertEqual(expected, a, 'Comparison {} failed : {} vs {} (expected)'.format(msg, expected, a))

    def set_public_certificates(self, auth_id = CertificateAuthority.VENDOR_B, purpose = CertificatePurpose.SENSOR):
        """Create RSA public certificate and send by ProvisionAsset command"""

        rsa_key = self.crypto.generate_random_rsa_key(2048)
        pub_cert = RSAPublicCertificate(rsa_key, auth_id = auth_id, purpose = purpose, number = 100)
        asset = NvmRecordRSAPublicCert(pub_cert)
        HostCommandProvisionAsset(self.camera, self.key_set_name).execute(asset, asset_id = 6789, clock_speed_hz = 150000000, root_of_trust = RootOfTrust.CM)

        return pub_cert

    def get_certificates_in_secure_mode(self):
        """Check GetCertificates command in SECURE
           - use valid certificates for sensor and vendor
        """

        self._common_setup(LifecycleState.SECURE, PatchType.PATCH)

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()

        assert(LifecycleState.SECURE == response.current_lifecycle_state)

        # Create and set RSA public certificate
        sensor_cert = self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.SENSOR)
        print(sensor_cert)

        vendor_cert = self.set_public_certificates(auth_id = CertificateAuthority.VENDOR_A, purpose = CertificatePurpose.VENDOR)
        print(vendor_cert)

        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = CertificateAuthority.VENDOR_A.value;  # This value needs to match auth_id to pass in set_public_certificates().
        response = HostCommandGetCertificatesResponse()
        result = self.command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, verbosity=logging.INFO)
        expected = Error.SUCCESS
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('GetCertificates completed with {}'.format(result.name))

        bytes_sensor_cert = bytes(sensor_cert)
        len_sensor_cert = len(bytes_sensor_cert)
        bytes_vendor_cert = bytes(vendor_cert)
        len_vendor_cert = len(bytes_vendor_cert)

        self.compare_value(response.sensor_certificate_size_words * 2, len_sensor_cert, "Certificate sensor size")
        self.compare_value(response.sensor_certificate, bytes_sensor_cert, "Certificate sensor contents")
        self.compare_value(response.vendor_certificate_size_words * 2, len_vendor_cert, "Certificate vendor size")
        self.compare_value(response.vendor_certificate, bytes_vendor_cert, "Certificate vendor contents")

        # Issue GetStatus command
        response = HostCommandGetStatus(self.camera).execute()
        print("response = ", response)
        
        
        
        #return response.sensor_certificate, response.vendor_certificate
        vendor_certificate_bytes = bytes_vendor_cert
        ca_rsa_key = None
        from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
        vendor_certificate = RSAPublicCertificate.get_and_validate_certificate_from_bytes(ca_rsa_key, vendor_certificate_bytes)
        self.__logger.log(self.__verbosity, str(vendor_certificate))
        #sensor_ca_rsa_key = vendor_certificate.rsa_public_key
        vendor_rsa_pub_key = vendor_certificate.rsa_public_key
        print("pub_key =", vendor_rsa_pub_key)
        
        
        

class CryptoServicesClient():

    SERVER = 'gbbr01-css-crypt'

    '''
    def __init__(self, product_name, root_of_trust = RootOfTrust.CM, key_set_name = 'eval'):
        self._product_name = product_name
        self._root_of_trust = root_of_trust
        self._key_set_name = key_set_name
        print("Getuser :".format(getpass.getuser()))
        if os.name == 'nt':
            #self._cmd = r'plink -batch -ssh -i c:/Users/{}/.putty/id_rsa.ppk'.format(getpass.getuser())
            self._cmd = r'plink -batch -ssh -i c:/Users/{}/.putty/id_rsa.ppk'.format('fg64dw')
        else:
            self._cmd = 'ssh'
    '''
    def __init__(self, product_name, root_of_trust = RootOfTrust.CM, key_set_name = 'eval'):
        self._product_name = product_name
        self._root_of_trust = root_of_trust
        self._key_set_name = key_set_name
        #print("Getuser :".format(getpass.getuser()))
        
    '''
    def encrypt_asset(self, asset_id, asset_bin):

        script = 'sudo -u gbasdcss-admin /local/css_crypt/bin/encrypt_asset.sh'

        cmd = '{} {}@{} {}'.format(self._cmd, getpass.getuser(), self.SERVER, script)

        cmd += ' {}'.format(self._product_name)
        cmd += ' {}'.format(self._key_set_name)

        cmd += ' cm' if self._root_of_trust == RootOfTrust.CM else ' dm'

        cmd += ' {}'.format(asset_id)

        #Format: encrypt_asset.sh <product_name> <key_set_name> <root_of_trust> <asset_id>
        print("Calling asset encrypter for asset : 0x{:08X}".format(asset_id))
        run_output = subprocess.run(cmd, shell=True, input= asset_bin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if run_output.returncode == 0:
            bytes_output = run_output.stdout
        else:
            raise Exception('Command "{}" failed with error:\n{}'.format(cmd, run_output.stderr.decode()))

        return bytes_output

    def create_debug_entitlement(self,
                                key_cert = False,
                                sw_version = 0,
                                cert_version = 0,
                                soc_id = bytes(32),
                                debug_enabler_mask = 0,
                                debug_enabler_lock = 0,
                                debug_developer_mask = 0,
                                lcs = CC312LifeCycleState.Secure):

        script = 'sudo -u gbasdcss-admin /local/css_crypt/bin/create_debug_entitlement.sh'

        #cmd = '{} {}@{} {}'.format(self._cmd, getpass.getuser(), self.SERVER, script)
        cmd = '{} {}@{} {}'.format(self._cmd, "fg64dw", self.SERVER, script)

        if key_cert:
            cmd += ' --key_cert'

        cmd += ' {}'.format(self._product_name)
        cmd += ' {}'.format(self._key_set_name)

        cmd += ' cm' if self._root_of_trust == RootOfTrust.CM else ' dm'

        if lcs == CC312LifeCycleState.CM:
            lcs_str = 'cm'
        elif lcs == CC312LifeCycleState.DM:
            lcs_str = 'dm'
        elif lcs == CC312LifeCycleState.Secure:
            lcs_str = 'secure'
        else:
            raise Exception('RMA not supported')

        cmd += ' {}'.format(lcs_str)
        cmd += ' {}'.format(soc_id.hex())
        cmd += ' {}'.format(sw_version)
        cmd += ' {}'.format(cert_version)


        cmd += ' {}'.format(debug_enabler_mask)
        cmd += ' {}'.format(debug_enabler_lock)
        cmd += ' {}'.format(debug_developer_mask)

        #create_debug_entitlement.sh [--key_cert] <product_name> <key_set_name> <root_of_trust> <lcs> <soc_id> <sw_version> <cert_version> <enabler_mask> <enabler_lock> <developer_mask>
        print("Calling create debug entitlment for soc_id : {}".format(soc_id.hex()))
        print("Calling create debug entitlment command : {}".format(cmd))
        run_output = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if run_output.returncode == 0:
            bytes_output = run_output.stdout
        else:
            raise Exception('Command "{}" failed with error:\n{}'.format(cmd, run_output.stderr.decode()))

        return bytes_output

    def sign_rsassa_pss(self, key_name, blob_bin):

        script = 'sudo -u gbasdcss-admin /local/css_crypt/bin/sign_rsassa_pss.sh'

        cmd = '{} {}@{} {}'.format(self._cmd, "fg64dw", self.SERVER, script)

        cmd += ' {}'.format(self._product_name)
        cmd += ' {}'.format(self._key_set_name)

        cmd += ' {}'.format(key_name.name.lower())

        #sign_rsassa_pss.sh <product_name> <key_set_name> <key_name>
        print("Calling sign_rsassa_pss for key : {}".format(key_name.name.lower()))
        run_output = subprocess.run(cmd, shell=True, input= blob_bin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if run_output.returncode == 0:
            bytes_output = run_output.stdout
        else:
            raise Exception('Command "{}" failed with error:\n{}'.format(cmd, run_output.stderr.decode()))

        return bytes_output
    '''    
    '''
    def get_public_key(self, key_name):

        script = 'sudo -u gbasdcss-admin /local/css_crypt/bin/get_public_key.sh'

        cmd = '{} {}@{} {}'.format(self._cmd, "fg64dw", self.SERVER, script)

        cmd += ' {}'.format(self._product_name)
        cmd += ' {}'.format(self._key_set_name)

        cmd += ' {}'.format(key_name.name.lower())

        #get_public_key.sh <product_name> <key_set_name> <key_name>
        print("Calling get_public_key for key : {}".format(key_name.name.lower()))
        run_output = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if run_output.returncode == 0:
            bytes_output = run_output.stdout
        else:
            raise Exception('Command "{}" failed with error:\n{}'.format(cmd, run_output.stderr.decode()))

        modulus = int.from_bytes(bytes_output, 'little')
        return CryptoHelper().generate_rsa_key_from_components(modulus)
    '''
    def get_public_key(self, key_name):

        #get_public_key.sh <product_name> <key_set_name> <key_name>
        print("Calling get_public_key for key : {}".format(key_name.name.lower()))
        return GetCertificates().get_certificates_in_secure_mode()
    
    '''
    def encrypt_private_key(self, key_name, asset_id):
        """encrypt the key_name using it's default wrapping key"""

        script = 'sudo -u gbasdcss-admin /local/css_crypt/bin/encrypt_private_key.sh'

        cmd = '{} {}@{} {}'.format(self._cmd, "fg64dw", self.SERVER, script)

        cmd += ' {}'.format(self._product_name)
        cmd += ' {}'.format(self._key_set_name)

        cmd += ' {}'.format(key_name.name.lower())

        cmd += ' {}'.format(hex(asset_id))

        #encrypt_private_key.sh <product_name> <key_set_name> <key_name> <asset_id>
        print("Calling encrypt_private_key for key : {}".format(key_name.name.lower()))
        run_output = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if run_output.returncode == 0:
            bytes_output = run_output.stdout
        else:
            raise Exception('Command "{}" failed with error:\n{}'.format(cmd, run_output.stderr.decode()))

        return bytes_output
    '''
if __name__ == "__main__":
    client = CryptoServicesClient('CSS')
    #get_certificates_in_secure_mode()



