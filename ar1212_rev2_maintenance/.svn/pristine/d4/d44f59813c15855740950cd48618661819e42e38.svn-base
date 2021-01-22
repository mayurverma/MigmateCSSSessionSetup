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
#
# ApBase/DevWare Test Access Client implementation.
#
# The Test Access Client is a transport layer client that communicates with Test
# Access Server embedded in a virtual prototype and/or RTL simulation to provide
# register get/set access for verification use case tests.
#
####################################################################################
import logging
import os
import sys
import math
import struct

from time import sleep
from enum import IntEnum, unique

# work out the project and working copy root directories
__projroot = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))

# prepend library paths to the search path
sys.path.append(os.path.join(__projroot, 'contrib', 'pywin32', 'x64', 'pywin32-224-py3.6-win-amd64.egg'))

import pythoncom
import win32com.client

@unique
class TestAccessDevice(IntEnum):
    # Supported bulk read/write devices
    SRAM = 0
    OTPM = 1

@unique
class TestAccessResetType(IntEnum):
    # Supported reset types
    CAMERA_HARD_RESET = 0
    SENSOR_HARD_RESET = 1
    SENSOR_SOFT_RESET = 2

# Test Access Client - connects to the Test Access Server
class TestAccessClient():

    # Actual server version
    __TEST_ACCESS_SERVER_VERSION = 0x00010000 # v1.0

    def __init__(self, debug = False):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__debug = debug
        try:
            self.__apbaseCom = win32com.client.Dispatch("apbaseCom.ApBase")
        except:
            raise Exception("Creation of ApBase COM object failed")

        try:
            self.__apbaseCom_id = pythoncom.CoMarshalInterThreadInterfaceInStream(pythoncom.IID_IDispatch, self.__apbaseCom)
        except:
            raise Exception("Failed to create dispatch id")

        self.__shd_mem_addr = 0xF100    # TODO: Hardcoded, but its not going to change
        self.__ship_address = 0x20      # TODO: Default I2C address, can we interrogate apbase for this?
        self.__burst_length = 64
        self.__camera = None

    def connect(self):
        """Connect to the COM MigMate object
        """
        pythoncom.CoInitialize()

        # Get instance from the id
        self.__thread_apbaseCom = win32com.client.Dispatch(pythoncom.CoGetInterfaceAndReleaseStream(self.__apbaseCom_id, pythoncom.IID_IDispatch))

        # Probe, then check there is one and only one camera connected
        if self.__debug:
            self.__logger.info("Detecting camera...")

        res = self.__thread_apbaseCom.DeviceProbe("")

        if 0 != res:
            raise Exception("Camera probe failed ({})".format(res))

        if 1 != self.__thread_apbaseCom.NumCameras:
            raise Exception("No camera, or too many cameras ({})".format(self.__thread_apbaseCom.NumCameras))

        # Get the camera instance
        try:
            self.__camera = self.__thread_apbaseCom.Create(0)
        except:
            raise Exception("Failed to get camera instance")

        if self.__debug:
            self.__logger.info("Camera instance: ", self.__camera.NAME, self.__camera.PartNumber)

    def disconnect(self):
        """Disconnect from the server, and terminate it
        """
        if self.__debug:
            self.__logger.info("disconnect")
        self.__camera = None
        self.__apbaseCom = None

    def __get_server_version(self):
        """Get the server version.
        """
        return self.__TEST_ACCESS_SERVER_VERSION

    def set_register(self, addr, value, is_backdoor = False):
        """Set the specified register to the specified value
           - is_backdoor indicates whether the transaction should be frontdoor or backdoor
        """
        if is_backdoor:
            raise Exception("Back door nor supported")

        if self.__debug:
            self.__logger.info("WRITE: 0x{:04X} = 0x{:04X}".format(addr, value))
        try:
            self.__camera.SetSensorRegisterAddr(0, 0, addr, 16, value)
        except:
            raise Exception("Write failed")

    def get_register(self, addr, is_backdoor = False):
        """Get the specified register
           - is_backdoor not supportted
        """
        if is_backdoor:
            raise Exception("Back door nor supported")

        try:
            # read the uncached register
            value = self.__camera.GetSensorRegisterAddr(0, 0, addr, 16, 0)
        except:
            raise Exception("Read failed")

        if self.__debug:
            self.__logger.info("READ: 0x{:04X} = 0x{:04X}".format(addr, value))

        return value

    def wait_usec(self, delay):
        """Wait for number of microseconds to elapse in the model/simulation
        """
        sleep(delay / 1e6)

    def bulk_read(self, device, offset_bytes, num_bytes):
        """Requests bulk read of a memory device
           - returns data as a bytearray
        """
        addr = self.__shd_mem_addr + offset_bytes
        if self.__debug:
            self.__logger.info("BULK READ: device ({}), addr ({:04X}), offset ({}), num_bytes({})".format(device.name,
                           addr, offset_bytes, num_bytes))

        if TestAccessDevice.OTPM == device:
            raise Exception("OTPM device not supported")

        if self.__debug:
            print("Receiving bulk data...")

        recv_data = bytearray()
        words_to_recv = math.ceil(num_bytes / 2)

        burst_length = self.__burst_length
        burst_count = words_to_recv // burst_length
        burst_remainder = words_to_recv % burst_length

        for i in range(burst_count):
            try:
                if self.__debug:
                    self.__logger.info("ship_address {:X}, addr {:X}, burst_length {}".format(self.__ship_address, addr, burst_length))
                data = self.__camera.GetRegisters(0, self.__ship_address, addr, 16, 16, burst_length)
            except:
                raise Exception("Bulk read failed")
            else:
                addr += burst_length * 2
                data = struct.pack('<{}H'.format(len(data)), *data)
                recv_data.extend(data)

        if burst_remainder:
            try:
                if self.__debug:
                    self.__logger.info("ship_address {:X}, addr {:X}, burst_length {}".format(self.__ship_address, addr, burst_remainder))
                data = self.__camera.GetRegisters(0, self.__ship_address, addr, 16, 16, burst_remainder)
            except:
                raise Exception("Bulk read failed")
            else:
                addr += burst_remainder * 2
                data = struct.pack('<{}H'.format(len(data)), *data)
                recv_data.extend(data)

        if self.__debug:
            print("Received {0!r}".format(recv_data))

        return recv_data

    def bulk_write(self, device, offset_bytes, data):
        """Requests bulk write of a memory device
        """

        addr = self.__shd_mem_addr + offset_bytes
        if self.__debug:
            self.__logger.info("BULK WRITE: device ({}), addr ({:04X}), offset ({}), data ({})".format(device.name,
                           addr, offset_bytes, data))

        if TestAccessDevice.OTPM == device:
            raise Exception("OTPM device not supported")

        words_to_send = math.ceil(len(data) / 2)
        message_in_words = struct.unpack("<{:d}H".format(words_to_send), data)

        burst_length = self.__burst_length
        burst_count = words_to_send // burst_length
        burst_remainder = words_to_send % burst_length

        for i in range(burst_count):
            try:
                if self.__debug:
                    self.__logger.info("ship_address {:X}, addr {:X}, burst_length {}".format(self.__ship_address, addr, burst_length))

                # Create an array variant
                message = win32com.client.VARIANT(pythoncom.VT_ARRAY | pythoncom.VT_I4, message_in_words[i * burst_length:(i + 1) * burst_length])
                self.__camera.SetRegisters(0, self.__ship_address, addr, 16, 16, message)
            except:
                raise Exception("Bulk write failed")
            else:
                addr += burst_length * 2

        if burst_remainder:
            try:
                if self.__debug:
                    self.__logger.info("ship_address {:X}, addr {:X}, burst_length {}".format(self.__ship_address, addr, burst_remainder))

                # Create an array variant
                message = win32com.client.VARIANT(pythoncom.VT_ARRAY | pythoncom.VT_I4, message_in_words[burst_count * burst_length:(burst_count * burst_length) + burst_remainder])
                self.__camera.SetRegisters(0, self.__ship_address, addr, 16, 16, message)
            except:
                raise Exception("Bulk write failed")
            else:
                addr += burst_remainder * 2

    def reset(self, reset_type):
        """Requests a hard reset.
        """
        self.__camera.SetState("Sensor Reset", 1)
        self.__camera.SetState("Sensor Reset", 0)

