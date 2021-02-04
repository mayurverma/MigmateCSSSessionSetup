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
# Test fixture for firmware test framework
####################################################################################
import os

from enum import Enum
from pyfwtest import TestFixture
from camera_fixture import CameraFixture

if os.name == 'nt':
    from migmate_camera_fixture import MigMateCameraFixture
    from devware_camera_fixture import DevWareCameraFixture

class FixtureType(Enum):
    """Fixture type enumeration."""
    MODEL   = 1
    SIM     = 2
    MIGMATE = 3
    DEVWARE = 4

class SimulatorTestFixture(TestFixture):

    # override method
    def _createFixtureElements(self, properties):
        """Factory method to allow derived classes to add fixture elements."""
        self.__camera = CameraFixture(properties)
        self.addElement(self.__camera)

    # Emulate midlib.Camera(0) interface
    def Camera(self, index):
        assert 0 == index, 'Only one camera is supported'
        return self.__camera

    def wait_usec(self, delay):
        """Wait for the specified number of microseconds."""
        self.__camera.wait_usec(delay)

    @classmethod
    def get_type(cls):
        return FixtureType.SIM

class ModelTestFixture(TestFixture):

    # override method
    def _createFixtureElements(self, properties):
        """Factory method to allow derived classes to add fixture elements."""
        self.__camera = CameraFixture(properties)
        self.addElement(self.__camera)

    # Emulate midlib.Camera(0) interface
    def Camera(self, index):
        assert 0 == index, 'Only one camera is supported'
        return self.__camera

    def wait_usec(self, delay):
        """Wait for the specified number of microseconds."""
        self.__camera.wait_usec(delay)

    @classmethod
    def get_type(cls):
        return FixtureType.MODEL

class MigmateTestFixture(TestFixture):

    # override method
    def _createFixtureElements(self, properties):
        """Factory method to allow derived classes to add fixture elements."""
        self.__camera = MigMateCameraFixture(properties)
        self.addElement(self.__camera)

    # Emulate midlib.Camera(0) interface
    def Camera(self, index):
        assert 0 == index, 'Only one camera is supported'
        return self.__camera

    def wait_usec(self, delay):
        """Wait for the specified number of microseconds."""
        self.__camera.wait_usec(delay)

    @classmethod
    def get_type(cls):
        return FixtureType.MIGMATE

class DevwareTestFixture(TestFixture):
    
    # override method
    def _createFixtureElements(self, properties):
        """Factory method to allow derived classes to add fixture elements."""
        self.__camera = DevWareCameraFixture(properties)
        self.addElement(self.__camera)
    
    # Emulate midlib.Camera(0) interface
    def Camera(self, index):
        assert 0 == index, 'Only one camera is supported'
        return self.__camera
    
    def wait_usec(self, delay):
        """Wait for the specified number of microseconds."""
        self.__camera.wait_usec(delay)

    @classmethod
    def get_type(cls):
        return FixtureType.DEVWARE
