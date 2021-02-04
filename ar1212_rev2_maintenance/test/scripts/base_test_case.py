####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
# Firmware test case base class
####################################################################################
import logging

from pyfwtest import *
from common_error import Error
from command_helper import CommandHelper
from patch_loader import PatchLoader

class BaseTestCase(TestCase):
    """Base class for all Test cases"""

    # override method
    def setUp(self):
        # Run base class setup method
        super().setUp()
        # Create objects needed for common setup
        self.__camera = self.fixture.Camera(0)
        self.__command_helper = CommandHelper(self.__camera)
        self.__patch_loader = PatchLoader(self.fixture.get_type(), self.__command_helper, logging.INFO)

    @property
    def camera(self):
        """Returns the default camera instance."""
        return self.__camera

    @property
    def command_helper(self):
        """Returns the command helper for the default camera instance."""
        return self.__command_helper

    @property
    def patch_loader(self):
        """Returns the patch loader for the default camera instance."""
        return self.__patch_loader

    def _common_setup(self, lcs, patch, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """Common test setup for any test method."""
        #self._load_otpm(otpm)
        self._wait_for_system_ready(timeout_usec, interval_usec, verbosity)
        if patch is not None:
            self._load_patch(lcs, patch)

    def _load_otpm(self, otpm):
        """Load OTPM image into the device (only supported on model/simulator)."""
        # TODO: <MB>: Add OTPM loading using the <otpm> argument
        pass

    def _load_patch(self, lcs, patch):
        """Load patch into the device."""
        self.__patch_loader.load_patch(lcs, patch)

    def _wait_for_system_ready(self, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """Wait until system is ready."""
        result = self.__command_helper.wait_for_command_done(timeout_usec, interval_usec, verbosity)
        self.assertEqual(result, Error.SUCCESS, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

