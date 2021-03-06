;***********************************************************************************
; Copyright 2018 ON Semiconductor.  All rights reserved.
;
; This software and/or documentation is licensed by ON Semiconductor under limited
; terms and conditions. The terms and conditions pertaining to the software and/or
; documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
; ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
; Do not use this software and/or documentation unless you have carefully read and
; you agree to the limited terms and conditions. By using this software and/or
; documentation, you agree to the limited terms and conditions.
;***********************************************************************************

;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
; DO NOT EDIT - This file is auto-generated by the generate_fatal_errors.py script
;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

;-------------------------------------------------------------------------------
;                               Fatal Error Codes
;-------------------------------------------------------------------------------

SYSTEM_MGR_FATAL_ERROR_NONE                                          EQU 0x000 ; (0) None
SYSTEM_MGR_FATAL_ERROR_HARD_FAULT                                    EQU 0x001 ; (1) Hard Fault exception occurred
SYSTEM_MGR_FATAL_ERROR_UNHANDLED_EVENT                               EQU 0x002 ; (2) An unsupported event was signalled
SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_DOORBELL_BAD_PHASE                 EQU 0x003 ; (3) Doorbell event happened in unexpected phase
SYSTEM_MGR_FATAL_ERROR_INIT_FAILED                                   EQU 0x004 ; (4) Failed to initialize
SYSTEM_MGR_FATAL_ERROR_PLATFORM_INIT_FAILED                          EQU 0x005 ; (5) Platform-layer failed to initialize
SYSTEM_MGR_FATAL_ERROR_EXCEPTION_INIT_FAILED                         EQU 0x006 ; (6) Failed to init exceptions and interrupts
SYSTEM_MGR_FATAL_ERROR_COMPONENT_INIT_FAILED                         EQU 0x007 ; (7) Failed to init application components
SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_NO_PHASE_REENTRY                   EQU 0x008 ; (8) Should never re-enter the same phase
SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_SET_EVENT_INVALID                  EQU 0x009 ; (9) Event identifier should be valid
SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_REGISTER_DUMP_HANDLER_FAILED       EQU 0x00A ; (10) Attempt to register Diag dump handler failed
SYSTEM_MGR_FATAL_ERROR_SYSTEM_MGR_PHASE_INVALID                      EQU 0x00B ; (11) Invalid phase requested for SystemMgrNotifyEnterPhase

SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_BAD_PHASE                     EQU 0x010 ; (16) OnEntryPhase called with bad phase
SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_DOORBELL_INTERRUPT_FAILED EQU 0x011 ; (17) Not possible to enable mailbox interrupt
SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_ENABLE_ICORE_ECC_DED_INTERRUPT_FAILED EQU 0x012 ; (18) Not possible to enable ECC DED failure interrupt
SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_ICORE_ECC_DED_INTERRUPT_FAILED EQU 0x013 ; (19) Not possible to clear pending ECC DED failure interrupt
SYSTEM_MGR_FATAL_ERROR_COMMAND_HANDLER_CLEAR_DOORBELL_INTERRUPT_FAILED EQU 0x014 ; (20) Not possible to clear pending mailbox interrupt

SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_BAD_PHASE                      EQU 0x020 ; (32) OnEntryPhase called with bad phase
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STATUS_BAD_COMMAND         EQU 0x021 ; (33) Health Monitor expects GetStatus command code
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_ENABLE_MONITORING_BAD_COMMAND  EQU 0x022 ; (34) Health Monitor expects EnableMonitoring command code
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_USED_SIZE_NULL_PTR   EQU 0x023 ; (35) Health Monitor NULL pointer passed to HealthMonitorGetStackUsedSizeLongWords()
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_STACK_INFORMATION_ERROR    EQU 0x024 ; (36) Health Monitor call to InitMpuGetStackInformation() returned an error
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_REGISTER_CLOCK_CHANGE_NOTIFIER_FAILED EQU 0x025 ; (37) Health Monitor call to SystemMgrRegisterClockSpeedChangeNotifier() returned an error
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GOT_UNEXPECTED_CALL_TO_ON_CLOCK_CHANGE EQU 0x026 ; (38) Health Monitor got unexpected (not allowed) call to HealthMonitorOnClockChange()
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_STACK_OVERRUN                  EQU 0x027 ; (39) Health Monitor detected that the stack is nearly full
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_VCORE_ERROR                    EQU 0x028 ; (40) Health Monitor detected an error in the Vcore
SYSTEM_MGR_FATAL_ERROR_HEALTH_MONITOR_GET_VCORE_STATUS_NULL_PTR      EQU 0x029 ; (41) Health Monitor NULL pointer passed to HealthMonitorGetVCoreStatus()

SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_CRYPTO_CLOCK_GATING_FAULT           EQU 0x030 ; (48) Clock Gating failure
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_SEC_FILTER_FAULT                    EQU 0x031 ; (49) Fault when applying secure filter
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_PRIV_FILTER_FAULT                   EQU 0x032 ; (50) Fault when applying privilege filter
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_RETRIEVAL_FAULT                     EQU 0x033 ; (51) Failed to retrieve lifecycle state
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_OTPM_FAULT                          EQU 0x034 ; (52) OTPM error during hardware initialization
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_BAD_PHASE                       EQU 0x035 ; (53) OnEntryPhase called with bad phase
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_COMMAND EQU 0x036 ; (54) LifecycleMgrOnApplyDebugEntitlement received invalid command
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_BAD_PARAM EQU 0x037 ; (55) LifecycleMgrOnApplyDebugEntitlement received invalid parameter
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_INIT_FAILED               EQU 0x038 ; (56) Lifecycle Manager failure when entering phase SYSTEM_MGR_PHASE_DEBUG
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_CC312_CONFIG_FAILED             EQU 0x039 ; (57) Lifecycle Manager failure when entering phase SYSTEM_MGR_PHASE_CONFIGURE
SYSTEM_MGR_FATAL_ERROR_LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_RMA_PRESENTED EQU 0x03A ; (58) LifecycleMgrOnApplyDebugEntitlement received a RMA certificate

SYSTEM_MGR_FATAL_ERROR_SHARED_RAM_ECCDED_FAILED                      EQU 0x040 ; (64) ECC DED failure in Icore Shared-RAM

SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_ON_LOAD_PATCH_CHUNK_BAD_COMMAND     EQU 0x050 ; (80) Patch Manager command handling received invalid command
SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_HANDLE_COMMAND_BAD_PARAM            EQU 0x051 ; (81) Patch Manager command handling  received NULL pointer
SYSTEM_MGR_FATAL_ERROR_PATCH_MGR_INIT_FAILED                         EQU 0x052 ; (82) Patch Manager failure on entry to PHASE_INITIALIZE

SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_COMMAND      EQU 0x070 ; (112) Session Manager expects GetCertificates command code
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_GET_CERTIFICATES_BAD_PARAMS       EQU 0x071 ; (113) Session Manager expects valid params ptr to GetCertificates command
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_COMMAND EQU 0x072 ; (114) Session Manager expects SetEphemeralMasterSecret command code
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_EPHEMERAL_MASTER_SECRET_BAD_PARAMS EQU 0x073 ; (115) Session Manager expects valid params ptr to SetEphemeralMasterSecret command
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_COMMAND   EQU 0x074 ; (116) Session Manager expects SetPskSessionKey command code
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_PSK_SESSION_KEY_BAD_PARAMS    EQU 0x075 ; (117) Session Manager expects valid params ptr to SetPskSessionKey command
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_COMMAND       EQU 0x076 ; (118) Session Manager expects SetSessionKey command code
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_SESSION_KEY_BAD_PARAMS        EQU 0x077 ; (119) Session Manager expects valid params ptr to SetSessionKey command
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_COMMAND    EQU 0x078 ; (120) Session Manager expects SetVideoAuthRoi command code
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_SET_VIDEO_AUTH_ROI_BAD_PARAMS     EQU 0x079 ; (121) Session Manager expects valid params ptr to SetVideoAuthRoi command
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_KEY          EQU 0x07A ; (122) MasterSecret asset in NVM has invalid key type
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_NVM_MASTERSECRET_BAD_LENGTH       EQU 0x07B ; (123) MasterSecret asset in NVM has invalid length
SYSTEM_MGR_FATAL_ERROR_SESSION_MGR_DISABLE_VCORE_CLOCK_FAILED        EQU 0x07C ; (124) Session Manager failed to disable the Vcore clock during initialisation

