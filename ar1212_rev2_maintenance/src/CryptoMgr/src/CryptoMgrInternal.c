//***********************************************************************************
/// \file
///
/// Crypto Manager internal implementation
///
/// \addtogroup cryptomgr
/// @{
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
//
//*************************************************************************************
#include <stdlib.h>
#include <string.h>

#include "AR0820.h"
#include "Common.h"
#include "Diag.h"
#include "CommandHandler.h"
#include "CryptoMgr.h"
#include "CryptoMgrInternal.h"
#include "AssetMgr.h"
#include "CC312HalPal.h"

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
/// Crypto Manager internal control/state structure
CryptoMgrT cryptoMgr;

/// Compresses a BSV error before reporting it in the info Register
///
/// Function will call the DiagSetCheckpointWithInfo but the info needs to be 16 bits and the error coming
/// from the BSV software is 32 bits. So it needs compressing by removing the middle 16bits that are always zero
///
/// \param[in] checkpoint   Checkpoint to set
/// \param[in] error        Error to be compressed and reported as info
/// \returns void
///
void CryptoMgrReportBsvError(DiagCheckpointT const checkpoint, CCError_t const error)
{
    uint16_t info;
    uint32_t error_masked = error & 0xFF0000FFU;

    info = (uint16_t)(error_masked | (error_masked >> 16));
    DiagSetCheckpointWithInfo(checkpoint, info);
}

/// Compresses a mbedtls error before reporting it in the info Register
///
/// Function will negate the number to be easily check against the ARM source code
///
/// \param[in] checkpoint   Checkpoint to set
/// \param[in] error        Error to be compressed and reported as info
/// \returns void
///
void CryptoMgrReportMbedTlsError(DiagCheckpointT const checkpoint, int32_t const error)
{
    uint16_t info;

    info = (uint16_t)(-error);  // Note: It is negated to make it more clear when debugging against ARM codes
    DiagSetCheckpointWithInfo(checkpoint, info);
}

/// Handles initialization of the CC312 runtime library at Configure phase.
/// \returns
/// \retval ERROR_SUCCESS   Initialization completed succesfully
/// \retval ERROR_SYSERR    CC lib initialization failed
/// \retval ERROR_NOSPC     No space in memory for the initialization
///
ErrorT CryptoMgrOnConfigEntry(void)
{
    ErrorT status;
    AssetMgrCC312TrngCharT CC312TrngChar;

    // Initialize TRNG characterization params
    status = AssetMgrRetrieveCC312TrngChar(&CC312TrngChar);
    if (ERROR_NOENT == status)
    {
        // Asset not present, set the default values that work on RTL Sims.
        CC312TrngChar.R0 = CC_CONFIG_SAMPLE_CNT_ROSC_1;
        CC312TrngChar.R1 = CC_CONFIG_SAMPLE_CNT_ROSC_2;
        CC312TrngChar.R2 = CC_CONFIG_SAMPLE_CNT_ROSC_3;
        CC312TrngChar.R3 = CC_CONFIG_SAMPLE_CNT_ROSC_4;
        status = ERROR_SUCCESS;
    }
    if (ERROR_SUCCESS == status)
    {
        CCRndWorkBuff_t* rndWorkBuff;

        CC_PalTrngParamSet(&CC312TrngChar);

        // Now initialize the Runtime library
        cryptoMgr.rndContext.rndState = &cryptoMgr.rndState;
        cryptoMgr.rndContext.entropyCtx = &cryptoMgr.mbedtlsEntropy;

        rndWorkBuff = (CCRndWorkBuff_t*)calloc(1, sizeof(CCRndWorkBuff_t));

        if (NULL != rndWorkBuff)
        {
            CClibRetCode_t result;
            result = CC_LibInit(&cryptoMgr.rndContext, rndWorkBuff);

            if (CC_LIB_RET_OK != result)
            {
                DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION_ERROR, (uint16_t)result);
                status = ERROR_SYSERR;
            }
            // Clean/Free this memory should be ok as it seems that is not used again
            (void)memset((void*)rndWorkBuff, 0, sizeof(CCRndWorkBuff_t));
            free(rndWorkBuff);
        }
        else
        {
            status = ERROR_NOSPC;
        }
    }

    if (ERROR_SUCCESS == status)
    {
        cryptoMgr.status = CRYPTO_MGR_CONFIGURED;
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_CONFIGURATION, (uint16_t)status);

    return status;
}

