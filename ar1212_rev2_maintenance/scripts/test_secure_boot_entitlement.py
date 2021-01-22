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
# Unittest for secure Boot entitlements
#
########################################################################################
import unittest
from secure_boot_entitlement import SecureBootDebugEntitlement
from crypto_helper import CryptoHelper
from secure_boot_certificate import CC312LifeCycleState, HbkId

class CertificatetTestExample1():
    # certificate precomputed in the command line with the following parameters
    # cert-keypair =  key_free.pem
    # cert-keypair-pwd =
    # soc-id = soc_id1.bin
    # debug-mask[0-31]   = 0x00112233
    # debug-mask[32-63]  = 0x44556677
    # debug-mask[64-95]  = 0x8899AABB
    # debug-mask[96-127] = 0xCCDDEEFF
    # enabler-cert-pkg = cert_enabler_pkg.bin
    # cert-pkg = cert_developer_pkg.bin
    #[ENABLER-DBG-CFG]
    # cert-keypair =  key_free.pem
    # cert-keypair-pwd =
    # lcs =  1
    # #rma-mode =
    # debug-mask[0-31]   = 0x00112233
    # debug-mask[32-63]  = 0x44556677
    # debug-mask[64-95]  = 0x8899AABB
    # debug-mask[96-127] = 0xCCDDEEFF
    # debug-lock[0-31]   = 0x00112233
    # debug-lock[32-63]  = 0x44556677
    # debug-lock[64-95]  = 0x8899AABB
    # debug-lock[96-127] = 0xCCDDEEFF
    # #hbk-id = 2
    # key-cert-pkg = key_cert_hbk0.bin
    # next-cert-pubkey = key_free_public.pem
    # cert-pkg = cert_enabler_pkg.bin

    key_free_pem = '''-----BEGIN RSA PRIVATE KEY-----
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
-----END RSA PRIVATE KEY-----'''
    soc_id = bytes([0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x50, 0x51, 0x52, 0x53, 0x54, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F])
    full_developer_cert_bin = bytes([0x63, 0x6B, 0x42, 0x53, 0x01, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xD6, 0xF7, 0x9A, 0x36, 0x47, 0x82, 0xB4, 0xC1, 0xFD, 0xC7, 0xFB, 0x86, 0x18, 0xD5, 0x4E, 0xFB, 0x65, 0x18, 0xE5, 0x07, 0x79, 0x84, 0x01, 0x99, 0xD9, 0xD9, 0x86, 0x67, 0x1D, 0x6C, 0x5D, 0xDE, 0x13, 0xC8, 0x41, 0x99, 0x4C, 0x08, 0x7D, 0x0F, 0x33, 0x3F, 0xED, 0x2E, 0xFE, 0xE0, 0x14, 0xF9, 0x0D, 0xE0, 0xDC, 0x98, 0x9F, 0xF3, 0x9B, 0x8D, 0x96, 0x3B, 0x6E, 0x5A, 0x85, 0xB8, 0x31, 0x75, 0x2C, 0x90, 0x0B, 0x29, 0xB4, 0xA5, 0x9F, 0xB0, 0x2D, 0x16, 0x99, 0x24, 0x8A, 0x8A, 0x60, 0xBB, 0x3B, 0xCA, 0x26, 0x3A, 0xDF, 0x8B, 0xF0, 0x1B, 0x3F, 0x99, 0x6D, 0xD5, 0x1F, 0x78, 0xDB, 0xE0, 0xBE, 0x55, 0xC5, 0xB8, 0x59, 0xB3, 0x7E, 0x58, 0x28, 0x26, 0x61, 0x6C, 0x98, 0x34, 0xD6, 0x7E, 0xB8, 0xFA, 0x29, 0x49, 0x3F, 0xBC, 0x2E, 0x35, 0x11, 0x99, 0xD9, 0x19, 0xAC, 0x27, 0x48, 0x19, 0xA7, 0xB8, 0x1F, 0x04, 0x4E, 0x96, 0x2C, 0xF9, 0xE8, 0x4C, 0xAB, 0xEC, 0x01, 0x54, 0x80, 0xB1, 0xF2, 0x4F, 0x5D, 0x09, 0x34, 0xC2, 0x40, 0xCB, 0xA8, 0xBA, 0x00, 0xB8, 0x3E, 0x61, 0x3E, 0x8B, 0x70, 0x41, 0x88, 0x32, 0xA6, 0x3A, 0x79, 0xF8, 0x3F, 0x44, 0x88, 0xCC, 0x8D, 0xC8, 0x4C, 0x2E, 0x2D, 0x74, 0x56, 0xC2, 0x08, 0xD6, 0xDE, 0xC1, 0x7F, 0x9B, 0xE7, 0xF8, 0xDF, 0x7F, 0x61, 0xE6, 0x14, 0xEB, 0x71, 0x0B, 0x7A, 0x33, 0xB7, 0x53, 0x02, 0x9C, 0x9E, 0xBB, 0xA3, 0x20, 0xA4, 0xFC, 0xD0, 0xD9, 0x31, 0x95, 0x8D, 0xAD, 0x40, 0xAB, 0x78, 0x55, 0x9D, 0x3F, 0x6F, 0x5B, 0x33, 0x78, 0x81, 0x67, 0xAD, 0xFB, 0xF1, 0x41, 0x5D, 0xE3, 0x70, 0xE7, 0x87, 0xD5, 0xE3, 0x8A, 0x00, 0x79, 0x60, 0xA2, 0xAB, 0x85, 0xB7, 0x52, 0xB6, 0xDC, 0x66, 0x45, 0x85, 0x0C, 0xC1, 0xAC, 0xC3, 0x8D, 0xCD, 0xEE, 0x2D, 0x59, 0xBC, 0xCF, 0x1B, 0x07, 0x1B, 0x52, 0xEB, 0x3F, 0x94, 0xE1, 0xC6, 0xB6, 0x45, 0xE7, 0x89, 0x5A, 0x21, 0x35, 0xF2, 0x67, 0x5B, 0xA9, 0x70, 0xD2, 0x05, 0x2E, 0x19, 0xE7, 0x74, 0xB5, 0x22, 0x5D, 0xA0, 0xDB, 0xB2, 0xC4, 0xDE, 0x0C, 0x66, 0x90, 0x35, 0xFA, 0x83, 0x2E, 0x36, 0x12, 0x8C, 0x0C, 0x9A, 0xF2, 0xCF, 0xA4, 0x36, 0x89, 0x5C, 0x4C, 0xBF, 0x5A, 0xE6, 0x34, 0x9B, 0x7B, 0x47, 0x1A, 0x29, 0xF1, 0x40, 0xC2, 0x47, 0x66, 0x82, 0x39, 0x8A, 0x28, 0xBF, 0x3D, 0x9C, 0x37, 0x0D, 0x0C, 0x31, 0x45, 0x78, 0x18, 0x34, 0xA0, 0x6C, 0x21, 0xC5, 0xD5, 0x4F, 0x4F, 0xA3, 0x8E, 0xA1, 0x86, 0x00, 0xEC, 0xDA, 0x7E, 0x5A, 0xA6, 0xCD, 0x8B, 0x62, 0x34, 0x6F, 0x92, 0x21, 0x16, 0xC2, 0xE1, 0x6E, 0x9D, 0xEA, 0x68, 0xB5, 0xDD, 0xEF, 0xE9, 0xC1, 0x05, 0x42, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x6E, 0xBC, 0x36, 0x64, 0xEA, 0x13, 0x08, 0xF9, 0x05, 0x00, 0x00, 0x00, 0x5F, 0x5D, 0xCD, 0xBF, 0x44, 0x96, 0x25, 0xC6, 0x5D, 0x25, 0xFE, 0x14, 0x55, 0x74, 0x05, 0xD4, 0x78, 0xB9, 0xC4, 0x8D, 0x84, 0xF5, 0x9A, 0x95, 0x33, 0xED, 0x42, 0x8B, 0x88, 0x2D, 0x58, 0x68, 0x29, 0x17, 0x09, 0xE7, 0x23, 0x85, 0xA1, 0xED, 0x9C, 0xD1, 0x3E, 0x2C, 0xAB, 0xC7, 0xC7, 0x0B, 0x98, 0xD2, 0xF9, 0x54, 0x05, 0x9C, 0x04, 0xB2, 0xE7, 0xD6, 0xF7, 0xC2, 0xC0, 0x6B, 0x27, 0x83, 0x33, 0xFF, 0xC1, 0x34, 0x95, 0x4C, 0x20, 0xA5, 0x03, 0x12, 0x6E, 0xA2, 0x78, 0x9C, 0x5C, 0x39, 0x52, 0x2C, 0xC7, 0xD1, 0x19, 0x36, 0x49, 0x94, 0xC7, 0xEB, 0x17, 0x3C, 0x44, 0x98, 0x7B, 0xB8, 0x33, 0xE9, 0xBC, 0x8A, 0x40, 0x9F, 0x95, 0xC9, 0x56, 0x0C, 0x7E, 0x28, 0xBE, 0xBB, 0x3C, 0x39, 0xE3, 0x67, 0xED, 0x48, 0x98, 0xD8, 0x31, 0x6E, 0x04, 0x0A, 0x40, 0xC7, 0xCC, 0xBD, 0xE5, 0xD4, 0x6B, 0xA8, 0xCF, 0xAB, 0x7F, 0x85, 0x1B, 0x1A, 0xDA, 0x93, 0xC8, 0xC1, 0x59, 0x0B, 0xB3, 0x09, 0x09, 0x81, 0x18, 0x12, 0xEA, 0xB7, 0xBC, 0x6D, 0x4C, 0x9F, 0x89, 0xD2, 0x97, 0xCF, 0xFF, 0x8F, 0x27, 0xDD, 0xFA, 0xF0, 0xA2, 0x17, 0x83, 0x6D, 0x3C, 0x22, 0x44, 0xCA, 0x56, 0xA6, 0xA0, 0x63, 0xA9, 0x8E, 0x32, 0x26, 0x69, 0xDC, 0x63, 0x4E, 0x2B, 0x09, 0x36, 0x66, 0x70, 0xF1, 0x8D, 0x87, 0x13, 0x85, 0xCF, 0x6B, 0x97, 0x7A, 0x50, 0xED, 0xDF, 0xD0, 0xB8, 0x0F, 0x6F, 0xE9, 0x48, 0x52, 0xE6, 0x08, 0x3B, 0xBA, 0x87, 0xC2, 0xE1, 0x1F, 0xAB, 0x33, 0xDC, 0x7E, 0x7B, 0x48, 0x56, 0xB2, 0x72, 0x83, 0xAA, 0x85, 0x53, 0x4A, 0xA4, 0x2F, 0x3B, 0x48, 0x6F, 0xD1, 0xA0, 0x42, 0x56, 0xEE, 0xFC, 0x30, 0xE3, 0xA5, 0x42, 0x12, 0x87, 0xB6, 0x42, 0x17, 0xA9, 0x86, 0x1A, 0x28, 0x7A, 0x5C, 0x80, 0x6C, 0xF3, 0xC5, 0x77, 0x22, 0xE9, 0xCF, 0x8B, 0x8C, 0x55, 0x3B, 0x13, 0xB9, 0x84, 0x8F, 0xC6, 0xCD, 0x58, 0x60, 0xFE, 0x3A, 0x8F, 0xA6, 0x40, 0x82, 0x38, 0xE7, 0xA2, 0x9D, 0xE0, 0xC6, 0xEB, 0x15, 0x57, 0xC2, 0xDD, 0x58, 0x16, 0x50, 0x20, 0x10, 0x0B, 0x6E, 0xDD, 0xB8, 0x32, 0x36, 0xB6, 0x8F, 0x57, 0xC7, 0x69, 0xDA, 0x90, 0xEF, 0xEB, 0xCB, 0xB2, 0xE9, 0x01, 0xD4, 0xCB, 0x26, 0xF1, 0xCE, 0x96, 0xAA, 0x16, 0x26, 0x48, 0xC0, 0x8B, 0x3A, 0x3A, 0x97, 0x11, 0xC8, 0x58, 0xF5, 0x41, 0x8C, 0x20, 0x3F, 0x0E, 0x70, 0x3F, 0x86, 0x3D, 0x5A, 0x54, 0xFC, 0xEF, 0x8D, 0x55, 0x8E, 0xDB, 0x2A, 0x7C, 0xA1, 0x27, 0xA5, 0xEE, 0x75, 0xB7, 0x1E, 0xE5, 0xBD, 0x3B, 0x37, 0x21, 0xB0, 0x61, 0x43, 0xB7, 0x36, 0x21, 0x39, 0xB1, 0xA7, 0x86, 0xCD, 0x16, 0xE3, 0x29, 0x14, 0xFB, 0x49, 0x60, 0x18, 0x74, 0x11, 0xAA, 0xD2, 0x4E, 0x43, 0x79, 0x7C, 0x08, 0xE2, 0x2D, 0xD5, 0x00, 0x53, 0x06, 0x1D, 0xB8, 0xD3, 0xAB, 0xE3, 0x1B, 0x4F, 0x11, 0x74, 0x6C, 0xEF, 0xDF, 0xF3, 0xDF, 0x4E, 0x6E, 0x65, 0x64, 0x53, 0x01, 0x00, 0x00, 0x00, 0x79, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0xD6, 0xF7, 0x9A, 0x36, 0x47, 0x82, 0xB4, 0xC1, 0xFD, 0xC7, 0xFB, 0x86, 0x18, 0xD5, 0x4E, 0xFB, 0x65, 0x18, 0xE5, 0x07, 0x79, 0x84, 0x01, 0x99, 0xD9, 0xD9, 0x86, 0x67, 0x1D, 0x6C, 0x5D, 0xDE, 0x13, 0xC8, 0x41, 0x99, 0x4C, 0x08, 0x7D, 0x0F, 0x33, 0x3F, 0xED, 0x2E, 0xFE, 0xE0, 0x14, 0xF9, 0x0D, 0xE0, 0xDC, 0x98, 0x9F, 0xF3, 0x9B, 0x8D, 0x96, 0x3B, 0x6E, 0x5A, 0x85, 0xB8, 0x31, 0x75, 0x2C, 0x90, 0x0B, 0x29, 0xB4, 0xA5, 0x9F, 0xB0, 0x2D, 0x16, 0x99, 0x24, 0x8A, 0x8A, 0x60, 0xBB, 0x3B, 0xCA, 0x26, 0x3A, 0xDF, 0x8B, 0xF0, 0x1B, 0x3F, 0x99, 0x6D, 0xD5, 0x1F, 0x78, 0xDB, 0xE0, 0xBE, 0x55, 0xC5, 0xB8, 0x59, 0xB3, 0x7E, 0x58, 0x28, 0x26, 0x61, 0x6C, 0x98, 0x34, 0xD6, 0x7E, 0xB8, 0xFA, 0x29, 0x49, 0x3F, 0xBC, 0x2E, 0x35, 0x11, 0x99, 0xD9, 0x19, 0xAC, 0x27, 0x48, 0x19, 0xA7, 0xB8, 0x1F, 0x04, 0x4E, 0x96, 0x2C, 0xF9, 0xE8, 0x4C, 0xAB, 0xEC, 0x01, 0x54, 0x80, 0xB1, 0xF2, 0x4F, 0x5D, 0x09, 0x34, 0xC2, 0x40, 0xCB, 0xA8, 0xBA, 0x00, 0xB8, 0x3E, 0x61, 0x3E, 0x8B, 0x70, 0x41, 0x88, 0x32, 0xA6, 0x3A, 0x79, 0xF8, 0x3F, 0x44, 0x88, 0xCC, 0x8D, 0xC8, 0x4C, 0x2E, 0x2D, 0x74, 0x56, 0xC2, 0x08, 0xD6, 0xDE, 0xC1, 0x7F, 0x9B, 0xE7, 0xF8, 0xDF, 0x7F, 0x61, 0xE6, 0x14, 0xEB, 0x71, 0x0B, 0x7A, 0x33, 0xB7, 0x53, 0x02, 0x9C, 0x9E, 0xBB, 0xA3, 0x20, 0xA4, 0xFC, 0xD0, 0xD9, 0x31, 0x95, 0x8D, 0xAD, 0x40, 0xAB, 0x78, 0x55, 0x9D, 0x3F, 0x6F, 0x5B, 0x33, 0x78, 0x81, 0x67, 0xAD, 0xFB, 0xF1, 0x41, 0x5D, 0xE3, 0x70, 0xE7, 0x87, 0xD5, 0xE3, 0x8A, 0x00, 0x79, 0x60, 0xA2, 0xAB, 0x85, 0xB7, 0x52, 0xB6, 0xDC, 0x66, 0x45, 0x85, 0x0C, 0xC1, 0xAC, 0xC3, 0x8D, 0xCD, 0xEE, 0x2D, 0x59, 0xBC, 0xCF, 0x1B, 0x07, 0x1B, 0x52, 0xEB, 0x3F, 0x94, 0xE1, 0xC6, 0xB6, 0x45, 0xE7, 0x89, 0x5A, 0x21, 0x35, 0xF2, 0x67, 0x5B, 0xA9, 0x70, 0xD2, 0x05, 0x2E, 0x19, 0xE7, 0x74, 0xB5, 0x22, 0x5D, 0xA0, 0xDB, 0xB2, 0xC4, 0xDE, 0x0C, 0x66, 0x90, 0x35, 0xFA, 0x83, 0x2E, 0x36, 0x12, 0x8C, 0x0C, 0x9A, 0xF2, 0xCF, 0xA4, 0x36, 0x89, 0x5C, 0x4C, 0xBF, 0x5A, 0xE6, 0x34, 0x9B, 0x7B, 0x47, 0x1A, 0x29, 0xF1, 0x40, 0xC2, 0x47, 0x66, 0x82, 0x39, 0x8A, 0x28, 0xBF, 0x3D, 0x9C, 0x37, 0x0D, 0x0C, 0x31, 0x45, 0x78, 0x18, 0x34, 0xA0, 0x6C, 0x21, 0xC5, 0xD5, 0x4F, 0x4F, 0xA3, 0x8E, 0xA1, 0x86, 0x00, 0xEC, 0xDA, 0x7E, 0x5A, 0xA6, 0xCD, 0x8B, 0x62, 0x34, 0x6F, 0x92, 0x21, 0x16, 0xC2, 0xE1, 0x6E, 0x9D, 0xEA, 0x68, 0xB5, 0xDD, 0xEF, 0xE9, 0xC1, 0x05, 0x42, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x6E, 0xBC, 0x36, 0x64, 0xEA, 0x13, 0x08, 0xF9, 0x33, 0x22, 0x11, 0x00, 0x77, 0x66, 0x55, 0x44, 0xBB, 0xAA, 0x99, 0x88, 0xFF, 0xEE, 0xDD, 0xCC, 0x33, 0x22, 0x11, 0x00, 0x77, 0x66, 0x55, 0x44, 0xBB, 0xAA, 0x99, 0x88, 0xFF, 0xEE, 0xDD, 0xCC, 0x5F, 0x5D, 0xCD, 0xBF, 0x44, 0x96, 0x25, 0xC6, 0x5D, 0x25, 0xFE, 0x14, 0x55, 0x74, 0x05, 0xD4, 0x78, 0xB9, 0xC4, 0x8D, 0x84, 0xF5, 0x9A, 0x95, 0x33, 0xED, 0x42, 0x8B, 0x88, 0x2D, 0x58, 0x68, 0xFD, 0x69, 0x8F, 0x95, 0x34, 0x4E, 0xB7, 0x82, 0x40, 0x76, 0x66, 0x80, 0xE6, 0x50, 0x2D, 0xAB, 0x33, 0x35, 0x75, 0x4F, 0x02, 0x58, 0xDE, 0xC1, 0x62, 0xB4, 0x5A, 0x45, 0x1B, 0x20, 0xA8, 0x72, 0xA3, 0x24, 0xCF, 0xD0, 0x3B, 0x7E, 0xBE, 0x36, 0x4C, 0xED, 0x65, 0x58, 0x41, 0x84, 0xEB, 0x86, 0xE8, 0x59, 0x1D, 0x06, 0x19, 0xFF, 0xE4, 0xBD, 0x11, 0xF3, 0x2E, 0xBE, 0x05, 0x78, 0x5E, 0x6F, 0xCD, 0xAC, 0x5F, 0x89, 0x6B, 0x95, 0x6B, 0x7C, 0xA1, 0x92, 0x32, 0x73, 0x09, 0x14, 0x22, 0x3B, 0x69, 0x03, 0x55, 0xDB, 0x40, 0x56, 0xFF, 0x69, 0x3C, 0x96, 0xEA, 0x78, 0x31, 0xCA, 0x99, 0x5D, 0x7F, 0x9A, 0xCA, 0x4F, 0x17, 0xC5, 0xF9, 0x27, 0x2D, 0x9F, 0x15, 0x53, 0x73, 0x58, 0x1C, 0xEF, 0xFD, 0x18, 0x83, 0xF8, 0x80, 0xCF, 0x28, 0x08, 0x11, 0x26, 0xA9, 0x0D, 0x8D, 0xA6, 0xEB, 0x07, 0xD4, 0x36, 0x10, 0x49, 0x4D, 0xF0, 0x46, 0xBC, 0x12, 0xF6, 0x3A, 0x13, 0x01, 0xBF, 0xDD, 0x25, 0x03, 0x97, 0xE3, 0x5D, 0x5B, 0x4C, 0x52, 0xD8, 0xE3, 0x5A, 0x19, 0xF8, 0x4C, 0x93, 0x31, 0x88, 0xCD, 0x82, 0x91, 0xEB, 0xE5, 0x2E, 0x86, 0x38, 0xC3, 0x12, 0x00, 0x4B, 0x35, 0x15, 0xF0, 0xFB, 0xD2, 0xAC, 0x05, 0x30, 0x3A, 0xF3, 0xB4, 0x43, 0xA1, 0x6A, 0xF6, 0x2A, 0x3A, 0x4C, 0xE3, 0x2C, 0xD7, 0xA3, 0xCC, 0xE8, 0x72, 0x35, 0x75, 0x9E, 0xF5, 0x78, 0x99, 0x38, 0x25, 0xCF, 0x04, 0x1C, 0xE2, 0x84, 0xCC, 0xA6, 0x4F, 0xDC, 0x02, 0x8E, 0x61, 0x00, 0xF7, 0x9D, 0xDB, 0xCA, 0x6C, 0xA4, 0xDD, 0xFB, 0xA7, 0x81, 0xE5, 0x52, 0xB3, 0x08, 0x9B, 0x7B, 0x85, 0xED, 0x99, 0x4C, 0x99, 0x7F, 0x55, 0x1C, 0xD4, 0xBA, 0xEA, 0xE3, 0x9D, 0x20, 0xAD, 0xE2, 0xED, 0xE3, 0x94, 0xB1, 0xC8, 0x96, 0xF4, 0xAF, 0x2A, 0x42, 0xD7, 0xD7, 0xCE, 0xC9, 0xC2, 0x63, 0xC2, 0x8E, 0xBC, 0xAA, 0x54, 0xC6, 0x70, 0x60, 0xCA, 0xEE, 0xBB, 0xD7, 0x3D, 0x92, 0x3A, 0xAE, 0x47, 0xD1, 0x09, 0x8A, 0xEA, 0xDE, 0x5C, 0x9D, 0x60, 0x15, 0x38, 0x46, 0x25, 0x22, 0xDC, 0x36, 0xC4, 0x97, 0x59, 0x54, 0x65, 0xD6, 0x26, 0xB0, 0x71, 0x1C, 0xA7, 0x3E, 0x78, 0x0B, 0xB5, 0x52, 0xAB, 0x37, 0x4B, 0x02, 0x3C, 0xC3, 0x5F, 0x7E, 0x84, 0x7B, 0x16, 0xE0, 0xC1, 0xD1, 0x47, 0x51, 0xB8, 0x61, 0xC5, 0xB8, 0x67, 0x6D, 0x61, 0x49, 0x51, 0x36, 0x72, 0x40, 0x15, 0xD2, 0x08, 0xB4, 0xCA, 0x76, 0xFD, 0x72, 0x04, 0x5F, 0x36, 0x6C, 0xA7, 0x29, 0xAC, 0xAD, 0xD7, 0xD3, 0x45, 0xFE, 0x91, 0x00, 0x5D, 0x78, 0x0F, 0x14, 0x8D, 0xC1, 0xB8, 0xCF, 0x2E, 0x98, 0x7D, 0x15, 0x36, 0x68, 0x42, 0x44, 0xAD, 0x74, 0x15, 0x07, 0x65, 0x64, 0x64, 0x53, 0x01, 0x00, 0x00, 0x00, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD6, 0xF7, 0x9A, 0x36, 0x47, 0x82, 0xB4, 0xC1, 0xFD, 0xC7, 0xFB, 0x86, 0x18, 0xD5, 0x4E, 0xFB, 0x65, 0x18, 0xE5, 0x07, 0x79, 0x84, 0x01, 0x99, 0xD9, 0xD9, 0x86, 0x67, 0x1D, 0x6C, 0x5D, 0xDE, 0x13, 0xC8, 0x41, 0x99, 0x4C, 0x08, 0x7D, 0x0F, 0x33, 0x3F, 0xED, 0x2E, 0xFE, 0xE0, 0x14, 0xF9, 0x0D, 0xE0, 0xDC, 0x98, 0x9F, 0xF3, 0x9B, 0x8D, 0x96, 0x3B, 0x6E, 0x5A, 0x85, 0xB8, 0x31, 0x75, 0x2C, 0x90, 0x0B, 0x29, 0xB4, 0xA5, 0x9F, 0xB0, 0x2D, 0x16, 0x99, 0x24, 0x8A, 0x8A, 0x60, 0xBB, 0x3B, 0xCA, 0x26, 0x3A, 0xDF, 0x8B, 0xF0, 0x1B, 0x3F, 0x99, 0x6D, 0xD5, 0x1F, 0x78, 0xDB, 0xE0, 0xBE, 0x55, 0xC5, 0xB8, 0x59, 0xB3, 0x7E, 0x58, 0x28, 0x26, 0x61, 0x6C, 0x98, 0x34, 0xD6, 0x7E, 0xB8, 0xFA, 0x29, 0x49, 0x3F, 0xBC, 0x2E, 0x35, 0x11, 0x99, 0xD9, 0x19, 0xAC, 0x27, 0x48, 0x19, 0xA7, 0xB8, 0x1F, 0x04, 0x4E, 0x96, 0x2C, 0xF9, 0xE8, 0x4C, 0xAB, 0xEC, 0x01, 0x54, 0x80, 0xB1, 0xF2, 0x4F, 0x5D, 0x09, 0x34, 0xC2, 0x40, 0xCB, 0xA8, 0xBA, 0x00, 0xB8, 0x3E, 0x61, 0x3E, 0x8B, 0x70, 0x41, 0x88, 0x32, 0xA6, 0x3A, 0x79, 0xF8, 0x3F, 0x44, 0x88, 0xCC, 0x8D, 0xC8, 0x4C, 0x2E, 0x2D, 0x74, 0x56, 0xC2, 0x08, 0xD6, 0xDE, 0xC1, 0x7F, 0x9B, 0xE7, 0xF8, 0xDF, 0x7F, 0x61, 0xE6, 0x14, 0xEB, 0x71, 0x0B, 0x7A, 0x33, 0xB7, 0x53, 0x02, 0x9C, 0x9E, 0xBB, 0xA3, 0x20, 0xA4, 0xFC, 0xD0, 0xD9, 0x31, 0x95, 0x8D, 0xAD, 0x40, 0xAB, 0x78, 0x55, 0x9D, 0x3F, 0x6F, 0x5B, 0x33, 0x78, 0x81, 0x67, 0xAD, 0xFB, 0xF1, 0x41, 0x5D, 0xE3, 0x70, 0xE7, 0x87, 0xD5, 0xE3, 0x8A, 0x00, 0x79, 0x60, 0xA2, 0xAB, 0x85, 0xB7, 0x52, 0xB6, 0xDC, 0x66, 0x45, 0x85, 0x0C, 0xC1, 0xAC, 0xC3, 0x8D, 0xCD, 0xEE, 0x2D, 0x59, 0xBC, 0xCF, 0x1B, 0x07, 0x1B, 0x52, 0xEB, 0x3F, 0x94, 0xE1, 0xC6, 0xB6, 0x45, 0xE7, 0x89, 0x5A, 0x21, 0x35, 0xF2, 0x67, 0x5B, 0xA9, 0x70, 0xD2, 0x05, 0x2E, 0x19, 0xE7, 0x74, 0xB5, 0x22, 0x5D, 0xA0, 0xDB, 0xB2, 0xC4, 0xDE, 0x0C, 0x66, 0x90, 0x35, 0xFA, 0x83, 0x2E, 0x36, 0x12, 0x8C, 0x0C, 0x9A, 0xF2, 0xCF, 0xA4, 0x36, 0x89, 0x5C, 0x4C, 0xBF, 0x5A, 0xE6, 0x34, 0x9B, 0x7B, 0x47, 0x1A, 0x29, 0xF1, 0x40, 0xC2, 0x47, 0x66, 0x82, 0x39, 0x8A, 0x28, 0xBF, 0x3D, 0x9C, 0x37, 0x0D, 0x0C, 0x31, 0x45, 0x78, 0x18, 0x34, 0xA0, 0x6C, 0x21, 0xC5, 0xD5, 0x4F, 0x4F, 0xA3, 0x8E, 0xA1, 0x86, 0x00, 0xEC, 0xDA, 0x7E, 0x5A, 0xA6, 0xCD, 0x8B, 0x62, 0x34, 0x6F, 0x92, 0x21, 0x16, 0xC2, 0xE1, 0x6E, 0x9D, 0xEA, 0x68, 0xB5, 0xDD, 0xEF, 0xE9, 0xC1, 0x05, 0x42, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x6E, 0xBC, 0x36, 0x64, 0xEA, 0x13, 0x08, 0xF9, 0x33, 0x22, 0x11, 0x00, 0x77, 0x66, 0x55, 0x44, 0xBB, 0xAA, 0x99, 0x88, 0xFF, 0xEE, 0xDD, 0xCC, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x50, 0x51, 0x52, 0x53, 0x54, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0xC3, 0x47, 0x07, 0x07, 0xF8, 0x8C, 0x9F, 0xE7, 0x4D, 0x1F, 0x5E, 0xC5, 0xCC, 0x70, 0x76, 0x79, 0xC0, 0xFE, 0xD1, 0xC1, 0x3A, 0x45, 0x2A, 0x89, 0x7C, 0xF5, 0x9B, 0xAB, 0xE2, 0x1D, 0x10, 0xC8, 0x05, 0x8C, 0x35, 0x17, 0xEC, 0xB2, 0x90, 0x89, 0x80, 0xD2, 0x0C, 0xDE, 0x24, 0x56, 0xDC, 0x26, 0x70, 0x1E, 0x4A, 0x03, 0x08, 0x4F, 0xA8, 0x01, 0x34, 0x90, 0xA2, 0x63, 0x2C, 0x14, 0xD6, 0x9B, 0xAA, 0xA9, 0xE5, 0x73, 0xE9, 0xF8, 0x60, 0x04, 0x62, 0x79, 0x89, 0x6B, 0x0C, 0xEA, 0x1B, 0x51, 0x03, 0x67, 0x53, 0x6B, 0x93, 0x46, 0xCB, 0x3B, 0xC9, 0xDD, 0x1A, 0xAE, 0x3F, 0x7F, 0x29, 0x68, 0xE1, 0xA9, 0xDA, 0x92, 0x87, 0x5C, 0x6C, 0x98, 0x92, 0xEA, 0xA2, 0xA6, 0x50, 0xAE, 0x74, 0xD2, 0x98, 0x74, 0x36, 0x07, 0x40, 0x4E, 0xEF, 0xEA, 0x7D, 0x48, 0xD2, 0x1A, 0x3F, 0xA2, 0x62, 0xC8, 0x33, 0x50, 0xC1, 0xFD, 0x4C, 0xAE, 0x2A, 0x39, 0x55, 0x30, 0xE1, 0x1E, 0xCC, 0x17, 0x46, 0x9C, 0x6D, 0xF7, 0x80, 0x79, 0x8C, 0x51, 0x59, 0x30, 0x85, 0x8D, 0x26, 0x9E, 0x0E, 0x1C, 0xC7, 0x99, 0xA9, 0x3B, 0xA1, 0x38, 0xDC, 0x95, 0xC9, 0x4A, 0xEA, 0xEE, 0x00, 0x0F, 0xD4, 0xFC, 0xC3, 0x11, 0x82, 0x80, 0x34, 0x88, 0xA8, 0x59, 0x93, 0xB2, 0x40, 0x7E, 0x30, 0xEC, 0x22, 0x68, 0xED, 0x91, 0x06, 0xBF, 0xF8, 0x59, 0x09, 0xCA, 0x24, 0x06, 0x4D, 0x85, 0xD6, 0x41, 0x25, 0x16, 0x54, 0x56, 0x1B, 0x6C, 0x0D, 0x38, 0x99, 0xD1, 0xC6, 0x15, 0xE7, 0x6B, 0x6A, 0x1C, 0x0F, 0x5D, 0x6D, 0xD4, 0x10, 0xFA, 0x4B, 0xEB, 0x84, 0xA6, 0x1F, 0x01, 0x66, 0x7F, 0xC5, 0x86, 0x53, 0xAB, 0xCF, 0xDD, 0x07, 0x4B, 0xDB, 0x3E, 0x84, 0xB2, 0x61, 0x2A, 0x49, 0x1A, 0x17, 0x27, 0x54, 0x69, 0xD3, 0x54, 0x1A, 0x3C, 0x28, 0xE4, 0x21, 0x71, 0x03, 0xEE, 0x22, 0x91, 0x6B, 0x21, 0x9C, 0xF7, 0xD3, 0xB7, 0x1F, 0x5C, 0x47, 0x4E, 0x53, 0xE3, 0x96, 0x3A, 0xBB, 0x21, 0x4F, 0xC1, 0x49, 0x8D, 0x07, 0xD6, 0x46, 0x29, 0xBC, 0x56, 0x5E, 0xED, 0x60, 0x5E, 0x5E, 0xC1, 0xFB, 0xBF, 0x61, 0xFA, 0x86, 0x89, 0x16, 0x21, 0x67, 0x5E, 0xA4, 0x67, 0xCD, 0x64, 0xB9, 0xC3, 0xF4, 0xBA, 0x27, 0xC7, 0x3D, 0xD5, 0x93, 0x44, 0x7F, 0x56, 0x43, 0xB7, 0x96, 0xDF, 0x36, 0x72, 0xAE, 0x6E, 0x54, 0x71, 0x07, 0x44, 0x06, 0x90, 0xA7, 0x00, 0x74, 0x49, 0x8C, 0xE2, 0xFB, 0x1E, 0x35, 0xD4, 0xFC, 0x9C, 0xE8, 0x79, 0x5B, 0xDB, 0x20, 0xF1, 0x0F, 0x43, 0xC7, 0x7B, 0x2E, 0x12, 0xE3, 0x76, 0xB5, 0xCC, 0x8E, 0xA5, 0x60, 0xC9, 0x6E, 0x2D, 0xAF, 0x9A, 0xF5, 0xB2, 0x4F, 0xB5, 0x03, 0x80, 0xCC, 0x0F, 0x80, 0x11])


