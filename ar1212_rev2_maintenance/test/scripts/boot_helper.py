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
# Camera boot helper for firmware tests
####################################################################################
import logging

from diag_checkpoint import DiagCheckpoint

class BootHelper():
    """Boot and reset helper class."""

    #class ResetType(IntEnum):
    #    """Represents reset types."""
    #    CAMERA_HARD = 0
    #    SENSOR_HARD = 1
    #    SENSOR_SOFT = 2

    def __init__(self, camera):
        self.__camera = camera
        self.__sensor = camera.sensor
        self.__reg = camera.sensor.reg
        self.__logger = logging.getLogger(self.__class__.__name__)

    #def reset(self, type):
    #    """Sets the pausepoint checkpoint register value."""
    #    if type != ResetType.SENSOR_SOFT:
    #        raise NotImplementedError
    #    self.__reg.CSS_PAUSEPOINT.PAUSEPOINT = pausepoint.value
