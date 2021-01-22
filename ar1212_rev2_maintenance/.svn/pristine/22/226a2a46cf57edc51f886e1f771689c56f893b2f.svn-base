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
import argparse
import sys
import os

from root_of_trust import RootOfTrust
from crypto_services import CryptoServices
from secure_boot_entitlement import CC312LifeCycleState

# Create the argument parser
parser = argparse.ArgumentParser(prog='create_debug_entitlement.py',
    description='Create a Debug Entitlement')
parser.add_argument('-c', '--cert_version', required=True, help='Certificate version')
parser.add_argument('-d', '--developer_mask', required=True, help='Debug developer mask - 128 bits')
parser.add_argument('-a', '--enabler_mask', required=True, help='Debug enabler mask - 128 bits')
parser.add_argument('-o', '--enabler_lock', required=True, help='Debug enabler lock bits - 128 bits')
parser.add_argument('-e', '--key_cert', action='store_true', help='Use extra key certificate')
parser.add_argument('-k', '--key_set', choices=['eval', 'test'], required=True, help='Key set name')
parser.add_argument('-l', '--lcs', required=True, choices=['cm', 'dm', 'secure'], help='LifeCycleState')
parser.add_argument('-i', '--soc_id', required=True, help='Chip identifier')
parser.add_argument('-w', '--sw_version', required=True, help='Software version')
parser.add_argument('-p', '--product', required=True, help='Product name : CSS, AR0820_REV2, AR1212_REV1...')
parser.add_argument('-r', '--rot', required=True, choices=['cm', 'dm'], help='Root of Trust (CM or DM)')

# Parse the command line arguments
args = parser.parse_args()

# Determine root of trust enumeration
if args.rot == 'dm':
    root_of_trust = RootOfTrust.DM
else:
    root_of_trust = RootOfTrust.CM

# Determine CC312 internal lcs
if args.lcs == 'cm':
    lcs = CC312LifeCycleState.CM
elif args.lcs == 'dm':
    lcs = CC312LifeCycleState.DM
else:
    lcs = CC312LifeCycleState.Secure

# Convert the SOC ID to byte array
if args.soc_id.startswith('0x'):
    soc_id = bytes.fromhex(args.soc_id[2:])
else:
    soc_id = bytes.fromhex(args.soc_id)

# Convert decimal/hex string to integer
sw_version = int(args.sw_version, 0)
cert_version = int(args.cert_version, 0)
debug_enabler_mask = int(args.enabler_mask, 0)
debug_enabler_lock = int(args.enabler_lock, 0)
debug_developer_mask = int(args.developer_mask, 0)

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
debug_entitlement = crypto_service.create_debug_entitlement(args.key_cert,
    sw_version, cert_version, soc_id, debug_enabler_mask, debug_enabler_lock,
    debug_developer_mask, lcs)

# Write binary encrypted data to stdout
sys.stdout.buffer.write(debug_entitlement)




