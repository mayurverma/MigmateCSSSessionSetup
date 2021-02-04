#if !defined(SRC__PRODTESTAPP__SRC__PRODTESTAPPINTERNAL_H)
#define      SRC__PRODTESTAPP__SRC__PRODTESTAPPINTERNAL_H

//******************************************************************************
/// \file
///
/// Provides ProdTestApp internal header file
///
/// \addtogroup prodtestapp
/// @{
//******************************************************************************
// Copyright 2018 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************
//

#include "OtpmDrv.h"
#include "NvmMgr.h"
#include "PatchMgr.h"

#include "ProdTestApp.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

#define PROD_TEST_APP_NUM_PATCHED_FUNCTIONS 1U

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

/// Struct representing a block of error locations of a known length and a checksum
typedef struct
{
    uint16_t blockLengthWords;                                          ///< Length of data block
    uint16_t checkSum;                                                  ///< Checksum of data block, add all data together and invert
    uint16_t dataBlock[1];                                              ///< Array of uint16_t that represents error address offsets and type
} ProdTestAppResponseParametersT;

/// Represents the mode of OTPM blank checking or verifying
typedef enum
{
    PROD_TEST_APP_MODE_FAST = 0U,
    PROD_TEST_APP_MODE_SLOW = 1U
} ProdTestAppModeT;

/// Represents the command parameters for the OTPM_BLANK_CHECK and OTPM_VERIFY host commands
typedef struct
{
    ProdTestAppModeT mode;
} ProdTestAppCommandParamsT;

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------
extern const PatchMgrComparatorFunctionAddressT prodTestAppComparatorTable[PROD_TEST_APP_NUM_PATCHED_FUNCTIONS];

// After all patch chunks are downloaded, this function is called to load/hook the patched functions
extern ErrorT ProdTestAppLoader(void);

//Check for errors in OTPM
extern ErrorT ProdTestAppCalculateError(const bool isBlankCheck, ProdTestAppCommandParamsT* const params);

// Function for returning either release (default) or testing (if overridden by mock) value of
// the maximum number of possible errors that can be recorded
extern uint32_t ProdTestAppGetMaxErrorArrayLengthLongWords(void);

// Intend to replace the TRNG function CC_PalTrngParamSet() in ROM and fine-tune the TRNG parameters.
extern void CC_PalTrngParamSetPatched(const AssetMgrCC312TrngCharT* pTrngParams);

// @} endgroup prodtestapp
#endif  //SRC__PRODTESTAPP__SRC__PRODTESTAPPINTERNAL_H

