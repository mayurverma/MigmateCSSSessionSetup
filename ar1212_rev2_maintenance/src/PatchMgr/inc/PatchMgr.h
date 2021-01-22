#if !defined(SRC__PATCHMGR__INC__PATCHMGR_H)
#define      SRC__PATCHMGR__INC__PATCHMGR_H

//***********************************************************************************
/// \file
/// PatchMgr.h
///
/// \defgroup patchmgr   Patch Manager
///
///  A firmware patch is divided into one or more 'chunks' and downloaded to the device using the command
/// 'LoadPatchChunk' for each of the created 'chunks'. The PatchMgr component is responsible for handling the
/// 'LoadPatchChunk', checking it has been called in the correct manor and order, verifying the data (decrypting
///  if not in CM lifecycle state), loading into patch memory then calling the loader function.
///
///  The patch handler will call the patch loader once a valid patch has been applied. This loader is responsible
///  for configuring a software based jump table and configuring the associated hardware.
///
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

#define PATCH_MGR_MAX_PATCH_TABLE_ENTRIES 32U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Represents a patch version
typedef uint16_t PatchMgrPatchVersionT;

/// Loader function type
typedef ErrorT (* PatchMgrLoaderT)(void);

/// New Function to patch a existing one
typedef void (* PatchMgrPatchedFunctionAddressT)(void);

/// Function to be patched (from ROM)
typedef void (* PatchMgrComparatorFunctionAddressT)(void);

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

// Handles the LoadPatchChunk host command
extern ErrorT PatchMgrOnLoadPatchChunk(HostCommandCodeT const commandCode,
    CommandHandlerCommandParamsT params);
// Handles entry to an operating phase
extern ErrorT PatchMgrOnPhaseEntry(SystemMgrPhaseT const phase);
// Retrieves the current patch version (if loaded)
extern PatchMgrPatchVersionT PatchMgrGetPatchVersion(void);

// Add a function pointer to the comparator table
extern ErrorT PatchMgrSetupComparator(uint32_t const index, PatchMgrComparatorFunctionAddressT const function);
// Disable comparator
extern ErrorT PatchMgrDisableComparator(uint32_t const index);
// Add a function pointer to the comparator table and the patched function to the patch function table
extern ErrorT PatchMgrSetupComparatorAndPatchedTable(uint32_t const index,
    PatchMgrComparatorFunctionAddressT const function,
    PatchMgrPatchedFunctionAddressT const functionPatched,
    PatchMgrComparatorFunctionAddressT* const patchedFunctionTable,
    uint32_t const sizeTable);
// This function updates the comparators table
extern ErrorT PatchMgrSetupComparatorTable(uint32_t const startComparator,
    uint32_t const numComparators,
    PatchMgrComparatorFunctionAddressT const* const comparatorTable,
    uint32_t const sizeTable);

/// @} endgroup patchmgr
#endif  // !defined(SRC__PATCHMGR__INC__PATCHMGR_H)

