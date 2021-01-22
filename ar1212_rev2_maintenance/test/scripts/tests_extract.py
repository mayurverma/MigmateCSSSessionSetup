####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Converts test schedule from Excel to vManager Session Input File format.
####################################################################################
import argparse
import os
import sys

from enum import Enum
from operator import itemgetter

# work out the project and working copy root directories
__projroot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

# prepend library paths to the search path
sys.path.append(os.path.join(__projroot, 'contrib', 'xlrd'))

import xlrd

class TestScheduleParserError(Exception):
    pass

class TestScheduleParser:
    """Parser that reads the test catalogue from Excel spreadsheet."""
    def __init__(self):
        pass

    def _parse_enums(self, worksheet):
        """Parses enumerations and ensures members are unique with no blanks."""
        for col in range(0, worksheet.ncols):
            # First row holds the enumeration name
            name = worksheet.cell_value(0, col)
            values = worksheet.col_values(col, 1)
            members = []
            for text in values:
                # Ignore blanks
                if 0 < len(text):
                    # Check for duplicate members
                    if text in members:
                        raise TestScheduleParserError('ERROR: Enumeration {} has duplicate values "{}"'.format(name, text))
                    else:
                        members.append(text)
            # Create the enumeration and save it as an object attribute
            setattr(self, name, Enum(name, members))
        
    def parse(self, filename, fixture, label, test_class):
        # Parse the wookbork
        workbook = xlrd.open_workbook(filename)

        # Find the worksheets
        constants_worksheet = workbook.sheet_by_name('Constants')
        tests_worksheet = workbook.sheet_by_name('Tests')
        
        # Parse the constant enumerations
        self._parse_enums(constants_worksheet)
                
        # Read the test entries
        tests = {}
        for row in range(tests_worksheet.nrows):
            # Type (Group, Subgroup, Test), ID, Description, Test Specifier, OTPM, Model, Simulator, Migmate, Devware
            type, id, description, specifier, otpm, model, sim, migmate, devware = tests_worksheet.row_values(row, 0, 9)
            # Ignore anything that is not a test
            if type == self.Type.Test.name and 0 < len(specifier):
                try:
                    module, klass, method = specifier.split('.', 3)
                except:
                    raise TestScheduleParserError('ERROR: Row {} invalid test specifier "{}"'.format(row + 1, specifier))
                # Ignore tests that do not match the filter
                filters = {'model' : model, 'sim': sim, 'migmate' : migmate, 'devware' : devware}
                if (('ALL' == label and ('ALL' == test_class or klass == test_class)) or
                    (filters[fixture] == label and ('ALL' == test_class or klass == test_class)) and
                    len(otpm) > 0):
                    # Add test to the list
                    if module not in tests:
                        tests[module] = {}
                    if klass not in tests[module]:
                        tests[module][klass] = {}
                    if method not in tests[module][klass]:
                        tests[module][klass][method] = '{}'.format(otpm)
                    else:
                        raise TestScheduleParserError('ERROR: Row {} duplicate test specifier "{}"'.format(row+1, specifier))
                        
        return tests

class Writer:
    """Generic writer for test list"""

    def _generate_content(self, suite):
        content = 'TEST_NAMES=""\n'

        tests = suite['tests']
        for module in sorted(tests.keys()):
            for klass in sorted(tests[module].keys()):
                for method in sorted(tests[module][klass].keys()):
                    content += 'TEST_NAMES+="{}:{} "\n'.format(tests[module][klass][method],method)

        return content
    
    def write(self, tests):
        print(self._generate_content(tests))

if __name__ == "__main__":
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='tests_extract',
        description='Converts test schedule from Excel to simple lisa as used in our shell scripts')
    parser.add_argument('-s', '--schedule', default='ALL', choices=['Sanity', 'Nightly', 'Weekly'], help='Test schedule filter')
    parser.add_argument('-c', '--test_class', default='ALL', help='Test class filter')
    parser.add_argument('input', help='Test catalogue Excel spreadsheet')
    parser.add_argument('fixture', choices=['model', 'sim', 'migmate', 'devware'], help='Test fixture')

    # Parse the command line arguments
    args = parser.parse_args()
       
    # Create the suite description
    suite = {
        'name' : '{}_{}_{}'.format(args.fixture, args.schedule, args.test_class).upper(),
    }
    
    # Parse the test schedule
    schedule = TestScheduleParser()
    try:
        suite['tests'] = schedule.parse(args.input, args.fixture, args.schedule, args.test_class)
        # Write degfault format to stdout
        writer = Writer()
        writer.write(suite)
    except TestScheduleParserError as err:
        print(err, file=sys.stderr)
        sys.exit(1)
