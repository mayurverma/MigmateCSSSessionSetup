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

def hkdf_test(crypto_helper):
    """Derive keys using the same input as the OpenSSL version
    """
    master_secret = b'\xaa\xbb\xcc\xdd\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\x1a\x2a'
    if verbose: print("master_secret", master_secret.hex())

    host_salt = b'\x01\x02\x03\x04\x05\x06\x07\x08\x11\x12\x13\x14\x15\x16\x17\x18'
    sensor_salt = b'\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8'
    salt = host_salt + sensor_salt
    if verbose: print("salt", salt.hex())

    info = 'AR0820R2'.encode()
    rom_version = 0x1234
    patch_version = 0xa001
    info += rom_version.to_bytes(2, byteorder='little')
    info += patch_version.to_bytes(2, byteorder='little')

    soc_id = b'\xa1\xc2\xf3\xa4\xc5\xe6\x17\x38\xb1\xd2\xe3\xb4\xd5\xf6\x27\x48'
    soc_id += b'\xc1\xe2\xc3\xc4\xe5\x06\x37\x58\xd1\xf2\xb3\xd4\xf5\x16\x47\x68'
    info += soc_id

    if verbose: print("info", info.hex())

    key_len = 32
    keys = crypto_helper.hkdf(master_secret, key_len, salt, info)

    video_key = keys[:16]
    control_key = keys[16:]

    print("video_key", video_key.hex())
    print("control_key", control_key.hex())

def main():
    """main function"""
    print("======== Python HKDF test running ========")
    crypto_helper = CryptoHelper()

    hkdf_test(crypto_helper)

if __name__ == "__main__":
    main()

