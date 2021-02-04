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
# Camera diagnostics helper for firmware tests
####################################################################################
import logging

from diag_checkpoint import DiagCheckpoint
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError
from irq_types import IRQnType

class DiagHelper():
    """Diagnostics helper class."""

    def __init__(self, camera):
        self.__camera = camera
        self.__sensor = camera.sensor
        self.__reg = camera.sensor.reg
        self.__logger = logging.getLogger(self.__class__.__name__)

    def get_checkpoint(self):
        """Returns the checkpoint register value as an enumeration."""
        return DiagCheckpoint(self.__reg.CSS_CHECKPOINT.CHECKPOINT.uncached_value)

    def set_pausepoint(self, pausepoint):
        """Sets the pausepoint checkpoint register value."""
        self.__reg.CSS_PAUSEPOINT.PAUSEPOINT = pausepoint.value

    def get_pausepoint(self):
        """Returns the pausepoint checkpoint register value."""
        return self.__reg.CSS_PAUSEPOINT.PAUSEPOINT.uncached_value

    def is_paused(self):
        """Returns true if the firmware is paused at the required pausepoint."""
        return 0 != self.__reg.CSS_PAUSEPOINT.PAUSED.uncached_value

    def release_pausepoint(self):
        """Release the firmware from the current pausepoint."""
        self.__reg.CSS_PAUSEPOINT.PAUSED = 0

    def wait_for_pausepoint(self, timeout_usec=3000, interval_usec=100, verbosity=None):
        """Waits for the firmware to reach the requested pausepoint."""
        retries = 1 + (timeout_usec // interval_usec)
        for retry in range(retries):
            checkpoint = self.get_checkpoint()
            if verbosity:
                self.__logger.log(verbosity, 'CHECKPOINT = {}'.format(str(checkpoint)))
            if self.is_paused():
                return
            self.__camera.wait_usec(interval_usec)

        raise TimeoutError('Failed to reach pausepoint')

    def is_fatal_error(self):
        """Returns true if the firmware fatal error flag is set."""
        return 0 != self.__reg.CSS_CHECKPOINT.FATAL_ERROR.uncached_value

    def get_css_error(self):
        """Returns the CSS error register value as an enumeration tuple."""
        cryptoerror = self.__reg.CSS_ERROR.uncached_value
        fault = SystemMgrCssFault((cryptoerror & 0xFC00) >> 10)
        context = (cryptoerror & 0x3FF)
        if SystemMgrCssFault.FIRMWARE_FATAL == fault:
            context = SystemMgrFatalError(context)
        elif SystemMgrCssFault.UNEXPECTED_EXCEPTION == fault:
            context = IRQnType(context)
        return (fault, context)

    def get_info(self):
        """Returns the information register value."""
        return self.__reg.CSS_INFO.uncached_value

    def check_for_fatal_error(self):
        """ Checks for fatal error, prints out error info if identified """

        fault, context = self.get_css_error()
        info = self.get_info()
        if self.is_fatal_error():
            self.__logger.warn('!!! FATAL ERROR !!!')
            self.__logger.warn('CSS_ERROR.FAULT_CODE = {}'.format(str(fault)))
            self.__logger.warn('CSS_ERROR.FAULT_CONTEXT = {}'.format(str(context)))
            self.__logger.warn('CSS_INFO = 0x{:04X} ({:0d})'.format(info, info))

        return fault, context, info


