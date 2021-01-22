//***********************************************************************************
/// \file
///
/// PatchMgr common/utility functions for Patches
///
/// \addtogroup patchmgr
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
#include <string.h>

#include "Common.h"
#include "AR0820.h"
#include "PatchMgr.h"
#include "patcher.h"
#if defined(BUILD_TARGET_ARCH_x86)
#include "PatcherUnitTest.h"
#endif

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

/// Comparators Map of the Hardware (PATCHER_REGS)
typedef struct
{
    /// Function Address array list to simplify implementation
    __IOM PatchMgrComparatorFunctionAddressT function[PATCH_MGR_MAX_PATCH_TABLE_ENTRIES];
} PatchMgrComparatorsRegsT;

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

/// This function updates the comparator for a patched function given a specific index
///
/// \param[in]   index        Index for the comparator
/// \param[in]   function     Pointer to the function to be patched
/// \returns Error status
/// \retval ERROR_SUCCESS               Comparator loaded successfully.
/// \retval ERROR_SYSERR                Parameters are incorrect
///
ErrorT PatchMgrSetupComparator(uint32_t const index, PatchMgrComparatorFunctionAddressT const function)
{
    ErrorT result = ERROR_SUCCESS;
    PATCHER_REGS_T* patcherRegs = PATCHER_REGS;
    // This variable maps the comparators Hardware (PATCHER_REGS) and allows the usage of indexing.
    PatchMgrComparatorsRegsT* comparators = (PatchMgrComparatorsRegsT*)PATCHER_REG_BASE;

    if ((index < PATCH_MGR_MAX_PATCH_TABLE_ENTRIES) && (function != NULL))
    {
        patcherRegs->control = 0U;  // Instruction only compare
        comparators->function[index] = function;
        patcherRegs->enable |= ((uint32_t)0x1U) << index;
    }
    else
    {
        result = ERROR_SYSERR;
    }

    return result;
}

/// This function disables a comparator for a patched function given a specific index
///
/// \param[in]   index        Index for the comparator
/// \returns Error status
/// \retval ERROR_SUCCESS               Comparator disabled successfully.
/// \retval ERROR_SYSERR                Index out of range
///
ErrorT PatchMgrDisableComparator(uint32_t const index)
{
    ErrorT result = ERROR_SUCCESS;
    PATCHER_REGS_T* patcherRegs = PATCHER_REGS;

    if (index < PATCH_MGR_MAX_PATCH_TABLE_ENTRIES)
    {
        patcherRegs->enable &= ~(((uint32_t)0x1U) << index);
    }
    else
    {
        result = ERROR_SYSERR;
    }

    return result;
}

/// This function updates the comparator and the patched function to the patch function table
/// for a patched function given a specific index
///
/// \param[in]   index             Index for the comparator
/// \param[in]   function          Pointer to the function to be patched
/// \param[in]   functionPatched   Pointer to the replacement function
/// \param[in]   patchedFunctionTable   Pointer to the Patched Function Table (normally located at the beginning of the Patch space)
/// \param[in]   sizeTable         Size of the patchComparatorTable
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Comparator loaded successfully.
/// \retval ERROR_SYSERR                Parameters are incorrect
///
ErrorT PatchMgrSetupComparatorAndPatchedTable(uint32_t const index,
    PatchMgrComparatorFunctionAddressT const function,
    PatchMgrPatchedFunctionAddressT const functionPatched,
    PatchMgrComparatorFunctionAddressT* const patchedFunctionTable,
    uint32_t const sizeTable)
{
    ErrorT result = ERROR_SYSERR;

    if ((functionPatched != NULL) &&
        (patchedFunctionTable != NULL) &&
        (index < sizeTable))
    {
        result = PatchMgrSetupComparator(index, function);

        if (ERROR_SUCCESS == result)
        {
            patchedFunctionTable[index] = functionPatched;
        }
    }

    return result;
}

/// This function updates the comparators table
///
/// \param[in]   startComparator        First index for the comparator function (should be normally 0)
/// \param[in]   numComparators         Number of Comparators to setup from the index (should be the size of the Table normally)
/// \param[in]   comparatorTable        Pointer to the Comparator Table containing origianl function pointers
/// \param[in]   sizeTable              Size of the patchComparatorTable
///
/// \returns Error status
/// \retval ERROR_SUCCESS               Patch loaded successfully.
/// \retval ERROR_SYSERR                Error updating interrupt vector table
ErrorT PatchMgrSetupComparatorTable(uint32_t const startComparator,
    uint32_t const numComparators,
    PatchMgrComparatorFunctionAddressT const* const comparatorTable,
    uint32_t const sizeTable)
{
    ErrorT error = ERROR_SUCCESS;
    uint32_t lastIndex = startComparator + numComparators - 1U;

    if (lastIndex < sizeTable)
    {
        for (uint32_t index = startComparator; (index <= lastIndex) && (ERROR_SUCCESS == error); index++)
        {
            error = PatchMgrSetupComparator(index, comparatorTable[index]);
        }
    }
    else
    {
        error = ERROR_SYSERR;
    }

    return error;
}

/// @} endgroup patch

