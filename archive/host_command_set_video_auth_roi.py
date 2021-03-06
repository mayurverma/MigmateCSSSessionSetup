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
# Camera SetVideoAuthROI host command
####################################################################################
import logging
import struct

from enum import Enum
from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper

from host_command import HostCommandParams, HostCommandResponse


class VideoAuthModeType(Enum):
    FULL_FRAME = 0
    ROI = 1

class PixelPackModeType(Enum):
    OPTIMIZED = 0
    NON_OPTIMIZED = 1
    INVALID = 2


class HostCommandSetVideoAuthROIParams(HostCommandParams):
    """Host command SetVideoAuthROI parameters."""

    def __init__(self):

        self.__encryptedParams = None
        pass

    # overridden method
    def pack(self):

        if (self.__encryptedParams):
            members = struct.pack("<32s16s",
                self.__encryptedParams,
                self.__mac)
        else:
            members = struct.pack("<HHHHHHHHHHLQ16s",
                self.__videoAuthMode,
                self.__pixelPackMode,
                self.__pixelPackValue,
                self.__firstRow,
                self.__lastRow,
                self.__rowSkip,
                self.__firstColumn,
                self.__lastColumn,
                self.__columnSkip,
                0,
                0,
                self.__frameCounter,
                self.__mac)

        return members

    @property
    def encryptedParams(self):
        return self.__encryptedParams

    @encryptedParams.setter
    def encryptedParams(self, value):
        self.__encryptedParams = value

    @property
    def videoAuthMode(self):
        return self.__videoAuthMode

    @videoAuthMode.setter
    def videoAuthMode(self, value):
        self.__videoAuthMode = value

    @property
    def pixelPackMode(self):
        return self.__pixelPackMode

    @pixelPackMode.setter
    def pixelPackMode(self, value):
        self.__pixelPackMode = value

    @property
    def pixelPackValue(self):
        return self.__pixelPackValue

    @pixelPackValue.setter
    def pixelPackValue(self, value):
        self.__pixelPackValue = value

    @property
    def firstRow(self):
        return self.__firstRow

    @firstRow.setter
    def firstRow(self, value):
        self.__firstRow = value

    @property
    def lastRow(self):
        return self.__lastRow

    @lastRow.setter
    def lastRow(self, value):
        self.__lastRow = value

    @property
    def rowSkip(self):
        return self.__rowSkip

    @rowSkip.setter
    def rowSkip(self, value):
        self.__rowSkip = value

    @property
    def firstColumn(self):
        return self.__firstColumn

    @firstColumn.setter
    def firstColumn(self, value):
        self.__firstColumn = value

    @property
    def lastColumn(self):
        return self.__lastColumn

    @lastColumn.setter
    def lastColumn(self, value):
        self.__lastColumn = value

    @property
    def columnSkip(self):
        return self.__columnSkip

    @columnSkip.setter
    def columnSkip(self, value):
        self.__columnSkip = value

    @property
    def frameCounter(self):
        return self.__frameCounter

    @frameCounter.setter
    def frameCounter(self, value):
        self.__frameCounter = value

    @property
    def mac(self):
        return self.__mac

    @mac.setter
    def mac(self, value):
        self.__mac = value

class HostCommandSetVideoAuthROIResponse(HostCommandResponse):
    """Host command SetVideoAuthROI response."""

    pass

class HostCommandSetVideoAuthROI():
    """Get status helper class."""

    def __init__(self, session):
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__session = session
        self.__host_command_code = HostCommandCode.SET_VIDEO_AUTH_ROI

    def execute(self,
                  videoAuthMode = VideoAuthModeType.ROI,
                  pixelPackMode = PixelPackModeType.OPTIMIZED,
                  pixelPackValue = 0,
                  firstRow = 0,
                  lastRow = 0,
                  rowSkip = 0,
                  firstColumn = 0,
                  lastColumn = 0,
                  columnSkip = 0,
                  frameCounter = 0,
                  expected = Error.SUCCESS, timeout_usec=500000, interval_usec=100, verbosity=logging.INFO):

        """Issue the SetVideoAuthROI command and return response"""

        videoAuthConfigParams = struct.pack("<HHHHHHHHHHLQ",
                                            videoAuthMode.value,
                                            pixelPackMode.value,
                                            pixelPackValue,
                                            firstRow,
                                            lastRow,
                                            rowSkip,
                                            firstColumn,
                                            lastColumn,
                                            columnSkip,
                                            0,                  # Padding
                                            0,
                                            frameCounter)
        self.__logger.info('videoAuthConfigParams: \nvideoAuthMode:{}\npixelPackMode:{}\npixelPackValue:{}\nROI Row:{}_{}_{}, Col:{}_{}_{}'.format(videoAuthMode.value,pixelPackMode.value,pixelPackValue,firstRow,lastRow,rowSkip,firstColumn,lastColumn,columnSkip ))

        params = HostCommandSetVideoAuthROIParams()
        response = HostCommandSetVideoAuthROIResponse()

        params.encryptedParams = videoAuthConfigParams

        self.__session.encrypt_and_execute_command(self.__host_command_code, params, response,
                                                   expected = expected, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)

        return response

