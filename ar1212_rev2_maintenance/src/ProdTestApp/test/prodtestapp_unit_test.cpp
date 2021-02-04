//***********************************************************************************
/// \file
///
/// ProdTestApp unit test
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
#include "ProdTestApp.h"
#include "ProdTestAppInternal.h"
#include "LifecycleMgr.h"
#include "Toolbox.h"
#include "CC312HalPalPatch.h"
}

extern "C"
{
    // Make sure the TRNG are created
    CCBool isPalTrngParamSet = CC_FALSE;
    CC_PalTrngParams_t cc312PalTrngParameter;

	uint32_t ProdTestAppGetMaxErrorArrayLengthLongWords(void)
	{
		mock().actualCall("ProdTestAppGetMaxErrorArrayLengthLongWords");
        return (uint32_t)mock().returnUnsignedIntValueOrDefault(9U);
	}
}



//default number of addresses to test
#define ADDR_LENGTH_LONG_WORDS 10U

//for testing max error limit
#define MAX_ERROR_COUNT 15U

const uint32_t BAD_SECTOR_UNCORRECTABLE = 5U;
const uint32_t BAD_SECTOR_CORRECTABLE = 4U;

const uint32_t GOOD_SECTOR = 0U;

//dummy error for referencing in mocks when no error exists
const OtpmDrvErrorT defaultError = {0,true};

//dummy user space address for blank checks which don't require it
const NvmMgrAddressT dummyAddress = 0U;

ProdTestAppResponseParametersT commandResponse;
ProdTestAppCommandParamsT* commandParams = (ProdTestAppCommandParamsT*)&commandResponse;

//global default lifecycle state
LifecycleMgrLifecycleStateT lifeState;


TEST_GROUP(ProdTestAppPatch)
{
    void setup(void)
    {
        lifeState = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
    }

    void teardown(void)
    {

    }
};

TEST_GROUP(ProdTestAppBlankChecks)
{
	void setup(void)
	{
		lifeState = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
		mock().expectOneCall("ProdTestAppGetMaxErrorArrayLengthLongWords")
				.andReturnValue(9U);
	}

	void teardown(void)
	{

	}
};

TEST_GROUP(ProdTestAppVerifications)
{
	void setup(void)
	{
		lifeState = LIFECYCLE_MGR_LIFECYCLE_STATE_SECURE;
		mock().expectOneCall("ProdTestAppGetMaxErrorArrayLengthLongWords")
				.andReturnValue(9U);
	}

	void teardown(void)
	{

	}
};


// OTPM BLANK CHECK TESTS
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksErrorOverflow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		errorInjection.errorAddress = (uint16_t)i;
		mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_SUCCESS);
	}

	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SYSERR,err);

}
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksWrongLifecycleState)
{
	mock().clear();
	LifecycleMgrLifecycleStateT cmState = LIFECYCLE_MGR_LIFECYCLE_STATE_RMA;//for example
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&cmState,sizeof(LifecycleMgrLifecycleStateT));
	ErrorT err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_ACCESS,err);
}
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksLifecycleError)
{
	mock().clear();
	LifecycleMgrLifecycleStateT cmState = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&cmState,sizeof(LifecycleMgrLifecycleStateT))
			.andReturnValue(ERROR_SYSERR);
	ErrorT err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SYSERR,err);
}

TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksWrongCommandCode)
{
	mock().clear();
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	ErrorT err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_NULL,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_INVAL,err);

}
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksNullPointer)
{
	mock().clear();
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	ErrorT err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)NULL);
	CHECK_EQUAL(ERROR_INVAL,err);
}
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankChecksBadParams)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	uint32_t invalid = 100;
	ErrorT err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)&invalid);
	CHECK_EQUAL(ERROR_INVAL,err);
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckNoErrorsSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(0,commandResponse.blockLengthWords);
	CHECK_EQUAL(65535,commandResponse.checkSum);
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckNoErrorsFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(0,commandResponse.blockLengthWords);
	CHECK_EQUAL(65535,commandResponse.checkSum);
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckGoodWordMultipleBadECCSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;

	OtpmDrvErrorT firstErrorInjection = {4,true};
	OtpmDrvErrorT secondErrorInjection = {6,false};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(4U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else if(6U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&secondErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&defaultError,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(2U,commandResponse.blockLengthWords);
	CHECK_EQUAL((4U | (1<<12)),commandResponse.dataBlock[0]);
	CHECK_EQUAL((6U | (1<<13)),commandResponse.dataBlock[1]);
	CHECK_EQUAL((uint16_t)~(((4U | (1<<12))+(6U | (1<<13))+2U)),commandResponse.checkSum);
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckOneCorrectableErrorFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {5U,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
			//PROD_TEST_APP_MODE_FAST mode, so break on first error
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}

	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((5U | (1<<12))+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U | (1<<12),commandResponse.dataBlock[0])
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckOneCorrectableErrorSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {5U,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((5U | (1<<12))+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U | (1<<12),commandResponse.dataBlock[0])
}


TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckOneUncorrectableErrorFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {5U,false};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
			//PROD_TEST_APP_MODE_FAST mode, so break on first error
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}

	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((5U | (1<<13))+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U | (1<<13),commandResponse.dataBlock[0])
}



TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckOneUncorrectableErrorSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {5U,false};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((5U | (1<<13))+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U | (1<<13),commandResponse.dataBlock[0])
}

TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckBlankECCNonBlankWordSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0U;
	commandResponse.checkSum = 0U;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0U,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(5U+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U,commandResponse.dataBlock[0]);
}

TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckBlankECCMultipleNonBlankWordSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0U;
	commandResponse.checkSum = 0U;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0U,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		if((5U == i)||(7U == i))
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(2U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(5U+7U+2U),commandResponse.checkSum);
	CHECK_EQUAL(5U,commandResponse.dataBlock[0]);
	CHECK_EQUAL(7U,commandResponse.dataBlock[1]);
}

TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckBlankECCMultipleNonBlankWordFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0U;
	commandResponse.checkSum = 0U;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0U,true};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		}
	}

	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(5U+1U),commandResponse.checkSum);
	CHECK_EQUAL(5U,commandResponse.dataBlock[0]);

}

// Uncorrectable errors at address 3 and 9, correctable error at address 5
TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckMultipleErrorsSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT firstErrorInjection = {3U,false};
	OtpmDrvErrorT secondErrorInjection = {5U,true};
	OtpmDrvErrorT thirdErrorInjection = {9U,false};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(3U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else if(5U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&secondErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else if(9U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&thirdErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(3U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(((5U | 1<<12)+(3U | 1<<13)+(9U | 1<<13))+3U),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<13),commandResponse.dataBlock[0]);
	CHECK_EQUAL(5U | (1<<12),commandResponse.dataBlock[1]);
	CHECK_EQUAL(9U | (1<<13),commandResponse.dataBlock[2]);
}

TEST(ProdTestAppBlankChecks,TestProdTestAppBlankCheckMultipleErrorsFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&dummyAddress,sizeof(NvmMgrAddressT));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT firstErrorInjection = {3U,false};

	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(3U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&BAD_SECTOR_UNCORRECTABLE,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(BAD_SECTOR_UNCORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}

	err = ProdTestAppOnOtpmBlankCheck(HOST_COMMAND_CODE_OTPM_BLANK_CHECK,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((3U | (1<<13))+1U),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<13),commandResponse.dataBlock[0]);
}





//OTPM VERIFICATION TESTS
TEST(ProdTestAppVerifications,TestProdTestAppVerificationsWrongLifecycleState)
{
	mock().clear();
	LifecycleMgrLifecycleStateT cmState = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&cmState,sizeof(LifecycleMgrLifecycleStateT));
	ErrorT err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_ACCESS,err);
}
TEST(ProdTestAppVerifications,TestProdTestAppVerificationsLifecycleError)
{
	mock().clear();
	LifecycleMgrLifecycleStateT cmState = LIFECYCLE_MGR_LIFECYCLE_STATE_CM;
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&cmState,sizeof(LifecycleMgrLifecycleStateT))
			.andReturnValue(ERROR_SYSERR);
	ErrorT err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SYSERR,err);
}
TEST(ProdTestAppVerifications,TestProdTestAppVerificationsWrongCommandCode)
{
	mock().clear();
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));

	ErrorT err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_NULL,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_INVAL,err);

}
TEST(ProdTestAppVerifications,TestProdTestAppVerificationsNullPointer)
{
	mock().clear();
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));

	ErrorT err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)NULL);
	CHECK_EQUAL(ERROR_INVAL,err);
}
TEST(ProdTestAppVerifications,TestProdTestAppVerificationsBadParams)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	uint32_t invalid = 100;
	ErrorT err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)&invalid);
	CHECK_EQUAL(ERROR_INVAL,err);
}


TEST(ProdTestAppVerifications,TestProdTestAppBlankCheckNvmMgrUninitialised)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	NvmMgrAddressT address = 0U;
	NvmMgrAddressT* offset = &address;
	ErrorT err = ERROR_SUCCESS;
mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
		.withOutputParameterReturning("offset",offset,sizeof(NvmMgrAddressT))
		.andReturnValue(ERROR_ACCESS);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_ACCESS,err);
}


TEST(ProdTestAppVerifications,TestProdTestAppVerifyNoErrorsSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));
	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(0,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~0,commandResponse.checkSum);
}


