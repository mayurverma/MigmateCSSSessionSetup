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
# Generates a set of LoadPatchChunk binary files
#
########################################################################################
import argparse
import os
import sys
from struct import pack

from patch_helper import get_chunks, auto_int, Command, image_to_csv, image_to_string
from process_map import ProcessMap
from crypto_services_client import CryptoServicesClient
from root_of_trust import RootOfTrust

def main():
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='create_patch',
        description='Create LoadPatchChunk command messages')
    parser.add_argument('-i', '--input', required=True, help='Patch file (*.bin, output from patch generation flow)')
    parser.add_argument('-m', '--mapFile', required=True, help='Map file associated with patch')
    parser.add_argument('-o', '--output', required=True, help='Basename of output file (i.e. without extension) will be basename for chunks')
    parser.add_argument('-s', '--chunksize', required=False, type=auto_int, default=(512 - 20), choices=range(1, 251*16), metavar="[0-4016]", help='Maximum size of chunk in words')
    parser.add_argument('-r', '--romVersion', required=True,  type=auto_int, choices=range(0, 0xffff), metavar="[0-0xFFFF]", help='ROM version associated with patch')
    parser.add_argument('-p', '--patchVersion', required=True, type=auto_int, choices=range(1, 0xffff), metavar="[0-0xFFFF]", help='PATCH version')
    parser.add_argument('-k', '--keyStorage', required=False, help='Key storage name')
    parser.add_argument('-n', '--productName', required=True, help='Product name')
    parser.add_argument('-v', '--verbose', action="store_true", help='Increase output verbosity')

    # Parse the command line arguments
    args = parser.parse_args()

    command = []
    chunks = get_chunks(args.romVersion, args.patchVersion, args.input, args.mapFile, args.chunksize)
    total_chunks = len(chunks.chunks)
    if args.keyStorage:
        crypto = CryptoServicesClient(product_name = args.productName, root_of_trust = RootOfTrust.CM, key_set_name = args.keyStorage)
        # Generate encrypted assets
        for x in range(total_chunks):
            chunk_bin = chunks.chunks[x].image()
            asset_id = chunks.chunks[x].asset_id()
            asset = crypto.encrypt_asset(asset_id, chunk_bin)
            cmd = b""
            cmd += pack("<L",asset_id)
            cmd += asset
            command.append(cmd)
    else:
        for x in range(total_chunks):
            chunk_bin = chunks.chunks[x].image()
            asset_id = chunks.chunks[x].asset_id()
            cmd = Command().generate(asset_id, chunk_bin)
            command.append(cmd.image())

    # Create manifest file to capture all files created to support patching
    manifest_filename = "{}.chunks".format(args.output)
    print("Creating manifest file {}".format(manifest_filename))
    with open(manifest_filename, 'w') as manifest:
        for x in range(total_chunks):
            # print(assets.assets[x])
            print("\nLoadPatchChunk[{}]".format(x))
            print("Length (Bytes)      :  {}".format(len(command[x])))
            if args.verbose:
                print(chunks.chunks[x])
                print("Command:")
                print(image_to_string(command[x]))
            filename = "{}_c{:02X}.bin".format(args.output, x)
            manifest.write("{}\n".format(filename))
            print("Creating file {}".format(filename))
            with open(filename, 'wb') as ostream:
                ostream.write(command[x])
            filename = "{}_c{:02X}.csv".format(args.output, x)
            manifest.write("{}\n".format(filename))
            print("Creating file {}".format(filename))
            with open(filename, 'w') as ostream:
                ostream.write(image_to_csv(command[x]))

    processed = ProcessMap(args.mapFile)

    size = processed.patch_size()
    max_size = processed.patch_max_size()
    print("\n ------- Total Patch size (CODE + DATA): {}({}%) bytes of {} bytes available  in Patch RAM  ----\n".format(size, (size*100 // max_size), max_size))

if __name__ == "__main__":
    # Run the main() function
    main()

