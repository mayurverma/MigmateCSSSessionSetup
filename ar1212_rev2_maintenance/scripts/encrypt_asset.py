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
# Script to encrypt an asset
#
########################################################################################
import argparse
import sys

from root_of_trust import RootOfTrust
from crypto_services import CryptoServices

# Create the argument parser
parser = argparse.ArgumentParser(prog='encrypt_asset.py',
    description='Generate encrypted/signed blobs e.g. maintenance patch or debug entitlement')
parser.add_argument('-a', '--asset_id', required=True, help='Asset id (decimal or hex integer')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable debug output')
parser.add_argument('-k', '--key_set', choices=['eval', 'test'], required=True, help='Key set name')
parser.add_argument('-p', '--product', required=True, help='Product name : CSS, AR0820_REV2, AR1212_REV1...')
parser.add_argument('-r', '--rot', choices=['cm', 'dm'], required=True, help='Root of Trust (CM or DM)')

# Parse the command line arguments
args = parser.parse_args()

# Determine root of trust enumeration
if args.rot == 'dm':
    root_of_trust = RootOfTrust.DM
else:
    root_of_trust = RootOfTrust.CM

# Convert decimal/hex string to integer
asset_id = int(args.asset_id, 0)

# Read binary clear data from stdin
asset_data = sys.stdin.buffer.read()

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
crypto_service = CryptoServices(key_set, root_of_trust)
encrypted_asset_data = crypto_service.encrypt_asset(asset_id, asset_data)

# Write binary encrypted data to stdout
sys.stdout.buffer.write(encrypted_asset_data)

