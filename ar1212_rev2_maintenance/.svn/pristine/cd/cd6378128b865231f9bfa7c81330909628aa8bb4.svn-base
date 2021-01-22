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
# Camera Debug Get Registers host command
####################################################################################
import struct

from host_command import HostCommandParams, HostCommandResponse
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase

class HostCommandDebugGetRegistersParams(HostCommandParams):
    """Host command DebugGetRegisters parameters."""
    pass # No parameters
    
class HostCommandDebugGetRegistersResponse(HostCommandResponse):
    """Host command DebugGetRegisters response."""
    
    def __init__(self):
        super().__init__('<LLLLLLLLLL')

    # overridden method
    def unpack(self, payload):
        """Unpacks the host command response payload into its structure members."""
        members = struct.unpack(self._format, payload)
        self.__reg0 = members[0]
        self.__reg1 = members[1]
        self.__reg2 = members[2]
        self.__reg3 = members[3]
        self.__reg4 = members[4]
        self.__reg5 = members[5]
        self.__reg6 = members[6]
        self.__reg7 = members[7]
        self.__reg8 = members[8]
        self.__reg9 = members[9]

    @property
    def debug_reg0(self):
        return self.__reg0

    @property
    def debug_reg1(self):
        return self.__reg1

    @property
    def debug_reg2(self):
        return self.__reg2

    @property
    def debug_reg3(self):
        return self.__reg3

    @property
    def debug_reg4(self):
        return self.__reg4

    @property
    def debug_reg5(self):
        return self.__reg5

    @property
    def debug_reg6(self):
        return self.__reg6

    @property
    def debug_reg7(self):
        return self.__reg7

    @property
    def debug_reg8(self):
        return self.__reg8

    @property
    def debug_reg9(self):
        return self.__reg9
