#######################################################################################
# Copyright 2017 ON Semiconductor.  All rights reserved.
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
# Generates a style report for a list of source files.
#
# For each file in the source list, the requested style checker program(s) are invoked
# to generate a style corrected file which is then compared with the original file
# using the diff program. The diff program output is parsed into a list of style
# violations where each violation has a starting line number, the original text lines
# and the corrected text lines. The violations are then dumped as a checkstyle XML
# report for parsing by a CI server job.
#
########################################################################################
import argparse
import json
import re
import shutil
import subprocess
import sys
import xml.dom.minidom

# Exception class thrown by diff parser
class DiffParserError(Exception):
    pass

# Abstract style check launcher class
class IStyleCheckLauncher():
    def __init__(self, program, options, debug=False):
        self.program = program
        self.options = options
        self.debug = debug

    def execute(self, filename):
        raise NotImplementedError()
        
# Style check launcher for Artistic Style program
class AStyleLauncher(IStyleCheckLauncher):
    def __init__(self, program, options, debug=False):
        super(AStyleLauncher, self).__init__(program, options, debug)

    def execute(self, filename):
        args = [self.program, '-q', '-n',
            '--project=none',
            '--options={}'.format(self.options),
            filename]
        if self.debug:
            print('Launcher args : {}'.format(args))
        subprocess.check_call(args)

# Style check launcher for Uncrustify program
class UncrustifyLauncher(IStyleCheckLauncher):
    def __init__(self, program, options, debug=False):
        super(UncrustifyLauncher, self).__init__(program, options, debug)

    def execute(self, filename):
        args = [self.program, '-q', '--no-backup',
            '-l', 'C',
            '-c', self.options,
            filename]
        if self.debug:
            print('Launcher args : {}'.format(args))
        subprocess.check_call(args)

# Style checker class
class StyleChecker():
    def __init__(self, launchers, tempdir, debug=False):
        # Save the constructor arguments
        self.launchers = launchers
        self.output_filename = tempdir + '/style_check_output.tmp'
        self.diff_filename = tempdir + '/style_check_diff.tmp'
        self.debug = debug
        # Create a blank report
        self.report = {'violations' : []}

    # Parse the output of the diff command into a list of text hunks each of
    # which consists of a starting line number, the original text lines and
    # the corrected text lines.
    def parse_diff_output(self, filename):
        # Read the diff command output from the file
        with open(filename, 'r') as infile:
            diff_output_lines = infile.readlines()
        # Compile the section header regex pattern
        pattern = re.compile(r"^@@ -(\d+)(?:,(\d+))? \+(\d+)(?:,(\d+))? @@")
        # Setup loop processing variables
        file1_num_lines = 0
        file2_num_lines = 0
        hunks_list = []
        for line in range(len(diff_output_lines)):
            text = diff_output_lines[line].rstrip('\r\n')
            if self.debug:
                print(text)
            line += 1
            if text.startswith('---'):
                # Discard file1 name 
                continue
            elif text.startswith('+++'):
                # Discard file2 name
                continue
            elif text.startswith('@@'):
                # If this is the start of a new diff hunk
                match_result = pattern.match(text)
                if match_result:
                    # Extract the new/old file line number and number of lines
                    match_groups = match_result.groups('1')
                    file1_line = int(match_groups[0])
                    file1_num_lines = int(match_groups[1])
                    file2_line = int(match_groups[2])
                    file2_num_lines = int(match_groups[3])
                    hunk = {'line' : file1_line, 'old' : [], 'new' : []}
                else:
                    raise DiffParserError(filename, line, text)
            elif text.startswith('-') and 0 < file1_num_lines:
                # Capture file1 contents (strip leading -)
                hunk['old'].append(text[1:])
                file1_num_lines -= 1
            elif text.startswith('+') and 0 < file2_num_lines:
                # Capture file2 contents (strip leading +)
                hunk['new'].append(text[1:])
                file2_num_lines -= 1
            else:
                raise DiffParserError(filename, line, text)

            # If we have reached the end of a diff hunk
            if file1_num_lines == 0 and file2_num_lines == 0:
                # Append the hunk information to the list of hunks
                hunks_list.append(hunk)

        return hunks_list

    # Diff the style corrected <file2> against the original <file1> and create
    # a list of style violations.
    def diff_files(self, file1, file2):
        # Create diff process command line arguments list
        args = ['diff', '-U', '0', '-a', file1, file2]
        if self.debug:
            print('Diff args : {}'.format(args))

        # The diff command returns either:
        # 0 : Inputs are identical
        # 1 : Inputs are different
        # 2 : Progam error
        try:
            with open(self.diff_filename, 'w') as outfile:
                status = subprocess.check_call(args, stdout=outfile, universal_newlines=True)
        except subprocess.CalledProcessError as err:
            if 1 == err.returncode:
                status = 1
            else:
                raise

        file_violations = {'file' : file1}
        # If differences were detected
        if 0 != status:
            # Parse the diff output to create the file violations
            file_violations['hunks'] = self.parse_diff_output(self.diff_filename)

        return file_violations

    # Performs style check on a single file
    def do_style_check(self, input_filename):
        # Take a copy of the input file
        shutil.copyfile(input_filename, self.output_filename)
        
        # Launch the style check program(s) to create the corrected file
        for launcher in self.launchers:
            launcher.execute(self.output_filename)

        # Check the corrected file agains the original to detect any violations
        file_violations = self.diff_files(input_filename, self.output_filename)
        self.report['violations'].append(file_violations)
        
        # Determine the number of style violations
        if 'hunks' in file_violations:
            return len(file_violations['hunks'])
        else:
            return 0
        
    # Performs style check on a list of source files
    def check_sources(self, sources):
        for filename in sources:
            print('Style checking {}'.format(filename), end='', flush=True)
            num_violations = self.do_style_check(filename)
            if 0 != num_violations:
                print(' found {} violations'.format(num_violations))
            else:
                print('')

        return self.generate_summary()
            
    # Generates the report summary information
    def generate_summary(self):
        # Calculate the total number of style violations
        num_checked_files = len(self.report['violations'])
        num_files = 0
        num_violations = 0
        for index in range(num_checked_files):
            module = self.report['violations'][index]
            if 'hunks' in module:
                num_files += 1
                num_violations += len(module['hunks'])
        self.report['num_files'] = num_files
        self.report['num_violations'] = num_violations
        
        # Display summary
        if 0 != num_files:
            print('WARNING: Found {} violations in {} of {} files'.format(
                num_violations, num_files, num_checked_files))
        else:
            print('SUCCESS: Found no style violations in {} files'.format(num_checked_files))
        
        return num_violations

    # Generate the report output as JSON string
    def generate_json_report(self):
        # For debugging set the indent level to cause pretty printing
        if self.debug:
            indent_level = 2
        else:
            indent_level = None

        # Dump the style report in JSON format (handles quote mark encoding)
        return json.dumps(self.report, sort_keys=True, indent=indent_level)

    # Generate the report output as XML string
    def generate_xml_report(self):
        dom = xml.dom.minidom.getDOMImplementation()
        document = dom.createDocument(None, 'checkstyle', None)
        root = document.documentElement
        root.setAttribute('version', '8.7')
        for module in self.report['violations']:
            if 'hunks' in module:
                fileElement = document.createElement('file')
                fileElement.setAttribute('name', module['file'])
                root.appendChild(fileElement)
                for index in range(len(module['hunks'])):
                    hunk = module['hunks'][index]
                    violation = document.createElement('error')
                    violation.setAttribute('line', str(hunk['line']))
                    violation.setAttribute('severity', 'error')
                    if 0 != len(hunk['old']):
                        violation.setAttribute('message', hunk['old'][0])
                    else:
                        violation.setAttribute('message', '+ ' + hunk['new'][0])
                    fileElement.appendChild(violation)
        
        # For debugging use pretty printing
        if self.debug:
            return document.toprettyxml(indent='  ')
        else:
            return document.toxml()

