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
# Converts ROM binary image to IEEE Verilog readmemh format.
#
# The script outputs the ROM image with and without ECC protection. The ROM image is
# split into a number of ROM instances based on the specified options.
#
########################################################################################
import argparse
import os
import struct
import sys

# Returns true if bit in word is set
def bit_is_set(word, bit):
    return 0 != (word & (1 << bit))

# Encodes a 32-bit data word with ECC & parity protection and returns a 39-bit word
def ecc_encode(word):

    # Determine the 6 ECC check bits
    check_bits = 0
    if (bit_is_set(word, 31)): check_bits ^= 38
    if (bit_is_set(word, 30)): check_bits ^= 37
    if (bit_is_set(word, 29)): check_bits ^= 36
    if (bit_is_set(word, 28)): check_bits ^= 35
    if (bit_is_set(word, 27)): check_bits ^= 34
    if (bit_is_set(word, 26)): check_bits ^= 33
    if (bit_is_set(word, 25)): check_bits ^= 31
    if (bit_is_set(word, 24)): check_bits ^= 30
    if (bit_is_set(word, 23)): check_bits ^= 29
    if (bit_is_set(word, 22)): check_bits ^= 28
    if (bit_is_set(word, 21)): check_bits ^= 27
    if (bit_is_set(word, 20)): check_bits ^= 26
    if (bit_is_set(word, 19)): check_bits ^= 25
    if (bit_is_set(word, 18)): check_bits ^= 24
    if (bit_is_set(word, 17)): check_bits ^= 23
    if (bit_is_set(word, 16)): check_bits ^= 22
    if (bit_is_set(word, 15)): check_bits ^= 21
    if (bit_is_set(word, 14)): check_bits ^= 20
    if (bit_is_set(word, 13)): check_bits ^= 19
    if (bit_is_set(word, 12)): check_bits ^= 18
    if (bit_is_set(word, 11)): check_bits ^= 17
    if (bit_is_set(word, 10)): check_bits ^= 15
    if (bit_is_set(word,  9)): check_bits ^= 14
    if (bit_is_set(word,  8)): check_bits ^= 13
    if (bit_is_set(word,  7)): check_bits ^= 12
    if (bit_is_set(word,  6)): check_bits ^= 11
    if (bit_is_set(word,  5)): check_bits ^= 10
    if (bit_is_set(word,  4)): check_bits ^=  9
    if (bit_is_set(word,  3)): check_bits ^=  7
    if (bit_is_set(word,  2)): check_bits ^=  6
    if (bit_is_set(word,  1)): check_bits ^=  5
    if (bit_is_set(word,  0)): check_bits ^=  3

    # Stripe the 6 ECC check bits across the 32-bit word
    encoded = \
        (((word       >> 26) & 0x3F)   << 32) + \
        (((check_bits >>  5) & 0x01)   << 31) + \
        (((word       >> 11) & 0x7FFF) << 16) + \
        (((check_bits >>  4) & 0x01)   << 15) + \
        (((word       >>  4) & 0x7F)   << 8) + \
        (((check_bits >>  3) & 0x01)   << 7) + \
        (((word       >>  1) & 0x07)   << 4) + \
        (((check_bits >>  2) & 0x01)   << 3) + \
        (((word            ) & 0x01)   << 2) + \
        (((check_bits      ) & 0x03)       )

    # Determine whether the 38-bit word needs to set the parity bit 38
    parity = 0
    for bit in range(38):
        if bit_is_set(encoded, bit):
            parity ^= 1
    encoded += parity << 38

    return(encoded)

# Create the argument parser
parser = argparse.ArgumentParser(prog='bin2readmemh',
    description='Convert ROM image to readmemh files')
parser.add_argument('-q', '--quiet', default=False, action='store_true', help='Suppress information messages')
parser.add_argument('-o', '--output', required=True, help='Output directory for C header files')
parser.add_argument('-b', '--base', required=True, help='Base name for Verilog output files')
parser.add_argument('-v', '--vpm_base', required=True, help='Base name for VPM output files')
parser.add_argument('-e', '--extension', default='rom.v', help='Output file extension')
parser.add_argument('-s', '--size', type=int, required=True, help='Size of ROM image')
parser.add_argument('-i', '--instances', type=int, default=1, help='Number of ROM instances')
parser.add_argument('-p', '--pad', choices=['zero', 'address'], default='address', help='ROM image padding data')
parser.add_argument('input', help='Input bin file')

# Parse the command line arguments
args = parser.parse_args()

# Read the image data from the binary input file
with open(args.input, 'rb') as istream:
    image = istream.read()

# Convert little endian bytes to 32-bit words
image_size_words = len(image)//4
rom_words = struct.Struct('<{}L'.format(image_size_words)).unpack(image)

# Pad the ROM image
rom_size_words = args.size//4
pad_range = range(image_size_words, rom_size_words)
if args.pad == 'zero':
    # Pad with data = 0
    rom_words = list(rom_words) + [0 for address in pad_range]
else:
    # Pad with data = address
    rom_words = list(rom_words) + [address*4 for address in pad_range]

# Allocate storage for the instance filenames
vpm_filenames = []
rom_filenames = []

# For each ROM instance
instance_size_words = rom_size_words//args.instances
for instance in range(args.instances):
    # Calculate the instance address range
    start_address = instance * instance_size_words
    end_address = start_address + instance_size_words

    # Write the non-ECC ROM data to its instance file
    filename = '{}/{}_{}x32_{}.vpm'.format(args.output, args.vpm_base, instance_size_words, instance)
    vpm_filenames.append(filename)
    if not args.quiet:
        print('Writing {}'.format(filename))
    with open(filename, 'wt', newline='\n') as ostream:
        # Virtual prototype memory file needs page address within the instance
        ostream.write('0000000000000000\n')
        for address in range(start_address, end_address):
            ostream.write('{0:08x}\n'.format(rom_words[address]))

    # Write the ECC ROM data to its instance file
    filename = '{}/{}_{}x39_{}.{}'.format(args.output, args.base, instance_size_words, instance, args.extension)
    rom_filenames.append(filename)
    if not args.quiet:
        print('Writing {}'.format(filename))
    with open(filename, 'wt', newline='\n') as ostream:
        for address in range(start_address, end_address):
            ostream.write('{0:010x}\n'.format(ecc_encode(rom_words[address])))

# Write the VPM instance filenames to the VPM manifest file
filename = '{}/{}.vpm.instances'.format(args.output, args.vpm_base)
if not args.quiet:
    print('Writing {}'.format(filename))
with open(filename, 'wt', newline='\n') as ostream:
    for filename in vpm_filenames:
        ostream.write('{}\n'.format(filename))

# Write the ROM instance filenames to the ROM manifest file
filename = '{}/{}.rom.v.instances'.format(args.output, args.base)
if not args.quiet:
    print('Writing {}'.format(filename))
with open(filename, 'wt', newline='\n') as ostream:
    for filename in rom_filenames:
        ostream.write('{}\n'.format(filename))
