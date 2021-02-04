//***********************************************************************************
/// \file
///
/// System Driver default Mocks
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
#include "SystemDrv.h"
#include "Diag.h"
}

// Mocks
extern "C"
{
#if !defined(MOCK_DISABLE_SYSTEM_DRV_GET_CHECKPOINT)
    uint16_t SystemDrvGetCheckpoint(void)
    {
        mock().actualCall("SystemDrvGetCheckpoint");
        return (uint16_t)mock().returnUnsignedIntValueOrDefault(0);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_SET_CHECKPOINT)
    void SystemDrvSetCheckpoint(uint16_t const checkpoint)
    {
        mock().actualCall("SystemDrvSetCheckpoint")
            .withParameter("checkpoint", checkpoint);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_SET_INFO)
    void SystemDrvSetInfo(uint16_t const info)
    {
        mock().actualCall("SystemDrvSetInfo")
            .withParameter("info", info);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_SET_CRYPTO_ERROR)
    void SystemDrvSetCryptoError(uint32_t const errorId, uint32_t const context)
    {
        mock().actualCall("SystemDrvSetCryptoError")
            .withParameter("errorId", errorId)
            .withParameter("context", context);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_TERMINATE)
    void SystemDrvTerminate(void)
    {
        mock().actualCall("SystemDrvTerminate");
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_GET_PAUSEPOINT)
    uint16_t SystemDrvGetPausepoint(void)
    {
        mock().actualCall("SystemDrvGetPausepoint");
        return (uint16_t)mock().returnUnsignedIntValueOrDefault(DIAG_CHECKPOINT_DONT_USE);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_SET_PAUSED)
    void SystemDrvSetPaused(void)
    {
        mock().actualCall("SystemDrvSetPaused");
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_IS_PAUSED)
    bool SystemDrvIsPaused(void)
    {
        mock().actualCall("SystemDrvIsPaused");
        return (bool)mock().returnBoolValueOrDefault(false);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_WAIT_FOR_INTERRUPT)
    void SystemDrvWaitForInterrupt(void)
    {
        mock().actualCall("SystemDrvWaitForInterrupt");
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_ENABLE_CLOCK)
    ErrorT SystemDrvEnableClock(SystemDrvCssClockT const clock, bool const enable)
    {
        mock().actualCall("SystemDrvEnableClock")
            .withParameter("clock", clock)
            .withParameter("enable", enable);
        return (ErrorT)mock().returnUnsignedIntValueOrDefault(ERROR_SUCCESS);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_ENABLE_CPU_WAIT)
    void SystemDrvEnableCpuWait(bool const enable)
    {
        mock().actualCall("SystemDrvEnableCpuWait")
            .withParameter("enable", enable);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_ENABLE_I2C_WRITE_CRC_RESET)
    void SystemDrvEnableI2cWriteCrcReset(bool const enable)
    {
        mock().actualCall("SystemDrvEnableI2cWriteCrcReset")
            .withParameter("enable", enable);
    }
#endif
#if !defined(MOCK_DISABLE_SYSTEM_DRV_GET_PLL_LOCK_STATUS)
    bool SystemDrvIsPllLocked(void)
    {
        mock().actualCall("SystemDrvIsPllLocked");
        return (bool)mock().returnBoolValueOrDefault(false);
    }
#endif
}