# Create the argument parser
parser = argparse.ArgumentParser(prog='check-style',
    description='Generate a style check report for a list of files')
parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable diff command debug output')
parser.add_argument('-l', '--launcher', required=True, action='append', nargs=3, \
    help='Define the style checker(s) launcher parameters ([astyle|uncrustify], command, options)')
parser.add_argument('-t', '--tempdir', required=True, help='Output directory for temporary files')
parser.add_argument('-r', '--report', required=True, help='Report filename')
parser.add_argument('-i', '--input', help='Optional file containing a space separated list of source files')
parser.add_argument('-v', '--violations', type=int, default=0, \
    help='Return an error if the number of violations exceed this threshold (disabled by negative value)')
parser.add_argument('sources', nargs='*', help='List of source files')

# Parse the command line arguments
args = parser.parse_args()

# Create the style checker launchers
launchers = []
for (launcher_class, launcher_command, launcher_options) in args.launcher:
    if 'astyle' == launcher_class:
        launchers.append(AStyleLauncher(launcher_command, launcher_options, args.debug))
    elif 'uncrustify' == launcher_class:
        launchers.append(UncrustifyLauncher(launcher_command, launcher_options, args.debug))
    else:
        print('Unknown style checker launcher class : {}'.format(launcher_class), file=sys.stderr)
        sys.exit(1)

# Create the style checker
checker = StyleChecker(launchers, args.tempdir, args.debug)

# If there is an input file containing the source file list
if args.input is not None:
    # Parse the input file into a list of source files
    with open(args.input, 'r') as infile:
        sources = infile.readline().rstrip('\r\n').split()
else:
    sources = args.sources

# Style check the source files
try:
    num_violations = checker.check_sources(sources)
except subprocess.CalledProcessError as process_error:
    # The style checker encountered an error
    print('Program launcher error : {}'.format(process_error), file=sys.stderr)
    sys.exit(1)
except DiffParserError as parser_error:
    # The style checker encountered an error
    print('Diff parser error in {}, line {}, text {}'.format(
        parser_error.filename, parser_error.line, parser_error.text), file=sys.stderr)
    sys.exit(2)

# Dump the violations report
report = checker.generate_xml_report()
if args.debug:
    print(report)
with open(args.report, 'w') as outfile:
    print(report, file=outfile)

# Decide on the return value
if args.violations >= 0 and num_violations > args.violations:
    sys.exit(3)
