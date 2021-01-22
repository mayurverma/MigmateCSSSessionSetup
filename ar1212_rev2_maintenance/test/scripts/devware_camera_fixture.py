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
# DevWare Camera fixture element for firmware tests
####################################################################################
import logging
import time

from ar0825_sensor import AR0825Sensor
from pyfwtest import ITestRunnerListener
from registerset import RegisterSet
from devware_test_access_client import TestAccessClient, TestAccessResetType
from reset_types import ResetType

class DevWareCameraFixture(ITestRunnerListener):
    """Represents the camera interface."""

    def __init__(self, properties, debug = False):
        self.__tac = TestAccessClient(debug)
        self.__reg = RegisterSet(self.__tac, properties['PROJECT_SENSOR_FILE'])
        self.__sensor = AR0825Sensor(self.__reg)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__debug = debug

    @property
    def sensor(self):
        return self.__sensor

    # ITestRunnerListener method
    def startTestRun(self):
        """Notification handler for the start of a test run."""
        self.__logger.info('Trying to connect with DevWare test access server')
        self.__tac.connect()
        self.__logger.info('Connected to test access server')

    # ITestRunnerListener method
    def stopTestRun(self):
        """Notification handler for the end of a test run."""
        self.__tac.disconnect()

    def wait_usec(self, delay):
        """Wait for the specified number of microseconds."""
        self.__tac.wait_usec(delay)

    def reset(self, reset_type):
        """Performs requested reset operation"""

        if ResetType.CAMERA == reset_type:
            raise NotImplementedError()
        elif ResetType.SENSOR_HARD == reset_type:
            self.__tac.reset(TestAccessResetType.SENSOR_HARD_RESET)
        elif ResetType.SENSOR_SOFT == reset_type:
            self.__sensor.reset(reset_type)
        else:
            raise NotImplementedError()

    def WriteSensorGpio(self, value):
        """ Write to DEMO3 sensor GPIO control """

        self.__tac.set_camera_register(bus=0, device=0x64, addr=0x42, addr_bits=8, data_bits=16, value=value)

