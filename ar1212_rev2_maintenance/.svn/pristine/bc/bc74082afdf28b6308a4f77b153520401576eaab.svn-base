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
# Converts OTPM data image to IEEE Verilog readmemh format.
#
# The script outputs the OTPM image with and without ECC protection. The OTPM image is
# split into a number of OTPM instances based on the specified options.
#
########################################################################################
import argparse
import os
import sys

# Returns the 41 bit XOR of the data input word
def reduce_xor(input):
    result = 0
    for bit in range(41):
        result ^= ((input >> bit) & 1)
    return result

# Encodes a 16-bit data word with ECC & parity protection and returns a 22-bit word
def ecc_encode(input, bit_flip_mask=0):

    # Determine the 5 ECC check bits
    check_input = \
        (((input >> 11) & 0x1F) << 17) + \
        (((input >>  4) & 0x7F) <<  9) + \
        (((input >>  1) & 0x07) <<  5) + \
        (((input      ) & 0x01) <<  3)
    check_bits = \
        (reduce_xor(check_input & 0x000FFFF0000) << 4) + \
        (reduce_xor(check_input & 0x000FF00FF00) << 3) + \
        (reduce_xor(check_input & 0x0F0F0F0F0F0) << 2) + \
        (reduce_xor(check_input & 0x0CCCCCCCCCC) << 1) + \
        (reduce_xor(check_input & 0x1AAAAAAAAAA)     )

    # Stripe the 5 ECC check bits across the 16-bit word
    encoded = \
        (((input      >> 11) & 0x1F) << 16) + \
        (((check_bits >>  4) & 0x01) << 15) + \
        (((input      >>  4) & 0x7F) <<  8) + \
        (((check_bits >>  3) & 0x01) <<  7) + \
        (((input      >>  1) & 0x07) <<  4) + \
        (((check_bits >>  2) & 0x01) <<  3) + \
        (((input           ) & 0x01) <<  2) + \
        (((check_bits      ) & 0x03)      )

    # Determine whether the 21-bit word needs to set the parity bit 21
    encoded += reduce_xor(encoded) << 21

    # Corrupts specified bits
    encoded ^= bit_flip_mask

    return(encoded)

# Decodes a 22-bit word with 5 ECC bits and a parity bit
# Corrects a single bit filp and returns error code 1
# Detects a double bit flip and returns error code 2
# If no bit flips are detected, returns error code 0
def ecc_decode(encoded):
    error_code = 0

    # Determines syndrome
    shifted_encoded = ((encoded & 0x1fffff) << 1)
    syndrome = \
        (reduce_xor(shifted_encoded & 0x000FFFF0000) << 4) + \
        (reduce_xor(shifted_encoded & 0x000FF00FF00) << 3) + \
        (reduce_xor(shifted_encoded & 0x0F0F0F0F0F0) << 2) + \
        (reduce_xor(shifted_encoded & 0x0CCCCCCCCCC) << 1) + \
        (reduce_xor(shifted_encoded & 0x1AAAAAAAAAA)     )

    # Checks for parity error
    parity = reduce_xor( encoded & 0x1FFFFF )
    if not parity == (encoded >> 21):
        parity_error = True
    else:
        parity_error = False

    # Checks for single bit error
    if syndrome and parity_error:
        error_code = 1
        # corrects any single bit error
        encoded ^= (1 << (syndrome -1))

    # Checks for double bit error
    if syndrome and not parity_error:
        error_code = 2

    # Retrieves data bits
    decoded = \
        (((encoded >> 16) & 0x1F) << 11) + \
        (((encoded >>  8) & 0x7F) <<  4) + \
        (((encoded >>  4) & 0x07) <<  1) + \
        (((encoded >>  2) & 0x01)      )

    return(error_code, decoded)

# Create the argument parser
parser = argparse.ArgumentParser(prog='otpm2readmemh',          description='Convert OTPM image to readmemh files'               )
parser.add_argument('input',                                           help='Input data file (C data array format)'              )
parser.add_argument('-q',   '--quiet',               default=False,    help='Suppress information messages',  action='store_true')
parser.add_argument('-o',   '--output',              required=True,    help='Output directory for readmemh files'                )
parser.add_argument('-b',   '--base',                required=True,    help='Base name for output files'                         )
parser.add_argument('-e',   '--extension',           default='otpm.v', help='Output file extension'                              )
parser.add_argument('-s',   '--size',                required=True,    help='Size of OTPM image',             type=int           )
parser.add_argument('-i',   '--instances',           default=1,        help='Number of OTPM instances',       type=int           )
parser.add_argument('-pv',  '--pad_value',           default='0x0',    help='Pads the image with the given half word value'      )
parser.add_argument('-ma',  '--mask_all',            default=None,     help='Applies the given bit flip 22bit half word mask to '
                                                                            'every location (The mask should be given in hex. '
                                                                            'The maximum value of the mask is 0x3FFFFF)'         )
parser.add_argument('-avf', '--address_value_file',  default=None,     help='File containing values for given addresses'
                                                                            ' (The addresses stated in this file have their '
                                                                            'values changed to the values stated in this file.)' )
parser.add_argument('-bff', '--bit_flip_file',       default=None,     help='File containing bit flip masks for given addresses'
                                                                            ' (The mask is applied to the image with 44bit '
                                                                            'long-words after the ECC bits have been generated.)')

