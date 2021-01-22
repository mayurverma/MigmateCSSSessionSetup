#if !defined(SRC__LIFECYCLEMGR__SRC__LIFECYCLEMGRINTERNAL_H)
#define      SRC__LIFECYCLEMGR__SRC__LIFECYCLEMGRINTERNAL_H
//******************************************************************************
/// \file
///
/// Provides LifecycleMgr internal header file
///
/// \addtogroup lifecyclemgr
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "Common.h"
#include "Diag.h"
#include "dx_crys_kernel.h"

#if defined(BUILD_TARGET_ARCH_x86)
// To support MinGW32 based unit testing the crypto memory is mapped to an internal buffer as the actual
// memory is outside the address range of a MinGW32 build.
extern uint8_t cryptoMemory[(DX_HOST_DCU_EN0_REG_OFFSET + 16U)];
#undef CRYPTOCELL_CORE_BASE

#define CRYPTOCELL_CORE_BASE ((unsigned long)&cryptoMemory[0])
#endif

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------
/// Life Cycle Manager control/status structure
typedef struct
{
    bool debugEntitlementApplied;       ///< If true Debug entitlement applied
} LifecycleMgrT;

/// Apply Debug Entitlement structure (header plus first location of blob)
typedef struct
{
    uint32_t clockSpeedHz;              ///< Crypto subsystem clock speed in Hertz
    uint32_t blobSizeWords;             ///< Size of the entitlement BLOB
    uint32_t entitlement;               ///< Encrypted debug entitlement BLOB
} LifecycleMgrApplyDebugEntitlementHeaderT;
//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
// Lifecycle Manager internal control/state structure
extern LifecycleMgrT lifecycleMgr;

// Initializes internal control structure
extern ErrorT LifecycleMgrInit(void);
// Initialize the CC312 Secure Boot library
extern ErrorT LifecycleMgrCc312Init(void);
// Reports a SW fatal error
extern void LifecycleMgrSwFatalError(SystemMgrFatalErrorT const context, uint32_t const infoParam);
// Reports a HW fatal error
extern void LifecycleMgrHwFatalError(SystemMgrFatalErrorT const context, uint32_t const infoParam);
// Reads DCU bit controlling access to info register
extern bool LifecycleMgrGetInfoEnable(void);
// Dumps Lifecycle Manager state in response to a fatal error
extern void LifecycleMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer, uint32_t const sizeLongWords);

/// @} endgroup lifecyclemgr
#endif  // !defined(SRC__LIFECYCLEMGR__SRC__LIFECYCLEMGRINTERNAL_H)

