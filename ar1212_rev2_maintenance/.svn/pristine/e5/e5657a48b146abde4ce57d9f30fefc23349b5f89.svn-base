####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
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
# Test Firmware processes the SetVideoAuthRoi command correctly.
####################################################################################
import logging
import os
from enum import Enum

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from common_error import Error
from command_helper import CommandHelper
from lifecycle_state import LifecycleState
from systemmgr_phase import SystemMgrPhase
from crypto_helper import CryptoHelper
from nvm_record import NvmRecord, NvmRecordRSAPublicCert, NvmRecordRSAPrivateKey
from patch_type import PatchType
from host_session import HostSession, SessionContext, SessionMode, SymmetricAECipherMode, SymmetricAuthCipherMode
from host_command_code import HostCommandCode
from host_command_debug_get_registers import HostCommandDebugGetRegistersParams, HostCommandDebugGetRegistersResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from reset_types import ResetType

class VideoAuthModeType(Enum):
    FULL_FRAME = 0
    ROI = 1

class PixelPackModeType(Enum):
    OPTIMIZED = 0
    NON_OPTIMIZED = 1
    INVALID = 2

class SensorControl():

    def __init__(self, camera):
        self._camera = camera
        self._reg = self._camera.sensor.reg

    def streaming_enable(self, enable = True):
        """Enable/Disable streaming"""

        if os.name == 'nt':
            if enable:
                self._reg.RESET_REGISTER.RESET_REGISTER_STREAM = 1
            else:
                self._reg.RESET_REGISTER.RESET_REGISTER_STREAM = 0

    def embedded_data_enable(self, enable = True):
        """Enable/Disable streaming"""

        if os.name == 'nt':
            if enable:
                self._reg.SMIA_TEST.SMIA_TEST_EMBEDDED_DATA_EN = 1
            else:
                self._reg.SMIA_TEST.SMIA_TEST_EMBEDDED_DATA_EN = 0

    def mac_rows_enable(self, enable = True):
        """Enable/Disable streaming"""

        if os.name == 'nt':
            if enable:
                self._reg.DARK_CONTROL.DARK_CONTROL_SHOW_MAC_ROWS = 1
            else:
                self._reg.DARK_CONTROL.DARK_CONTROL_SHOW_MAC_ROWS = 0