TEST(ProdTestAppVerifications,TestProdTestAppVerifyNoErrorsFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT errorInjection = {0,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(0,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~0,commandResponse.checkSum);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyOneCorrectableECCErrorSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0,true};
	OtpmDrvErrorT errorInjection = {3,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{

			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		if(3U == i)
		{
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((3U | (1<<12))+1U),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<12),commandResponse.dataBlock[0]);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyOneCorrectableECCErrorFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0,true};
	OtpmDrvErrorT errorInjection = {3,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{

			mock().expectOneCall("OtpmDrvRead")
					.withParameter("address",i)
					.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
					.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		if(3U == i)
		{
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&errorInjection,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_SUCCESS);
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvGetLastError")
					.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
					.andReturnValue(ERROR_NOENT);
		}
	}

	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~((3U | (1<<12))+1U),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<12),commandResponse.dataBlock[0]);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyBlankECCBadWordSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(7U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		}
		mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(7U+1U),commandResponse.checkSum);
	CHECK_EQUAL(7U,commandResponse.dataBlock[0]);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyBlankECCBadWordFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0;
	commandResponse.checkSum = 0;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
		if(7U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
		}

	}

	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(7U+1U),commandResponse.checkSum);
	CHECK_EQUAL(7U,commandResponse.dataBlock[0]);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyBadECCBeforeUserSpaceAndBadWordAndBadECCAfterSlow)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0U;
	commandResponse.checkSum = 0U;
	commandParams->mode = PROD_TEST_APP_MODE_SLOW;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0U,true};
	OtpmDrvErrorT firstErrorInjection = {3U,false};
	OtpmDrvErrorT secondErrorInjection = {7U,true};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(7U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&secondErrorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_SUCCESS);
		}
		else if(8U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&BAD_SECTOR_CORRECTABLE,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(BAD_SECTOR_CORRECTABLE)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		}
		else if(3U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_SUCCESS);
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		}

	}
	mock().expectOneCall("OtpmDrvRead")
			.withParameter("address",ADDR_LENGTH_LONG_WORDS)
			.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
			.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t))
			.andReturnValue(ERROR_RANGE);
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
			.andReturnValue(ERROR_NOENT);
	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(3U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(3U + (3U | (1<<13)) + (7U | (1<<12)) + 8U),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<13),commandResponse.dataBlock[0]);
	CHECK_EQUAL(7U | (1<<12),commandResponse.dataBlock[1]);
	CHECK_EQUAL(8U,commandResponse.dataBlock[2]);
}

TEST(ProdTestAppVerifications,TestProdTestAppVerifyBadECCBeforeUserSpaceAndBadWordAndBadECCAfterFast)
{
	mock().expectOneCall("LifecycleMgrGetCurrentLcs")
			.withOutputParameterReturning("lcsCurrent",&lifeState,sizeof(LifecycleMgrLifecycleStateT));
	mock().expectOneCall("OtpmDrvGetLastError")
			.withOutputParameterReturning("error",&defaultError,sizeof("OtpmDrvErrorT"));

	commandResponse.blockLengthWords = 0U;
	commandResponse.checkSum = 0U;
	commandParams->mode = PROD_TEST_APP_MODE_FAST;
	ErrorT err = ERROR_SUCCESS;
	OtpmDrvErrorT defaultECC = {0U,true};
	OtpmDrvErrorT firstErrorInjection = {3U,false};
 	NvmMgrAddressT freeSpace = 5U;

	mock().expectOneCall("NvmMgrGetFreeSpaceAddress")
			.withOutputParameterReturning("offset",&freeSpace,sizeof(NvmMgrAddressT))
			.andReturnValue(ERROR_SUCCESS);
	for(uint32_t i = 0U;i<ADDR_LENGTH_LONG_WORDS;i++)
	{
		if(3U == i)
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&firstErrorInjection,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_SUCCESS);
			break;
		}
		else
		{
			mock().expectOneCall("OtpmDrvRead")
				.withParameter("address",i)
				.withOutputParameterReturning("buffer",&GOOD_SECTOR,sizeof(uint32_t))
				.withParameter("numLongWords",sizeof(GOOD_SECTOR)/sizeof(uint32_t));
			mock().expectOneCall("OtpmDrvGetLastError")
				.withOutputParameterReturning("error",&defaultECC,sizeof(OtpmDrvErrorT))
				.andReturnValue(ERROR_NOENT);
		}
	}

	err = ProdTestAppOnOtpmVerify(HOST_COMMAND_CODE_OTPM_VERIFY,(CommandHandlerCommandParamsT)commandParams);
	CHECK_EQUAL(ERROR_SUCCESS,err);
	CHECK_EQUAL(1U,commandResponse.blockLengthWords);
	CHECK_EQUAL((uint16_t)~(1U + (3U | (1<<13))),commandResponse.checkSum);
	CHECK_EQUAL(3U | (1<<13),commandResponse.dataBlock[0]);
}