/// Handles finalization of the CC312 runtime library at Shutdown phase.
///
/// \returns
/// \retval ERROR_SUCCESS   Shutdown completed succesfully
/// \retval ERROR_SYSERR    Function failed due to a system issue
///
ErrorT CryptoMgrOnShutdownEntry(void)
{
    ErrorT status = CryptoMgrCheckStatus();

    if (ERROR_SUCCESS == status)
    {
        CClibRetCode_t result;
        cryptoMgr.status = CRYPTO_MGR_UNINIT;

        result = CC_LibFini(&cryptoMgr.rndContext);

        if (CC_LIB_RET_OK != result)
        {
            DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN_ERROR, (uint16_t)result);
            status = ERROR_SYSERR;
        }
    }

    DiagSetCheckpointWithInfo(DIAG_CHECKPOINT_CRYPTO_MGR_ON_SHUTDOWN, (uint16_t)status);

    return status;
}

/// Gets the GetSensorID hash value
///
/// \param[out] sensorId     Sensor ID value
///
/// \returns
/// \retval ERROR_SUCCESS   Function completed succesfully
/// \retval ERROR_IO        Function failed to compute the sensor ID
/// \retval ERROR_SYSERR    Function failed due to a system issue
///
ErrorT CryptoMgrGetSensorId(CCHashResult_t sensorId)
{
    ErrorT status = ERROR_SUCCESS;
    LifecycleMgrLifecycleStateT lcsCurrent;

    status = LifecycleMgrGetCurrentLcs(&lcsCurrent);

    if (ERROR_SUCCESS == status)
    {
        if (LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE == lcsCurrent)
        {
            CCError_t error;

            error = CC_BsvSocIDCompute(CRYPTOCELL_CORE_BASE, sensorId);
            if ((CCError_t)CC_OK != error)
            {
                CryptoMgrReportBsvError(DIAG_CHECKPOINT_CRYPTO_MGR_ON_GET_SENSOR_ID_ERROR, error);
                status = ERROR_IO;
            }
        }
        else
        {
            (void)memset(sensorId, 0, sizeof(CCHashResult_t));
        }
    }

    return status;
}

/// Increments by one a number represented by an array (big endian)
///
/// This is based on CC_CommonIncLsbUnsignedCounter but using uint8
/// - modified to increment the least-significant byte first, which is the
///   element at index (dataSizeBytes - 1).
///
/// \param[in] dataSizeBytes   The counter size in bytes
/// \param[in] data            The buffer containing the counter
///
/// \returns
/// \retval ERROR_SUCCESS   Function completed succesfully
/// \retval ERROR_SYSERR    Data pointer is NULL
///
ErrorT CryptoMgrIncrementNumber(uint32_t const dataSizeBytes,
    uint8_t* const data)
{
    ErrorT status;

    status = (NULL == data) ? ERROR_SYSERR : ERROR_SUCCESS;

    if (ERROR_SUCCESS == status)
    {
        uint8_t currCounterVal;
        uint8_t val = 1U;
        uint32_t i;

        for (i = 0U; i < dataSizeBytes; i++)
        {
            uint32_t idx = dataSizeBytes - 1U - i;

            currCounterVal = data[idx];
            data[idx] += val;
            val = (currCounterVal > data[idx]) ? 1U : 0U;
            /// Note that the computation time is deterministic (all loops are taken)
            /// to avoid any side channel attacks
        }
    }

    return status;
}

/// Check if the CryptoMgr has been initialized
///
/// \returns
/// \retval ERROR_SUCCESS   Initialization successful
/// \retval ERROR_SYSERR    Initialization failed or didn't happen
///
ErrorT CryptoMgrCheckStatus(void)
{
    ErrorT error = ERROR_SUCCESS;

    if (CRYPTO_MGR_UNINIT == cryptoMgr.status)
    {
        error = ERROR_SYSERR;
    }

    return error;
}

/// @} endgroup cryptomgr

