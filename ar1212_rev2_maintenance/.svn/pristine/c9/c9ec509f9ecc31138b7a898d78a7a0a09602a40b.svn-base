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
# OpenSSL examples test methods
#
########################################################################################

from crypto_helper import CryptoHelper
from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority

verbose = False

def rsa_verify_test(crypto_helper):
    """Generates an RSA key and the custom public certificate signed with this key,
       saves the certificate to a binary file
    """
    rsa_key = crypto_helper.generate_random_rsa_key(2048)
    rsa_cert = RSAPublicCertificate(rsa_key,
                                    auth_id = CertificateAuthority.VENDOR_A,
                                    purpose = CertificatePurpose.VENDOR,
                                    version = 0,
                                    number = 0)
    print("rsa_cert", rsa_cert)
    print("signature", rsa_cert.rsa_signature.hex())
    print("cert", rsa_cert.__bytes__().hex())

    # store to a file
    with open("doc/UserGuideExamples/output/rsa_cert.bin", 'w+b') as cert_file:
        cert_file.write(rsa_cert.__bytes__())

def main():
    """main function"""
    print("======== Python RSA verify test running ========")
    crypto_helper = CryptoHelper()

    rsa_verify_test(crypto_helper)

if __name__ == "__main__":
    main()

