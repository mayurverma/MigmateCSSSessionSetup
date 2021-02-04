//***********************************************************************************
/// \file
///
/// Patch Loader function
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
#include "Patch.h"

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

/// After all patch chunks are downloaded, this function is called to load/hook the patched functions
/// and patched-in new commands.

///
/// \returns Error status
/// \retval ERROR_SUCCESS               Patch loaded successfully.
ErrorT PatchLoader(void)
{
    // This is a 'dummy' empty patch for use on the trunk
    return ERROR_SUCCESS;
}

/// @} endgroup patch

