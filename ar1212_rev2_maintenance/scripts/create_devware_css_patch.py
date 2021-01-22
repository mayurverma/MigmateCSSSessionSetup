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
# Generates the DevWare CSS patch INI file
#
########################################################################################
import argparse
import os
import sys

def main():
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='create_devware_css_patch',
        description='Create the DevWare CSS patch INI file')
    parser.add_argument('-i', '--input', required=True, help='Chunks file (*.chunks, output from patch generation flow)')
    parser.add_argument('-r', '--version', required=True, help='Patch version')
    parser.add_argument('-o', '--output', required=True, help='Basename of output file (i.e. without extension) will be basename for INI file')
    parser.add_argument('-v', '--verbose', action="store_true", help='Increase output verbosity')

    # Parse the command line arguments
    args = parser.parse_args()

    if args.verbose:
        print("Command args: input = {}, version = {}, output = {}".format(args.input, args.version, args.output))

    # Build a list of chunks...
    chunks = list()

    # Open the chunks file to get the patch chunk binary file names
    with open(args.input, 'r') as chunks_file:
        for line in chunks_file:
            if args.verbose:
                print("line: ", line.rstrip())

            line = line.rstrip()

            if line.endswith(".bin"):
                # retrieve the chunk from the binary file
                if args.verbose:
                    print("Reading chunk from", line)

                with open(line, 'rb') as bin_file:
                    chunk = bin_file.read()
                    chunks.append(chunk)

    if args.verbose:
        print("Read {} chunks".format(len(chunks)))

    output_str  = "###################################################\n"
    output_str += "# Maintenance patch {} chunks - DO NOT EDIT!\n".format(args.version)
    output_str += "###################################################\n"
    output_str += "\n[--- Maintenance patch {} ---]\n".format(args.version)
    output_str += "\n[Python: get maintenance patch chunks]\n"
    output_str += "class CssMaintenancePatch(object):\n"
    output_str += '    """ Maintenance patch for AR0825AT-REV2 """\n'
    output_str += "    def __init__(self):\n"
    output_str += "        self.__version = {}\n".format(args.version)
    output_str += "        self.__chunks = list()\n\n"

    # Create and write the output file
    if args.verbose:
        print("Writing INI file...")

    with open(args.output, 'w') as ini_file:
        ini_file.write(output_str)

        for idx in range(len(chunks)):
            output_str  = "        # Chunk {}\n".format(idx)
            output_str += "        self.__chunks.append(\n            "
            output_str += str(chunks[idx])
            output_str += "\n        )\n"

            ini_file.write(output_str)

        output_str  = "\n    @property\n"
        output_str += "    def version(self):\n"
        output_str += "        return self.__version\n\n"

        output_str += "    @property\n"
        output_str += "    def chunks(self):\n"
        output_str += "        return self.__chunks\n\n"

        output_str += "css_maintenance_patch = CssMaintenancePatch()\n"

        output_str += "\n### END ###\n"
        ini_file.write(output_str)

    if args.verbose:
        print("Done")

if __name__ == "__main__":
    # Run the main() function
    main()

