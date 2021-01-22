//***********************************************************************************
/// \file
///
/// OTPM Driver default Mocks
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
#include "crypto_otpm.h"
#include "OtpmDrv.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_OTPM_DRV_INIT)
    ErrorT OtpmDrvInit(void)
    {
        mock().actualCall("OtpmDrvInit");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_CONFIGURE)
    // Configures the OTPM driver
    ErrorT OtpmDrvConfigure(const AssetMgrOtpmConfigT* const config, uint32_t clockRateHz)
    {
#if defined(MOCK_OTPM_DRV_CONFIGURE_VERBOSE)
        uint32_t* configuration = (uint32_t*)config;

        printf("OtpmDrvConfigure: ");
        if (NULL != configuration)
        {
            for (uint32_t i = 0; i < sizeof(AssetMgrOtpmConfigT)/sizeof(uint32_t); i++)
            {
                printf("%d:[%x]\t", i, configuration[i]);
            }
        }
        printf("\n");
#endif

        mock().actualCall("OtpmDrvConfigure")
            .withPointerParameter("config", (void*)config)
            .withParameter("clockRateHz", clockRateHz);
            return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_READ)
    // Reads long-words from OTPM
    ErrorT OtpmDrvRead(OtpmDrvAddressT const address,
        uint32_t* const buffer,
        uint32_t numLongWords)
    {
        mock().actualCall("OtpmDrvRead")
            .withParameter("address", address)
            .withOutputParameter("buffer", buffer)
            .withParameter("numLongWords", numLongWords);

#if defined(MOCK_OTPM_DRV_READ_VERBOSE)
        printf("OtpmDrvRead: ");
        for (uint32_t i = 0; i < numLongWords; i++)
        {
            printf("%d:[%x]\t", address+i, buffer[i]);
        }
        printf("\n");
#endif

        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_WRITE)
    // Writes long-words to OTPM
    ErrorT OtpmDrvWrite(OtpmDrvAddressT const address,
        uint32_t* const buffer,
        uint32_t numLongWords)
    {

#if defined(MOCK_OTPM_DRV_WRITE_VERBOSE)
        printf("OtpmDrvWrite: ");
        for (uint32_t i = 0; i < numLongWords; i++)
        {
            printf("%d:[%x]\t", address+i, buffer[i]);
        }
        printf("\n");
#endif

        mock().actualCall("OtpmDrvWrite")
            .withParameter("address", address)
            .withParameter("buffer", buffer)
            .withParameter("numLongWords", numLongWords);
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_GET_LAST_ERROR)
    // Returns the last hardware error, and clears error flags to prevent false
    ErrorT OtpmDrvGetLastError(OtpmDrvErrorT* const error)
    {
        mock().actualCall("OtpmDrvGetLastError")
            .withOutputParameter("error", error);
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_IS_HARDWARE_FAULT)
    // Indicates if a hardware fault has occurred
    bool OtpmDrvIsHardwareFault(void)
    {
        mock().actualCall("OtpmDrvIsHardwareFault");
        return (bool)mock().returnBoolValueOrDefault(false);
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_ARRAY_SIZE_LONG_WORDS)
    // Returns size of OTPM array in long words
    uint32_t OtpmDrvGetArraySizeLongWords(void)
    {
        mock().actualCall("OtpmDrvGetArraySizeLongWords");
        return (ErrorT)mock().returnUnsignedIntValueOrDefault((CRYPTO_OTPM_REG_BASE - CRYPTO_OTPM_DATA) / sizeof(uint32_t));
    }
#endif

#if !defined(MOCK_DISABLE_OTPM_DRV_GET_USER_SPACE_ADDRESS)
    // Returns address of start of user space
    OtpmDrvAddressT OtpmDrvGetUserSpaceAddress(void)
    {
        mock().actualCall("OtpmDrvGetUserSpaceAddress");
        return (OtpmDrvAddressT)mock().returnUnsignedIntValueOrDefault(44U);
    }
#endif


}
