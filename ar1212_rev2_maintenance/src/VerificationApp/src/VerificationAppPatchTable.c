//***********************************************************************************
/// \file
///
/// Patch Table configuration
///
/// \addtogroup verificationapp
/// @{
//***********************************************************************************
// Copyright 2019 ON Semiconductor.  All rights reserved.
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
#include "VerificationAppInternal.h"
#include "SystemMgr.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// The contents of this table (verificationAppPatchedFunctionTable) and the comparators table
/// (verificationAppComparatorTable) must be kept in sync so the function indexes have to match its patched version.
/// The 'jump' table used by SVC handler and configured by the Loader on successfully receiving a valid set of
/// 'LoadPatchChunk commands consists of 32 entries (matching the number of comparators in the hardware)
const PatchMgrPatchedFunctionAddressT verificationAppPatchedFunctionTable[VERIFICATION_APP_NUM_PATCHED_FUNCTIONS] =
{
    (PatchMgrPatchedFunctionAddressT)SystemMgrGetRomVersionPatched,
    (PatchMgrPatchedFunctionAddressT)MacDrvGetCmacStatusPatched,
};

/// The contents of this table (comparators) should be in sync with the patchFunctionTable
const PatchMgrComparatorFunctionAddressT verificationAppComparatorTable[VERIFICATION_APP_NUM_PATCHED_FUNCTIONS] =
{
    (PatchMgrComparatorFunctionAddressT)SystemMgrGetRomVersion,
    (PatchMgrComparatorFunctionAddressT)MacDrvGetCmacStatus,
};

/// @} endgroup verificationapp

