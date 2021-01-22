//***********************************************************************************
/// \file
///
/// xMAC Driver unit test
//
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

#include "cpputest_support.h"

extern "C"
{
#include "Common.h"
#include "AR0820.h"
#include "AR0820_memmap.h"
#include "crypto_vcore.h"
#include "MacDrvInternal.h"


#if defined(BUILD_TARGET_ARCH_x86)
CRYPTO_VCORE_REGS_T vcoreRegsUnitTest = {};
#endif
}

#if defined(BUILD_TARGET_ARCH_x86)
// Get access to the Unit test 'registers'
CRYPTO_VCORE_REGS_T* vcore_regs = &vcoreRegsUnitTest;
#else
// Get access to the real VCore registers
CRYPTO_VCORE_REGS_T* vcore_regs = (CRYPTO_VCORE_REGS_T*)CRYPTO_VCORE_REG_BASE;
#endif

// AESCMAC_CNFG and AESHP_CNFG registers have HW defaults which the driver must not override
uint32_t AesCmacCnfgDefault, AesCmacCnfgDefaultMask;
uint32_t AesGmacCnfgDefault, AesGmacCnfgDefaultMask;

TEST_GROUP(MacDrvInitGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
    }
};

TEST_GROUP(MacDrvGroup)
{
    // Optional setup - called prior to each test
    void setup(void)
    {
        ErrorT status = MacDrvInit();
        LONGS_EQUAL(ERROR_SUCCESS, status);
        CHECK_FALSE(macDrvState.isActive);

        // AESCMAC_CNFG has a HW default which the driver must not override
        AesCmacCnfgDefault = (0x0F << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_MODE__SHIFT) |      // 4-bit field, just a test value
                             (0x01 << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_ENCDEC__SHIFT);     // 1-bit field, just a test value
        vcore_regs->aescmac_cnfg = AesCmacCnfgDefault;

        // AESHP_CNFG has a HW default which the driver must not override
        AesGmacCnfgDefault = (0x01 << CRYPTO_VCORE_AESHP_CNFG_AESHP_ENCDEC__SHIFT);         // 1-bit field, just a test value
        vcore_regs->aeshp_cnfg = AesGmacCnfgDefault;

        // The bits that are to be preserved
        AesCmacCnfgDefaultMask = CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_MODE__MASK |
                                 CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_ENCDEC__MASK;
        AesGmacCnfgDefaultMask = CRYPTO_VCORE_AESHP_CNFG_AESHP_ENCDEC__MASK;
    }

    // Optional teardown - called after each test
    void teardown(void)
    {
        vcore_regs->aeshp_cnfg = 0;
        vcore_regs->aeshp_start = 0;
        vcore_regs->aescmac_cnfg = 0;
        vcore_regs->aescmac_start = 0;
        vcore_regs->roi = 0;
    }
};

TEST(MacDrvInitGroup, TestInit)
{
    ErrorT status = MacDrvInit();
    LONGS_EQUAL(ERROR_SUCCESS, status);
    LONGS_EQUAL(0U, vcore_regs->cnfg & CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__MASK);
    CHECK_FALSE(macDrvState.isActive);
    CHECK_FALSE(macDrvState.isConfigured);
}

