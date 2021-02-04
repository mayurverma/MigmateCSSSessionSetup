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
# Generate a function coverage report from a tarmac log and a symbol file.
####################################################################################
import argparse
import datetime
import glob
import re
import sys
import os
import xml.dom.minidom

# work out the project and working copy root directories
__projroot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

# prepend library paths to the search path
sys.path.append(os.path.join(__projroot, 'contrib', 'pyelftools'))

from elftools.elf.elffile import ELFFile

class CoverageElement():
    """Base class for a coverage report element."""
    def _append_child(self, document, parent, name):
        """Creates a child element with the given tag name and appends it to the parent node"""
        node = document.createElement(name)
        parent.appendChild(node)
        return node

class CoveragePackage(CoverageElement):
    """Package element of a coverage report."""
    def __init__(self, name):
        self.name = name
        self.line_rate = 0.0
        self.branch_rate = 0.0
        self.complexity = 0.0
        self.sources = {}

    def add_source(self, source):
        """Add source element to the package."""
        source.parent = self
        self.sources[source.name] = source

    def generate_xml(self, document, sources, packages):
        """Generate the XML report."""
        # Create the package element
        element = self._append_child(document, packages, 'package')
        element.setAttribute('name', self.name)
        element.setAttribute('line_rate', str(self.line_rate))
        element.setAttribute('branch_rate', str(self.branch_rate))
        element.setAttribute('complexity', str(self.complexity))
        # Create the classes child element
        classes = self._append_child(document, element, 'classes')
        # Create the sources and classes child elements
        for source in self.sources.values():
            source.generate_xml(document, sources, classes)

class CoverageSource(CoverageElement):
    """Source file element of a coverage report."""
    def __init__(self, srcdir, prefix, name):
        self.parent = None
        self.name = '{}/{}'.format(prefix, name)
        self.sections = []
        self._line_numbers = {}
        # For C++ the source file would be parsed for the package and the function
        # symbols would contain the classname. For C and Assembler, the package is
        # derived from the directory structure and filename.
        pkgname, basename = os.path.split(name)
        if pkgname == '':
            pkgname = '.'
        self.pkgname = pkgname
        classname, extension = basename.split('.', 1)
        self.classes = {classname : CoverageClass(self, classname)}
        # Parse the source file for function line numbers
        abspath = '{}/{}'.format(srcdir, name)
        if abspath.endswith('.s'):
            self._parse_asm(abspath)
        else:
            self._parse_c(abspath)

    def _parse_asm(self, filename):
        """Parse the source file to retrieve line numbers."""
        pattern = re.compile(r'^(\w+) +PROC')
        with open(filename) as f:
            lines = f.readlines()
            length = len(lines)
            for index in range(length):
                text = lines[index].rstrip()
                if text.endswith('\\'):
                    for index2 in range(index + 1, length):
                        text = text[:-1] + lines[index2].rstrip()
                        if not text.endswith('\\'):
                            break
                result = pattern.match(text)
                if result:
                    self._line_numbers[result.group(1)] = index

    def _parse_c(self, filename):
        """Parse the source file to retrieve line numbers."""
        pattern = re.compile(r'^\w+ (?:\w+ )*(\w+)\(')
        with open(filename) as f:
            lines = f.readlines()
            length = len(lines)
            for index in range(length):
                result = pattern.match(lines[index])
                if result:
                    self._line_numbers[result.group(1)] = index

    def add_method(self, name, address, size):
        """Add method element to the relevant class element."""
        # Assume there is only one classname for C functions.
        klass = list(self.classes.values())[0]
        number = self._line_numbers.get(name, 1)
        klass.add_method(name, number, address, size)

    def generate_xml(self, document, sources, classes):
        """Generate the XML report."""
        # Create the source child element
        element = self._append_child(document, sources, 'source')
        element.appendChild(document.createTextNode(self.name))
        # Create the class child elements
        for klass in self.classes.values():
            klass.generate_xml(document, classes)

