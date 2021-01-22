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
# Generates Keys for DepuApp
#
########################################################################################
import argparse
import os
import sys
import getpass

# Add Python libraries to the import search path
rootdir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
scripts_dir = os.path.join(rootdir, 'scripts')

if scripts_dir not in sys.path:
    sys.path.append(scripts_dir)

from key_storage import KeyStorage, KeyStorageId

# Create the argument parser
parser = argparse.ArgumentParser(prog='generate_depu_keys',
    description='Generate key files for Depu App')
parser.add_argument('-v', '--verbose', default=False, action='store_true', help='Enable debug output')
parser.add_argument('-o', '--outputdir', required=False, help='Directory that contains the keys generated')

# Parse the command line arguments
args = parser.parse_args()

# Generate files
# print('Generating keys...')

key_storage = KeyStorage()

password = getpass.getpass('Enter Password:')
if password == '':
    password = None

keys_dir = args.outputdir

key_storage.generate_random_keys()
key_storage.save_files(keys_dir, password = password)

