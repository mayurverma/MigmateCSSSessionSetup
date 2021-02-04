#ifndef SRC__SYSTEMMGR__INC__SYSTEMMGR_CSS_FAULT
#define SRC__SYSTEMMGR__INC__SYSTEMMGR_CSS_FAULT
//***********************************************************************************
/// \file
///
/// CSS fault codes.
///
//***********************************************************************************
// Copyright 2018 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//***********************************************************************************

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT EDIT - This file is auto-generated by the generate_css_fault.py script
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//------------------------------------------------------------------------------
//                               CSS Fault Codes
//------------------------------------------------------------------------------

// *INDENT-OFF*

/// CSS fault codes
typedef enum
{
    SYSTEM_MGR_CSS_FAULT_NONE = 0x00U,                                     ///< (0) None
    SYSTEM_MGR_CSS_FAULT_WATCHDOG = 0x01U,                                 ///< (1) Processor watchdog expired (firmware dead-lock/runaway)
    SYSTEM_MGR_CSS_FAULT_FIRMWARE_FATAL = 0x02U,                           ///< (2) Firmware detected fatal error
    SYSTEM_MGR_CSS_FAULT_FIRMWARE_TERMINATED = 0x03U,                      ///< (3) Firmware normal termination
    SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_SW_ERROR = 0x04U,                      ///< (4) Cryptocell software reported error
    SYSTEM_MGR_CSS_FAULT_CRYPTOCELL_HW_ERROR = 0x05U,                      ///< (5) Cryptocell hardware reported error
    SYSTEM_MGR_CSS_FAULT_CMAC_ENGINE_ERROR = 0x06U,                        ///< (6) CMAC hardware reported error
    SYSTEM_MGR_CSS_FAULT_GMAC_ENGINE_ERROR = 0x07U,                        ///< (7) GMAC hardware reported error
    SYSTEM_MGR_CSS_FAULT_OTPM_ERROR = 0x08U,                               ///< (8) OTPM hardware reported error
    SYSTEM_MGR_CSS_FAULT_ICORE_ECC_FAILURE = 0x09U,                        ///< (9) ICore shared-memory ECC DED
    SYSTEM_MGR_CSS_FAULT_PROCESSOR_LOCKUP = 0x0AU,                         ///< (10) Processor LOCK_UP signal asserted (firmware terminated)
    SYSTEM_MGR_CSS_FAULT_PROCESSOR_STATE = 0x0BU,                          ///< (11) Processor state is incorrect
    SYSTEM_MGR_CSS_FAULT_HARDFAULT = 0x0CU,                                ///< (12) HardFault handler triggered
    SYSTEM_MGR_CSS_FAULT_UNEXPECTED_EXCEPTION = 0x0DU,                     ///< (13) Unexpected exception or interrupt
    SYSTEM_MGR_CSS_FAULT_ROM_ECC_FAILURE = 0x0EU,                          ///< (14) Processor ROM ECC failure
    SYSTEM_MGR_CSS_FAULT_RAM_ECC_FAILURE = 0x0FU,                          ///< (15) Processor RAM ECC failure
    SYSTEM_MGR_CSS_FAULT_STACK_OVERFLOW = 0x10U,                           ///< (16) RAM stack overflow failure

    SYSTEM_MGR_CSS_FAULT_MAX = 0x3FU,                                      ///< (63) Maximum fault code value
} SystemMgrCssFaultT;

#endif /* SRC__SYSTEMMGR__INC__SYSTEMMGR_CSS_FAULT */