class TestSecureBootEntitlement(unittest.TestCase):


    def setUp(self):
        self.crypto_helper = CryptoHelper()

    def test_basic_3_certificates(self):

        self.crypto_helper.rand_func = lambda x : bytes(x) # deterministic
        #sign_rsa_key = crypto_helper.generate_random_rsa_key()
        #next_rsa_key = crypto_helper.generate_random_rsa_key()

        c = SecureBootDebugEntitlement(key_cert_key = self.crypto_helper.rsa_import_key_from_contents(CertificatetTestExample1.key_free_pem),
                             enabler_cert_key = self.crypto_helper.rsa_import_key_from_contents(CertificatetTestExample1.key_free_pem),
                             developer_cert_key = self.crypto_helper.rsa_import_key_from_contents(CertificatetTestExample1.key_free_pem),
                             cert_version = 1,
                             hbk_id = HbkId.HBK,
                             sw_version = 5,
                             lcs = CC312LifeCycleState.DM,
                             debug_enabler_mask   = 0xCCDDEEFF8899AABB4455667700112233,
                             debug_enabler_lock   = 0xCCDDEEFF8899AABB4455667700112233,
                             debug_developer_mask = 0xCCDDEEFF8899AABB4455667700112233,
                             soc_id = CertificatetTestExample1.soc_id)


        self.assertEqual(bytes(c)[:0x1c8], CertificatetTestExample1.full_developer_cert_bin[:0x1c8])
        self.assertEqual(bytes(c)[0x348:0x52C], CertificatetTestExample1.full_developer_cert_bin[0x348:0x52C])
        self.assertEqual(bytes(c)[0x6AC:881], CertificatetTestExample1.full_developer_cert_bin[0x6AC:881])


    def test_basic_2_certificates(self):
        c = SecureBootDebugEntitlement(
                             enabler_cert_key = self.crypto_helper.rsa_import_key_from_contents(CertificatetTestExample1.key_free_pem),
                             developer_cert_key = self.crypto_helper.rsa_import_key_from_contents(CertificatetTestExample1.key_free_pem),
                             cert_version = 1,
                             hbk_id = HbkId.HBK,
                             sw_version = 5,
                             lcs = CC312LifeCycleState.DM,
                             debug_enabler_mask   = 0xCCDDEEFF8899AABB4455667700112233,
                             debug_enabler_lock   = 0xCCDDEEFF8899AABB4455667700112233,
                             debug_developer_mask = 0xCCDDEEFF8899AABB4455667700112233,
                             soc_id = CertificatetTestExample1.soc_id)

        #self.assertEqual(bytes(c)[:0x1c8], CertificatetTestExample1.full_developer_cert_bin[:0x1c8])
        #self.assertEqual(bytes(c)[0x348:0x52C], CertificatetTestExample1.full_developer_cert_bin[0x348:0x52C])
        self.assertEqual(bytes(c)[0x6AC-0x348:881-0x348], CertificatetTestExample1.full_developer_cert_bin[0x6AC:881])



if __name__ == "__main__":
    unittest.main()





