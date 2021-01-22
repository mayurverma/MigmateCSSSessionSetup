//***********************************************************************************
/// \file
///
/// Toolbox API functions
///
/// \addtogroup toolbox
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
#include "Toolbox.h"

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

/// Calculates the checksum (using RFC1071)
///
/// Note this implementation is only valid for the little endian architecture (see errata in RFC1071)
///
/// \param[in] buffer                Buffer with the data contents to calculate the CRC
/// \param[in] sizeBytes             Number of bytes in the Buffer
/// \returns CRC value
///
uint16_t ToolboxCalcChecksum(uint16_t* const buffer, uint16_t const sizeBytes)
{
    uint32_t const sizeWords = (uint32_t)sizeBytes / sizeof(uint16_t);
    uint32_t sum = 0U;

    for (uint32_t index = 0U; index < sizeWords; index += 1U)
    {
        sum += buffer[index];
    }

    //  Add left-over byte, if any
    if (0U != (sizeBytes & 0x0001U))
    {
        sum += (uint32_t)buffer[sizeWords] & 0xFFU;
    }

    // Final sum to collapse the 32 bit counter into 16 bits. Note that this while loop can be executed more than
    // once for some scenarios (i.e. sum = 0x1FFFF)
    while (sum > 0xFFFFU)
    {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }

    return (uint16_t) ~sum;
}

/// @} endgroup toolbox

