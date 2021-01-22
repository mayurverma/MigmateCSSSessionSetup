//***********************************************************************************
/// \file
///
/// Session Manager internal implementation
///
/// \addtogroup sessionmgr
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
//*************************************************************************************

#include <string.h>     // for memset()/memcpy()

#include "Common.h"
#include "Diag.h"

#include "SessionMgrInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

SessionMgrStateT sessionMgrState;

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// Dumps Session Mgr state in response to a fatal error
///
/// \param[in] dumpBuffer       Pointer to buffer to contain the dump
/// \param[in] sizeLongWords    Size of the dump buffer
///
/// \returns void
///
void SessionMgrOnDiagFatalErrorDump(DiagFatalErrorDumpBufferT dumpBuffer,
    uint32_t const sizeLongWords)
{
    uint32_t sizeBytes = sizeLongWords * (sizeof(uint32_t));

    if (NULL != dumpBuffer)
    {
        uint32_t dumpSizeBytes = sizeof(sessionMgrState);

        if (dumpSizeBytes > sizeBytes)
        {
            dumpSizeBytes = sizeBytes;
        }

        // Erase the 'secrets' before dumping the state variables
        sessionMgrState.certificateState.publicKey = NULL;
        sessionMgrState.certificateState.publicKeySizeBytes = 0U;

        (void)memset(sessionMgrState.sessionState.sessionParams.controlChannelNonce,
            0, sizeof(sessionMgrState.sessionState.sessionParams.controlChannelNonce));

        (void)memset(sessionMgrState.sessionState.sessionKeys.videoAuthKey,
            0, sizeof(sessionMgrState.sessionState.sessionKeys.videoAuthKey));

        (void)memset(sessionMgrState.sessionState.sessionKeys.controlChannelKey,
            0, sizeof(sessionMgrState.sessionState.sessionKeys.controlChannelKey));

        // Copy the state variables into the specified buffer
        (void)memcpy((void*)dumpBuffer, (void*)&sessionMgrState, dumpSizeBytes);
    }
}

/// @} endgroup sessionmgr