SYSTEM_MGR_FATAL_ERROR_UNUSED                                        EQU 0x080 ; (128) Unused (was SysTickDrv)

SYSTEM_MGR_FATAL_ERROR_PLATFORM_CRITICAL_SECTION_NESTING_NOT_ZERO    EQU 0x090 ; (144) Platform expects the critical section nesting to be non-zero

SYSTEM_MGR_FATAL_ERROR_CC312_PAL_TRNG_PARAM_NOT_SET                  EQU 0x0A0 ; (160) CC312 PAL expects the calibrated TRNG parameters to be copied before

SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_GET_SENSOR_ID_BAD_ARGS             EQU 0x0B0 ; (176) Crypto Manager detected wrong arguments in the GetSensorId function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_DERIVE_KEYS_BAD_ARGS               EQU 0x0B1 ; (177) Crypto Manager detected wrong arguments in the derive keys function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_PATCH_CHUNK_BAD_ARGS  EQU 0x0B2 ; (178) Crypto Manager detected wrong arguments in the authenticate patch chunk function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AUTHENTICATE_ASSET_BAD_ARGS        EQU 0x0B3 ; (179) Crypto Manager detected wrong arguments in the authenticate asset function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_SELF_TEST_BAD_ARGS                 EQU 0x0B4 ; (180) Crypto Manager detected wrong arguments in the self test function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RSA_DECRYPT_BAD_ARGS               EQU 0x0B5 ; (181) Crypto Manager detected wrong arguments in the RSA decryption function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_AEAD_DECRYPT_BAD_ARGS              EQU 0x0B6 ; (182) Crypto Manager detected wrong arguments in the AEAD decryption function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_RANDOM_GENERATOR_BAD_ARGS          EQU 0x0B7 ; (183) Crypto Manager detected wrong arguments in the random gen function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_KEY_SIZE_PARAM                 EQU 0x0B8 ; (184) Crypto Manager detected invalid ptr argument to GetSymmetricKeySize function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_BAD_IS_CALLED_PARAM                EQU 0x0B9 ; (185) Crypto Manager detected invalid ptr argument to GetSensorIdCalled function
SYSTEM_MGR_FATAL_ERROR_CRYPTO_MGR_ON_PHASE_ENTRY_CONFIGURATION       EQU 0x0BA ; (186) Crypto Manager detected a Fatal error during configuration

