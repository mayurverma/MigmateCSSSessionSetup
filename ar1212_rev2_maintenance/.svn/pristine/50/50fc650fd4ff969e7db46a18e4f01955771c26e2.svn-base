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
    def __init__(self, test_access_timeout, uvm_timeout, coverage):
        self.__test_access_timeout = test_access_timeout
        self.__uvm_timeout = uvm_timeout
        self.__coverage = coverage

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

    def parse(self, filename, worksheet, fixture, labels):
        # Parse the wookbork
        workbook = xlrd.open_workbook(filename)

        # Find the worksheets
        constants_worksheet = workbook.sheet_by_name('Constants')
        tests_worksheet = workbook.sheet_by_name(worksheet)

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
                if (len(labels) == 0 or filters[fixture] in labels) and len(otpm) > 0:
                    # Add test to the list
                    if module not in tests:
                        tests[module] = {}
                    if klass not in tests[module]:
                        tests[module][klass] = {}
                    if method not in tests[module][klass]:
                        sim_args = 'TESTLIST={}.{}.{} OTPMIMAGE={} TEST_ACCESS_TIMEOUT={} UVMTIMEOUT={}'.format(
                            module, klass, method, otpm, self.__test_access_timeout, self.__uvm_timeout)
                        if self.__coverage:
                            sim_args += ' COVERAGE=1'
                        tests[module][klass][method] = {'sim_args' : '"{}"'.format(sim_args)}
                    else:
                        raise TestScheduleParserError('ERROR: Row {} duplicate test specifier "{}"'.format(row+1, specifier))

        return tests

class VsifWriter:
    """VManager Session Input File (VSIF) writer."""

    def _generate_attributes(self, formatter, attributes):
        content = ''
        for key in sorted(attributes.keys()):
            content += formatter.format(key, attributes[key])
        return content

    def _generate_content(self, session, suite):
        content = 'session {} {{\n'.format(session['name'])
        content += self._generate_attributes('    {} : {};\n', session['attributes'])
        content += '};\n'
        content += '\n'
        content += 'group {} {{\n'.format(suite['name'])
        if 'attributes' in suite:
            content += self._generate_attributes('  {} : {};\n', suite['attributes'])
            content += '\n'
        tests = suite['tests']
        for module in sorted(tests.keys()):
            content += '  group {} {{\n'.format(module)
            for klass in sorted(tests[module].keys()):
                content += '    group {} {{\n'.format(klass)
                for method in sorted(tests[module][klass].keys()):
                    content += '      test {} {{\n'.format(method)
                    content += self._generate_attributes('        {} : {};\n', tests[module][klass][method])
                    content += '      }\n'
                content += '    }\n'
            content += '  }\n'
        content += '};\n'

        return content

    def write(self, session, tests):
        print(self._generate_content(session, tests))

class CampaignWriter:
    """XML campaign format writer."""

    def _generate_attributes(self, formatter, attributes):
        content = '\t<properties>\n'
        for key in sorted(attributes.keys()):
            content += '\t\t<property name="{}" type="{}">\n'.format(key, "list")
            content += '\t\t\t<value>{}</value>\n'.format(attributes[key])
            content += '\t\t</property>\n'.format(key)
        content += '\t</properties>\n'
        return content

    def _generate_content(self, session, suite):
        content = '<?xml version="1.0" encoding="UTF-8"?>\n'
        content += '<campaign name="{}">\n'.format(session['name'])
        content += self._generate_attributes('    {} : {};\n', session['attributes'])
        if 'attributes' in suite:
            content += self._generate_attributes('  {} : {};\n', suite['attributes'])
        tests = suite['tests']
        content += '\t<suite>\n'
        for module in sorted(tests.keys()):
            content += '\t\t<module name="{}">\n'.format(module)
            for klass in sorted(tests[module].keys()):
                content += '\t\t\t<class name="{}">\n'.format(klass)
                for method in sorted(tests[module][klass].keys()):
                    content += '\t\t\t\t<test name="{}"/>\n'.format(method)
                content += '\t\t\t</class>\n'
            content += '\t\t</module>\n'
        content += '\t</suite>\n'
        content += '</campaign>\n'
        return content

    def write(self, session, tests):
        print(self._generate_content(session, tests))

