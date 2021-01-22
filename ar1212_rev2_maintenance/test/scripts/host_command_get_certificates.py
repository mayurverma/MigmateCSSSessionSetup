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
# Camera GetStatus host command
####################################################################################
import logging
import struct

from common_error import Error
from host_command_code import HostCommandCode
from command_helper import CommandHelper
from host_command import HostCommandParams, HostCommandResponse

class HostCommandGetCertificatesParams(HostCommandParams):
    """Host command GetCertificates parameters."""

    def pack(self):
        """Packs the host command into a bytes for transport."""
        result = self.__certificate_auth_id.to_bytes(2, 'little')
        return result

    @property
    def certificate_auth_id(self):
        return self.__certificate_auth_id;

    @certificate_auth_id.setter
    def certificate_auth_id(self, val):
        self.__certificate_auth_id = val;


class HostCommandGetCertificatesResponse(HostCommandResponse):
    """Host command GetCertificates response."""
    # overriden method
    def size_bytes(self):
        """Returns the length of the expected response."""
        return (1032 * 2) + 4 #worst case

    # overridden method
    def unpack(self, payload):
        print("UNPACKING")
        (self.__sensor_certificate_size_words, ) = struct.unpack_from("<H", payload, offset=0)
        print(self.__sensor_certificate_size_words)
        (self.__sensor_certificate, ) = struct.unpack_from("<{}s".format(self.__sensor_certificate_size_words * 2),
            payload, offset=2)
        (self.__vendor_certificate_size_words, ) = struct.unpack_from("<H", payload,
            offset=((self.__sensor_certificate_size_words * 2) + 2))
        if 0 != self.__vendor_certificate_size_words:
            (self.__vendor_certificate, ) = struct.unpack_from("<{}s".format(self.__vendor_certificate_size_words * 2),
                payload, offset=((self.__sensor_certificate_size_words * 2) + 4))

    @property
    def sensor_certificate_size_words(self):
        return self.__sensor_certificate_size_words

    @property
    def sensor_certificate(self):
        return self.__sensor_certificate

    @property
    def vendor_certificate_size_words(self):
        return self.__vendor_certificate_size_words

    @property
    def vendor_certificate(self):
        return self.__vendor_certificate

class HostCommandGetCertificates():
    """Get certificates execute"""

    def __init__(self, camera):
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)

    def execute(self, auth_id, expected = Error.SUCCESS, timeout_usec=500000000, interval_usec=100, verbosity=logging.INFO):
        """ Issue GetSensorId command """

        params = HostCommandGetCertificatesParams()
        response = HostCommandGetCertificatesResponse()
        params.certificate_auth_id = auth_id.value;

        self.__logger.info('Sending command: GET_CERTIFICATES')
        result = self.__command_helper.execute(HostCommandCode.GET_CERTIFICATES, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != expected.value:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.__logger.info('Received response')

        return response
