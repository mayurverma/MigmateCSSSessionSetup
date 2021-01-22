//***********************************************************************************
/// \file
///
/// Patch Table configuration
///
/// \addtogroup patch
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
#include <string.h>

#include "Patch.h"
#include "PatchMgr.h"

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------
#define PATCH_NUM_PATCHED_FUNCTIONS 1
//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// The contents of this table (patchPatchedFunctionTable) and the comparators table (patchComparatorTable) must be kept in sync
/// so the function indexes have to match its patched version.
/// The 'jump' table used by SVC handler and configured by the Loader on successfully receiving a valid set of
/// 'LoadPatchChunk commands consists of 32 entries (matching the number of comparators in the hardware)
const PatchMgrPatchedFunctionAddressT patchPatchedFunctionTable[PATCH_NUM_PATCHED_FUNCTIONS] =
{
    (PatchMgrPatchedFunctionAddressT)NULL
};

/// The contents of this table (comparators) should be in sync with the patchFunctionTable
const PatchMgrComparatorFunctionAddressT patchComparatorTable[] =
{
    (PatchMgrComparatorFunctionAddressT)NULL
};

/// @} endgroup patch

