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
# Test the production test application.
####################################################################################
import logging
import sys
from enum import Enum

from pyfwtest import *
from base_test_case import BaseTestCase
from diag_helper import DiagHelper
from common_error import Error
from host_command_code import HostCommandCode
from systemmgr_phase import SystemMgrPhase
from lifecycle_state import LifecycleState
from host_command_get_status import HostCommandGetStatus
from host_command_prod_test_app_otpm import HostCommandOtpmBlankCheckParams, HostCommandOtpmBlankCheckResponse
from host_command_prod_test_app_otpm import HostCommandOtpmVerifyParams,     HostCommandOtpmVerifyResponse
from patch_type import PatchType
from crypto_helper import CryptoHelper
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_depu_program_otpm import HostCommandDepuProgramOtpmParams, HostCommandDepuProgramOtpmResponse
from build_properties import BuildProperties
from reset_types import ResetType
from toolbox import Toolbox

depu_app_output_dir = BuildProperties["DEPU"].output_path
if depu_app_output_dir not in sys.path:
    sys.path.append(depu_app_output_dir)
from importlib import import_module

class ProdTestAppMode(Enum):
    FAST = 0
    SLOW = 1
    INVALID = 2

class OtpmError(Enum):
    NON_ZERO   = 0
    ECC_SINGLE = 1 << 12
    ECC_DOUBLE = 1 << 13

