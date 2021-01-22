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
# Camera fixture element for firmware tests
####################################################################################
import logging
import time

from ar0825_sensor import AR0825Sensor
from pyfwtest import ITestRunnerListener
from registerset import RegisterSet
from test_access_client import TestAccessClient, TestAccessResetType, TestAccessDevice
from xmlrpc.client import ServerProxy
from reset_types import ResetType

class CameraFixture(ITestRunnerListener):
    """Represents the camera interface."""

    def __init__(self, properties, debug = False):
        self.__token_server_host = properties['FIXTURE_TOKEN_SERVER_HOST']
        self.__token_server_port = properties['FIXTURE_TOKEN_SERVER_PORT']
        self.__token_server_timeout = properties['FIXTURE_TOKEN_SERVER_TIMEOUT']
        self.__test_access_token = properties['FIXTURE_TEST_ACCESS_TOKEN']
        self.__tac = TestAccessClient(debug)
        self.__reg = RegisterSet(self.__tac, properties['PROJECT_SENSOR_FILE'])
        self.__sensor = AR0825Sensor(self.__reg)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__debug = debug

    def __get_token_value(self):
        """Retrieves the token value from the token server."""
        socket = None
        url = 'http://{}:{}/RPC2'.format(self.__token_server_host, self.__token_server_port)
        with ServerProxy(url, verbose = False) as proxy:
            timenow_secs = int(round(time.time()))
            timeout_secs = timenow_secs + self.__token_server_timeout
            self.__logger.debug("Waiting {} secs for token {} to become available...".format(
                self.__token_server_timeout, self.__test_access_token))
            while timenow_secs < timeout_secs:
                status, message = proxy.get_token(self.__test_access_token)
                if 0 == status:
                    # Received the test access socket from the token server
                    socket = message
                    break
                time.sleep(2)
                timenow_secs = int(round(time.time()))

        return socket

    @property
    def sensor(self):
        return self.__sensor

    # ITestRunnerListener method
    def startTestRun(self):
        """Notification handler for the start of a test run."""
        self.__logger.info('Waiting for test access server to start')
        socket = self.__get_token_value()
        if socket is None:
            msg = 'Timed-out waiting for token {}'.format(self.__test_access_token)
            self.__logger.warning(msg)
            raise TimeoutError(msg)
        else:
            # Parse the token server response into host:port
            host, port = socket.split(':')
            self.__logger.info('Trying to connect with test access server at {}'.format(socket))
            self.__tac.connect(host, int(port))
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

    def dump_otpm(self, start = 0, length = 1024*4):
        """Returns contents of OTPM"""

        return self.__tac.bulk_read(TestAccessDevice.OTPM, start, length)