SYSTEM_MGR_FATAL_ERROR_NVM_MGR_BAD_PHASE                             EQU 0x0C0 ; (192) NvmMgrOnPhaseEntry called with bad phase
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_OTPM_CONFIGURATION_FAILED             EQU 0x0C1 ; (193) NVM Manager  configuration of OTPM driver failed
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_ARGS                   EQU 0x0C2 ; (194) NVM Manager internal parameter passing error
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_CONTEXT          EQU 0x0C3 ; (195) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_ADDRESS    EQU 0x0C4 ; (196) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_BAD_PARAM_ASSET_LENGTH     EQU 0x0C5 ; (197) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FIRST_NOT_INITIALIZED            EQU 0x0C6 ; (198) NVM manager component not initialized
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_CONTEXT           EQU 0x0C7 ; (199) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_ADDRESS     EQU 0x0C8 ; (200) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_PARAM_ASSET_LENGTH      EQU 0x0C9 ; (201) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_NOT_INITIALIZED             EQU 0x0CA ; (202) NVM manager component not initialized
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_NEXT_BAD_MAGIC                   EQU 0x0CB ; (203) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_NOT_INITIALIZED            EQU 0x0CC ; (204) NVM manager component not initialized
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_READ_ASSET_BAD_PARAM_BUFFER           EQU 0x0CD ; (205) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_NOT_INITIALIZED           EQU 0x0CE ; (206) NVM manager component not initialized
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_WRITE_ASSET_BAD_PARAM_BUFFER          EQU 0x0CF ; (207) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_RECORD           EQU 0x0D0 ; (208) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS    EQU 0x0D1 ; (209) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_PARAM_START_ADDRESS EQU 0x0D2 ; (210) NVM Manager unexpected NULL pointer passed into function
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_INIT_FAILED                           EQU 0x0D3 ; (211) NVM Manager failure on entry to PHASE_INITIALIZE
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_OTPM_DRV_CONFIGURE_FAILED             EQU 0x0D4 ; (212) NVM Manager failure from OTPM configuration
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_ON_CLOCK_CHANGE_ASSET_RETRIEVAL_FAILED EQU 0x0D5 ; (213) NVM Manager failure from OTPM configuration
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_LENGTH_NOT_ZERO_IN_EMPTY EQU 0x0D6 ; (214) NVM Manager asset type empty does not have an associated zero length field
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_LENGTH_NOT_ZERO_IN_EMPTY   EQU 0x0D7 ; (215) NVM Manager asset type empty does not have an associated zero length field
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_BAD_RECORD_LENGTH     EQU 0x0D8 ; (216) NVM Manager, NvmMgrFindFreeSpace identified an invalid lentgh in a header record
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_RECORD_LENGTH          EQU 0x0D9 ; (217) NVM Manager, NvmMgrFindAsset identified an invalid lentgh in a header record
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_ASSET_BAD_PARAM_START_ADDRESS_VALUE EQU 0x0DA ; (218) NVM Manager, NvmMgrFindAsset called with address pointer value beyond valid range
SYSTEM_MGR_FATAL_ERROR_NVM_MGR_FIND_FREE_SPACE_OTPM_READ_UNEXPECTED_ERROR EQU 0x0DB ; (219) NVM Manager, NvmMgrFindFreeSpace call to OtpmDrvRead() received unexpected error

SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_ON_PROVISION_ASSET_BAD_ARGS         EQU 0x0E0 ; (224) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_GET_PUBLIC_KEY_BAD_ARGS             EQU 0x0E1 ; (225) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_CERTIFICATE_BAD_ARGS       EQU 0x0E2 ; (226) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_PRIVATE_KEY_BAD_ARGS       EQU 0x0E3 ; (227) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_TRNG_BAD_ARGS              EQU 0x0E4 ; (228) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_PSK_BAD_ARGS               EQU 0x0E5 ; (229) Asset Manager unexpected bad arguments
SYSTEM_MGR_FATAL_ERROR_ASSET_MGR_RETRIEVE_OTPM_CONFIG_BAD_ARGS       EQU 0x0E6 ; (230) Asset Manager unexpected bad arguments

SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_BAD_COMMAND                       EQU 0x300 ; (768) Test Patch command handler received invalid command
SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_BAD_PARAM                         EQU 0x301 ; (769) Test Patch command handler received NULL pointer
SYSTEM_MGR_FATAL_ERROR_PATCH_DEBUG_FILL_STACK_ERROR                  EQU 0x302 ; (770) Test Patch, error when executing PatchFillStack()

SYSTEM_MGR_FATAL_ERROR_MAX                                           EQU 0x3FF ; (1023) Maximum fatal error code value

                END
