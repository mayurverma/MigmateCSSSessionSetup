####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Merge two JSON format files
####################################################################################
import argparse
import json

def merge_dict(dict1, dict2):
    for key in dict1.keys():
        if key in dict2:
            # If both items are dictionaries then recurse
            if isinstance(dict1[key], dict) and isinstance(dict2[key], dict):
                merge_dict(dict1[key], dict2[key])
            else:
                dict1[key] = dict2[key]

# Create the argument parser
parser = argparse.ArgumentParser(prog='merge_parameters',
    description='Merge two JSON format files')
parser.add_argument('file1', help='Base parameters file')
parser.add_argument('file2', help='Input parameters file')

# Parse the command line arguments
args = parser.parse_args()

# Read the base file
with open(args.file1, encoding='utf-8') as file1:
    data1 = json.loads(file1.read())

# Read the input file
with open(args.file2, encoding='utf-8') as file2:
    data2 = json.loads(file2.read())

# Merge the input into the base (new values are added, existing values are overwritten)
merge_dict(data1, data2)

# Dump the merged data
print(json.dumps(data1, ensure_ascii=False, indent=2, sort_keys=True))
