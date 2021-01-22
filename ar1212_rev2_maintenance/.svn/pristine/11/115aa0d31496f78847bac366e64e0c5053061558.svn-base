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
# Test Access Client implementation.
#
# The Test Access Client is a transport layer client that communicates with Test
# Access Server embedded in a virtual prototype and/or RTL simulation to provide
# register get/set access for verification use case tests.
#
####################################################################################
import argparse
import logging
import os
import socket
import struct
import time

from enum import IntEnum, unique

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

    # Expected server version
    __TEST_ACCESS_SERVER_VERSION = 0x00010005 # v1.5
    # Actual client version
    __TEST_ACCESS_CLIENT_VERSION = 0x00010005 # v1.5

    # TAS requests - MUST match the TAS_REQUEST_E enum in test_access_server.h
    __TAS_REQUEST_VERSION     = 0
    __TAS_REQUEST_SET_REG     = 1
    __TAS_REQUEST_GET_REG     = 2
    __TAS_REQUEST_WAIT_USEC   = 3
    __TAS_REQUEST_BULK_READ   = 4
    __TAS_REQUEST_BULK_WRITE  = 5
    __TAS_REQUEST_RESET       = 6

    def __init__(self, debug=False):
        self.__client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__connected = False
        self.__debug = debug

    def connect(self, host, port):
        """Connect to the server at host:port
        """
        self.__logger.debug('Connecting to {}:{}...'.format(host, port))
        self.__client_socket.setblocking(1)
        self.__client_socket.connect((host, port))
        self.__connected = True
        self.__logger.debug('Connected to server @ {}'.format(self.__client_socket.getpeername()))

        # check server version to make sure we are using the right protocol
        self.__logger.debug('Checking server version')
        version = self.__get_server_version()
        if version != self.__TEST_ACCESS_SERVER_VERSION:
            raise Exception("Unexpected server version: Expected 0x{:08x}, Actual: {:08x}".format(version, self.__TEST_ACCESS_SERVER_VERSION))
        self.__logger.debug('Client version matches server version')


    def disconnect(self):
        """Disconnect from the server, and terminate it
        """
        if self.__connected:
            self.__logger.debug('Disconnecting...')
            self.__client_socket.shutdown(socket.SHUT_WR)
            # Flush the socket, then close
            resp = self.__client_socket.recv(4)
            self.__client_socket.close()

    def __get_server_version(self):
        """Get the server version.
        """
        # Packing is: request, payload_size_bytes, client version
        req = struct.pack("!LLL", self.__TAS_REQUEST_VERSION, 4, self.__TEST_ACCESS_CLIENT_VERSION)

        if self.__debug:
            print("Sending {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(8)
        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, value) = struct.unpack("!LL", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

        if self.__debug:
            print("Received value 0x{:08x}".format(value))
        return value

    def set_register(self, addr, value, is_backdoor=False):
        """Set the specified register to the specified value
           - is_backdoor indicates whether the transaction should be frontdoor or backdoor
        """
        backdoor = 0
        if is_backdoor:
            backdoor = 1

        # Packing is: request, payload_size_bytes, addr, value, is_backdoor
        req = struct.pack("!LLLLL", self.__TAS_REQUEST_SET_REG, 12, addr, value, backdoor)

        if self.__debug:
            print("Sending {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(4)

        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, ) = struct.unpack("!L", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

    def get_register(self, addr, is_backdoor=False):
        """Get the specified register
           - is_backdoor indicates whether the transaction should be frontdoor or backdoor
        """
        backdoor = 0
        if is_backdoor:
            backdoor = 1

        # Packing is: request, payload_size_bytes, addr, is_backdoor
        req = struct.pack("!LLLL", self.__TAS_REQUEST_GET_REG, 8, addr, backdoor)

        if self.__debug:
            print("Sending {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(8)
        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, value) = struct.unpack("!LL", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

        if self.__debug:
            print("Received value 0x{:08x}".format(value))
        return value

    def wait_usec(self, delay):
        """Wait for number of microseconds to elapse in the model/simulation
        """

        # Packing is: request, payload_size_bytes, clocks
        req = struct.pack("!LLL", self.__TAS_REQUEST_WAIT_USEC, 4, delay)
        if self.__debug:
            print("Starting wait for {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(4)

        if self.__debug:
           print("Received {0!r}".format(resp))

        try:
            (response_status, ) = struct.unpack("!L", resp)
            if 0 != response_status:
                raise Exception('Received bad response {}'.format(response_status), response_status)
        except OSError as err:
           print("Received invalid server response {0!r}".format(resp))
           raise err

    def bulk_read(self, device, offset_bytes, num_bytes):
        """Requests bulk read of OTPM
           - returns data as a list
        """
        # Packing is: request, payload_size_bytes, device, offset_bytes, num_bytes
        req = struct.pack("!LLLLL", self.__TAS_REQUEST_BULK_READ, 12, device, offset_bytes, num_bytes)
        if self.__debug:
            print("Issuing bulk read request {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(4)
        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, ) = struct.unpack("!L", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

        # Receive the bulk data - note .recv() may not return all the requested
        # data in one call
        if self.__debug:
            print("Receiving bulk data...")

        recv_data = bytearray()
        bytes_to_recv = num_bytes
        while 0 != bytes_to_recv:
            data = self.__client_socket.recv(num_bytes)
            if self.__debug:
                print("Received {} bytes".format(len(data)))
            bytes_to_recv -= len(data)
            recv_data.extend(data)

        if self.__debug:
            print("Received {0!r}".format(recv_data))

        if len(recv_data) != num_bytes:
            print("Invalid length {}".format(len(recv_data)))

        return recv_data

    def bulk_write(self, device, offset_bytes, data):
        """Requests bulk write of OTPM
        """
        # Packing is: request, payload_size_bytes, device, offset_bytes, num_bytes
        req = struct.pack("!LLLLL", self.__TAS_REQUEST_BULK_WRITE, 12, device, offset_bytes, len(data))
        if self.__debug:
            print("Issuing bulk write request {0!r}...".format(req))
        self.__client_socket.send(req)

        # Transmit the bulk data
        if self.__debug:
            print("Sending bulk data...")
        self.__client_socket.send(data)
        if self.__debug:
            print("Sent {} bytes".format(len(data)))

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(4)
        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, ) = struct.unpack("!L", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

    def reset(self, reset_type):
        """Requests a reset.
        """
        # Packing is: request, payload_size_bytes, reset_type
        req = struct.pack("!LLL", self.__TAS_REQUEST_RESET, 4, reset_type)

        if self.__debug:
            print("Sending {0!r}...".format(req))
        self.__client_socket.send(req)

        if self.__debug:
            print("Receiving...")
        resp = self.__client_socket.recv(4)
        if self.__debug:
            print("Received {0!r}".format(resp))

        (response_status, ) = struct.unpack("!L", resp)
        if 0 != response_status:
            raise Exception('Received bad response {}'.format(response_status), response_status)

