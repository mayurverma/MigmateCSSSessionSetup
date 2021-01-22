//***********************************************************************************
/// \file
///
/// HAL/PAL Configuration
///
/// \addtogroup cc312halpal
/// @{
//***********************************************************************************
//
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.
//       ALL RIGHTS RESERVED
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//
//***********************************************************************************
//
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
#include "cc_pal_types.h"
#include "cc_pal_error.h"
#include "cc_pal_mem.h"

extern CCError_t CC_PalSecMemCmp(const uint8_t* aTarget, const uint8_t* aSource, size_t aSize);
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

/// This function purpose is to perform secured memory comparison between two given buffers according to given size.
///
/// The function will compare each byte till aSize number of bytes was compared even if the bytes are different.
/// The function should be used to avoid security timing attacks.
///
/// param[out] aTarget       The target buffer to compare.
/// param[in]  aSource       The Source buffer to compare to.
/// param[in]  aSize         Number of bytes to compare.
/// \returns Function Success/Failure
/// \retval CC_SUCCESS Success
/// \retval CC_PAL_MEM_BUF2_GREATER
/// \retval CC_PAL_MEM_BUF1_GREATER
///
CCError_t CC_PalSecMemCmp(const uint8_t* aTarget,
    const uint8_t* aSource,
    size_t aSize)
{
    /* internal index */
    uint32_t i = 0;

    /* error return */
    CCError_t error = (CCError_t)CC_SUCCESS;

    /*------------------
      * CODE
      * -------------------*/

    /* Go over aTarget till aSize is reached (even if its not equal) */
    for (i = 0; i < aSize; i++)
    {
        if (aTarget[i] != aSource[i])
        {
            if (error != CC_SUCCESS)
            {
                continue;
            }
            else
            {
                if (aTarget[i] < aSource[i])
                {
                    error = (CCError_t)CC_PAL_MEM_BUF2_GREATER;
                }
                else
                {
                    error = (CCError_t)CC_PAL_MEM_BUF1_GREATER;
                }
            }
        }
    }

    return error;
}  /* End of CC_PalSecMemCmp */

/// This function maps to C library function memcmp().
///
/// param[out] aTarget       The target buffer to compare.
/// param[in]  aSource       The Source buffer to compare to.
/// param[in]  aSize         Number of bytes to compare.
/// \returns A non-zero value on failure.
///
int32_t CC_PalMemCmpPlat(const void* aTarget, const void* aSource, size_t aSize)
{
    return memcmp(aTarget, aSource, aSize);
}  /* End of CC_PalMemCmpPlat */

/// This function maps to C library function memmove().
///
/// param[out] aDestination  The destination buffer to copy bytes to.
/// param[in]  aSource       The Source buffer to copy from.
/// param[in]  aSize         Number of bytes to copy.
/// \returns A non-zero value on failure.
///
void * CC_PalMemCopyPlat(void* aDestination, const void* aSource, size_t aSize)
{
    return memmove(aDestination, aSource, aSize);
}  /* End of CC_PalMemCopyPlat */

/// This function purpose is to copy aSize bytes from source buffer to destination buffer.
/// This function Supports overlapped buffers.
///
/// param[out] aDestination  The destination buffer to copy bytes to.
/// param[in]  aSource       The Source buffer to copy from.
/// param[in]  aSize         Number of bytes to copy.
/// \returns void
///
void CC_PalMemMovePlat(void* aDestination, const void* aSource, size_t aSize)
{
    (void)memmove(aDestination, aSource, aSize);
}  /* End of CC_PalMemMovePlat */

/// This function purpose is to set aSize bytes in the given buffer with aChar.
///
/// param[out] aTarget  The target buffer to set.
/// param[in]  aChar    The char to set into aTarget.
/// param[in]   aSize   Number of bytes to set.
/// \returns void
///
void CC_PalMemSetPlat(void* aTarget, uint8_t aChar, size_t aSize)
{
    (void)memset(aTarget, (int)aChar, aSize);
}  /* End of CC_PalMemSetPlat */

/// This function purpose is to set aSize bytes in the given buffer with zeroes.
///
/// param[out] aTarget  The target buffer to set.
/// param[in]   aSize   Number of bytes to set.
/// \returns void
///
void CC_PalMemSetZeroPlat(void* aTarget, size_t aSize)
{
    (void)memset(aTarget, 0x00, aSize);
}  /* End of CC_PalMemSetZeroPlat */

/// This function purpose is to allocate a memory buffer according to aSize.
///
/// param[in]   aSize   Number of bytes to allocate.
/// \returns The function returns a pointer to the newly allocated buffer or NULL if allocation failed.
///
void * CC_PalMemMallocPlat(size_t aSize)
{
    return malloc(aSize);
}  /* End of CC_PalMemMallocPlat */

/// This function purpose is to reallocate a memory buffer according to aNewSize.
/// The content of the old buffer is moved to the new location.
///
/// param[in]  aBuffer    Pointer to allocated buffer.
/// param[in]  aNewSize  Number of bytes to reallocate.
/// \returns The function returns a pointer to the newly allocated buffer or NULL if allocation failed.
///
void * CC_PalMemReallocPlat(void* aBuffer, size_t aNewSize)
{
    return realloc(aBuffer, aNewSize);
}  /* End of CC_PalMemReallocPlat */

/// This function purpose is to free allocated buffer.
///
/// param[in]  aBuffer    Pointer to allocated buffer.
/// \returns void
///
void CC_PalMemFreePlat(void* aBuffer)
{
    free(aBuffer);
}  /* End of CC_PalMemFreePlat */

/// @} endgroup cc312halpal

