####################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions. The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions. By using this software and/or
# documentation, you agree to the limited terms and conditions.
#
####################################################################################

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# DO NOT EDIT - This file is auto-generated by the generate_checkpoints.py script
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

from enum import IntEnum, unique

@unique
class DiagCheckpoint(IntEnum):
    """Diagnostics checkpoint enumeration."""

    DONT_USE = 0                                                        # (0x0000) Indicates no checkpoint - don't use!
    INIT_START = 1                                                      # (0x0001) InitStart entry point
    INIT_STACK = 2                                                      # (0x0002) Initialize the stack contents
    INIT_HEAP = 3                                                       # (0x0003) Initialize the heap contents
    INIT_DATA = 4                                                       # (0x0004) Initialize the data contents
    INIT_PATCH = 5                                                      # (0x0005) Initialize the patch contents
    INIT_SYSTEM = 6                                                     # (0x0006) Initializing the system and MPU
    INIT_C_STARTUP = 7                                                  # (0x0007) Initializing the C start-up

    MAIN = 16                                                           # (0x0010) Main called

    COMMAND_HANDLER_INIT = 32                                           # (0x0020) Command Handler has initialized
    COMMAND_HANDLER_ENTER_DEBUG = 33                                    # (0x0021) Command Handler has entered debug phase
    COMMAND_HANDLER_DOORBELL_ISR = 34                                   # (0x0022) Command Handler doorbell ISR active
    COMMAND_HANDLER_ON_DOORBELL_EVENT = 35                              # (0x0023) Command Handler OnDoorbellEvent called
    COMMAND_HANDLER_INVOKE_HANDLER = 36                                 # (0x0024) Command Handler OnDoorbellEvent about to invoke handler
    COMMAND_HANDLER_ON_DOORBELL_EVENT_DONE = 37                         # (0x0025) Command Handler completed Doorbell event processing

    SYSTEM_MGR_INIT_START = 64                                          # (0x0040) System Manager starting initialization
    SYSTEM_MGR_INIT_DONE = 65                                           # (0x0041) System Manager has completed initialization
    SYSTEM_MGR_NOTIFY_ENTER_PHASE = 66                                  # (0x0042) System Manager notifying a phase change handler
    SYSTEM_MGR_NOTIFY_ENTER_PHASE_DONE = 67                             # (0x0043) System Manager phase change complete
    SYSTEM_MGR_ON_DOORBELL_EVENT = 68                                   # (0x0044) System Manager OnDoorbellEvent called
    SYSTEM_MGR_ON_INITIALIZE = 69                                       # (0x0045) System Manager entering the Initialize phase
    SYSTEM_MGR_ON_DEBUG_START = 70                                      # (0x0046) System Manager entering the Debug phase
    SYSTEM_MGR_ON_PATCH_START = 71                                      # (0x0047) System Manager entering the Patch phase
    SYSTEM_MGR_ON_PATCH_LOAD_COMPLETE = 72                              # (0x0048) System Manager has received the Patch Load Complete event
    SYSTEM_MGR_ON_CONFIGURE_START = 73                                  # (0x0049) System Manager entering the Configure phase
    SYSTEM_MGR_ON_SESSION_START = 74                                    # (0x004A) System Manager entering the Session phase
    SYSTEM_MGR_ON_SHUTDOWN = 75                                         # (0x004B) System Manager entering the Shutdown phase
    SYSTEM_MGR_ON_WAIT_FOR_INTERRUPT = 76                               # (0x004C) System Manager has processed all pending events

    PATCH_MGR_INIT = 96                                                 # (0x0060) Patch Manager has initialized
    PATCH_MGR_ON_LOAD_PATCH_CHUNK = 97                                  # (0x0061) PatchMgrOnLoadPatchChunk() called
    PATCH_MGR_ON_LOAD_PATCH_CHUNK_PROCESS = 98                          # (0x0062) PatchMgrOnLoadPatchChunk() passed initial checks and started processing command parameters
    PATCH_MGR_ON_LOAD_PATCH_CHUNK_DECRYPTING = 99                       # (0x0063) About to decrypt asset
    PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT = 100                            # (0x0064) PatchMgrOnLoadPatchChunk() exit without error
    PATCH_MGR_ON_LOAD_PATCH_CHUNK_EXIT_ERROR = 101                      # (0x0065) PatchMgrOnLoadPatchChunk() exit without error
    PATCH_MGR_PROCESS_CHUNK = 102                                       # (0x0066) PatchMgrProcessChunk() called
    PATCH_MGR_PROCESS_CHUNK_PROCESSING = 103                            # (0x0067) Command format OK - start processing asset
    PATCH_MGR_PROCESS_CHUNK_COPY_PATCH = 104                            # (0x0068) Command and asset passed all test - copy data
    PATCH_MGR_PROCESS_CHUNK_CALL_LOADER = 105                           # (0x0069) Patch download complete execute loader
    PATCH_MGR_PROCESS_CHUNK_EXIT = 106                                  # (0x006A) PatchMgrProcessChunk() exit without error
    PATCH_MGR_PROCESS_CHUNK_EXIT_ERROR = 107                            # (0x006B) PatchMgrProcessChunk() exit with error

    HEALTH_MONITOR_INIT = 128                                           # (0x0080) Health Monitor has initialized
    HEALTH_MONITOR_ENTER_SHUTDOWN = 129                                 # (0x0081) Health Monitor has entered shutdown
    HEALTH_MONITOR_ON_ENABLE_MONITORING = 130                           # (0x0082) Health Monitor handling EnableMonitoring command
    HEALTH_MONITOR_ON_GET_STATUS = 131                                  # (0x0083) Health Monitor handling GetStatus command

    PLATFORM_INIT_START = 160                                           # (0x00A0) Platform initialization has started
    PLATFORM_INIT_DONE = 161                                            # (0x00A1) Platform initialization has completed
    DRIVER_INIT_START = 162                                             # (0x00A2) Driver initialization has started
    DRIVER_INIT_DONE = 163                                              # (0x00A3) Driver initialization is complete

    LIFECYCLE_MGR_INIT = 192                                            # (0x00C0) Lifecycle Manager initialization has started
    LIFECYCLE_MGR_CC312_INIT_ENTRY = 193                                # (0x00C1) Lifecycle Manager cc312 initialization entry point
    LIFECYCLE_MGR_CC312_INIT_FAIL = 194                                 # (0x00C2) Lifecycle Manager cc312 initialization failed
    LIFECYCLE_MGR_CC312_GET_LCS = 195                                   # (0x00C3) Lifecycle Manager cc312 get life cycle state
    LIFECYCLE_MGR_CC312_CONFIG_GATING = 196                             # (0x00C4) Lifecycle Manager cc312 configure gating
    LIFECYCLE_MGR_CC312_CONFIG_SEC_FILTERS = 197                        # (0x00C5) Lifecycle Manager cc312 configure secure filter
    LIFECYCLE_MGR_CC312_CONFIG_PRIV_FILTERS = 198                       # (0x00C6) Lifecycle Managercc312 configure priviledge filter
    LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ENTRY = 199                   # (0x00C7) LifecycleMgrOnApplyDebugEntitlement entry point
    LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT = 200                         # (0x00C8) LifecycleMgrOnApplyDebugEntitlement  call CC_BsvSecureDebugSet()
    LIFECYCLE_MGR_APPLY_DEBUG_ENTITLEMENT_ERROR = 201                   # (0x00C9) LifecycleMgrOnApplyDebugEntitlement failure

    CRYPTO_MGR_ON_CONFIGURATION = 224                                   # (0x00E0) Crypto Manager finshed configuration
    CRYPTO_MGR_ON_SHUTDOWN = 225                                        # (0x00E1) Crypto Manager finshed shutdown
    CRYPTO_MGR_ON_CONFIGURATION_ERROR = 226                             # (0x00E2) Crypto Manager reporting an error from CC library during configuration
    CRYPTO_MGR_ON_SHUTDOWN_ERROR = 227                                  # (0x00E3) Crypto Manager reporting an error from CC library during shutdown
    CRYPTO_MGR_ON_GET_SENSOR_ID = 228                                   # (0x00E4) Crypto Manager handling GetSensorId command
    CRYPTO_MGR_ON_GET_SENSOR_ID_ERROR = 229                             # (0x00E5) Crypto Manager reporting a Bsv error when handling GetSensorId command
    CRYPTO_MGR_ON_PAL_ABORT = 230                                       # (0x00E6) Crypto Manager abort after CC runtime error
    CRYPTO_MGR_ON_AUTHENTICATE_PATCH = 231                              # (0x00E7) Crypto Manager finished patch authentication
    CRYPTO_MGR_ON_AUTHENTICATE_PATCH_ERROR = 232                        # (0x00E8) Crypto Manager reporting a Bsv error when authenticating a patch
    CRYPTO_MGR_ON_AUTHENTICATE_ASSET = 233                              # (0x00E9) Crypto Manager finished asset authentication
    CRYPTO_MGR_ON_AUTHENTICATE_ASSET_ERROR = 234                        # (0x00EA) Crypto Manager finished asset authentication with a mbedtls error
    CRYPTO_MGR_ON_RSA_DECRYPT = 235                                     # (0x00EB) Crypto Manager finished the rsa decryption
    CRYPTO_MGR_ON_RSA_DECRYPT_ERROR = 236                               # (0x00EC) Crypto Manager finished the rsa decryption with a mbedtls error
    CRYPTO_MGR_ON_AEAD_DECRYPT = 237                                    # (0x00ED) Crypto Manager finished the ccm/gsm decryption
    CRYPTO_MGR_ON_AEAD_DECRYPT_ERROR = 238                              # (0x00EE) Crypto Manager finished the ccm/gsm decryption with a mbedtls error
    CRYPTO_MGR_ON_KEY_DERIVATION = 239                                  # (0x00EF) Crypto Manager finished the key derivation
    CRYPTO_MGR_ON_KEY_DERIVATION_ERROR = 240                            # (0x00F0) Crypto Manager finished the key derivation with a mbedtls error
    CRYPTO_MGR_ON_RANDOM_GENERATOR = 241                                # (0x00F1) Crypto Manager finished the random generation
    CRYPTO_MGR_ON_RANDOM_GENERATOR_ERROR = 242                          # (0x00F2) Crypto Manager finished the random generation with a mbedtls error

    SESSION_MGR_INIT = 256                                              # (0x0100) Session Manager initialization has started
    SESSION_MGR_ENTER_SHUTDOWN = 257                                    # (0x0101) Session Manager has entered shutdown
    SESSION_MGR_ON_GET_CERTIFICATES = 258                               # (0x0102) Session Manager handling GetCertificates command
    SESSION_MGR_ON_GET_CERTIFICATES_DONE = 259                          # (0x0103) Session Manager completed GetCertificates command
    SESSION_MGR_ON_SET_EPH_MASTER_SECRET = 260                          # (0x0104) Session Manager handling SetEphemeralMasterSecret command
    SESSION_MGR_ON_SET_EPH_MASTER_SECRET_DONE = 261                     # (0x0105) Session Manager completed SetEphemeralMasterSecret command
    SESSION_MGR_ON_SET_PSK_SESSION_KEY = 262                            # (0x0106) Session Manager handling SetPskSessionKey command
    SESSION_MGR_ON_SET_PSK_SESSION_KEY_DONE = 263                       # (0x0107) Session Manager completed SetPskSessionKey command
    SESSION_MGR_ON_SET_SESSION_KEY = 264                                # (0x0108) Session Manager handling SetSessionKey command
    SESSION_MGR_ON_SET_SESSION_KEY_DONE = 265                           # (0x0109) Session Manager completed SetSessionKey command
    SESSION_MGR_ON_SET_VIDEO_AUTH_ROI = 266                             # (0x010A) Session Manager handling SetVideoAuthRoi command
    SESSION_MGR_ON_SET_VIDEO_AUTH_ROI_DONE = 267                        # (0x010B) Session Manager completed SetVideoAuthRoi command
    SESSION_MGR_NO_PUBLIC_KEY = 268                                     # (0x010C) Session Manager failed to retrieve public key
    SESSION_MGR_NO_SENSOR_CERTIFICATE = 269                             # (0x010D) Session Manager failed to retrieve the sensor certificate
    SESSION_MGR_NO_VENDOR_CERTIFICATE = 270                             # (0x010E) Session Manager failed to retrieve the vendor certificate
    SESSION_MGR_CONFIG_MAC_DRIVER = 271                                 # (0x010F) Session Manager about to configure the MAC driver
    SESSION_MGR_CONFIG_MAC_ROI = 272                                    # (0x0110) Session Manager about to configure the MAC ROI
    SESSION_MGR_GENERATE_RANDOMS = 273                                  # (0x0111) Session Manager about to generate the salt, IV and Nonce
    SESSION_MGR_DERIVE_KEYS = 274                                       # (0x0112) Session Manager about to derive the session keys
    SESSION_MGR_RSA_DECRYPT = 275                                       # (0x0113) Session Manager about to decrypt the RSA-encrypted master secret
    SESSION_MGR_VALIDATE_RSA_SESSION_PARAMS = 276                       # (0x0114) Session Manager has validated the RSA session params

    NVM_MGR_INIT = 288                                                  # (0x0120) NVM Manager initialization has started
    NVM_MGR_READ_ASSET_ENTRY = 289                                      # (0x0121) NVM Manager reached main body of function
    NVM_MGR_FIND_FIRST_ENTRY = 290                                      # (0x0122) NVM Manager reached main body of function
    NVM_MGR_FIND_FIRST_SUCCESS = 291                                    # (0x0123) NVM Manager asset found
    NVM_MGR_FIND_NEXT_ENTRY = 292                                       # (0x0124) NVM Manager reached main body of function
    NVM_MGR_FIND_NEXT_SUCCESS = 293                                     # (0x0125) NVM Manager asset found
    NVM_MGR_WRITE_ASSET_ENTRY = 294                                     # (0x0126) NVM Manager reached main body of function
    NVM_MGR_WRITE_ASSET_FIND_BLANK = 295                                # (0x0127) NVM Manager check for empty location
    NVM_MGR_WRITE_ASSET = 296                                           # (0x0128) NVM Manager start writing asset
    NVM_MGR_WRITE_ASSET_RECORD_HEADER_CHECK = 297                       # (0x0129) NVM Manager check record header written successfully
    NVM_MGR_WRITE_ASSET_PAYLOAD_CHECK = 298                             # (0x012A) NVM Manager check asset written successfully
    NVM_MGR_ON_CLOCK_CHANGE_ENTRY = 299                                 # (0x012B) NVM Manager, NvmMgrOnClockChange() entry point
    NVM_MGR_ON_CLOCK_CHANGE_FOUND_ASSET = 300                           # (0x012C) NVM Manager, read OTPM configuration from NVM
    NVM_MGR_ON_CLOCK_CHANGE_NO_ASSET_FOUND = 301                        # (0x012D) NVM Manager, No OTPM configuration in NVM

    ASSET_MGR_ON_PROVISION_ASSET = 320                                  # (0x0140) Asset Manager Provision Asset command finished
    ASSET_MGR_GET_PUBLIC_KEY = 321                                      # (0x0141) Asset Manager Get Public Key
    ASSET_MGR_RETRIEVE_CERTIFICATE = 322                                # (0x0142) Asset Manager retrieve public certificate finished
    ASSET_MGR_RETRIEVE_PRIVATE_KEY = 323                                # (0x0143) Asset Manager retrieve private key finished
    ASSET_MGR_RETRIEVE_CC312_TRNG_CHAR = 324                            # (0x0144) Asset Manager retrieve CC312 TRNG characterization  finished
    ASSET_MGR_RETRIEVE_PSK_MASTER_SECRET = 325                          # (0x0145) Asset Manager retrieve private shared key  finished
    ASSET_MGR_RETRIEVE_OTPM_CONFIG = 326                                # (0x0146) Asset Manager retrieve OTPM configuration finished

    OTPM_DRV_WRITE = 352                                                # (0x0160) OTPM Driver current write location
    OTPM_DRV_WRITE_ERROR = 353                                          # (0x0161) OTPM Driver write attempted to none blank location
    OTPM_DRV_REGISTER_WRITE_ERROR = 354                                 # (0x0162) OTPM Driver register write failed due to readback error

    DEPU_APP_ON_PROGRAM_OTPM = 29696                                    # (0x7400) Depu app DEPU_PROGRAM_OTPM host command start
    DEPU_APP_CMPU_PROVISIONING = 29697                                  # (0x7401) Depu app CMPU provisioning
    DEPU_APP_DMPU_PROVISIONING = 29698                                  # (0x7402) Depu app DMPU provisioning
    DEPU_APP_ASSET_PROVISIONING = 29699                                 # (0x7403) Depu app asset provisioning
    DEPU_APP_CMPU_ERROR = 29700                                         # (0x7404) Depu app CMPU error
    DEPU_APP_DMPU_ERROR = 29701                                         # (0x7405) Depu app DMPU error
    DEPU_APP_ON_CC312_HARD_RESET = 29702                                # (0x7406) Depu app reseting CC312 hardware

    VALIDATION_APP_ON_DEBUG_GET_REGISTERS = 29952                       # (0x7500) Validation app DEBUG_GET_REGISTERS host command start


    MAX = 32768                                                         # (0x8000) Max checkpoint value

    def __str__(self):
        return '0x{:04X}:{}'.format(self.value, self.name)

