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
# Generates C and Python System Manager phase header files from a master CSV file
#
########################################################################################
import argparse
import csv
import os
import sys

class SystemMgrPhaseGenerator():
    """System Manager phase header generator class
    """

    C_HEADER = \
        '#if !defined(SRC__SYSTEMMGR__INC__SYSTEMMGRPHASE_H)\n' \
        '#define      SRC__SYSTEMMGR__INC__SYSTEMMGRPHASE_H\n' \
        '//***********************************************************************************\n' \
        '/// \\file\n' \
        '/// System Manager public API - phases\n' \
        '///\n' \
        '/// \\addtogroup systemmgr   System Manager\n' \
        '/// @{\n' \
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
        '// DO NOT EDIT - This file is auto-generated by the generate_system_phases.py script\n' \
        '// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '\n' \
        '//------------------------------------------------------------------------------\n' \
        '//                             System Manager Phases\n' \
        '//------------------------------------------------------------------------------\n' \
        '\n' \
        '// *INDENT-OFF*\n' \
        '\n' \
        '/// System phases\n' \
        'typedef enum\n' \
        '{\n'

    C_FOOTER = \
        '} SystemMgrPhaseT;\n' \
        '\n' \
        '/// @} endgroup systemmgr\n' \
        '#endif  // !defined(SRC__SYSTEMMGR__INC__SYSTEMMGRPHASE_H)\n' \
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
        '# DO NOT EDIT - This file is auto-generated by the generate_system_phases.py script\n' \
        '#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n' \
        '\n' \
        'from enum import IntEnum, unique\n' \
        '\n' \
        '@unique\n' \
        'class SystemMgrPhase(IntEnum):\n' \
        '    """System Manager phases enumeration."""\n'

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
        """Checks for duplicate system phase values
        """
        if self._debug: print("Validating system phases...")
        found_entries = list()

        with open(self._csv_filename, newline='') as csvfile:
            entries = csv.DictReader(csvfile)
            for row in entries:
                if "" != row['Name'].strip():
                    phase_name = row['Name'].strip()
                    phase_value = int(row['Value'].strip(), 16)

                    if self._debug: print("Evaluating system phase: {}: {}".format(phase_name, phase_value))

                    # Look for a duplicate
                    for (existing_phase_value, existing_phase_name) in found_entries:
                        if existing_phase_value == phase_value:
                            raise Exception("Duplicate system phase found: {} duplicates {}\n".format(phase_name, existing_phase_name))

                    found_entries.append((phase_value, phase_name))

        if self._debug: print("All system phases valid\n")

    def generate_c(self, filename):
        """Generates the C .h header file.
        """
        if self._debug:
            print("Generating C system phases...")

        # Build the C system phases content
        content = self.C_HEADER

        with open(self._csv_filename, newline='') as csvfile:
            entries = csv.DictReader(csvfile)
            next_phase = 0
            for row in entries:
                if "" != row['Name'].strip():
                    if int(row['Value'].strip()) != next_phase:
                        content += '\n'
                        next_phase = int(row['Value'].strip()) + 1
                    else:
                        next_phase += 1
                    nameValue = '{}_{} = 0x{:02X}U,'.format(self._prefix, row['Name'].strip(), int(row['Value'].strip()))
                    content += "    {:40s} ///< {}\n".format(nameValue, row['Comment'].strip())

        content += self.C_FOOTER

        # write the C system phases content
        if self._debug: print("Writing {}...".format(filename))
        with open(filename, "wt", newline='\n') as ostream:
            ostream.write(content)

    def generate_python(self, filename):
        """Generates the Python .py header file.
        """
        if self._debug:
            print("Generating Python system phases...")

        # Build the Python system phases content
        content = self.PYTHON_HEADER

        with open(self._csv_filename, newline='') as csvfile:
            entries = csv.DictReader(csvfile)
            next_phase = 0
            for row in entries:
                if "" != row['Name'].strip():
                    if int(row['Value'].strip()) != next_phase:
                        content += '\n'
                        next_phase = int(row['Value'].strip()) + 1
                    else:
                        next_phase += 1
                    nameValue = '    {} = {}'.format(row['Name'].strip(), row['Value'].strip())
                    content += "{:20s}  # {}\n".format(nameValue, row['Comment'].strip())

        content += self.PYTHON_FOOTER

        # write the Python system phases content
        if self._debug: print("Writing {}...".format(filename))
        with open(filename, "wt", newline='\n') as ostream:
            ostream.write(content)

# Create the argument parser
parser = argparse.ArgumentParser(prog='generate_system_phases',
    description='Generate C and Python header files for System Manager phases')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable debug output')
parser.add_argument('-c', '--csvfile', required=True, help='System Manager phases master CSV file')
parser.add_argument('-o', '--output', required=True, help='Output directory for ASM and C header files')
parser.add_argument('-s', '--scripts', required=True, help='Output directory for Python scripts')
parser.add_argument('-v', '--validate', required=False, action='store_true', help='Validate system phases')

# Parse the command line arguments
args = parser.parse_args()

generator = SystemMgrPhaseGenerator(args.csvfile, 'SYSTEM_MGR_PHASE', args.debug)
if args.validate:
    generator.validate()

# Generate C and Python system phases header files
print('Generating system phases...')
generator.generate_c(os.path.join(args.output, 'SystemMgrPhase.h'))
generator.generate_python(os.path.join(args.scripts, 'systemmgr_phase.py'))