TEST(MacDrvGroup, TestSetSessionKey128Bit)
{
    // session key in little-endian format
    uint8_t sessionKey[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    ErrorT status;

    status = MacDrvSetSessionKey(sessionKey, sizeof(sessionKey));
    LONGS_EQUAL(ERROR_SUCCESS, status);

    LONGS_EQUAL(0U, vcore_regs->session_key0);
    LONGS_EQUAL(0U, vcore_regs->session_key1);
    LONGS_EQUAL(0U, vcore_regs->session_key2);
    LONGS_EQUAL(0U, vcore_regs->session_key3);

    uint32_t idx;
    uint8_t* expectedKey = &sessionKey[sizeof(sessionKey) - 4];
    __IOM uint32_t* sessionKeyReg = &vcore_regs->session_key4;

    // key is byte-reversed when written to the hardware, MSB
    // is written to bits 31..24 of session_key7

    for (idx = 0; idx < 4U; idx++)
    {
        uint32_t expected_val = (uint32_t)((expectedKey[0] << 24U) |
            (expectedKey[1] << 16U) |
            (expectedKey[2] << 8U) |
            (expectedKey[3] << 0U));
        LONGS_EQUAL(expected_val, *sessionKeyReg);

        expectedKey -= 4U;
        sessionKeyReg++;
    }
}

TEST(MacDrvGroup, TestSetSessionKey192Bit)
{
    // session key in little-endian format
    uint8_t sessionKey[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    };
    ErrorT status;

    status = MacDrvSetSessionKey(sessionKey, sizeof(sessionKey));
    LONGS_EQUAL(ERROR_SUCCESS, status);

    LONGS_EQUAL(0U, vcore_regs->session_key0);
    LONGS_EQUAL(0U, vcore_regs->session_key1);

    uint32_t idx;
    uint8_t* expectedKey = &sessionKey[sizeof(sessionKey) - 4];
    __IOM uint32_t* sessionKeyReg = &vcore_regs->session_key2;

    // key is byte-reversed when written to the hardware, MSB
    // is written to bits 31..24 of session_key7

    for (idx = 0; idx < 6U; idx++)
    {
        uint32_t expected_val = (uint32_t)((expectedKey[0] << 24) |
            (expectedKey[1] << 16) |
            (expectedKey[2] << 8) |
            (expectedKey[3] << 0));
        LONGS_EQUAL(expected_val, *sessionKeyReg);

        expectedKey -= 4U;
        sessionKeyReg++;
    }
}

TEST(MacDrvGroup, TestSetSessionKey256Bit)
{
    // session key in little-endian format
    uint8_t sessionKey[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
    };
    ErrorT status;

    status = MacDrvSetSessionKey(sessionKey, sizeof(sessionKey));
    LONGS_EQUAL(ERROR_SUCCESS, status);

    uint32_t idx;
    uint8_t* expectedKey = &sessionKey[sizeof(sessionKey) - 4];
    __IOM uint32_t* sessionKeyReg = &vcore_regs->session_key0;

    // key is byte-reversed when written to the hardware, MSB
    // is written to bits 31..24 of session_key7

    for (idx = 0; idx < 8U; idx++)
    {
        uint32_t expected_val = (uint32_t)((expectedKey[0] << 24) |
            (expectedKey[1] << 16) |
            (expectedKey[2] << 8) |
            (expectedKey[3] << 0));
        LONGS_EQUAL(expected_val, *sessionKeyReg);

        expectedKey -= 4U;
        sessionKeyReg++;
    }
}

TEST(MacDrvGroup, TestSetSessionKeyBadKeyLength)
{
    uint8_t sessionKey[18];
    ErrorT status;

    status = MacDrvSetSessionKey(sessionKey, sizeof(sessionKey));
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(MacDrvGroup, TestSetSessionKeyNoKey)
{
    ErrorT status;

    status = MacDrvSetSessionKey(NULL, 42U);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(MacDrvGroup, TestSetGmacIv)
{
    uint8_t iv[] =
    {
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4A, 0x4B
    };
    ErrorT status;

    status = MacDrvSetGmacIv(iv, sizeof(iv));
    LONGS_EQUAL(ERROR_SUCCESS, status);

    LONGS_EQUAL(0x00000001U, vcore_regs->iv0);

    uint32_t idx;
    uint8_t* expectedIv = &iv[sizeof(iv) - 4];
    __IOM uint32_t* ivReg = &vcore_regs->iv1;

    for (idx = 0; idx < 3U; idx++)
    {
        uint32_t expected_val = (uint32_t)((expectedIv[0] << 24) |
            (expectedIv[1] << 16) |
            (expectedIv[2] << 8) |
            (expectedIv[3] << 0));
        LONGS_EQUAL(expected_val, *ivReg);

        expectedIv -= 4U;
        ivReg++;
    }
}

TEST(MacDrvGroup, TestResetIv)
{
    // the function should clear these four registers
    vcore_regs->iv0 = 0x87654321UL;
    vcore_regs->iv1 = 0x98765432UL;
    vcore_regs->iv2 = 0xA9876543UL;
    vcore_regs->iv3 = 0xBA987654UL;

    MacDrvResetIv();

    LONGS_EQUAL(0L, vcore_regs->iv0);
    LONGS_EQUAL(0L, vcore_regs->iv1);
    LONGS_EQUAL(0L, vcore_regs->iv2);
    LONGS_EQUAL(0L, vcore_regs->iv3);
}

TEST(MacDrvGroup, TestSetGmacIvBadIvLength)
{
    uint8_t iv[] =
    {
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F
    };
    ErrorT status;

    status = MacDrvSetGmacIv(iv, sizeof(iv));
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(MacDrvGroup, TestSetGmacIvNoIv)
{
    ErrorT status;

    status = MacDrvSetGmacIv(NULL, 1234U);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(MacDrvGroup, TestSetMacConfigRejectedWhenAlreadyConfigured)
{
    MacDrvConfigT macConfig;

    // Force the driver state
    macDrvState.isConfigured = true;

    ErrorT status = MacDrvSetConfig(&macConfig);
    LONGS_EQUAL(ERROR_ALREADY, status);
}

TEST(MacDrvGroup, TestSetMacConfigRejectedWithNullConfig)
{
    ErrorT status = MacDrvSetConfig(NULL);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(MacDrvGroup, TestSetMacConfigRejectedWithInvalidConfig)
{
    MacDrvConfigT macConfig;

    macConfig.cmacNotGmac = true;
    macConfig.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;

    ErrorT status = MacDrvSetConfig(&macConfig);
    LONGS_EQUAL(ERROR_INVAL, status);

    macConfig.cmacNotGmac = false;
    macConfig.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;

    status = MacDrvSetConfig(&macConfig);
    LONGS_EQUAL(ERROR_INVAL, status);
}

TEST(MacDrvGroup, TestSetMacConfigForGmac)
{
    MacDrvConfigT macConfig;

    macConfig.cmacNotGmac = false;
    macConfig.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    macConfig.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macConfig.pixelPackingValue = 0x4243U;
    macConfig.frameCount = 0x12345678U;

    ErrorT status = MacDrvSetConfig(&macConfig);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(macDrvState.isConfigured);
    CHECK_FALSE(macDrvState.isActive);

    LONGS_EQUAL(macConfig.pixelPackingValue, vcore_regs->pack_val_lo);
    LONGS_EQUAL(macConfig.frameCount, vcore_regs->strt_frame_ident);

    uint32_t cnfg = vcore_regs->cnfg;

    LONGS_EQUAL(MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME,
        (cnfg & CRYPTO_VCORE_CNFG_PACK_MODE__MASK) >> CRYPTO_VCORE_CNFG_PACK_MODE__SHIFT);

    LONGS_EQUAL(1U,
        (cnfg & CRYPTO_VCORE_CNFG_PACK_ENABLE__MASK) >> CRYPTO_VCORE_CNFG_PACK_ENABLE__SHIFT);

    LONGS_EQUAL(1U,
        (cnfg & CRYPTO_VCORE_CNFG_AUTH_FRAME_START_EN__MASK) >> CRYPTO_VCORE_CNFG_AUTH_FRAME_START_EN__SHIFT);

    LONGS_EQUAL(0U,
        (cnfg & CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__MASK) >> CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__SHIFT);

    LONGS_EQUAL(0,
        (cnfg & CRYPTO_VCORE_CNFG_AESCMAC_EN__MASK) >> CRYPTO_VCORE_CNFG_AESCMAC_EN__SHIFT);

    LONGS_EQUAL(0U, vcore_regs->roi);
    LONGS_EQUAL(0U, vcore_regs->roi_frame_ident);
}

TEST(MacDrvGroup, TestSetMacConfigForCmac)
{
    MacDrvConfigT macConfig;

    macConfig.cmacNotGmac = true;
    macConfig.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macConfig.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    macConfig.pixelPackingValue = 0x5678U;
    macConfig.frameCount = 0xfedcba98U;

    ErrorT status = MacDrvSetConfig(&macConfig);
    LONGS_EQUAL(ERROR_SUCCESS, status);
    CHECK_TRUE(macDrvState.isConfigured);
    CHECK_FALSE(macDrvState.isActive);

    LONGS_EQUAL(macConfig.pixelPackingValue, vcore_regs->pack_val_lo);
    LONGS_EQUAL(macConfig.frameCount, vcore_regs->strt_frame_ident);

    uint32_t cnfg = vcore_regs->cnfg;

    LONGS_EQUAL(MAC_DRV_VIDEO_AUTH_MODE_ROI,
        (cnfg & CRYPTO_VCORE_CNFG_PACK_MODE__MASK) >> CRYPTO_VCORE_CNFG_PACK_MODE__SHIFT);

    LONGS_EQUAL(1U,
        (cnfg & CRYPTO_VCORE_CNFG_PACK_ENABLE__MASK) >> CRYPTO_VCORE_CNFG_PACK_ENABLE__SHIFT);

    LONGS_EQUAL(1U,
        (cnfg & CRYPTO_VCORE_CNFG_AUTH_FRAME_START_EN__MASK) >> CRYPTO_VCORE_CNFG_AUTH_FRAME_START_EN__SHIFT);

    LONGS_EQUAL(0U,
        (cnfg & CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__MASK) >> CRYPTO_VCORE_CNFG_AUTH_AUTO_STRT_EN__SHIFT);

    LONGS_EQUAL(1U,
        (cnfg & CRYPTO_VCORE_CNFG_AESCMAC_EN__MASK) >> CRYPTO_VCORE_CNFG_AESCMAC_EN__SHIFT);

    LONGS_EQUAL(0U, vcore_regs->roi);
    LONGS_EQUAL(0U, vcore_regs->roi_frame_ident);
}

TEST(MacDrvGroup, TestSetRoiConfigRejectedWhenNotConfigured)
{
    MacDrvRoiConfigT roiConfig;

    macDrvState.isConfigured = false;

    ErrorT status = MacDrvSetRoiConfig(&roiConfig);
    LONGS_EQUAL(ERROR_AGAIN, status);
}

TEST(MacDrvGroup, TestSetRoiConfigRejectedWithNullConfig)
{
    macDrvState.isConfigured = true;

    ErrorT status = MacDrvSetRoiConfig(NULL);
    LONGS_EQUAL(ERROR_NOENT, status);
}

TEST(MacDrvGroup, TestSetRoiConfigRejectedWhenGmacMode)
{
    MacDrvRoiConfigT roiConfig;

    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = false;

    ErrorT status = MacDrvSetRoiConfig(&roiConfig);
    LONGS_EQUAL(ERROR_NOEXEC, status);
}

TEST(MacDrvGroup, TestSetRoiConfigWhenNotActive)
{
    MacDrvRoiConfigT roiConfig;

    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = true;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x1234U;
    macDrvState.config.frameCount = 0U;

    roiConfig.firstRow = 0xFFFU;
    roiConfig.lastRow = 0xFFFU;
    roiConfig.rowSkip = 0xFFU;

    roiConfig.firstCol = 8U;
    roiConfig.lastCol = 800U;
    roiConfig.colSkip = 2U;

    roiConfig.frameCount = 0x12345678U;

    ErrorT status = MacDrvSetRoiConfig(&roiConfig);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    uint32_t val = vcore_regs->roi_row_cnfg;

    LONGS_EQUAL(roiConfig.firstRow,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__SHIFT);
    LONGS_EQUAL(roiConfig.lastRow,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__SHIFT);
    LONGS_EQUAL(roiConfig.rowSkip,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__SHIFT);

    val = vcore_regs->roi_col_cnfg;

    LONGS_EQUAL(roiConfig.firstCol,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__SHIFT);
    LONGS_EQUAL(roiConfig.lastCol,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__SHIFT);
    LONGS_EQUAL(roiConfig.colSkip,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__SHIFT);

    LONGS_EQUAL(0U, vcore_regs->roi_frame_ident);
    LONGS_EQUAL(CRYPTO_VCORE_ROI_ROI_MODE_EN__MASK | CRYPTO_VCORE_ROI_ROI_INIT__MASK, vcore_regs->roi);
}

TEST(MacDrvGroup, TestSetRoiConfigWhenActive)
{
    MacDrvRoiConfigT roiConfig;

    macDrvState.isActive = true;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = true;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_NON_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0U;

    roiConfig.firstRow = 82U;
    roiConfig.lastRow = 804U;
    roiConfig.rowSkip = 2U;

    roiConfig.firstCol = 0xFFFU;
    roiConfig.lastCol = 0xFFFU;
    roiConfig.colSkip = 0xFFU;

    roiConfig.frameCount = 0x9abcdef0U;

    ErrorT status = MacDrvSetRoiConfig(&roiConfig);
    LONGS_EQUAL(ERROR_SUCCESS, status);

    uint32_t val = vcore_regs->roi_row_cnfg;

    LONGS_EQUAL(roiConfig.firstRow,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_FIRST_ROW__SHIFT);
    LONGS_EQUAL(roiConfig.lastRow,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_LAST_ROW__SHIFT);
    LONGS_EQUAL(roiConfig.rowSkip,
        (val & CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__MASK) >> CRYPTO_VCORE_ROI_ROW_CNFG_ROW_SKIP__SHIFT);

    val = vcore_regs->roi_col_cnfg;

    LONGS_EQUAL(roiConfig.firstCol,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_FIRST_COL__SHIFT);
    LONGS_EQUAL(roiConfig.lastCol,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_LAST_COL__SHIFT);
    LONGS_EQUAL(roiConfig.colSkip,
        (val & CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__MASK) >> CRYPTO_VCORE_ROI_COL_CNFG_COL_SKIP__SHIFT);

    LONGS_EQUAL(roiConfig.frameCount, vcore_regs->roi_frame_ident);
    LONGS_EQUAL(CRYPTO_VCORE_ROI_ROI_MODE_EN__MASK | CRYPTO_VCORE_ROI_ROI_FC_UPDATE__MASK, vcore_regs->roi);
}

TEST(MacDrvGroup, TestStartAuthenticationRejectedIfNotConfigured)
{
    macDrvState.isActive = false;
    macDrvState.isConfigured = false;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_AGAIN, status);
}

TEST(MacDrvGroup, TestStartAuthenticationRejectedIfAlreadyActive)
{
    macDrvState.isActive = true;
    macDrvState.isConfigured = true;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_ALREADY, status);
}

TEST(MacDrvGroup, TestStartAuthenticationRejectedIfBadKeySize)
{
    macDrvState.isActive = false;
    macDrvState.isConfigured = true;

    macDrvState.keyLenBytes = 42U;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_INVAL, status);
    LONGS_EQUAL(macDrvState.isActive, false);
}

TEST(MacDrvGroup, TestStartAuthenticationGmac)
{
    uint32_t expectedValue;

    // To prove this function doesn't touch the IV registers
    vcore_regs->iv0 = 0x87654321UL;
    vcore_regs->iv1 = 0x98765432UL;
    vcore_regs->iv2 = 0xA9876543UL;
    vcore_regs->iv3 = 0xBA987654UL;

    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = false;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0x0U;
    macDrvState.keyLenBytes = 16U;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_TRUE(macDrvState.isActive);
    LONGS_EQUAL(CRYPTO_VCORE_AESHP_START_AESHP_START__MASK, vcore_regs->aeshp_start);

    expectedValue = CRYPTO_VCORE_AESHP_CNFG_AESHP_AUTHMODE__MASK |
                    (0U << CRYPTO_VCORE_AESHP_CNFG_AESHP_KEYSIZE__SHIFT) |
                    AesGmacCnfgDefault;
    LONGS_EQUAL(expectedValue, vcore_regs->aeshp_cnfg);

    LONGS_EQUAL(0U, vcore_regs->aescmac_start);
    LONGS_EQUAL(AesCmacCnfgDefault, vcore_regs->aescmac_cnfg);
    LONGS_EQUAL(0x87654321UL, vcore_regs->iv0);                     // untouched
    LONGS_EQUAL(0x98765432UL, vcore_regs->iv1);                     // untouched
    LONGS_EQUAL(0xA9876543UL, vcore_regs->iv2);                     // untouched
    LONGS_EQUAL(0xBA987654UL, vcore_regs->iv3);                     // untouched
}

TEST(MacDrvGroup, TestStartAuthenticationCmac)
{
    uint32_t expectedValue;

    // To prove the function doesn't touch the IV registers
    vcore_regs->iv0 = 0x87654321UL;
    vcore_regs->iv1 = 0x98765432UL;
    vcore_regs->iv2 = 0xA9876543UL;
    vcore_regs->iv3 = 0xBA987654UL;

    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = true;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0x0U;
    macDrvState.keyLenBytes = 32U;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_TRUE(macDrvState.isActive);
    LONGS_EQUAL(CRYPTO_VCORE_AESCMAC_START_AESCMAC_START__MASK, vcore_regs->aescmac_start);

    expectedValue = CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_AUTHMODE__MASK |
                    (2U << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_KEYSIZE__SHIFT) |
                    AesCmacCnfgDefault;   // encdec, mode defaults
    LONGS_EQUAL(expectedValue, vcore_regs->aescmac_cnfg);

    LONGS_EQUAL(0U, vcore_regs->aeshp_start);
    LONGS_EQUAL(AesGmacCnfgDefault, vcore_regs->aeshp_cnfg);
    LONGS_EQUAL(0x87654321UL, vcore_regs->iv0);
    LONGS_EQUAL(0x98765432UL, vcore_regs->iv1);
    LONGS_EQUAL(0xA9876543UL, vcore_regs->iv2);
    LONGS_EQUAL(0xBA987654UL, vcore_regs->iv3);
}

TEST(MacDrvGroup, TestStartAuthenticationGmacPreservesHwDefaults)
{
    uint32_t expectedValue;

    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = false;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_FULL_FRAME;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0x0U;
    macDrvState.keyLenBytes = 16U;

    // Set the encdec bits to prove the call doesn't overwrite them
    // Other tests set (and test) all the default bits. This test can use cleared bits
    AesGmacCnfgDefault = 0x00000000UL & AesGmacCnfgDefaultMask;
    vcore_regs->aeshp_cnfg = AesGmacCnfgDefault;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_TRUE(macDrvState.isActive);
    LONGS_EQUAL(CRYPTO_VCORE_AESHP_START_AESHP_START__MASK, vcore_regs->aeshp_start);

    expectedValue = CRYPTO_VCORE_AESHP_CNFG_AESHP_AUTHMODE__MASK |
                    (0U << CRYPTO_VCORE_AESHP_CNFG_AESHP_KEYSIZE__SHIFT) |
                    AesGmacCnfgDefault;
    LONGS_EQUAL(expectedValue, vcore_regs->aeshp_cnfg);

    LONGS_EQUAL(0U, vcore_regs->aescmac_start);
    LONGS_EQUAL(AesCmacCnfgDefault, vcore_regs->aescmac_cnfg);
}

TEST(MacDrvGroup, TestStartAuthenticationCmacPreservesHwDefaults)
{
    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = true;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0x0U;
    macDrvState.keyLenBytes = 32U;

    // Set the mode and encdec bits to prove the call doesn't overwrite them
    // Other tests set and test all the default bits. This test can use cleared bits
    AesCmacCnfgDefault = 0x00000000UL & AesCmacCnfgDefaultMask;
    vcore_regs->aescmac_cnfg = AesCmacCnfgDefault;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_TRUE(macDrvState.isActive);
    LONGS_EQUAL(CRYPTO_VCORE_AESCMAC_START_AESCMAC_START__MASK, vcore_regs->aescmac_start);
    LONGS_EQUAL(CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_AUTHMODE__MASK |
        (2U << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_KEYSIZE__SHIFT) |
        AesCmacCnfgDefault,
        vcore_regs->aescmac_cnfg);

    LONGS_EQUAL(0U, vcore_regs->aeshp_start);
    LONGS_EQUAL(AesGmacCnfgDefault, vcore_regs->aeshp_cnfg);
}

TEST(MacDrvGroup, TestStartAuthenticationCmac192bitKey)
{
    uint32_t aescmacCnfgExpectedValue;

    macDrvState.isActive = false;
    macDrvState.isConfigured = true;
    macDrvState.config.cmacNotGmac = true;
    macDrvState.config.videoAuthmode = MAC_DRV_VIDEO_AUTH_MODE_ROI;
    macDrvState.config.pixelPackingMode = MAC_DRV_PIXEL_PACKING_MODE_OPTIMIZED;
    macDrvState.config.pixelPackingValue = 0x5678U;
    macDrvState.config.frameCount = 0x0U;
    macDrvState.keyLenBytes = 24U;

    ErrorT status = MacDrvStartAuthentication();
    LONGS_EQUAL(ERROR_SUCCESS, status);

    CHECK_TRUE(macDrvState.isActive);
    LONGS_EQUAL(CRYPTO_VCORE_AESCMAC_START_AESCMAC_START__MASK, vcore_regs->aescmac_start);

    aescmacCnfgExpectedValue = CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_AUTHMODE__MASK |
                    (1U << CRYPTO_VCORE_AESCMAC_CNFG_AESCMAC_KEYSIZE__SHIFT) |
                    (AesCmacCnfgDefault);   // encdec, mode defaults

    LONGS_EQUAL(aescmacCnfgExpectedValue, vcore_regs->aescmac_cnfg);

    LONGS_EQUAL(0U, vcore_regs->aeshp_start);
    LONGS_EQUAL(AesGmacCnfgDefault, vcore_regs->aeshp_cnfg);
}

TEST(MacDrvGroup, TestGetCmacStatus)
{
    uint32_t    AesCmacStatusTestValue = 0x87654321;
    uint32_t    retVal;

    // Register is RO, so need a bit of disguise to trick the compiler into letting us write to it
    uint32_t    *aescmac_status_ptr;
    aescmac_status_ptr = (uint32_t*)(&vcore_regs->aescmac_status);
   *aescmac_status_ptr = AesCmacStatusTestValue;

    retVal = MacDrvGetCmacStatus();
    LONGS_EQUAL(AesCmacStatusTestValue, retVal)
}