if __name__ == "__main__":
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='test_schedule',
        description='Converts test schedule from Excel to vManager Session Input File format')
    parser.add_argument('-c', '--coverage', action="store_true", help='Enable function coverage logging')
    parser.add_argument('-f', '--filter', default='All', choices=['Sanity', 'Nightly', 'Weekly'], help='Test label filter')
    parser.add_argument('-n', '--name', help='Session name')
    parser.add_argument('-t', '--test_access_timeout', default=300, help='Test access server timeout (seconds)')
    parser.add_argument('-u', '--uvm_timeout', default=30000000000, help='UVM timeout (nanoseconds)')
    parser.add_argument('-w', '--worksheet', default='Trunk', help='Worksheet containing test catalogue')
    parser.add_argument('workbook', help='Test catalogue Excel spreadsheet')
    parser.add_argument('fixture', choices=['model', 'sim', 'migmate', 'devware'], help='Test fixture')
    parser.add_argument('sessions', help='Sessions root directory')
    parser.add_argument('fwroot', help='Firmware root directory')

    # Parse the command line arguments
    args = parser.parse_args()

    # Create the session description
    if args.name is None:
        title = 'firmware_{}_{}'.format(args.fixture, args.filter).lower()
    else:
        title = args.name
    session = { \
        'name' : title,
        'attributes' : { \
            'top_dir' : args.sessions,
            'output_mode' : 'log_only',
            #'drm' : 'lsf',
            'create_debug_logs' : 'TRUE',			
            'drm' : 'serial local',
            'post_session_script' : '"{}/test/scripts/vmanager_post_session_cleanup.sh"'.format(args.fwroot)
        }
    }
    if args.fixture == 'model':
        # There are 6 pairs of licences with 1 dedicated to running Jenkins sessions
        session['attributes']['max_runs_in_parallel'] = '1'
        if args.coverage:
            session['attributes']['post_session_script'] = '"{}/test/scripts/vmanager_post_session_coverage.sh"'.format(args.fwroot)

    # Create the suite description
    suite = {
        'name' : '{}_{}'.format(args.fixture, args.filter).lower(),
        'attributes' : { \
            'sv_seed' : 'random',
            'timeout' : '0',
        }
    }
    if args.fixture == 'sim':
        suite['attributes']['run_script'] = '"{}/test/scripts/vmanager_run_sim.sh"'.format(args.fwroot)
        suite['attributes']['scan_script'] = '"vm_scan.pl {}/test/scripts/vmanager_scan.flt"'.format(args.fwroot)
        suite['attributes']['runs_dispatch_parameters'] = '-q incsim'
    if args.fixture == 'model':
        suite['attributes']['run_script'] = '"{}/test/scripts/vmanager_run_model.sh"'.format(args.fwroot)
        suite['attributes']['scan_script'] = '"vm_scan.pl {}/test/scripts/vmanager_scan.flt"'.format(args.fwroot)

    # Create the label matcher filter
    if args.filter == 'All':
        labels = ()
    elif args.filter == 'Weekly':
        # Weekly includes : Weekly, Nightly and Sanity
        labels = ('Weekly', 'Nightly', 'Sanity')
    elif args.filter == 'Nightly':
        # Nightly includes : Nightly and Sanity
        labels = ('Nightly', 'Sanity')
    else:
        labels = (args.filter, )

    # Parse the test schedule
    schedule = TestScheduleParser(args.test_access_timeout, args.uvm_timeout, args.coverage)
    try:
        suite['tests'] = schedule.parse(args.workbook, args.worksheet, args.fixture, labels)
        if args.fixture != 'devware':
            # Write the VSIF file to stdout
            writer = VsifWriter()
        else:
            writer = CampaignWriter()
        writer.write(session, suite)
    except TestScheduleParserError as err:
        print(err, file=sys.stderr)
        sys.exit(1)
