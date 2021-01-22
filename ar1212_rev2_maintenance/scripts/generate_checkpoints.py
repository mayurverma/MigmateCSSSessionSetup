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
# Generates Assembler, C and Python checkpoint header files from a master CSV file
#
########################################################################################
import argparse
import csv
import os
import sys

class CheckpointsGenerator():
    """Checkpoints header generator class
    """

    ASM_HEADER = \
        ';***********************************************************************************\n' \
        '; Copyright 2018 ON Semiconductor.  All rights reserved.\n' \
        ';\n' \
        '; This software and/or documentation is licensed by ON Semiconductor under limited\n' \
        '; terms and conditions. The terms and conditions pertaining to the software and/or\n' \
        '; documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf\n' \
        '; ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").\n' \
        '; Do not use this software and/or documentation unless you have carefully read and\n' \
        '; you agree to the limited terms and conditions. By using this software and/or\n' \
        '; documentation, you agree to the limited terms and conditions.\n' \
        ';***********************************************************************************\n' \
        '\n' \
        ';!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '; DO NOT EDIT - This file is auto-generated by the generate_checkpoints.py script\n' \
        ';!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '\n' \
        ';-------------------------------------------------------------------------------\n' \
        ';                                  Checkpoints\n' \
        ';-------------------------------------------------------------------------------\n'


    ASM_FOOTER = \
        '\n' \
        '                END\n'

    C_HEADER = \
        '#ifndef DIAG_CHECKPOINT_H\n' \
        '#define DIAG_CHECKPOINT_H\n' \
        '//***********************************************************************************\n' \
        '/// \\file\n' \
        '///\n' \
        '/// Diagnostic checkpoints.\n' \
        '///\n' \
        '//***********************************************************************************\n' \
        '// Copyright 2018 ON Semiconductor.  All rights reserved.\n' \
        '//\n' \
        '// This software and/or documentation is licensed by ON Semiconductor under limited\n' \
        '// terms and conditions. The terms and conditions pertaining to the software and/or\n' \
        '// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf\n' \
        '// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").\n' \
        '// Do not use this software and/or documentation unless you have carefully read and\n' \
        '// you agree to the limited terms and conditions. By using this software and/or\n' \
        '// documentation, you agree to the limited terms and conditions.\n' \
        '//***********************************************************************************\n' \
        '\n' \
        '// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '// DO NOT EDIT - This file is auto-generated by the generate_checkpoints.py script\n' \
        '// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '\n' \
        '//------------------------------------------------------------------------------\n' \
        '//                                 Checkpoints\n' \
        '//------------------------------------------------------------------------------\n' \
        '\n' \
        '// *INDENT-OFF*\n' \
        '\n' \
        '/// Host command codes\n' \
        'typedef enum\n' \
        '{'

    C_FOOTER = \
        '} DiagCheckpointT;\n' \
        '\n' \
        '#endif /* DIAG_CHECKPOINT_H */\n' \
        '\n'

    PYTHON_HEADER = \
        '####################################################################################\n' \
        '# Copyright 2018 ON Semiconductor.  All rights reserved.\n' \
        '#\n' \
        '# This software and/or documentation is licensed by ON Semiconductor under limited\n' \
        '# terms and conditions. The terms and conditions pertaining to the software and/or\n' \
        '# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf\n' \
        '# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").\n' \
        '# Do not use this software and/or documentation unless you have carefully read and\n' \
        '# you agree to the limited terms and conditions. By using this software and/or\n' \
        '# documentation, you agree to the limited terms and conditions.\n' \
        '#\n' \
        '####################################################################################\n' \
        '\n' \
        '#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '# DO NOT EDIT - This file is auto-generated by the generate_checkpoints.py script\n' \
        '#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '\n' \
        'from enum import IntEnum, unique\n' \
        '\n' \
        '@unique\n' \
        'class DiagCheckpoint(IntEnum):\n' \
        '    """Diagnostics checkpoint enumeration."""\n'

    PYTHON_FOOTER = \
        '\n' \
        '    def __str__(self):\n' \
        '        return \'0x{:04X}:{}\'.format(self.value, self.name)\n' \
        '\n'

    def __init__(self, csv_filename, prefix, debug=False):
        self._csv_filename = csv_filename
        self._prefix = prefix
        self._debug = debug

    def validate(self):
        """Checks for duplicate checkpoint values
        """
        if self._debug: print("Validating checkpoints...")
        found_checkpoints = list()

        with open(self._csv_filename, newline='') as csvfile:
            checkpoints = csv.DictReader(csvfile)
            for row in checkpoints:
                if "" != row['Name'].strip():
                    checkpoint_name = row['Name'].strip()
                    checkpoint = int(row['Value'].strip())

                    if self._debug: print("Evaluating checkpoint: {}: {}".format(checkpoint_name, checkpoint))

                    # Look for a duplicate
                    for (existing_checkpoint, existing_checkpoint_name) in found_checkpoints:
                        if existing_checkpoint == checkpoint:
                            raise Exception("Duplicate checkpoint found: {} duplicates {}\n".format(checkpoint_name, existing_checkpoint_name))

                    found_checkpoints.append((checkpoint, checkpoint_name))

        if self._debug: print("All checkpoints valid\n")

    def generate_asm(self, filename):
        """Generates the Assembler .inc header file.
        """
        if self._debug:
            print("Generating assembler checkpoints...")

        # Build the ASM checkpoint content
        content = self.ASM_HEADER

        with open(self._csv_filename, newline='') as csvfile:
            checkpoints = csv.DictReader(csvfile)
            next_checkpoint = 1
            for row in checkpoints:
                if "" != row['Name'].strip():
                    if int(row['Value'].strip()) != next_checkpoint:
                        content += '\n'
                        next_checkpoint = int(row['Value'].strip()) + 1
                    else:
                        next_checkpoint += 1
                    content += "{}_{:45s} EQU {:5s} ; (0x{:04X}) {}\n".format(self._prefix, \
                        row['Name'].strip(), row['Value'].strip(), int(row['Value'].strip()), row['Comment'].strip())

        content += self.ASM_FOOTER

        # write the ASM checkpoints content
        if self._debug: print("Writing {}...".format(filename))
        with open(filename, "wt", newline='\n') as ostream:
            ostream.write(content)

    def generate_c(self, filename):
        """Generates the C .h header file.
        """
        if self._debug:
            print("Generating C checkpoints...")

        # Build the C checkpoint content
        content = self.C_HEADER

        with open(self._csv_filename, newline='') as csvfile:
            checkpoints = csv.DictReader(csvfile)
            next_checkpoint = 1
            for row in checkpoints:
                if "" != row['Name'].strip():
                    if int(row['Value'].strip()) != next_checkpoint:
                        content += '\n'
                        next_checkpoint = int(row['Value'].strip()) + 1
                    else:
                        next_checkpoint += 1
                    nameValue = '{}_{} = {},'.format(self._prefix, row['Name'].strip(), row['Value'].strip())
                    content += "    {:67s} ///< (0x{:04X}) {}\n".format(\
                        nameValue, int(row['Value'].strip()), row['Comment'].strip())

        content += self.C_FOOTER

        # write the C checkpoints content
        if self._debug: print("Writing {}...".format(filename))
        with open(filename, "wt", newline='\n') as ostream:
            ostream.write(content)

    def generate_python(self, filename):
        """Generates the Python .py header file.
        """
        if self._debug:
            print("Generating Python checkpoints...")

        # Build the Python checkpoint content
        content = self.PYTHON_HEADER

        with open(self._csv_filename, newline='') as csvfile:
            checkpoints = csv.DictReader(csvfile)
            next_checkpoint = 1
            for row in checkpoints:
                if "" != row['Name'].strip():
                    if int(row['Value'].strip()) != next_checkpoint:
                        content += '\n'
                        next_checkpoint = int(row['Value'].strip()) + 1
                    else:
                        next_checkpoint += 1
                    nameValue = '    {} = {}'.format(row['Name'].strip(), row['Value'].strip())
                    content += "{:70s}  # (0x{:04X}) {}\n".format(\
                        nameValue, int(row['Value'].strip()), row['Comment'].strip())

        content += self.PYTHON_FOOTER

        # write the Python checkpoints content
        if self._debug: print("Writing {}...".format(filename))
        with open(filename, "wt", newline='\n') as ostream:
            ostream.write(content)

# Create the argument parser
parser = argparse.ArgumentParser(prog='generate_checkpoints',
    description='Generate Assembler, C and Python header files for Diagnostic checkpoints')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable debug output')
parser.add_argument('-c', '--csvfile', required=True, help='Checkpoints master CSV file')
parser.add_argument('-o', '--output', required=True, help='Output directory for ASM and C header files')
parser.add_argument('-s', '--scripts', required=True, help='Output directory for Python scripts')
parser.add_argument('-v', '--validate', required=False, action='store_true', help='Validate checkpoints')

# Parse the command line arguments
args = parser.parse_args()

generator = CheckpointsGenerator(args.csvfile, 'DIAG_CHECKPOINT', args.debug)
if args.validate:
    generator.validate()

# Generate ASM, C and Python checkpoint header files
print('Generating checkpoints...')
generator.generate_asm(os.path.join(args.output, 'DiagCheckpoint.inc'))
generator.generate_c(os.path.join(args.output, 'DiagCheckpoint.h'))
generator.generate_python(os.path.join(args.scripts, 'diag_checkpoint.py'))