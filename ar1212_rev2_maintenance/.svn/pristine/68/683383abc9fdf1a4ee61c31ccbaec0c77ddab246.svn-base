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
# Sensor control for firmware tests
####################################################################################
from reset_types import ResetType

class AR0825Sensor():
    """Represents the sensor interface."""

    def __init__(self, reg):
        self.__reg = reg

    @property
    def reg(self):
        return self.__reg

    def reset(self, reset_type):
        """Performs requested reset operation"""

        if ResetType.SENSOR_SOFT == reset_type:
            self.__reg.RESET_REGISTER.RESET_REGISTER_RESET = 1
        else:
            raise NotImplementedError()