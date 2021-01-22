####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################
# Imports the register set from the XSDAT description file.
#
# The RegisterSet implements a small part of the DevSuite ApBase API so that test
# cases can run against a virtual prototype and/or RTL simulation for verification
# as well as using DevWare/midlib for real silicon validation testing.
####################################################################################
import argparse

from test_access_client import TestAccessDevice, TestAccessClient
from xml.etree.ElementTree import ElementTree


class Bitfield():
    """Emulation of part of the ApBase Bitfield class"""
    def __init__(self, parent, name, mask):
        self.__parent = parent
        self.__name = name
        self.__mask = mask
        # Calculate the bit shift from the mask
        for index in range(32):
            if 0 != ((1 << index) & mask):
                self.__shift = index
                break

    def __str__(self):
        return self.__name

    ########################################################
    # The following properties should match ApBase (midlib)
    ########################################################

    @property
    def mask(self):
        return self.__mask

    @property
    def name(self):
        return self.__name

    @property
    def uncached_value(self):
        return (self.__parent.uncached_value & self.__mask) >> self.__shift

    @property
    def value(self):
        return (self.__parent.value & self.__mask) >> self.__shift

    @value.setter
    def value(self, value):
        registerValue = (self.__parent.uncached_value & ~self.__mask) | \
            ((value << self.__shift) & self.__mask)
        self.__parent.value = registerValue

class Register(dict):
    """Emulation of part of the ApBase Register class"""
    def __init__(self, tac, name, address, span, mask, minimum, maximum, default):
        self.__tac = tac
        self.__name = name
        self.__address = address
        self.__span = span
        self.__mask = mask
        self.__minimum = minimum
        self.__maximum = maximum
        self.__default = default
        self.__value = default

    def __dir__(self):
        return self.keys()

    def __getattr__(self, name):
        if name in self:
            return self[name]
        else:
            raise AttributeError('No such attribute: {}'.format(name))

    def __setattr__(self, name, value):
        if name in self:
            self[name].value = value
        else:
            super().__setattr__(name, value)

    def __delattr__(self, name):
        # Make the dictionary read-only
        raise AttributeError('Deleting bitfields is not allowed')

    def __iter__(self):
        for value in self.values():
            yield value

    def __str__(self):
        return self.__name

    # Create a bit field for this parent register
    def bitfield(self, name, mask):
        self[name] = Bitfield(self, name, mask)

    ########################################################
    # The following properties should match ApBase (midlib)
    ########################################################

    @property
    def addr(self):
        return self.__address

    @property
    def default(self):
        return self.__default

    @property
    def mask(self):
        return self.__mask

    @property
    def maximum(self):
        return self.__maximum

    @property
    def minimum(self):
        return self.__minimum

    @property
    def name(self):
        return self.__name

    @property
    def span(self):
        return self.__span

    @property
    def uncached_value(self):
        self.__value = self.__tac.get_register(self.__address)
        return self.__value

    @property
    def value(self):
        return self.__value

    @value.setter
    def value(self, value):
        if isinstance(value, bytes):
            self.__tac.bulk_write(TestAccessDevice.SRAM, 0, value)
        else:
            self.__tac.set_register(self.__address, value)
            self.__value = value

    def burst_read(self, length):
        return self.__tac.bulk_read(TestAccessDevice.SRAM, 0, length)

class RegisterSet(dict):
    """Emulation of the ApBase RegisterSet class"""
    def __init__(self, tac, filename):
        self.__tac = tac
        # Parse the register set XSDAT file
        parser = ElementTree()
        tree = parser.parse(filename)
        # Walk the tree to create the registers
        for registerElement in tree.find('registers').findall('reg'):
            regName = registerElement.get('name')
            address = int(registerElement.get('addr'), 16)
            # The 8 bit registers do not specify a span attribute
            span = int(registerElement.get('span', '1'))
            mask = int(registerElement.get('mask'), 16)
            # Some elements do not have a range attribute so check for it
            range = registerElement.get('range', None)
            if range is not None:
                # Split the range attribute into minimum/maximum values
                limits = range.split(' ')
                min = int(limits[0], 16)
                max = int(limits[1], 16)
            else:
                # Infer the minimum/maximum values from the span
                min = 0
                max = (1 << (span * 8)) - 1
            # Some elements do not have a default attribute so check for it
            default = int(registerElement.get('default', '0'), 16)
            # Create the register object
            registerObject = Register(self.__tac, regName, address, span, mask, min, max, default)
            # Search for any bitfields
            for bitfieldElement in registerElement.findall('bitfield'):
                bitfieldMask = int(bitfieldElement.get('mask'), 16)
                registerObject.bitfield(bitfieldElement.get('name'), bitfieldMask)
            # Add the register object to the set
            setattr(self, regName, registerObject)

    def __getattr__(self, name):
        if name in self:
            return self[name]
        else:
            raise AttributeError('No such attribute: {}'.format(name))

    def __setattr__(self, name, value):
        if name in self:
            # If the register object already exists then set its value property
            self[name].value = value
        else:
            # Otherwise insert the register object in the dictionary
            self[name] = value

    def __delattr__(self, name):
        # Make the dictionary read-only
        raise AttributeError('Deleting registers is not allowed')

    def __iter__(self):
        for value in self.values():
            yield value

    # ITestRunnerListener method
    def startTestRun(self):
        """Notification handler for the start of a test run."""
        self.__tac.startTestRun()

    # ITestRunnerListener method
    def stopTestRun(self):
        """Notification handler for the end of a test run."""
        self.__tac.stopTestRun()


