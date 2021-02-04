//***********************************************************************************
/// \file
///
/// CSS Shared Memory accessor function definitions
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

#include "i2c_driver.h"
#include "css_shared_mem.h"

/// Internal function: sets the shared-memory page
///
/// If the requested page is not the current page, sets the CSS_PAGE
/// register to access the requested page.
///
/// \param[in]  page    shared-memory page to access
///
/// \returns new value of the CSS_PAGE register
///
static uint16_t CssSetSharedMemPage(uint16_t const page)
{
    // Local to track the current shared-memory page
    static uint16_t currentPage = 0;

    if (page != currentPage)
    {
        I2cWriteReg16(CSS_PAGE, page);
        currentPage = page;
    }

    return currentPage;
}

/// Internal function: sets the shared-memory page given a word offset
///
/// \param[in]  page    offset of (16-bit) word in shared-memory to access
///
/// \returns new value of the CSS_PAGE register
///
static uint16_t CssSetSharedMemPageFromWordOffset(uint16_t const wordOffset)
{
    return CssSetSharedMemPage(wordOffset / CSS_SHARED_MEMORY_PAGE_SIZE_WORDS);
}

/// Internal function: type-safe unsigned 16-bit MIN()
///
/// \param[in]  a   first parameter
/// \param[in]  b   second parameter
///
/// \returns minimum of a and b
///
static uint16_t min_uint16(uint16_t const a, uint16_t const b)
{
    return (a > b) ? b : a;
}

/// Writes an unsigned 16-bit value to shared-memory at the specified word offset
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
/// \param[in]  value       16-bit value to write
///
/// \returns void
///
void CssWriteSharedMemUint16(uint16_t const wordOffset, uint16_t const value)
{
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage * 2)), value);
}

/// Writes an unsigned 32-bit value to shared-memory at the specified word offset
///
/// \warning Does not check that both words are in the same page
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
/// \param[in]  value       32-bit value to write
///
/// \returns void
///
void CssWriteSharedMemUint32(uint16_t const wordOffset, uint32_t const value)
{
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)),
                  (uint16_t)((value >> 0)   & 0xFFFFU));
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage   * 2)),
                  (uint16_t)((value >> 16U) & 0xFFFFU));
}

/// Writes an unsigned 64-bit value to shared-memory at the specified word offset
///
/// \warning Does not check that both words are in the same page
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
/// \param[in]  value       64-bit value to write
///
/// \returns void
///
void CssWriteSharedMemUint64(uint16_t const wordOffset, uint64_t const value)
{
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)),
                  (uint16_t)((value >> 0)   & 0xFFFFU));
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)),
                  (uint16_t)((value >> 16U) & 0xFFFFU));
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)),
                  (uint16_t)((value >> 32U) & 0xFFFFU));
    I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage   * 2)),
                  (uint16_t)((value >> 48U) & 0xFFFFU));
}

/// Reads an unsigned 16-bit value from shared-memory at the specified word offset
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
///
/// \returns 16-bit value retrieve from shared-memory
///
uint16_t CssReadSharedMemUint16(uint16_t const wordOffset)
{
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);
    return I2cReadReg16((CssRegisterT)(CSS_PARAMS_0 + (offsetInPage * 2)));
}

/// Reads an unsigned 32-bit value from shared-memory at the specified word offset
///
/// \warning Does not check that both words are in the same page
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
///
/// \returns 32-bit value retrieve from shared-memory
///
uint32_t CssReadSharedMemUint32(uint16_t const wordOffset)
{
    uint32_t retValue = 0;
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);

    // Read the least-significant word first
    retValue =   (uint32_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)));
    retValue |= ((uint32_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage   * 2)))) << 16U;

    return retValue;
}

