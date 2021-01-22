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
# Test fault paths
####################################################################################
import logging

from pyfwtest import *
from base_test_case import BaseTestCase
from common_error import Error
from lifecycle_state import LifecycleState

from patch_type import PatchType
from diag_helper import DiagHelper
from systemmgr_css_fault import SystemMgrCssFault
from systemmgr_fatal_error import SystemMgrFatalError

from host_command_get_status import HostCommandGetStatus
from host_command_execute_debug import HostCommandExecuteDebug
from verification_app_debug_id import DebugId
from irq_types import IRQnType
from reset_types import ResetType

class TestFaults(BaseTestCase):
    """Test fault paths"""

    def setUp(self):
        # Run base class setup method
        super().setUp()

        self.verbose = True
        self.rom_version = self.patch_loader.get_rom_version()
        self.patch_version = self.patch_loader.get_patch_version(PatchType.VERIFICATION)
        self.diag_helper = DiagHelper(self.camera)
        self.logger.debug('setUp complete')


    @testIdentifier('3.8.1')
    def test_lifecycle_mgr_sw_fatal_error_cm(self):
        """ Execute LifecycleMgrSwFatalError() using test patch and check response """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Generate crypto fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.LIFECYCLE_MGR_SW_FATAL_ERROR, expected = Error.SYSERR)

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.CRYPTOCELL_SW_ERROR, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

    @testIdentifier('3.8.2')
    def test_icore_ecc_ded_fatal_error_cm(self):
        """ Trigger icore memory fatal error using test patch and check response """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Generate crypto fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter = IRQnType.IcoreEccDed_IRQn.value)

        # Wait for system to go fatal
        self.fixture.wait_usec(10000)        #10 mSec

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.ICORE_ECC_FAILURE, fault)
        self.assertEqual(SystemMgrFatalError.NONE, context)

    @testIdentifier('3.8.3')
    def test_icore_ecc_sec_unexpected_error_cm(self):
        """ Trigger icore memory unexpected error using test patch and check response """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Generate crypto fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter = IRQnType.IcoreEccSec_IRQn.value, expected = Error.SYSERR)

        # Wait for system to go fatal
        self.fixture.wait_usec(10000)        #10 mSec

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION, fault)
        self.assertEqual(IRQnType.IcoreEccSec_IRQn.value, context.value)

    @testIdentifier('3.8.4')
    def test_cc_power_down_ready_unexpected_irq_cm(self):
        """ Trigger CCPowerdownReady_IrqHandler() using test patch and check response """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Generate crypto fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
            parameter = IRQnType.CCPowerdownReady_IRQn.value, expected = Error.SYSERR)

        # Wait for system to go fatal
        self.fixture.wait_usec(10000)        #10 mSec

        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION, fault)
        self.assertEqual(IRQnType.CCPowerdownReady_IRQn.value, context.value)

    @testIdentifier('3.8.5')
    def test_sys_tick_unexpected_irq_cm(self):
        """ Trigger SysTick_Handler() using test patch and check response """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        status = HostCommandGetStatus(self.camera).execute()
        self.assertEqual(0xCAFE, status.rom_version)

        # Generate fault
        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYS_TICK,
            expected = Error.SYSERR)

        fault, context = self.diag_helper.get_css_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION.value, fault)
        self.assertEqual(IRQnType.SysTick_Handler, context)

    @testIdentifier('3.8.6')
    def test_all_unexpected_irq_cm(self):
        """ Trigger all unexpected IRQ handlers using test patch and check response """
        # Check system is ready

        excluded_irq = [IRQnType.Doorbell_IRQn, IRQnType.IcoreEccSec_IRQn, IRQnType.IcoreEccDed_IRQn,
            IRQnType.Watchdog_IRQn]

        for irq in IRQnType:
            if irq not in excluded_irq and irq.value < IRQnType.NMI_Handler.value:
                self.logger.info('>>> Testing for unexpected IRQ "{}" <<<'.format(irq.name))
                self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

                status = HostCommandGetStatus(self.camera).execute()
                self.assertEqual(0xCAFE, status.rom_version)

                # Generate crypto fault
                response = HostCommandExecuteDebug(self.camera).execute(DebugId.TRIGGER_INTERRUPT,
                    parameter = irq.value, expected = Error.SYSERR)

                # Wait for system to go fatal
                self.fixture.wait_usec(10000)        #10 mSec

                fault, context = self.diag_helper.get_css_error()
                self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION.value, fault)
                self.assertEqual(irq, context)

                # Reset device and change IRQ handler
                self.camera.reset(ResetType.SENSOR_HARD)
                self.logger.info('Resetting device......')
                self.camera.wait_usec(100)

    @testIdentifier('3.8.7')
    def test_force_optm_ded_cm(self):
        """ Set OTPM DED bit in register and check it is interpreted correctly using OtpmDrvIsHardwareFault() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.FORCE_OTPM_ECC_DED, expected = Error.SUCCESS)
        self.assertEqual(0x2, response.result & 0x2, "DED bit not set")

    @testIdentifier('3.8.8')
    def test_execute_nmi_handler_cm(self):
        """ Direct call to NMI_Handler() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.NMI_HANDLER, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context = self.diag_helper.get_css_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION.value, fault)
        self.assertEqual(IRQnType.NMI_Handler, context)

    @testIdentifier('3.8.9')
    def test_execute_reserved_handler_cm(self):
        """ Direct call to Reserved_Handler() """
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.RESERVED_HANDLER, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context = self.diag_helper.get_css_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION.value, fault)
        self.assertEqual(IRQnType.Reserved_Handler1, context)

    @testIdentifier('3.8.10')
    def test_invalid_thumb_state_cm(self):
        """ Generate a HardFault by calling a function without the Thumb bit set """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.BAD_FUNCTION, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.PROCESSOR_STATE.value, fault)
        self.assertEqual(2, context)        # INIT_HARD_FAULT_PROCESSOR_STATE_CONTEXT_THUMB_CLEAR

    @testIdentifier('3.8.11')
    def test_unaligned_access_cm(self):
        """ Generate a HardFault via an unaligned access """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.UNALIGNED_ACCESS, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.HARDFAULT.value, fault)

    @testIdentifier('3.8.12')
    def test_pending_sv_cm(self):
        """ Set the aborted flag and trigger PendSV """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.PENDSV, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.UNEXPECTED_EXCEPTION.value, fault)
        self.assertEqual(IRQnType.PendSV_Handler, context)

    @testIdentifier('3.8.13')
    def test_illegal_instruction_cm(self):
        """ Generate a HardFault via an illegal instruction """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.ILLEGAL_INSTRUCTION, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        # considering InitHardFaultHandler when hardfault :  info = PC [15..0] context = PC [25..16]
        program_counter = info + (context << 16)
        # check the PC is in the heap region as the verification app should execute the bad instruction from there..
        self.assertTrue(0xD000 <= program_counter <= (0xD000 + 0x2000), "unexpected program counter = 0x{:x}".format(program_counter))
        self.assertEqual(SystemMgrCssFault.HARDFAULT.value, fault)

    @testIdentifier('3.9.1')
    def test_sys_exit_cm(self):
        """ Execute sys_exit(), not reachable in reality"""
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        try:
            response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYS_EXIT, expected = Error.SUCCESS, timeout_usec = 7000)
        except TimeoutError as e:
            self.logger.info("Timeout received, as expected")
        else:
            self.assertTrue(False, "Expected timeout")

    @testIdentifier('3.9.2')
    def test_sys_tty_chr_cm(self):
        """ Execute _ttywrch()"""
        # Check system is ready

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(DebugId.SYS_TTY_CHR, expected = Error.SUCCESS)
        status = HostCommandGetStatus(self.camera).execute()


    def test_mpu_common(self, debug_id):
        """ Common function for all mpu test """

        self._common_setup(LifecycleState.CM, PatchType.VERIFICATION)

        response = HostCommandExecuteDebug(self.camera).execute(debug_id, expected = Error.SYSERR)
        self.camera.wait_usec(3000)
        fault, context, info = self.diag_helper.check_for_fatal_error()
        self.assertEqual(SystemMgrCssFault.HARDFAULT.value, fault)
        return context, info

    @testIdentifier('3.10.1')
    def test_mpu_write_start_rom_cm(self):
        """ Generate a HardFault via write to start of ROM """
        context, info = self.test_mpu_common(DebugId.MPU_WRITE_START_ROM)

    @testIdentifier('3.10.2')
    def test_mpu_write_end_rom_cm(self):
        """ Generate a HardFault via write to end of ROM """
        context, info = self.test_mpu_common(DebugId.MPU_WRITE_END_ROM)

    @testIdentifier('3.10.3')
    def test_mpu_read_end_rom_cm(self):
        """ Generate a HardFault via read past end of ROM """
        context, info = self.test_mpu_common(DebugId.MPU_READ_END_ROM)

    @testIdentifier('3.10.4')
    def test_mpu_execute_end_rom_cm(self):
        """ Generate a HardFault via execute past end of ROM """
        context, info = self.test_mpu_common(DebugId.MPU_EXECUTE_END_ROM)
        program_counter = info + (context << 16)
        self.assertEqual(0x40000, program_counter)

    @testIdentifier('3.10.5')
    def test_mpu_execute_patch_ram_cm(self):
        """ Generate a HardFault via execute from patch RAM """
        context, info = self.test_mpu_common(DebugId.MPU_EXECUTE_PATCH_RAM)

    @testIdentifier('3.10.6')
    def test_mpu_execute_ram_cm(self):
        """ Generate a HardFault via execute from data RAM """
        context, info = self.test_mpu_common(DebugId.MPU_EXECUTE_RAM)
        program_counter = info + (context << 16)
        self.assertEqual(0xC000, program_counter)

    @testIdentifier('3.10.7')
    def test_mpu_read_end_stack_cm(self):
        """ Generate a HardFault via read beyond the end of the stack """
        context, info = self.test_mpu_common(DebugId.MPU_READ_END_STACK)

    @testIdentifier('3.10.8')
    def test_mpu_read_stack_guard_cm(self):
        """ Generate a HardFault via read within the stack guard """
        context, info = self.test_mpu_common(DebugId.MPU_READ_STACK_GUARD)

    @testIdentifier('3.10.9')
    def test_mpu_execute_shared_ram_cm(self):
        """ Generate a HardFault via execute from ICore shared-memory """
        context, info = self.test_mpu_common(DebugId.MPU_EXECUTE_SHARED_MEM)
        program_counter = info + (context << 16)
        self.assertEqual(0x3FF1000, program_counter)

    @testIdentifier('3.10.10')
    def test_mpu_read_end_shared_ram_cm(self):
        """ Generate a HardFault via read beyond the end of the shared-memory """
        context, info = self.test_mpu_common(DebugId.MPU_READ_END_SHARED_MEM)

    @testIdentifier('3.10.11')
    def test_mpu_execute_vcore_address_cm(self):
        """ Generate a HardFault via execute from the Vcore region """
        context, info = self.test_mpu_common(DebugId.MPU_EXECUTE_VCORE_ADDR)
        program_counter = info + (context << 16)
        self.assertEqual(0x004000, program_counter)

    @testIdentifier('3.10.12')
    def test_mpu_read_end_vcore_address_cm(self):
        """ Generate a HardFault via read beyond the end of the Vcore region """
        context, info = self.test_mpu_common(DebugId.MPU_READ_END_VCORE_ADDR)



