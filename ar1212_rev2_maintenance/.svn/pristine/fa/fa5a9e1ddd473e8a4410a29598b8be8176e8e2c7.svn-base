####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Generates a random token string for use in scripts with authentication.
####################################################################################
import argparse
import random
import string

# Create the argument parser
parser = argparse.ArgumentParser(prog='token_generator',
    description='Token generator that creates a random token string.')
parser.add_argument('-l', '--length', type=int, default='16', help='Token length')

# Parse the command line arguments
args = parser.parse_args()

# Generate the token
token = ''.join(random.choices(string.ascii_lowercase + string.digits, k=args.length))
print(token)
