//***********************************************************************************
/// \file
///
/// System Manager default Mocks
//
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

#include "cpputest_support.h"

extern "C"
{
#include "SessionMgr.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_GET_CERTIFICATES)
    ErrorT SessionMgrOnGetCertificates(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnGetCertificates")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_PHASE_ENTRY)
    ErrorT SessionMgrOnPhaseEntry(SystemMgrPhaseT const phase)
    {
        mock().actualCall("SessionMgrOnPhaseEntry")
            .withParameter("phase", phase);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_SET_EPHEMERAL_MASTER_SECRET)
    ErrorT SessionMgrOnSetEphemeralMasterSecret(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetEphemeralMasterSecret")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_SET_PSK_SESSION_KEYS)
    ErrorT SessionMgrOnSetPskSessionKeys(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetPSKSessionKey")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_SET_SESSION_KEYS)
    ErrorT SessionMgrOnSetSessionKeys(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetSessionKey")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_ON_SET_VIDEO_AUTH_ROI)
    ErrorT SessionMgrOnSetVideoAuthROI(HostCommandCodeT const commandCode,
        CommandHandlerCommandParamsT params)
    {
        mock().actualCall("SessionMgrOnSetVideoAuthROI")
            .withParameter("commandCode", commandCode)
            .withParameter("params", params);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SESSION_MGR_DECRYPT_COMMAND_REQUEST_PARAMS)
    ErrorT SessionMgrDecryptCommandRequestParams(HostCommandCodeT const commandCode,
            uint8_t* const encryptedParams,
            uint8_t* const decryptedParams,
            uint32_t const paramSizeBytes)
    {
        mock().actualCall("SessionMgrDecryptCommandRequestParams")
            .withParameter("commandCode", commandCode)
            .withParameter("encryptedParams", encryptedParams)
            .withOutputParameter("decryptedParams", decryptedParams)
            .withParameter("paramSizeBytes", paramSizeBytes);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
}