class TestHardware(BaseTestCase):
    """Hardware based verification testing"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.get_status_helper = HostCommandGetStatus(self.camera)
        self.get_sensor_id_helper =  HostCommandGetSensorId(self.camera)
        self.logger.debug('setUp complete')
        #self.camera.reset(ResetType.SENSOR_HARD)
        self.sensor_control = SensorControl(self.camera)

    @testIdentifier('12.4.1')
    def test_gcm_cm(self):
        """Check FW responds to SetVideoAuthROI correctly in CM"""

        self.sensor_control.streaming_enable(False)

        self._common_setup(LifecycleState.CM, PatchType.VALIDATION)

        # get into CONFIGURE phase
        id = self.get_sensor_id_helper.execute(timeout_usec=500000000)
        status = self.get_status_helper.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        context = SessionContext(video_auth_cipher_mode=SymmetricAuthCipherMode.AES_GCM_GMAC,
            control_channel_cipher_mode=SymmetricAECipherMode.AES_GCM)
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.UNSECURE_EPHEMERAL_MODE, ca_rsa_key=None, master_secret=bytes([0]*16), salt=bytes([0]*16))

        self.logger.info(session)
        session.set_video_auth_roi(
            videoAuthMode = VideoAuthModeType.FULL_FRAME.value,
            pixelPackMode = PixelPackModeType.OPTIMIZED.value,
            )

        # Check we're still in SESSION phase
        status = self.get_status_helper.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        self.sensor_control.streaming_enable(True)
        self.sensor_control.embedded_data_enable(True)
        self.sensor_control.mac_rows_enable(True)

    @testIdentifier('12.4.2')
    def test_ccm_cm(self):
        """Check FW responds to SetVideoAuthROI correctly in CM"""

        self.sensor_control.streaming_enable(False)

        self._common_setup(LifecycleState.CM, PatchType.VALIDATION)

        # get into CONFIGURE phase
        id = self.get_sensor_id_helper.execute(timeout_usec=500000000)
        status = self.get_status_helper.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        context = SessionContext(video_auth_cipher_mode=SymmetricAuthCipherMode.AES_CMAC,
            control_channel_cipher_mode=SymmetricAECipherMode.AES_CCM)
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.UNSECURE_EPHEMERAL_MODE, ca_rsa_key=None, master_secret=bytes([0]*16), salt=bytes([0]*16))

        self.logger.info(session)
        session.set_video_auth_roi(
            videoAuthMode = VideoAuthModeType.ROI.value,
            pixelPackMode = PixelPackModeType.OPTIMIZED.value,
            pixelPackValue  = 0x1234,
            firstRow        = 0,
            lastRow         = 10,
            rowSkip         = 3000,
            firstColumn     = 0,
            lastColumn      = 10,
            columnSkip      = 3000,
            frameCounter    = 0
            )

        # Check we're still in SESSION phase
        status = self.get_status_helper.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        self.sensor_control.streaming_enable(True)
        self.sensor_control.embedded_data_enable(True)
        self.sensor_control.mac_rows_enable(True)

    def test_ccm_debug_reads_cm(self):
        """Check FW responds to DebugGetRegisters correctly in CM"""

        self.sensor_control.streaming_enable(False)

        self._common_setup(LifecycleState.CM, PatchType.VALIDATION)

        # get into CONFIGURE phase
        id = self.get_sensor_id_helper.execute(timeout_usec=500000000)
        status = self.get_status_helper.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        context = SessionContext(video_auth_cipher_mode=SymmetricAuthCipherMode.AES_CMAC,
            control_channel_cipher_mode=SymmetricAECipherMode.AES_CCM)
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.UNSECURE_EPHEMERAL_MODE, ca_rsa_key=None, master_secret=bytes([0]*16), salt=bytes([0]*16))

        self.logger.info(session)
        session.set_video_auth_roi(
            videoAuthMode = VideoAuthModeType.ROI.value,
            pixelPackMode = PixelPackModeType.OPTIMIZED.value,
            pixelPackValue  = 0x1234,
            firstRow        = 0,
            lastRow         = 10,
            rowSkip         = 3000,
            firstColumn     = 0,
            lastColumn      = 10,
            columnSkip      = 3000,
            frameCounter    = 0
            )

        # Check we're still in SESSION phase
        status = self.get_status_helper.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        self.sensor_control.streaming_enable(True)
        self.sensor_control.embedded_data_enable(True)
        self.sensor_control.mac_rows_enable(True)

        # Validation App has added the DebugGetRegisters command, lets read the Vcore.cmac registers
        params = HostCommandDebugGetRegistersParams()
        response = HostCommandDebugGetRegistersResponse()
        self.logger.info('Sending command: DEBUG_GET_REGISTERS')
        timeout_usec=5000
        interval_usec=100
        verbosity=logging.INFO
        result = self.command_helper.execute(HostCommandCode.DEBUG_GET_REGISTERS, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != Error.SUCCESS:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Log response
        self.logger.info('VCore Register cmac0 = 0x{:08x}'.format(response.debug_reg0))
        self.logger.info('VCore Register cmac1 = 0x{:08x}'.format(response.debug_reg1))
        self.logger.info('VCore Register cmac2 = 0x{:08x}'.format(response.debug_reg2))
        self.logger.info('VCore Register cmac3 = 0x{:08x}'.format(response.debug_reg3))
        self.logger.info('VCore Register gmac0 = 0x{:08x}'.format(response.debug_reg4))
        self.logger.info('VCore Register gmac1 = 0x{:08x}'.format(response.debug_reg5))
        self.logger.info('VCore Register gmac2 = 0x{:08x}'.format(response.debug_reg6))
        self.logger.info('VCore Register gmac3 = 0x{:08x}'.format(response.debug_reg7))
        self.logger.info('VCore Register pack_val_lo = 0x{:08x}'.format(response.debug_reg8))
        self.logger.info('Marker should be 0x12345678: 0x{:08X}'.format(response.debug_reg9))

    def test_gcm_debug_reads_cm(self):
        """Check FW responds to SetVideoAuthROI correctly in CM"""

        self.sensor_control.streaming_enable(False)

        self._common_setup(LifecycleState.CM, PatchType.VALIDATION)

        # get into CONFIGURE phase
        id = self.get_sensor_id_helper.execute(timeout_usec=500000000)
        status = self.get_status_helper.execute()
        self.assertEqual(SystemMgrPhase.CONFIGURE, status.current_phase)

        context = SessionContext(video_auth_cipher_mode=SymmetricAuthCipherMode.AES_GCM_GMAC,
            control_channel_cipher_mode=SymmetricAECipherMode.AES_GCM)
        session = HostSession(context, self.camera)
        session.connect(mode = SessionMode.UNSECURE_EPHEMERAL_MODE, ca_rsa_key=None, master_secret=bytes([0]*16), salt=bytes([0]*16))

        self.logger.info(session)
        session.set_video_auth_roi(
            videoAuthMode = VideoAuthModeType.FULL_FRAME.value,
            pixelPackMode = PixelPackModeType.OPTIMIZED.value,
            )

        # Check we're still in SESSION phase
        status = self.get_status_helper.execute()
        self.assertEqual(status.current_phase, SystemMgrPhase.SESSION)

        self.sensor_control.streaming_enable(True)
        self.sensor_control.embedded_data_enable(True)
        self.sensor_control.mac_rows_enable(True)

        # Validation App has added the DebugGetRegisters command, lets read some registers
        params = HostCommandDebugGetRegistersParams()
        response = HostCommandDebugGetRegistersResponse()
        self.logger.info('Sending command: DEBUG_GET_REGISTERS')
        timeout_usec=5000
        interval_usec=100
        verbosity=logging.INFO
        result = self.command_helper.execute(HostCommandCode.DEBUG_GET_REGISTERS, params, response, timeout_usec=timeout_usec, interval_usec=interval_usec, verbosity=verbosity)
        if result != Error.SUCCESS:
            raise Exception('Operation failed with error {}'.format(result.name))
        self.logger.info('Received response')

        # Log response
        self.logger.info('VCore Register cmac0 = 0x{:08x}'.format(response.debug_reg0))
        self.logger.info('VCore Register cmac1 = 0x{:08x}'.format(response.debug_reg1))
        self.logger.info('VCore Register cmac2 = 0x{:08x}'.format(response.debug_reg2))
        self.logger.info('VCore Register cmac3 = 0x{:08x}'.format(response.debug_reg3))
        self.logger.info('VCore Register gmac0 = 0x{:08x}'.format(response.debug_reg4))
        self.logger.info('VCore Register gmac1 = 0x{:08x}'.format(response.debug_reg5))
        self.logger.info('VCore Register gmac2 = 0x{:08x}'.format(response.debug_reg6))
        self.logger.info('VCore Register gmac3 = 0x{:08x}'.format(response.debug_reg7))
        self.logger.info('VCore Register pack_val_lo = 0x{:08x}'.format(response.debug_reg8))
        self.logger.info('Marker should be 0x12345678: 0x{:08X}'.format(response.debug_reg9))