/// Reads an unsigned 64-bit value from shared-memory at the specified word offset
///
/// \warning Does not check that both words are in the same page
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
///
/// \returns 64-bit value retrieve from shared-memory
///
uint64_t CssReadSharedMemUint64(uint16_t const wordOffset)
{
    uint64_t retValue = 0;
    uint16_t offsetInPage = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    (void)CssSetSharedMemPageFromWordOffset(wordOffset);

    // Read the least-significant word first
    retValue =   (uint64_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)));
    retValue |= ((uint64_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)))) << 16U;
    retValue |= ((uint64_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage++ * 2)))) << 32U;
    retValue |= ((uint64_t)I2cReadReg16(
                    (CssRegisterT)(CSS_PARAMS_0 + (offsetInPage   * 2)))) << 48U;

    return retValue;
}

/// Writes a multi-byte sequence to shared-memory starting at the specified word offset
///
/// Each byte-pair is swapped due to the endianess requirements. Supports multi-page writes.
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
/// \param[in]  data        multi-byte sequence to write
/// \param[in]  numWords    number of (16-bit) words to write
///
/// \returns void
///
void CssWriteSharedMemMultiByte(uint16_t const wordOffset,
                                uint8_t* const data,
                                uint32_t const numWords)
{
    int currentPage = CssSetSharedMemPageFromWordOffset(wordOffset);
    uint16_t currentWord = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    uint16_t wordsInPage = min_uint16(numWords,
                                      CSS_SHARED_MEMORY_PAGE_SIZE_WORDS - currentWord);
    uint32_t wordsWritten = 0;
    uint8_t* bytePair = data;

    while (wordsWritten < numWords)
    {
        (void)CssSetSharedMemPage(currentPage);

        // Byte-swap every byte-pair written due to endianess
        for (;
             wordsInPage != 0;
             wordsInPage -= 1U, wordsWritten += 1U, currentWord += 1U)
        {
            uint32_t bytesWritten = wordsWritten * 2U;
            uint16_t wordVal = (uint16_t)bytePair[bytesWritten + 1U] << 8U;
            wordVal |= (uint16_t)bytePair[bytesWritten + 0U] << 0U;
            I2cWriteReg16((CssRegisterT)(CSS_PARAMS_0 + (currentWord * 2U)), wordVal);
        }

        currentPage += 1U;
        currentWord = 0U;
        wordsInPage = min_uint16(numWords - wordsWritten,
                                 CSS_SHARED_MEMORY_PAGE_SIZE_WORDS);
    }
}

/// Reads a multi-byte sequence from shared-memory starting at the specified word offset
///
/// Each byte-pair is swapped due to the endianess requirements. Supports multi-page reads.
///
/// \param[in]  wordOffset  16-bit offset within the shared-memory
/// \param[out] data        buffer to contain multi-byte sequence
/// \param[in]  numWords    number of (16-bit) words to read
///
/// \returns void
///
void CssReadSharedMemMultiByte( uint16_t const wordOffset,
                                uint8_t* const data,
                                uint32_t const numWords)
{
    int currentPage = CssSetSharedMemPageFromWordOffset(wordOffset);
    uint16_t currentWord = wordOffset % CSS_SHARED_MEMORY_PAGE_SIZE_WORDS;
    uint16_t wordsInPage = min_uint16(numWords,
                                      CSS_SHARED_MEMORY_PAGE_SIZE_WORDS - currentWord);
    uint32_t wordsRead = 0;
    uint8_t* bytePair = data;

    while (wordsRead < numWords)
    {
        (void)CssSetSharedMemPage(currentPage);

        // Byte-swap every byte-pair read due to endianess
        for (;
             wordsInPage != 0U;
             wordsInPage -= 1U, wordsRead += 1U, currentWord += 1U)
        {
            uint16_t wordVal = I2cReadReg16(
                                    (CssRegisterT)(CSS_PARAMS_0 + (currentWord * 2U)));
            uint32_t bytesRead = wordsRead * 2U;
            bytePair[bytesRead + 1] = (wordVal & 0xFF00U) >> 8U;
            bytePair[bytesRead + 0] = (wordVal & 0x00FFU) >> 0U;
        }

        currentPage += 1U;
        currentWord = 0U;
        wordsInPage = min_uint16(numWords - wordsRead,
                                 CSS_SHARED_MEMORY_PAGE_SIZE_WORDS);
    }
}

