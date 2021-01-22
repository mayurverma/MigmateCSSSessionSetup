#######################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Softwar").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
########################################################################################
#
# IRQ definition
#
########################################################################################
from enum import Enum

class IRQnType(Enum):
    # Internal interrupts/exceptions
    NMI_Handler = 1010         # NMI Handler
    HardFault_Handler = 1011   # Hard Fault Handler
    Reserved_Handler1 = 1012   # Reserved
    Reserved_Handler2 = 1013   # Reserved
    Reserved_Handler3 = 1014   # Reserved
    Reserved_Handler4 = 1015   # Reserved
    Reserved_Handler5 = 1016   # Reserved
    Reserved_Handler6 = 1017   # Reserved
    Reserved_Handler7 = 1018   # Reserved
    SVC_Handler = 1019         # SVCall Handler
    Reserved_Handler8 = 1020   # Reserved
    Reserved_Handler9 = 1021   # Reserved
    PendSV_Handler = 1022      # PendSV Handler
    SysTick_Handler = 1023     # SysTick Handler
    
    # External Interrupts
    Doorbell_IRQn = 0          # Command register doorbell interrupt (pulsed)
    SOF_IRQn = 1               # Start-of-frame interrupt (pulsed)
    EOF_IRQn = 2               # End-of-frame interrupt (pulsed)
    SOL_IRQn = 3               # Start-of-line interrupt (pulsed)
    EOL_IRQn = 4               # End-of-line interrupt (pulsed)
    CCIsIdle_IRQn = 5          # CC312 is_idle signal (pulsed)
    CCPowerdownReady_IRQn = 6  # CC312 powerdown-ready signal (pulsed)
    CCHostIntReq_IRQn = 7      # CC312 host interrupt request signal (pulsed)
    PllLocked_IRQn = 8         # Sensor PLL locked (pulsed)
    PllNotLocked_IRQn = 9      # Sensor PLL not locked (pulsed)
    IcoreEccDed_IRQn = 10      # Icore RAM ECC DED Failure interrupt
    IcoreEccSec_IRQn = 11      # Icore RAM ECC SEC Correction interrupt
    Reserved12_IRQn = 12       # Reserved
    DualTimerInt1_IRQn = 13    # Dual timer interrupt 1 (pulsed)
    DualTimerInt2_IRQn = 14    # Dual timer interrupt 2 (pulsed)
    DualTimerIntC_IRQn = 15    # Dual timer interrupt C (common) (pulsed)
    Watchdog_IRQn = 16         # Watchdog timer interrupt (pulsed)
    CyberRomDed_IRQn = 17      # Cyber ROM ECC DED Failure (pulsed)
    CyberRomSec_IRQn = 18      # Cyber ROM ECC single bit correction (pulsed)
    CyberRamDed_IRQn = 19      # Cyber RAM ECC DED Failure (pulsed)
    CyberRamSec_IRQn = 20      # Cyber RAM ECC single bit correction (pulsed)
    Reserved21_IRQn = 21       # Reserved
    Reserved22_IRQn = 22       # Reserved
    Reserved23_IRQn = 23       # Reserved
    Reserved24_IRQn = 24       # Reserved
    Reserved25_IRQn = 25       # Reserved
    Reserved26_IRQn = 26       # Reserved
    Reserved27_IRQn = 27       # Reserved
    Reserved28_IRQn = 28       # Reserved
    Reserved29_IRQn = 29       # Reserved
    Reserved30_IRQn = 30       # Reserved
    Reserved31_IRQn = 31       # Reserved

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

