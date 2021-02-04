#if !defined(SRC__SESSIONMGR__INC__SESSIONMGR_H)
#define      SRC__SESSIONMGR__INC__SESSIONMGR_H

//***********************************************************************************
/// \file
/// Session Manager public API
///
/// \defgroup sessionmgr   Session Manager
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

// Handles entry to an operating phase
extern ErrorT SessionMgrOnPhaseEntry(SystemMgrPhaseT const phase);

// Handles the GetCertificates host command
extern ErrorT SessionMgrOnGetCertificates(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the SetEphemeralMasterSecret host command
extern ErrorT SessionMgrOnSetEphemeralMasterSecret(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the SetPskSessionKey host command
extern ErrorT SessionMgrOnSetPskSessionKeys(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the SetSessionKeys host command
extern ErrorT SessionMgrOnSetSessionKeys(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Handles the SetVideoAuthROI host command
extern ErrorT SessionMgrOnSetVideoAuthROI(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);

// Decrypts a host message params
extern ErrorT SessionMgrDecryptCommandRequestParams(HostCommandCodeT const commandCode,
    uint8_t* const encryptedParams,
    uint8_t* const decryptedParams,
    uint32_t const paramSizeBytes);

/// @} endgroup sessionmgr
#endif  // !defined(SRC__SESSIONMGR__INC__SESSIONMGR_H)

