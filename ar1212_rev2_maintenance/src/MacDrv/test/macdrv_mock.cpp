//***********************************************************************************
/// \file
///
/// MAC Driver default Mocks
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
#include "MacDrv.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_MAC_DRV_INIT)
    ErrorT MacDrvInit(void)
    {
        mock().actualCall("MacDrvInit");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_RESET_IV)
    void MacDrvResetIv(void)
    {
        mock().actualCall("MacDrvResetIv");
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_SET_SESSION_KEY)
    ErrorT MacDrvSetSessionKey(uint8_t* const key,
        uint32_t const keyLenBytes)
    {
        mock().actualCall("MacDrvSetSessionKey")
            .withParameter("key", key)
            .withParameter("keyLenBytes", keyLenBytes);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_SET_GMAC_IV)
    ErrorT MacDrvSetGmacIv(uint8_t* const iv,
        uint32_t const ivLenBytes)
    {
        mock().actualCall("MacDrvSetGmacIv")
            .withParameter("iv", iv)
            .withParameter("ivLenBytes", ivLenBytes);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_SET_CONFIG)
    ErrorT MacDrvSetConfig(MacDrvConfigT* const config)
    {
        mock().actualCall("MacDrvSetConfig")
            .withParameter("config", config);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_SET_ROI_CONFIG)
    ErrorT MacDrvSetRoiConfig(MacDrvRoiConfigT* const config)
    {
        mock().actualCall("MacDrvSetRoiConfig")
            .withParameter("config", config);
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_START_SELF_TEST)
    ErrorT MacDrvStartSelfTest(void)
    {
        mock().actualCall("MacDrvStartSelfTest");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_RESET)
    void MacDrvResetToIdle(void)
    {
        mock().actualCall("MacDrvResetToIdle");
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_START_AUTHENTICATION)
    ErrorT MacDrvStartAuthentication(void)
    {
        mock().actualCall("MacDrvStartAuthentication");
        return (ErrorT)mock().returnLongIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_MAC_DRV_GET_CMAC_STATUS)
    uint32_t MacDrvGetCmacStatus(void)
    {
        mock().actualCall("MacDrvGetCmacStatus");
        return (uint32_t)mock().returnLongIntValueOrDefault(0x00000000);
    }
#endif
}
