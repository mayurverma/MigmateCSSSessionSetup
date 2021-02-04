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
# Script to sign a blob
#
########################################################################################
import argparse
import sys

from root_of_trust import RootOfTrust
from crypto_services import CryptoServices
from key_name import KeyName

# Create the argument parser
parser = argparse.ArgumentParser(prog='sign_rsassa_pss.py',
    description='Sign a blob using RSA algorithm')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable debug output')
parser.add_argument('-k', '--key_set', choices=['eval', 'test'], required=True, help='Key set name')
parser.add_argument('-p', '--product', required=True, help='Product name : CSS, AR0820_REV2, AR1212_REV1...')
parser.add_argument('-n', '--key_name', choices=['sensor_pair', 'vendor_pair'], required=True, help='Key name')

# Parse the command line arguments
args = parser.parse_args()

# Determine key id from key name
key_name = getattr(KeyName, args.key_name.upper())

# Read binary clear data from stdin
data_bin = sys.stdin.buffer.read()

# Create the key set
try:
    # Key vault only exists on the CSS server
    from key_vault import KeyVault
    key_set = KeyVault.get_key_set(args.product, args.key_set)
except ImportError:
    # Test the CSS server infrastructure with fake key set
    from key_storage import get_key_storage
    key_set = get_key_storage(args.key_set)

# Encrypt the asset data
crypto_service = CryptoServices(key_set)
signature = crypto_service.sign_rsassa_pss(key_name, data_bin)

# Write binary encrypted data to stdout
sys.stdout.buffer.write(signature)

