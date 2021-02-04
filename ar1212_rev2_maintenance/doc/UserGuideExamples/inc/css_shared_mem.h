#if !defined(CSS_SHARED_MEM_H)
#define      CSS_SHARED_MEM_H

//***********************************************************************************
/// \file
///
/// CSS Shared Memory accessor function declarations
///
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
//***********************************************************************************

#include <stdint.h>
#include <stdbool.h>

/// Size of the CSS shared-memory in bytes
#define CSS_SHARED_MEMORY_PAGE_SIZE_BYTES 1024U

/// Size of the CSS shared-memory in (16-bit) words
#define CSS_SHARED_MEMORY_PAGE_SIZE_WORDS (CSS_SHARED_MEMORY_PAGE_SIZE_BYTES / 2U)

// Writes an unsigned 16-bit value to shared-memory at the specified word offset
extern void CssWriteSharedMemUint16(uint16_t const wordOffset, uint16_t const value);

// Writes an unsigned 32-bit value to shared-memory at the specified word offset
extern void CssWriteSharedMemUint32(uint16_t const wordOffset, uint32_t const value);

// Writes an unsigned 64-bit value to shared-memory at the specified word offset
extern void CssWriteSharedMemUint64(uint16_t const wordOffset, uint64_t const value);

// Reads an unsigned 16-bit value from shared-memory at the specified word offset
extern uint16_t CssReadSharedMemUint16(uint16_t const wordOffset);

// Reads an unsigned 32-bit value from shared-memory at the specified word offset
extern uint32_t CssReadSharedMemUint32(uint16_t const wordOffset);

// Reads an unsigned 64-bit value from shared-memory at the specified word offset
extern uint64_t CssReadSharedMemUint64(uint16_t const wordOffset);

// Writes a multi-byte sequence to shared-memory starting at the specified word offset
extern void CssWriteSharedMemMultiByte( uint16_t const wordOffset,
                                        uint8_t* const data,
                                        uint32_t const numWords);

// Reads a multi-byte sequence from shared-memory starting at the specified word offset
extern void CssReadSharedMemMultiByte(  uint16_t const wordOffset,
                                        uint8_t* const data,
                                        uint32_t const numWords);

#endif // !defined(CSS_SHARED_MEM_H)
