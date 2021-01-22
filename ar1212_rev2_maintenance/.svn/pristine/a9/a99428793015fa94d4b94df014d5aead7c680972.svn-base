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
# Camera command helper for firmware tests
####################################################################################
import logging

from common_error import Error
from diag_helper import DiagHelper

class CommandHelper():
    """Command helper class."""

    # CSS Shared RAM sizes
    __SRAM_NUM_PAGES = 4
    __SRAM_PAGE_SIZE = 1024

    def __init__(self, camera):
        self.__diag_helper = DiagHelper(camera)
        self.__camera = camera
        self.__sensor = camera.sensor
        self.__reg = camera.sensor.reg
        self.__logger = logging.getLogger(self.__class__.__name__)

    def is_command_done(self):
        """Returns true if the last command is complete."""
        return 0 == self.__reg.CSS_COMMAND.DOORBELL.uncached_value

    def wait_for_command_done(self, timeout_usec=5000, interval_usec=100, verbosity=None):
        """Waits for the current command to complete."""
        retries = 1 + (timeout_usec // interval_usec)
        for retry in range(retries):
            response = self.__reg.CSS_COMMAND.uncached_value
            if 0 == (response & 0x8000):
                return Error(response)
            if verbosity is not None:
                self.__logger.log(verbosity, 'CHECKPOINT = {:50} (INFO = {})'.format(str(self.__diag_helper.get_checkpoint()), str(self.__diag_helper.get_info())))
            self.__camera.wait_usec(interval_usec)

        raise TimeoutError('Host timed out waiting for command to complete')

    def execute(self, command_code, command_params, command_response, timeout_usec=5000, interval_usec=100, force_get_response=False, verbosity=None):
        """Executes command and retrieves response."""
        # Get the payload bytes
        payload = command_params.pack()
        # Send payload one page at a time
        remainder = len(payload)
        for page in range(self.__SRAM_NUM_PAGES):
            if remainder > 0:
                self.__reg.CSS_PAGE = page
                start = page * self.__SRAM_PAGE_SIZE
                length = min(remainder, self.__SRAM_PAGE_SIZE)
                stop = start + length
                self.__reg.CSS_PARAMS_0 = payload[start:stop]
                remainder -= length
            else:
                break
        # Start the command by writing the command code
        self.__reg.CSS_COMMAND = command_code.value

        result = self.wait_for_command_done(timeout_usec, interval_usec, verbosity)
        if (result == Error.SUCCESS or force_get_response==True):
            # If this command has response data
            remainder = command_response.size_bytes()
            if remainder > 0:
                # Read the response data one page at a time
                payload = bytearray()
                for page in range(self.__SRAM_NUM_PAGES):
                    if remainder > 0:
                        self.__reg.CSS_PAGE = page
                        length = min(remainder, self.__SRAM_PAGE_SIZE)
                        payload.extend(self.__reg.CSS_PARAMS_0.burst_read(length))
                        remainder -= length
                    else:
                        break
                command_response.unpack(payload)
        elif result == Error.SYSERR:
            # Wait for the firmware to set the fatal error information (needed by sim/model)
            self.__camera.wait_usec(300)
            self.__diag_helper.check_for_fatal_error()
        return result
