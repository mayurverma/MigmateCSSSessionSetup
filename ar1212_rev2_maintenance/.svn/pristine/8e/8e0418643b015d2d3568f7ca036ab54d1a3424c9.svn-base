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
# Camera SetSessionKeys host command
####################################################################################
import struct

from host_command_set_psk_session_keys import HostCommandSetPskSessionKeysParams, HostCommandSetPskSessionKeysResponse

class HostCommandSetSessionKeysParams(HostCommandSetPskSessionKeysParams):
    """Host command SetSessionKeys parameters."""

    def __init__(self):
        pass

    # overridden method
    def pack(self):

        encryptedParamslen = len(self.__encryptedParams)

        members = super().pack()
        members += struct.pack("<HH{}s".format(encryptedParamslen),
                self.__sessionParamsCipher,
                self.__encryptedParamsSizeWords,
                self.__encryptedParams)
        return members

    @property
    def sessionParamsCipher(self):
        return self.__sessionParamsCipher

    @sessionParamsCipher.setter
    def sessionParamsCipher(self, value):
        self.__sessionParamsCipher = value

    @property
    def encryptedParamsSizeWords(self):
        return self.__sencryptedParamsSizeWords

    @encryptedParamsSizeWords.setter
    def encryptedParamsSizeWords(self, value):
        self.__encryptedParamsSizeWords = value

    @property
    def encryptedParams(self):
        return self.__encryptedParams

    @encryptedParams.setter
    def encryptedParams(self, value):
        self.__encryptedParams = value

class HostCommandSetSessionKeysResponse(HostCommandSetPskSessionKeysResponse):
    """Host command SetSessionKeys response."""

    pass

