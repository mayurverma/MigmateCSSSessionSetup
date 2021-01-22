//***********************************************************************************
/// \file
///
/// Command Handler internal data
///
/// \addtogroup commandhandler
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

#include "HealthMonitor.h"
#include "LifecycleMgr.h"
#include "CryptoMgr.h"
#include "SessionMgr.h"
#include "AssetMgr.h"
#include "PatchMgr.h"
#include "CommandHandlerInternal.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

/// The ROM host command table
const CommandHandlerCommandTableEntryT commandHandlerROMCommandTable[COMMAND_HANDLER_NUM_COMMANDS_ROM_TABLE] =
{
    {
        HOST_COMMAND_CODE_GET_STATUS,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_DEBUG) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_BOOT) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_INITIALIZE) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SHUTDOWN),
        HealthMonitorOnGetStatus
    },

    {
        HOST_COMMAND_CODE_APPLY_DEBUG_ENTITLEMENT,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_DEBUG),
        LifecycleMgrOnApplyDebugEntitlement
    },

    {
        HOST_COMMAND_CODE_AUTHORIZE_RMA,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_DEBUG),
        LifecycleMgrOnAuthorizeRMA
    },

    {
        HOST_COMMAND_CODE_LOAD_PATCH_CHUNK,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_DEBUG) | \
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_PATCH),
        PatchMgrOnLoadPatchChunk
    },

    {
        HOST_COMMAND_CODE_PROVISION_ASSET,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        AssetMgrOnProvisionAsset
    },

    {
        HOST_COMMAND_CODE_SELF_TEST,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        CryptoMgrOnSelfTest
    },

    {
        HOST_COMMAND_CODE_GET_SENSOR_ID,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        CryptoMgrOnGetSensorId
    },

    {
        HOST_COMMAND_CODE_GET_CERTIFICATES,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        SessionMgrOnGetCertificates
    },

    {
        HOST_COMMAND_CODE_SET_SESSION_KEYS,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        SessionMgrOnSetSessionKeys
    },

    {
        HOST_COMMAND_CODE_SET_PSK_SESSION_KEYS,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        SessionMgrOnSetPskSessionKeys
    },

    {
        HOST_COMMAND_CODE_SET_EPHEMERAL_MASTER_SECRET,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_CONFIGURE),
        SessionMgrOnSetEphemeralMasterSecret
    },

    {
        HOST_COMMAND_CODE_SET_VIDEO_AUTH_ROI,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION),
        SessionMgrOnSetVideoAuthROI
    },

    {
        HOST_COMMAND_CODE_ENABLE_MONITORING,
        COMMAND_HANDLER_SET_PHASE_MASK(SYSTEM_MGR_PHASE_SESSION),
        HealthMonitorOnEnableMonitoring
    }
};

/// @} endgroup commandhandler