# Parse the command line arguments
args = parser.parse_args()

# Print if not quiet
def print_ifnq(input):
    if not args.quiet:
        print(input)

# Input bit flip file
ADDRESSES_BEGIN = 2 # line at which addresses start
DELIMITER = '|' # delimiter used
corrupt_halfwords = {}
if args.bit_flip_file:
    with open(args.bit_flip_file, 'rt') as istream:
        corrupt_words = istream.readlines()
    # removes header lines
    corrupt_words = corrupt_words[ADDRESSES_BEGIN:]
    # removes white-space and splits elements into lists
    corrupt_words = [''.join(x.split()).split(DELIMITER) for x in corrupt_words]
    # converts corrupt_words into a dictionary of integers
    corrupt_words = dict((int(x[0], 16), int(x[1], 16)) for x in corrupt_words)
    for address in corrupt_words:
        corrupt_halfwords[2*address]    = (  corrupt_words[address]         & 0x3FFFFF)
        corrupt_halfwords[2*address +1] = ( (corrupt_words[address] >> 22 ) & 0x3FFFFF)

# Input address value file
ADDRESSES_BEGIN = 2 # line at which addresses start
DELIMITER = '|' # delimiter used
half_address_values = {}
if args.address_value_file:
    with open(args.address_value_file, 'rt') as istream:
        address_values = istream.readlines()
    # removes header lines
    address_values = address_values[ADDRESSES_BEGIN:]
    # removes white-space and splits elements into lists
    address_values = [''.join(x.split()).split(DELIMITER) for x in address_values]
    # converts address_values into a dictionary of integers
    address_values = dict((int(x[0], 16), int(x[1], 16)) for x in address_values)
    for address in address_values:
        half_address_values[2*address]    = (  address_values[address]         & 0xFFFF )
        half_address_values[2*address +1] = ( (address_values[address] >> 16 ) & 0xFFFF )

# Read the image data from the input file into 16-bit words
with open(args.input, 'rt') as istream:
    image = istream.readlines()
otpm_halfwords = [int(x.strip().rstrip(','), 16) for x in image]
image_size_halfwords = len(image)

# Pad the OTPM image
pad_value = int(args.pad_value, 16)
print_ifnq('Padding image with the value 0x{:04X}'.format(pad_value))
otpm_size_halfwords = args.size//2
pad_range = range(image_size_halfwords, otpm_size_halfwords)
otpm_halfwords = list(otpm_halfwords) + [pad_value for address in pad_range]

# Change half words if value given in address_values_file
for address in range(0, len(otpm_halfwords)):
    if address in half_address_values:
        print_ifnq('Changing value at half address 0x{:04X} to 0x{:04X}'.format(address, half_address_values[address]))
        otpm_halfwords[address] = half_address_values[address]

# Write the non-ECC OTPM data to its model file
filename = os.path.join(args.output, '{}_{}x32.vpm'.format(args.base, otpm_size_halfwords//2, args.extension))
print_ifnq('Writing {}'.format(filename))
with open(filename, 'wt', newline='\n') as ostream:
    ostream.write('0000000000000000\n')
    for address in range(0, otpm_size_halfwords, 2):
        ostream.write('{:04x}{:04x}\n'.format(otpm_halfwords[address+1], otpm_halfwords[address]))

# For each OTPM instance
instance_size_halfwords = otpm_size_halfwords//args.instances
for instance in range(args.instances):
    # Calculate the instance address range
    start_address = instance * instance_size_halfwords
    end_address = start_address + instance_size_halfwords

    # Write the non-ECC OTPM data to its instance file
    filename = os.path.join(args.output, '{}_{}x16_{}.{}'.format(args.base, instance_size_halfwords, instance, args.extension))
    print_ifnq('Writing {}'.format(filename))
    with open(filename, 'wt', newline='\n') as ostream:
        for address in range(start_address, end_address):
            ostream.write('{0:04x}\n'.format(otpm_halfwords[address]))

    # Write the ECC OTPM data to its instance file
    filename = os.path.join(args.output, '{}_{}x22_{}.{}'.format(args.base, instance_size_halfwords, instance, args.extension))
    print_ifnq('Writing {}'.format(filename))
    with open(filename, 'wt', newline='\n') as ostream:
        if args.mask_all:
            mask = int(args.mask_all, 16) & 0x3FFFFF
            print_ifnq('Corrupting all half word locations with the mask: 0x{:06X}'.format(mask))
            for address in range(start_address, end_address):
                if 0 == (address & 1):
                    val = ecc_encode(otpm_halfwords[address], mask)
                else:
                    # invert all the bits
                    val = 0xffffff - ecc_encode(otpm_halfwords[address], mask)
                ostream.write('{0:06x}\n'.format(val))
        else:
            for address in range(start_address, end_address):
                if address in corrupt_halfwords:
                    mask = corrupt_halfwords[address]
                    print_ifnq('Corrupting half word address 0x{:04x} with the mask: 0x{:06X}'.format(address, mask))
                else:
                    mask = 0
                if 0 == (address & 1):
                    val = ecc_encode(otpm_halfwords[address], mask)
                else:
                    # invert all the bits
                    val = 0xffffff - ecc_encode(otpm_halfwords[address], mask)
                ostream.write('{0:06x}\n'.format(val))
