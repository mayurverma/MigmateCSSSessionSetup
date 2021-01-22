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

def rsa_decrypt_test(crypto_helper):
    """Decrypts session parameters stored in a file, using a private key
       stored in a PEM file
    """
    # retrieve the encrypted parameters
    with open("doc/UserGuideExamples/output/encParams.bin", 'rb') as enc_params:
        encrypted_params = enc_params.read()

    print("Encrypted params", encrypted_params.hex())

    # read the public key from a file
    rsa_key = crypto_helper.rsa_import_key("doc/UserGuideExamples/output/rsa2048_private.pem")
    print("RSA key: {:x}".format(rsa_key.n))

    # Decrypt the session parameters
    session_params = crypto_helper.rsa_oaep_decrypt(rsa_key, encrypted_params)
    print("Session params", session_params.hex())

def main():
    """main function"""
    print("======== Python RSA decrypt test running ========")
    crypto_helper = CryptoHelper()

    rsa_decrypt_test(crypto_helper)

if __name__ == "__main__":
    main()

