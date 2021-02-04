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
import struct

verbose = False

def aes_ccm_test(crypto_helper):
    """Encrypts dummy video authentication configuration params and
       writes the resultant ciphertext and MAC to a file
    """
    control_channel_key = b'\xd7\xeb\x75\x92\x4d\xf2\x23\xcd\xe1\xbb\xaa\xcc\xdf\x5c\x9e\x21'
    control_channel_nonce = b'\xa5\xd2\xb5\xd0\xc8\xa4\x9e\x3d\x09\x19\x25\xa5'

    videoAuthMode = 2
    pixelPackMode = 1
    pixelPackValue = 0xbb
    firstRow = 2
    lastRow = 0x5678
    rowSkip = 8
    firstColumn = 6
    lastColumn = 0xabcd
    columnSkip = 16
    frameCounter = 0x11223344abcdef01

    videoAuthConfigParams = struct.pack("<HHHHHHHHHHLQ",
                                        videoAuthMode,
                                        pixelPackMode,
                                        pixelPackValue,
                                        firstRow,
                                        lastRow,
                                        rowSkip,
                                        firstColumn,
                                        lastColumn,
                                        columnSkip,
                                        0,                  # Padding
                                        0,
                                        frameCounter)

    associated_data = int(0x8030).to_bytes(2, 'little')

    print("videoAuthMode:  0x{:04x}".format(videoAuthMode));
    print("pixelPackMode:  0x{:04x}".format(pixelPackMode));
    print("pixelPackValue: 0x{:04x}".format(pixelPackValue));
    print("firstRow:       0x{:04x}".format(firstRow));
    print("lastRow:        0x{:04x}".format(lastRow));
    print("rowSkip:        0x{:04x}".format(rowSkip));
    print("firstColumn:    0x{:04x}".format(firstColumn));
    print("lastColumn:     0x{:04x}".format(lastColumn));
    print("columnSkip:     0x{:04x}".format(columnSkip));
    print("frameCounter:   0x{:16x}".format(frameCounter));

    print("\nConfig:", bytes(videoAuthConfigParams).hex())

    ciphertext, mac = crypto_helper.ccm_encrypt(control_channel_key,
                                              control_channel_nonce,
                                              bytes(videoAuthConfigParams),
                                              associated_data)
    print("\nCiphertext:", ciphertext.hex())
    print("\nMAC:", mac.hex())

    # write ciphertext and mac to a file
    with open("doc/UserGuideExamples/output/aes_ccm_ciphertext.bin", 'w+b') as cipher_file:
        cipher_file.write(ciphertext)
        cipher_file.write(mac)

def main():
    """main function"""
    print("======== Python AES-CCM test running ========")
    crypto_helper = CryptoHelper()

    aes_ccm_test(crypto_helper)

if __name__ == "__main__":
    main()