class CoverageClass(CoverageElement):
    """Class element of a coverage report."""
    def __init__(self, parent, name):
        self.parent = parent
        self.name = name
        self.line_rate = 0.0
        self.branch_rate = 0.0
        self.complexity = 0.0
        self.methods = {}

    def add_method(self, name, number, address, size):
        """Add method element."""
        self.methods[name] = CoverageMethod(self, name, number, address, size)

    def generate_xml(self, document, classes):
        """Generate the XML report."""
        # Create the class element
        element = self._append_child(document, classes, 'class')
        element.setAttribute('name', self.name)
        element.setAttribute('filename', self.parent.name)
        element.setAttribute('line_rate', str(self.line_rate))
        element.setAttribute('branch_rate', str(self.branch_rate))
        element.setAttribute('complexity', str(self.complexity))
        # Create the methods child elements
        methods = self._append_child(document, element, 'methods')
        # Create the method child elements
        for method in self.methods.values():
            method.generate_xml(document, methods)

class CoverageMethod(CoverageElement):
    """Method element of a coverage report."""
    def __init__(self, parent, name, number, address, size):
        self.parent = parent
        self.name = name
        self.number = number
        self.address = address
        self.size = size
        self.signature = '()'
        self.line_rate = 0.0
        self.branch_rate = 0.0
        self.hits = 0
        #print('{} {} {}'.format(number, address, size))
        
    def generate_xml(self, document, methods):
        """Generate the XML report."""
        # Create the method element
        element = self._append_child(document, methods, 'method')
        element.setAttribute('name', self.name)
        element.setAttribute('signature', self.signature)
        element.setAttribute('line_rate', str(self.line_rate))
        element.setAttribute('branch_rate', str(self.branch_rate))
        # Create the lines child element
        lines = self._append_child(document, element, 'lines')
        # Create the line child element
        line = self._append_child(document, lines, 'line')
        line.setAttribute('branch', 'false')
        line.setAttribute('hits', str(self.hits))
        line.setAttribute('number', str(self.number))

    def hit(self):
        """Apply coverage hit for this method."""
        self.hits += 1

class CoverageReport():
    """Top level coverage report class."""
    def __init__(self, srcdir, prefix, filename):
        self._packages = {}
        self._sections = {}
        self.line_rate = 0.0
        self.branch_rate = 0.0
        self.tzinfo = datetime.datetime.now(datetime.timezone.utc).astimezone().tzinfo
        self.timestamp = datetime.datetime.now(self.tzinfo)
        self.version = "1"
        with open(filename, 'rb') as f:
            elffile = ELFFile(f)
            symtab = elffile.get_section_by_name('.symtab')
            previous_region = {}
            for symbol in symtab.iter_symbols():
                bind = symbol.entry.st_info['bind']
                type = symbol.entry.st_info['type']
                if bind == 'STB_LOCAL':
                    if type == 'STT_FILE':
                        filename = symbol.name.replace('\\', '/')
                        if filename.startswith('src/'):
                            source = self._get_source(srcdir, prefix, filename)
                        else:
                            source = None
                    elif type == 'STT_SECTION':
                        # Save the code sections
                        section = symbol.name
                        address = symbol.entry.st_value
                        size = symbol.entry.st_size
                        # Only interested in vectors and code
                        if section in ('VECTORS', '.text'):
                            #print(section)
                            region = {'address' : address, 'size' : size}
                            if source is not None:
                                source.sections.append(region)
                            if section in previous_region:
                                # If the previous region in this section has zero size then
                                # determine its size from the start address of this region
                                if previous_region[section]['size'] == 0:
                                    previous_region[section]['size'] = address - previous_region[section]['address']
                            previous_region[section] = region
                    elif type == 'STT_FUNC':
                        # Create a method element
                        if source is not None:
                            source.add_method(symbol.name, symbol.entry.st_value, symbol.entry.st_size)
                    elif type in ('STT_NOTYPE', 'STT_OBJECT'):
                        # Ignore labels and data objects
                        pass
                    else:
                        raise SyntaxError('Unexpected type : {}'.format(type))
                elif bind == 'STB_WEAK':
                    if type == 'STT_FUNC':                
                        self._add_method_by_address(symbol.name, symbol.entry.st_value, symbol.entry.st_size)
                    else:
                        raise SyntaxError('Unexpected type : {}'.format(type))
                elif bind == 'STB_GLOBAL':
                    if type == 'STT_FUNC':
                        self._add_method_by_address(symbol.name, symbol.entry.st_value, symbol.entry.st_size)
                    elif type in ('STT_NOTYPE', 'STT_OBJECT'):
                        # Ignore labels and data objects
                        pass
                    else:
                        raise SyntaxError('Unexpected type : {}'.format(type))
                else:
                    raise SyntaxError('Unexpected bind : {}'.format(bind))

    def _add_method_by_address(self, name, address, size):
        """Insert method in the source tree by address match of sections."""
        for package in self._packages.values():
            for source in package.sources.values():
                for section in source.sections:
                    start = section['address']
                    end = start + section['size']
                    if address >= start and address < end:
                        source.add_method(name, address, size)
                        return

    def _get_source(self, srcdir, prefix, filename):
        """Return the source node from the coverage tree (or create one if it does not exist)."""
        # Check if source already exists then return it
        for package in self._packages.values():
            for source in package.sources.values():
                if filename == source.name:
                    return source
        # Create source file element
        source = CoverageSource(srcdir, prefix, filename)
        pkgname = source.pkgname
        # Create the parent package if it does not already exist
        package = self._packages.get(pkgname, None)
        if package is None:
            package = CoveragePackage(pkgname)
            self._packages[pkgname] = package
        # Add the source element to the parent package
        package.add_source(source)
        return source
        
    def _append_child(self, document, parent, name):
        """Creates a child element with the given tag name and appends it to the parent node"""
        node = document.createElement(name)
        parent.appendChild(node)
        return node

    def get_methods_by_address(self):
        """Generate a map of address to method."""
        map = {}
        for pkgname, package in self._packages.items():
            for source in package.sources.values():
                for klass in source.classes.values():
                    for method in klass.methods.values():
                        map[method.address] = method
        return map
        
    def generate_xml(self):
        """Generate the XML report."""
        impl = xml.dom.minidom.getDOMImplementation()
        doctype = impl.createDocumentType('coverage', 'SYSTEM', 'http://cobertura.sourceforge.net/xml/coverage-03.dtd')
        document = impl.createDocument(None, 'coverage', doctype)
        root = document.documentElement
        root.setAttribute('line_rate', str(self.line_rate))
        root.setAttribute('branch_rate', str(self.branch_rate))
        root.setAttribute('version', str(self.version))
        root.setAttribute('timestamp', str(self.timestamp))
        
        sources = self._append_child(document, root, 'sources')
        packages = self._append_child(document, root, 'packages')
        for pkgname, package in self._packages.items():
            package.generate_xml(document, sources, packages)
        
        return document

