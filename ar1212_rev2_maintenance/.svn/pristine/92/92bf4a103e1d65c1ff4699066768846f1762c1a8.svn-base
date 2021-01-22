#if !defined(SRC__LIFECYCLEMGR__INC__LIFECYCLEMGR_H)
#define      SRC__LIFECYCLEMGR__INC__LIFECYCLEMGR_H

//***********************************************************************************
/// \file
/// Lifecycle Manager public API
///
/// \defgroup lifecyclemgr   Lifecycle Manager
/// @{
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
//
//***********************************************************************************

#include "Common.h"
#include "SystemMgr.h"
#include "CommandHandler.h"

// Include the auto-generated lifecycle states
#include "LifecycleMgrLifecycleState.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Returns the current lifecycle state
extern ErrorT LifecycleMgrGetCurrentLcs(LifecycleMgrLifecycleStateT* const lcsCurrent);

// Handles the ApplyDebugEntitlement host command
extern ErrorT LifecycleMgrOnApplyDebugEntitlement(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the AuthorizeRMA host command
extern ErrorT LifecycleMgrOnAuthorizeRMA(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles entry to an operating phase
extern ErrorT LifecycleMgrOnPhaseEntry(SystemMgrPhaseT const phase);

/// @} endgroup lifecyclemgr
#endif  // !defined(SRC__LIFECYCLEMGR__INC__LIFECYCLEMGR_H)

