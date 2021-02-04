//***********************************************************************************
/// \file
///
/// Production Test Application implementation
///
/// \addtogroup prodtestapp
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

#include "ProdTestAppInternal.h"
#include "Toolbox.h"

//---------------------------------------------------------------------------------
//                                  Constants
//---------------------------------------------------------------------------------

//checksum for when the OTPM is error free
#define PROD_TEST_APP_NO_ERROR_CHECKSUM 0xFFFFU;

//Create masks for ProdTestAppErrorT datatypes to set bits 12 and 13 to indicate error type
#define PROD_TEST_APP_ERROR_SINGLE_OFFSET 12U
#define PROD_TEST_APP_ERROR_DOUBLE_OFFSET 13U
//cast 1U to unsigned int to prevent klocwork shift out of range error
//since 1U by default is unsigned char and only 8 bits klocwork counts it as an error
#define PROD_TEST_APP_ERROR_SINGLE_MASK ((unsigned int)1U << PROD_TEST_APP_ERROR_SINGLE_OFFSET)
#define PROD_TEST_APP_ERROR_DOUBLE_MASK ((unsigned int)1U << PROD_TEST_APP_ERROR_DOUBLE_OFFSET)

//---------------------------------------------------------------------------------
//                                  Types
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Macros
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
//                                  Exports
//---------------------------------------------------------------------------------

/// Calculates OTPM errors with verify or blank check setting, if isBlankCheckModeActive is set the function will perform a blank check, verify otherwise
///
/// \param[in]  isBlankCheck         Choose verify (false) or blank check (true) mode
/// \param[in]  params               Pointer to command params of calling function (in shared memory)
/// \returns
/// \retval ERROR_SUCCESS            Error check completed successfully
/// \retval ERROR_INVAL              Invalid parameter passed to function
/// \retval ERROR_SYSERR             Internal fault, command failure
///
ErrorT ProdTestAppCalculateError(const bool isBlankCheck, ProdTestAppCommandParamsT* const params)
{
    bool isBlankCheckModeActive;
    ErrorT err = ERROR_SUCCESS;
    ErrorT eccErr = ERROR_SUCCESS;
    ProdTestAppModeT mode = params->mode;
    ProdTestAppResponseParametersT* const response = (ProdTestAppResponseParametersT* const)params;
    uint16_t blockLengthSizeBytes = (uint16_t)sizeof(ProdTestAppResponseParametersT) - sizeof(uint16_t);  // header

    response->checkSum = 0U;
    response->blockLengthWords = 0U;
    NvmMgrAddressT blankAddress = 0U;
    uint32_t addressIndex = 0U;
    bool isSearching = true;
    uint32_t currentWord;
    OtpmDrvErrorT eccType;
    uint32_t maxError = ProdTestAppGetMaxErrorArrayLengthLongWords();
    //flush any pending errors from before calling this function - don't care about return value or parameter
    (void)OtpmDrvGetLastError(&eccType);
    if (isBlankCheck)
    {
        isBlankCheckModeActive = true;
    }
    else
    {
        isBlankCheckModeActive = false;
    }

    if ((PROD_TEST_APP_MODE_FAST != mode) && (PROD_TEST_APP_MODE_SLOW != mode))
    {
        err = ERROR_INVAL;
    }
    else
    {
        err = NvmMgrGetFreeSpaceAddress(&blankAddress);  // Determine start blank checking in verify mode
        if (ERROR_SUCCESS == err)
        {
            while (isSearching)  //loop infinitely (until read returns ERROR_RANGE) with counter
            {
                //start blank checking if in verify mode and reached user space
                if (!isBlankCheckModeActive && (addressIndex >= blankAddress))
                {
                    isBlankCheckModeActive = true;
                }
                err = OtpmDrvRead(addressIndex, &currentWord, sizeof(currentWord) / sizeof(uint32_t));
                eccErr = OtpmDrvGetLastError(&eccType);
                if (ERROR_RANGE == err)  //reached end of OTPM?
                {
                    err = ERROR_SUCCESS;
                    break;
                }
                if ((ERROR_NOENT != eccErr) || (isBlankCheckModeActive && (0U != currentWord)))
                {
                    // Location could have an error and/or is non blank
                    if (response->blockLengthWords < maxError)
                    {
                        //record error address in block starting at blockStart pointer
                        response->dataBlock[response->blockLengthWords] = (uint16_t)addressIndex;
                        // Check if we have an error at this location
                        if (ERROR_NOENT != eccErr)
                        {
                            if (eccType.isCorrectable)
                            {
                                response->dataBlock[response->blockLengthWords] |= PROD_TEST_APP_ERROR_SINGLE_MASK;
                            }
                            else
                            {
                                response->dataBlock[response->blockLengthWords] |= PROD_TEST_APP_ERROR_DOUBLE_MASK;
                            }
                        }
                        response->blockLengthWords++;
                        //only record one error in fast mode
                        if (PROD_TEST_APP_MODE_FAST == mode)
                        {
                            isSearching = false;
                        }
                    }
                    else
                    {
                        // Should never run out of error space, this is a serious error
                        err = ERROR_SYSERR;
                        isSearching = false;
                    }
                }
                addressIndex++;
            }
            blockLengthSizeBytes += (uint16_t)(response->blockLengthWords * sizeof(uint16_t));
            response->checkSum = ToolboxCalcChecksum((uint16_t* const)response, blockLengthSizeBytes);
        }
    }

    return err;
}

// @} endgroup prodtestapp