# Create the argument parser
parser = argparse.ArgumentParser(prog='function_coverage.py',
    description='Generate a function coverage report from an ELF file and a set of tarmac log files.')
parser.add_argument('-e', '--elf', required=True, help='ELF file')
parser.add_argument('-l', '--logname', default='tarmac.log', help='Tarmac log file name')
parser.add_argument('-o', '--output', default='function_coverage.xml', help='Function coverage report file')
parser.add_argument('-p', '--prefix', default='firmware', help='Source file prefix')
parser.add_argument('-s', '--srcdir', default='.', help='Source file root directory')
parser.add_argument('sessions', help='Sessions root directory')

# Parse the command line arguments
args = parser.parse_args()

# Create a new coverage report from the symbol table
print('Reading ELF file {}'.format(args.elf))
coverage = CoverageReport(args.srcdir, args.prefix, args.elf)
methods_by_address = coverage.get_methods_by_address()

# Search the sessions directory for the tarmac log files
print('Searching for tarmac log files in {}'.format(args.sessions))
pattern = '{}/**/{}'.format(args.sessions, args.logname)
paths = glob.glob(pattern, recursive=True)

# Remove symbolic links and duplicates
logs = set()
for path in paths:
    logs.add(os.path.realpath(path))

# Parse the tarmac logs for the methods executed
print('Reading tarmac log files ...')
for logfile in logs:
    print('Reading {}'.format(logfile))
    next_pc = 0
    with open(logfile) as f:
        for line in f:
            #print(line)
            fields = line.split()
            trace_type = fields[2]
            if trace_type == 'IT':
                pc = int(fields[4], 16)
                if pc != next_pc:
                    if fields[6] == 'T':
                        address = pc + 1
                    else:
                        address = pc
                    method = methods_by_address.get(address, None)
                    if method is not None:
                        method.hit()
                next_pc = pc + 2
            elif trace_type == 'IS':
                next_pc = int(fields[4], 16) + 2

# Generate the coverage XML report
print('Coverage report {}'.format(args.output))
report = coverage.generate_xml()
with open(args.output, 'wt') as f:
    print(report.toprettyxml(), file=f)