class TestProdTestApp(BaseTestCase):
    """ Test the production test application
    """
    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.diag_helper = DiagHelper(self.camera)
        self.crypto = CryptoHelper()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.DEPU)
        self.rom_version = self.patch_loader.get_rom_version()
        self.key_set_name = 'test'

        self.unreadable_addresses_cm = list()
        self.unreadable_addresses_dm = list( range(0x00, 0x10) )
        self.unreadable_addresses_se = list( range(0x00, 0x10) )\
                                     + list( range(0x19, 0x1D) )\
                                     + list( range(0x1D, 0x21) )

        self.logger.debug('setUp complete')

    def _otpm_blank_check(self, mode, expected=Error.SUCCESS):
        """ Issue the OtpmBlankCheck command and return response
        """
        params = HostCommandOtpmBlankCheckParams()
        response = HostCommandOtpmBlankCheckResponse()
        params.mode = mode
        self.logger.info('Sending OtpmBlankCheck command')
        result = self.command_helper.execute(HostCommandCode.OTPM_BLANK_CHECK, params,
                                                response, timeout_usec= 400000)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received OtpmBlankCheck response')
        return response

    def _otpm_verify(self, mode, expected=Error.SUCCESS):
        """ Issue the OtpmVerify command and return response
        """
        params = HostCommandOtpmVerifyParams()
        response = HostCommandOtpmVerifyResponse()
        params.mode = mode
        self.logger.info('Sending OtpmVerify command')
        result = self.command_helper.execute(HostCommandCode.OTPM_VERIFY, params,
                                                response, timeout_usec= 200000)
        self.assertEqual(result, expected.value, 'Operation failed with error {}'.format(result.name))
        self.logger.info('Received OtpmVerify response')
        return response

    def _check_system_ready_and_load_patch(self, life_cycle_state):
        """ Checks system is ready and loads the ProdTestApp patch
        """
        self._common_setup(life_cycle_state, PatchType.PROD_TEST, timeout_usec=200000)

    def isValidChecksum(self, response):
        """ Computes the RFC1071 checksum on the supplied response parameters
            Returns true if the checksum is valid (zero), false if not
        """
        checksum  = response.blockLengthWords
        checksum += response.checksum
        for data in response.dataBlock:
            checksum += data
        # Fold the carry bits back into the lower 16-bits, then invert
        while checksum > 0xFFFF:
            checksum = (checksum & 0xFFFF) + (checksum >> 16)
        checksum = ~checksum & 0xFFFF
        return(0 == checksum)

    def check_dataBlock(self, dataBlock, expected_errors):
        """ Checks the dataBlock only contains all of the prod_test_app_errors
             within the 'expected_errors' list
            Asserts that there are no false positives and no flase negatives
        """
        true_positives = []
        false_positives = []
        for prod_test_app_error in dataBlock:
            if prod_test_app_error in expected_errors:
                true_positives.append(prod_test_app_error)
                expected_errors.remove(prod_test_app_error)
            else:
                false_positives.append(prod_test_app_error)
                self.logger.info('Error (0x{:04x}) not expected'.format(prod_test_app_error))
        false_negatives = expected_errors

        self.logger.debug('Number of True Positives: {}'.format(len(true_positives)))
        self.logger.debug('True Positives: {}'.format(', '.join(hex(x) for x in true_positives)))
        self.logger.debug('Number of False Positives: {}'.format(len(false_positives)))
        self.logger.debug('False Positives: {}'.format(', '.join(hex(x) for x in false_positives)))
        self.logger.debug('Number of False Negatives: {}'.format(len(false_negatives)))
        self.logger.debug('False Negatives: {}'.format(', '.join(hex(x) for x in false_negatives)))
        self.assertEqual(len(false_positives), 0)
        self.assertEqual(len(false_negatives), 0)

    def check_ecc_single_all_locations(self, dataBlock, unreadable_addresses):
        """ Checks the dataBlock describes a single bit ECC error at every location
            Asserts that the above is true
        """
        start_address = 0x000 # First OTPM Address
        end_address   = 0x7E0 # One Address beyond the last OTPM Address
        expected_length = (end_address -start_address) -len(unreadable_addresses)
        self.assertEqual(expected_length, len(dataBlock))

        index = 0
        for address in range(start_address, end_address):
            if not address in unreadable_addresses:
                self.assertEqual(dataBlock[index], (address + OtpmError.ECC_SINGLE.value))
                index += 1

    def wait_for_system_ready(self, expected_result=Error.SUCCESS):
        """Waits until system is ready
        """
        result = self.command_helper.wait_for_command_done(verbosity=logging.INFO)
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))
        self.logger.info('System is ready')

    def check_for_expected_status(self, expected_phase, expected_lcs, expected = Error.SUCCESS, timeout_usec=5000, interval_usec=100, verbosity=logging.INFO):
        """ Check system is in the expected phase and lifecycle state
            - returns the GetStatus response object
        """

        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute(expected, timeout_usec, interval_usec, verbosity)

        self.assertEqual(response.current_phase, expected_phase,
            'Expected {} phase but was {}'.format(expected_phase.name, response.current_phase.name))
        self.assertEqual(response.current_lifecycle_state, expected_lcs,
            'Expected {} LCS but was {}'.format(expected_lcs.name, response.current_lifecycle_state.name))

        return response

    def run_depu_program_otpm(self, clockSpeedHz = 50000000, expected_result = Error.SUCCESS):

        depu_app_content = import_module('depu_app_content_{}'.format(self.key_set_name)).DepuAppContent

        # Issue ProgramOtpm command
        params = HostCommandDepuProgramOtpmParams()
        params.clockSpeedHz = clockSpeedHz
        params.blockLengthLongWords = len(depu_app_content)
        params.checksum = Toolbox().calculate_checksum(depu_app_content)
        params.dataBlob = depu_app_content
        response = HostCommandDepuProgramOtpmResponse()
        self.logger.info('Sending command: DEPU_PROGRAM_OTPM')
        result = self.command_helper.execute(HostCommandCode.DEPU_PROGRAM_OTPM, params, response, timeout_usec=500000000, verbosity=logging.INFO, force_get_response = True)

        # Log response
        self.logger.info('Received response ({})'.format(result.name))
        self.logger.info('Error Address = {}'.format(response.error_address))
        self.logger.info('Number of soft Errors = {}'.format(response.number_soft_errors))
        self.assertEqual(result, expected_result, 'Operation failed with error {}'.format(result.name))

        return response

    def sensor_soft_reset(self):
        """ Reset the sensor """
        self.camera.reset(ResetType.SENSOR_SOFT)
        self.camera.wait_usec(1) # workaround to prevent the command register read happening before the icore soft reset
                                 # See http://jira.onsemi.com/browse/AR0820FW-231 for more details
        self.wait_for_system_ready()

    def provision_device_with_depu(self):
        """ Use DEPU to provision device before use """

        self._common_setup(LifecycleState.CM, PatchType.DEPU, timeout_usec=200000)

        HostCommandGetSensorId(self.camera).execute()

        response = self.run_depu_program_otpm()
        self.assertEqual(0, response.error_address, 'Error Address incorrect: {}'.format(response.error_address))
        self.assertEqual(0, response.number_soft_errors, 'Num soft errors is incorrect: {}'.format(response.number_soft_errors))

        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.CONFIGURE, LifecycleState.SECURE)
        self.assertEqual(self.patch_version, response.patch_version)

        response = HostCommandGetSensorId(self.camera).execute()
        soc_id = bytes(response.unique_id)

        self.sensor_soft_reset()

    #   19.1   CM LCS
    #  ------ --------
    @testIdentifier('19.1.1')
    def test_otpm_blank_check_fast_no_errors_cm(self):
        """ Checks OtpmBlankCheck finds no errors when run in fast mode
             on a blank image with no errors in it
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 0)
        self.assertTrue(self.isValidChecksum(response))

    @testIdentifier('19.1.2')
    def test_otpm_blank_check_slow_no_errors_cm(self):
        """ Checks OtpmBlankCheck finds no errors when run in slow mode
             on a blank image with no errors in it
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Run OtpmBlankCheck in slow mode
        response = self._otpm_blank_check(ProdTestAppMode.SLOW)
        self.assertEqual(response.blockLengthWords, 0)
        self.assertTrue(self.isValidChecksum(response))

    @testIdentifier('19.1.3')
    @testIdentifier('SANITY')
    def test_otpm_blank_check_fast_with_ecc_single_cm(self):
        """ Checks OtpmBlankCheck can find a single bit ecc error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Expected error Address and        Type
        expected_error =  0x011   +  OtpmError.ECC_SINGLE.value

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.1.4')
    def test_otpm_blank_check_fast_with_ecc_double_cm(self):
        """ Checks OtpmBlankCheck can find a double bit ecc error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Expected error Address and        Type
        expected_error =  0x037   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.1.5')
    def test_otpm_blank_check_fast_with_non_zero_cm(self):
        """ Checks OtpmBlankCheck can find a non zero error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Expected error Address and        Type
        expected_error =  0x666   +  OtpmError.NON_ZERO.value

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.1.6')
    def test_otpm_blank_check_fast_slow_with_errors_cm(self):
        """ Checks OtpmBlankCheck can run twice (fast mode then slow mode)
             on a single patch load
            Checks OtpmBlankCheck can find and report the standard set of
             injected errors
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Expected error Address and        Type
        expected_error =  0x013   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

        # Expected error   Addresses and       Types
        expected_errors = [  0x013    +  OtpmError.ECC_DOUBLE.value,
                             0x014    +  OtpmError.ECC_SINGLE.value,
                             0x025    +  OtpmError.ECC_SINGLE.value,
                             0x023    +  OtpmError.ECC_DOUBLE.value,
                             0x02E    +  OtpmError.ECC_SINGLE.value,
                             0x030    +  OtpmError.ECC_DOUBLE.value,
                             0x039    +  OtpmError.ECC_DOUBLE.value,
                             0x041    +  OtpmError.ECC_SINGLE.value,
                             0x103    +  OtpmError.NON_ZERO.value,
                             0x271    +  OtpmError.ECC_SINGLE.value,
                             0x349    +  OtpmError.NON_ZERO.value,
                             0x4AD    +  OtpmError.ECC_DOUBLE.value,
                             0x5BC    +  OtpmError.ECC_SINGLE.value,
                             0x6E7    +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmBlankCheck in slow mode
        response = self._otpm_blank_check(ProdTestAppMode.SLOW)
        self.assertNotEqual(response.blockLengthWords, 6)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)

    @testIdentifier('19.1.7')
    def test_otpm_blank_check_slow_fast_with_errors_cm(self):
        """ Checks OtpmBlankCheck can run twice (slow mode then fast mode)
             on a single patch load
            Checks OtpmBlankCheck can find and report the standard set of
             injected errors
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Expected error   Addresses and       Types
        expected_errors = [  0x013    +  OtpmError.ECC_DOUBLE.value,
                             0x014    +  OtpmError.ECC_SINGLE.value,
                             0x025    +  OtpmError.ECC_SINGLE.value,
                             0x023    +  OtpmError.ECC_DOUBLE.value,
                             0x02E    +  OtpmError.ECC_SINGLE.value,
                             0x030    +  OtpmError.ECC_DOUBLE.value,
                             0x039    +  OtpmError.ECC_DOUBLE.value,
                             0x041    +  OtpmError.ECC_SINGLE.value,
                             0x103    +  OtpmError.NON_ZERO.value,
                             0x271    +  OtpmError.ECC_SINGLE.value,
                             0x349    +  OtpmError.NON_ZERO.value,
                             0x4AD    +  OtpmError.ECC_DOUBLE.value,
                             0x5BC    +  OtpmError.ECC_SINGLE.value,
                             0x6E7    +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmBlankCheck in slow mode
        response = self._otpm_blank_check(ProdTestAppMode.SLOW)
        self.assertNotEqual(response.blockLengthWords, 6)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)

        # Expected error Address and        Type
        expected_error =  0x013   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmBlankCheck in fast mode
        response = self._otpm_blank_check(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.1.8')
    def test_otpm_blank_check_slow_with_ecc_single_all_locations_cm(self):
        """ Checks OtpmBlankCheck can find a single bit ecc error in every
             readable location when a single bit ecc error has been injected
             into every OTPM location of a blank image
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Run OtpmBlankCheck in slow mode
        response = self._otpm_blank_check(ProdTestAppMode.SLOW)
        self.assertTrue(self.isValidChecksum(response))
        self.check_ecc_single_all_locations(response.dataBlock, self.unreadable_addresses_cm)

    @testIdentifier('19.1.9')
    def test_otpm_blank_check_invalid_parameter_cm(self):
        """ Checks OtpmBlankCheck will return the error INVAL
             when given an incorrect parameter value
        """

        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Pass an invalid parameter to OtpmBlankCheck
        self._otpm_blank_check(ProdTestAppMode.INVALID, expected=Error.INVAL)

    @testIdentifier('19.1.10')
    def test_otpm_verify_fast_no_errors_cm(self):
        """ Checks OtpmVerify will not run in the CM LCS
        """
        self._check_system_ready_and_load_patch(LifecycleState.CM)

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST, expected=Error.ACCESS)


    #   19.2   DM LCS
    #  ------ --------
    @testIdentifier('19.2.1')
    def test_otpm_verify_fast_no_errors_dm(self):
        """ Checks OtpmVerify finds no errors when run in fast mode
             on a dm image with no errors in it
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 0)
        self.assertTrue(self.isValidChecksum(response))

    @testIdentifier('19.2.2')
    def test_otpm_verify_slow_no_errors_dm(self):
        """ Checks OtpmVerify finds no errors when run in slow mode
             on a dm image with no errors in it
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 0)
        self.assertTrue(self.isValidChecksum(response))

    @testIdentifier('19.2.3')
    def test_otpm_verify_fast_no_errors_with_assets_dm(self):
        """ Checks OtpmVerify finds no errors when run in slow mode
             on a dm image with assets and no errors in it
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 0)
        self.assertTrue(self.isValidChecksum(response))

    @testIdentifier('19.2.4')
    def test_otpm_verify_fast_with_ecc_single_dm(self):
        """ Checks OtpmVerify can find a single bit ecc error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expected error Address and        Type
        expected_error =  0x011   +  OtpmError.ECC_SINGLE.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.2.5')
    def test_otpm_verify_fast_with_ecc_double_dm(self):
        """ Checks OtpmVerify can find a double bit ecc error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expected error Address and        Type
        expected_error =  0x037   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.2.6')
    def test_otpm_verify_fast_with_non_zero_dm(self):
        """ Checks OtpmVerify can find a non zero error
             when run in fast mode
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expected error Address and        Type
        expected_error =  0x666   +  OtpmError.NON_ZERO.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.2.7')
    def test_otpm_verify_fast_slow_with_errors_dm(self):
        """ Checks OtpmVerify can run twice (fast mode then slow mode)
             on a single patch load
            Checks OtpmVerify can find and report the standard set of
             injected errors when run on a dm image with these errors
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expected error Address and        Type
        expected_error =  0x013   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

        # Expected error   Addresses and       Types
        expected_errors = [  0x013    +  OtpmError.ECC_DOUBLE.value,
                             0x014    +  OtpmError.ECC_SINGLE.value,
                             0x025    +  OtpmError.ECC_SINGLE.value,
                             0x023    +  OtpmError.ECC_DOUBLE.value,
                             0x02E    +  OtpmError.ECC_SINGLE.value,
                             0x030    +  OtpmError.ECC_DOUBLE.value,
                             0x039    +  OtpmError.ECC_DOUBLE.value,
                             0x041    +  OtpmError.ECC_SINGLE.value,
                             0x103    +  OtpmError.NON_ZERO.value,
                             0x271    +  OtpmError.ECC_SINGLE.value,
                             0x349    +  OtpmError.NON_ZERO.value,
                             0x4AD    +  OtpmError.ECC_DOUBLE.value,
                             0x5BC    +  OtpmError.ECC_SINGLE.value,
                             0x6E7    +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertNotEqual(response.blockLengthWords, 6)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)

    @testIdentifier('19.2.8')
    def test_otpm_verify_slow_fast_with_errors_dm(self):
        """ Checks OtpmVerify can run twice (slow mode then fast mode)
             on a single patch load
            Checks OtpmVerify can find and report the standard set of
             injected errors when run on a dm image with these errors
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expected error   Addresses and       Types
        expected_errors = [  0x013    +  OtpmError.ECC_DOUBLE.value,
                             0x014    +  OtpmError.ECC_SINGLE.value,
                             0x025    +  OtpmError.ECC_SINGLE.value,
                             0x023    +  OtpmError.ECC_DOUBLE.value,
                             0x02E    +  OtpmError.ECC_SINGLE.value,
                             0x030    +  OtpmError.ECC_DOUBLE.value,
                             0x039    +  OtpmError.ECC_DOUBLE.value,
                             0x041    +  OtpmError.ECC_SINGLE.value,
                             0x103    +  OtpmError.NON_ZERO.value,
                             0x271    +  OtpmError.ECC_SINGLE.value,
                             0x349    +  OtpmError.NON_ZERO.value,
                             0x4AD    +  OtpmError.ECC_DOUBLE.value,
                             0x5BC    +  OtpmError.ECC_SINGLE.value,
                             0x6E7    +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertNotEqual(response.blockLengthWords, 6)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)

        # Expected error Address and        Type
        expected_error =  0x013   +  OtpmError.ECC_DOUBLE.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])

    @testIdentifier('19.2.9')
    def test_otpm_verify_slow_with_ecc_single_all_locations_dm(self):
        """ Checks OtpmVerify can find a single bit ecc error in every
             readable location when a single bit ecc error has been injected
             into every OTPM location of a dm image
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertTrue(self.isValidChecksum(response))
        self.check_ecc_single_all_locations(response.dataBlock, self.unreadable_addresses_dm)

    @testIdentifier('19.2.10')
    def test_otpm_verify_fast_boundary_dm(self):
        """ Checks OtpmVerify can correctly identify the boundary between assets
             and freespace.
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Expect non-zero error at the first address of Freespace
        expected_boundary = 0x42 + OtpmError.NON_ZERO.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_boundary, response.dataBlock[0])

    @testIdentifier('19.2.11')
    def test_otpm_verify_invalid_parameter_dm(self):
        """ Checks OtpmVerify will return the error INVAL
             when given an incorrect parameter value
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Pass an invalid parameter to OtpmVerify
        self._otpm_verify(ProdTestAppMode.INVALID, expected=Error.INVAL)

    @testIdentifier('19.2.12')
    def test_otpm_blank_check_fast_no_errors_dm(self):
        """ Checks OtpmBlankCheck will not run in the DM LCS
        """
        self._check_system_ready_and_load_patch(LifecycleState.DM)

        # Pass an invalid parameter to OtpmVerify
        self._otpm_blank_check(ProdTestAppMode.FAST, expected=Error.ACCESS)

    #   19.3   SE LCS
    #  ------ --------
    @testIdentifier('19.3.1')
    def test_otpm_verify_slow_with_errors_se(self):
        """ Checks OtpmVerify can find and report the standard set of
             injected errors when run on a se image with these errors
        """
        self._check_system_ready_and_load_patch(LifecycleState.SECURE)

        # Expected error   Addresses and       Types
        expected_errors = [  0x013    +  OtpmError.ECC_DOUBLE.value,
                             0x014    +  OtpmError.ECC_SINGLE.value,
                             0x025    +  OtpmError.ECC_SINGLE.value,
                             0x023    +  OtpmError.ECC_DOUBLE.value,
                             0x02E    +  OtpmError.ECC_SINGLE.value,
                             0x030    +  OtpmError.ECC_DOUBLE.value,
                             0x039    +  OtpmError.ECC_DOUBLE.value,
                             0x041    +  OtpmError.ECC_SINGLE.value,
                             0x103    +  OtpmError.NON_ZERO.value,
                             0x271    +  OtpmError.ECC_SINGLE.value,
                             0x349    +  OtpmError.NON_ZERO.value,
                             0x4AD    +  OtpmError.ECC_DOUBLE.value,
                             0x5BC    +  OtpmError.ECC_SINGLE.value,
                             0x6E7    +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertNotEqual(response.blockLengthWords, 6)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)

    @testIdentifier('19.3.2')
    def test_otpm_verify_slow_with_ecc_single_all_locations_se(self):
        """ Checks OtpmVerify can find a single bit ecc error in every
             readable location when a single bit ecc error has been injected
             into every OTPM location of a se image
        """
        self._check_system_ready_and_load_patch(LifecycleState.SECURE)

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertTrue(self.isValidChecksum(response))
        self.check_ecc_single_all_locations(response.dataBlock, self.unreadable_addresses_se)

    @testIdentifier('19.3.3')
    def test_otpm_blank_check_fast_no_errors_se(self):
        """ Checks OtpmBlankCheck will not run in the SE LCS
        """
        self._check_system_ready_and_load_patch(LifecycleState.SECURE)

        # Pass an invalid parameter to OtpmVerify
        self._otpm_blank_check(ProdTestAppMode.FAST, expected=Error.ACCESS)

    #####################################################################################
    # Execute on DEPU provisioned device.
    ####################################################################################
    @testIdentifier('19.4.1')
    def test_encrypted_otpm_blank_check_fast_no_errors_cm(self):
        """ Checks OtpmBlankCheck will not run in the SE LCS even with encrypted patch
        """
        self.provision_device_with_depu()
        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PROD_TEST, key_set_name = self.key_set_name)

        # Issue GetStatus command and check for the expected LCS, and for the expected phase
        response = HostCommandGetStatus(self.camera).execute(timeout_usec=200000)
        # Pass an invalid parameter to OtpmVerify
        self._otpm_blank_check(ProdTestAppMode.FAST, expected=Error.ACCESS)

    @testIdentifier('19.4.2')
    def test_encrypted_otpm_verify_fast_with_ecc_single_cm(self):
        """  Checks OtpmVerify can find and report single sbe error in fast mode on provisioned device
        """
        self.provision_device_with_depu()
        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PROD_TEST, key_set_name = self.key_set_name)

        # Expected error Address and        Type
        expected_error =  0x700   +  OtpmError.ECC_SINGLE.value

        # Run OtpmVerify in fast mode
        response = self._otpm_verify(ProdTestAppMode.FAST)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.assertEqual(expected_error, response.dataBlock[0])


    @testIdentifier('19.4.3')
    def test_encrypted_otpm_verify_slow_with_ecc_single_cm(self):
        """  Checks OtpmVerify can find and report single sbe error in slow mode on provisioned device
        """
        self.provision_device_with_depu()
        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PROD_TEST, key_set_name = self.key_set_name)

        # Expected error   Addresses and       Types
        expected_error =  [ 0x700   +  OtpmError.ECC_SINGLE.value ]

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertEqual(response.blockLengthWords, 1)
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_error)

    @testIdentifier('19.4.4')
    def test_encrypted_otpm_verify_slow_with_multiple_errors_cm(self):
        """  Checks OtpmVerify can find and report multiple errors in slow mode on provisioned device
        """
        self.provision_device_with_depu()
        # The provisioning patch code should put the camera in LCS Secure mode.
        response = self.check_for_expected_status(SystemMgrPhase.DEBUG, LifecycleState.SECURE)

        self.assertEqual(self.rom_version, response.rom_version)
        self.assertEqual(0, response.patch_version)

        self.patch_loader.load_patch(LifecycleState.SECURE, PatchType.PROD_TEST, key_set_name = self.key_set_name)

        # Expected error   Addresses and       Types
        expected_errors =  [ 0x620   +  OtpmError.ECC_SINGLE.value,
                             0x640   +  OtpmError.ECC_DOUBLE.value,
                             0x660   +  OtpmError.ECC_SINGLE.value,
                             0x680   +  OtpmError.ECC_DOUBLE.value ]

        # Run OtpmVerify in slow mode
        response = self._otpm_verify(ProdTestAppMode.SLOW)
        self.assertEqual(response.blockLengthWords, len(expected_errors))
        self.assertTrue(self.isValidChecksum(response))
        self.check_dataBlock(response.dataBlock, expected_errors)