if __name__ == "__main__":
    # Create the argument parser
    parser = argparse.ArgumentParser(prog='registerset',
        description='Tests importing the register set from the XSDAT description file')
    parser.add_argument('-d', '--debug', default=False, action='store_true', help='Enable debug output')
    parser.add_argument('-n', '--hostname', default='localhost', help='Test access server host name')
    parser.add_argument('-p', '--port', type=int, default=42140, help='Test access server port number')
    parser.add_argument('filename', help='XSDAT description file')

    # Parse the command line arguments
    args = parser.parse_args()

    tac = TestAccessClient(debug=True)
    tac.connect(args.host, args.port)
    registers = RegisterSet(tac, filename)
    if args.debug:
        for reg in registers:
            print("{}.addr : 0x{:04x}".format(reg.name, reg.addr))
        for bitfield in registers.CSS_COMMAND:
            print(bitfield)

    print('CSS_COMMAND.addr          : 0x{:04x}'.format(registers.CSS_COMMAND.addr))
    print('CSS_COMMAND.default       : 0x{:04x}'.format(registers.CSS_COMMAND.default))
    print('CSS_COMMAND.DOORBELL.mask : 0x{:04x}'.format(registers.CSS_COMMAND.DOORBELL.mask))
    print('CSS_COMMAND.RESULT.mask   : 0x{:04x}'.format(registers.CSS_COMMAND.RESULT.mask))
    print('CSS_CHECKPOINT.addr       : 0x{:04x}'.format(registers.CSS_CHECKPOINT.addr))
    print('CSS_CHECKPOINT.span       : {}'.format(registers.CSS_CHECKPOINT.span))
    print('CSS_CHECKPOINT.mask       : 0x{:04x}'.format(registers.CSS_CHECKPOINT.mask))
    print('CSS_CHECKPOINT.minimum    : 0x{:04x}'.format(registers.CSS_CHECKPOINT.minimum))
    print('CSS_CHECKPOINT.maximum    : 0x{:04x}'.format(registers.CSS_CHECKPOINT.maximum))
    print()
    print('CSS_COMMAND.value                   : 0x{:04x}'.format(registers.CSS_COMMAND.value))
    print('CSS_COMMAND.DOORBELL.value          : 0x{:04x}'.format(registers.CSS_COMMAND.DOORBELL.value))
    print('CSS_COMMAND.RESULT.value            : 0x{:04x}'.format(registers.CSS_COMMAND.RESULT.value))
    print('CSS_COMMAND.DOORBELL.uncached_value : 0x{:04x}'.format(registers.CSS_COMMAND.DOORBELL.uncached_value))
    print('CSS_COMMAND.RESULT.uncached_value   : 0x{:04x}'.format(registers.CSS_COMMAND.RESULT.uncached_value))

    registers.CSS_COMMAND = 0x89ab
    print('CSS_COMMAND.value          : 0x{:04x}'.format(registers.CSS_COMMAND.value))
    print('CSS_COMMAND.uncached_value : 0x{:04x}'.format(registers.CSS_COMMAND.uncached_value))
    print('CSS_COMMAND.value          : 0x{:04x}'.format(registers.CSS_COMMAND.value))

    print('CSS_COMMAND.DOORBELL.value          : 0x{:04x}'.format(registers.CSS_COMMAND.DOORBELL.value))
    print('CSS_COMMAND.RESULT.value            : 0x{:04x}'.format(registers.CSS_COMMAND.RESULT.value))
    print('CSS_COMMAND.DOORBELL.uncached_value : 0x{:04x}'.format(registers.CSS_COMMAND.DOORBELL.uncached_value))
    print('CSS_COMMAND.RESULT.uncached_value   : 0x{:04x}'.format(registers.CSS_COMMAND.RESULT.uncached_value))

