#######################################################################################
# Copyright 2018 ON Semiconductor.  All rights reserved.
#
# This software and/or documentation is licensed by ON Semiconductor under limited
# terms and conditions.  The terms and conditions pertaining to the software and/or
# documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
# ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Softwar").
# Do not use this software and/or documentation unless you have carefully read and
# you agree to the limited terms and conditions.  By using this software and/or
# documentation, you agree to the limited terms and conditions.
########################################################################################
#
# Library to generate a Session for testing.
#
########################################################################################
import logging
import struct

from enum import Enum
from common_error import Error

from rsa_public_certificate import RSAPublicCertificate, CertificatePurpose, CertificateAuthority
from crypto_helper import CryptoHelper
from common_error import Error
from lifecycle_state import LifecycleState
from command_helper import CommandHelper
from host_command_code import HostCommandCode
from host_command_set_session_keys import HostCommandSetSessionKeysParams, HostCommandSetSessionKeysResponse
from host_command_get_status import HostCommandGetStatus
from host_command_get_sensor_id import HostCommandGetSensorId
from host_command_get_certificates import HostCommandGetCertificatesParams, HostCommandGetCertificatesResponse
from host_command_set_psk_session_keys import HostCommandSetPskSessionKeys
from host_command_set_ephemeral_master_secret import HostCommandSetEthemeralMasterSecretParams, HostCommandSetEthemeralMasterSecretResponse
from host_session_types import SymmetricAuthCipherMode, SymmetricAECipherMode, AsymmetricCipherMode, SymmetricKeyType, SessionMode

class SessionContext():

    def __init__(self,
                       #session_mode,
                       #rom_version,
                       #patch_version,
                       #soc_id,
                       master_key_type = SymmetricKeyType.AES_128,
                       video_auth_cipher_mode = SymmetricAuthCipherMode.AES_CMAC,
                       video_auth_key_type = SymmetricKeyType.AES_128,
                       control_channel_cipher_mode = SymmetricAECipherMode.AES_CCM,
                       control_channel_key_type = SymmetricKeyType.AES_128,
                       session_params_cipher = AsymmetricCipherMode.RSAES_OAEP):
        self.__master_key_type = master_key_type
        self.__video_auth_cipher_mode = video_auth_cipher_mode
        self.__video_auth_key_type = video_auth_key_type
        self.__control_channel_cipher_mode = control_channel_cipher_mode
        self.__control_channel_key_type = control_channel_key_type
        self.__session_params_cipher = session_params_cipher

    def __str__(self):
        msg = ''
        msg += "master_key_type             : {}\n".format(self.__master_key_type)
        msg += "video_auth_cipher_mode      : {}\n".format(self.__video_auth_cipher_mode)
        msg += "video_auth_key_type         : {}\n".format(self.__video_auth_key_type)
        msg += "control_channel_cipher_mode : {}\n".format(self.__control_channel_cipher_mode)
        msg += "control_channel_key_type    : {}\n".format(self.__control_channel_key_type)
        msg += "session_params_cipher       : {}\n".format(self.__session_params_cipher)
        return msg

    @property
    def master_key_type(self):
        return self.__master_key_type;

    @property
    def video_auth_cipher_mode(self):
        return self.__video_auth_cipher_mode;

    @property
    def video_auth_key_type(self):
        return self.__video_auth_key_type;

    @property
    def control_channel_cipher_mode(self):
        return self.__control_channel_cipher_mode;

    @property
    def control_channel_key_type(self):
        return self.__control_channel_key_type;

    @property
    def session_params_cipher(self):
        return self.__session_params_cipher;


class HostSession():

    SALT_LENGTH = 16

    def __init__(self, context, camera, verbosity=logging.INFO):
        self.__context = context
        self.__camera = camera
        self.__command_helper = CommandHelper(camera)
        self.__logger = logging.getLogger(self.__class__.__name__)
        self.__crypto = CryptoHelper()

        self.__mode = None
        self.__video_key = None
        self.__control_key = None
        self.__nonce = None
        self.__video_auth_gmac_iv = None
        self.__verbosity = verbosity

    def __str__(self):
        msg = ''
        msg += self.__class__.__name__ + "\n"
        msg += str(self.__context)
        if self.__mode:
            msg += "mode                : {}\n".format(self.__mode)
        if self.__video_key:
            msg += "video   key         : {}\n".format(self.__video_key.hex())
        if self.__control_key:
            msg += "control key         : {}\n".format(self.__control_key.hex())
        if self.__nonce:
            msg += "nonce               : {}\n".format(self.__nonce.hex())
        if self.__video_auth_gmac_iv:
            msg += "video_auth_gmac_iv  : {}\n".format(self.__video_auth_gmac_iv.hex())
        return msg

    def connect(self, mode = SessionMode.EPHEMERAL_MODE, ca_rsa_key = None, master_secret = None, salt = None, auth_id = CertificateAuthority.VENDOR_A):

        import time

        lcs, rom_version, patch_version = self._get_status()
        master_key_size = self._get_key_size(self.__context.master_key_type)
        if salt == None:
            salt = self.__crypto.generate_random_aes_key(self.SALT_LENGTH)

        if len(salt) != self.SALT_LENGTH:
            raise Exception('Invalid Salt size {}'.format(salt))

        soc_id = self._get_soc_id()

        if mode == SessionMode.PSK_MODE :
            if master_secret == None:
                raise Exception("A master secret must be given for a PSK mode (this has to be also provisioned)")

            if len(master_secret) != master_key_size:
                raise Exception("Invalid Master Secret size {}".format(len(master_secret)))

            time_start = time.perf_counter()
            sensor_salt, self.__video_auth_gmac_iv, self.__nonce = self._set_psk_session(salt)
            time_end = time.perf_counter()

        elif mode == SessionMode.EPHEMERAL_MODE:
            if master_secret == None:
                master_secret = self.__crypto.generate_random_aes_key(master_key_size)

            # if ca_rsa_key is None, it is assumed the vendor certificate is autosigned
            sensor_certificate, vendor_certificate = self._check_and_get_certificates(auth_id, ca_rsa_key)

            public_key = sensor_certificate.rsa_public_key

            time_start = time.perf_counter()
            sensor_salt, self.__video_auth_gmac_iv, self.__nonce = self._set_rsa_session(public_key, master_secret, salt)
            time_end = time.perf_counter()

        elif mode == SessionMode.UNSECURE_EPHEMERAL_MODE :
            if lcs == LifecycleState.SECURE:
                raise Exception("SetEphemeralMasterSecret not supported in Secure")

            if master_secret == None:
                master_secret = self.__crypto.generate_random_aes_key(master_key_size)

            # we are trying a epheral master secret in DM/CM. No OTPM master secret should be present or it would fail
            self._set_ephemeral_master_secret(master_secret)

            if len(master_secret) != master_key_size:
                raise Exception("Invalid Master Secret size {}".format(len(master_secret)))

            time_start = time.perf_counter()
            sensor_salt, self.__video_auth_gmac_iv, self.__nonce = self._set_psk_session(salt)
            time_end = time.perf_counter()

        else:
            raise Exception("Mode {} not recognized".format(mode))

        video_key_size_bytes = self._get_key_size(self.__context.video_auth_key_type)
        control_key_size_bytes = self._get_key_size(self.__context.control_channel_key_type)
        self.__video_key, self.__control_key = self._derive_session_keys(master_secret, salt, sensor_salt, rom_version, patch_version, soc_id, video_key_size_bytes, control_key_size_bytes)
        self.__mode = mode

        # return the host command execution time
        return time_end - time_start

    def encrypt_and_execute_command(self,
                                            command_code,
                                            params,
                                            response,
                                            expected = Error.SUCCESS, timeout_usec=500000, interval_usec=100, verbosity=logging.INFO):

        associated_data = command_code.value.to_bytes(2, 'little')
        params_unencrypted_bytes = params.encryptedParams

        if self.__context.control_channel_cipher_mode == SymmetricAECipherMode.AES_CCM:
            payload, mac = CryptoHelper().ccm_encrypt(self.__control_key,
                                                      self.__nonce,
                                                      params_unencrypted_bytes,
                                                      associated_data)
        elif self.__context.control_channel_cipher_mode == SymmetricAECipherMode.AES_GCM:
            payload, mac = CryptoHelper().gcm_encrypt(self.__control_key,
                                                      self.__nonce,
                                                      params_unencrypted_bytes,
                                                      associated_data)
        else:
            raise Exception("Incorrect Control Channel Mode {}".format(self.__context.control_channel_cipher_mode))

        params.encryptedParams = payload
        params.mac = mac

        self.__logger.info('Sending command: {}'.format(command_code.name))
        result = self.__command_helper.execute(command_code, params, response, timeout_usec = timeout_usec, verbosity = verbosity)
        self.__logger.info('Received response: {}'.format(result.name))
        if expected != result:
            raise Exception('Operation failed with unexpected error {} (expected: {})'.format(result.name, expected.name))

        if Error.SUCCESS == result:
            # note the increase nonce should happen if an error occurs when the contents are invalid but there is no way
            # of detecting such condition because the might return the same error code
            self._increase_nonce()
            self.__logger.info('Nonce: {}'.format(self.__nonce.hex()))

        return response


    def _derive_session_keys(self, master_secret, host_salt, sensor_salt, rom_version, patch_version, soc_id, video_key_size_bytes, control_key_size_bytes):
        salt = host_salt + sensor_salt
        context = 'AR0820R2'.encode()
        context += rom_version.to_bytes(2, byteorder='little')
        context += patch_version.to_bytes(2, byteorder='little')
        context += soc_id
        key_len = video_key_size_bytes + control_key_size_bytes
        keys = CryptoHelper().hkdf(master_secret, key_len, salt, context)
        video_key = keys[:video_key_size_bytes]
        control_key = keys[video_key_size_bytes:]
        return video_key, control_key

    def _build_session_master_secret(self, key):
        """ Build the message before encryption for the set session key command """
        pad = bytes([0,0])
        if len(key) == 16:
            return bytes([SymmetricKeyType.AES_128.value,0]) + pad + key
        elif len(key) == 24:
            return bytes([SymmetricKeyType.AES_196.value,0]) + pad + key
        elif len(key) == 32:
            return bytes([SymmetricKeyType.AES_256.value,0]) + pad + key
        raise Exception("Incorrect Key Size {}".format(len(key)))

    def _get_key_size(self, key_type):
        if key_type == SymmetricKeyType.AES_128:
            return 16
        if key_type == SymmetricKeyType.AES_192:
            return 24
        if key_type == SymmetricKeyType.AES_256:
            return 32
        raise Exception("Incorrect Key type {}".format(key_type))

    def _get_type_from_size(self, size_bytes):
        if size_bytes == 16:
            return SymmetricKeyType.AES_128
        if size_bytes == 24:
            return SymmetricKeyType.AES_192
        if size_bytes == 32:
            return SymmetricKeyType.AES_256
        raise Exception("Incorrect Key size {}".format(size_bytes))

    def _increase_nonce(self):
        """ Nonce must be incremented from the least-significant byte """
        nonce_list = list(self.__nonce)
        for index in range(len(self.__nonce)-1, -1, -1):
            nonce_list[index] += 1
            nonce_list[index] &= 0xFF
            if nonce_list[index] != 0:
                break
        self.__nonce = bytes(nonce_list)

    def _get_status(self):
        """Issue the GetStatus command and return current lifecycle"""

        response = HostCommandGetStatus(self.__camera).execute()
        return response.current_lifecycle_state, response.rom_version, response.patch_version

    def _get_soc_id(self):
        """ Issue GetSensorId command """

        response = HostCommandGetSensorId(self.__camera).execute()
        return bytes(response.unique_id)

    def _get_certificates(self, auth_id = CertificateAuthority.VENDOR_A):
        # Issue GetCertificates command
        params = HostCommandGetCertificatesParams()
        params.certificate_auth_id = auth_id.value;
        response = HostCommandGetCertificatesResponse()
        result = self._execute_command(HostCommandCode.GET_CERTIFICATES, params, response)

        if response.vendor_certificate_size_words:
            vendor_certificate = response.vendor_certificate
        else:
            vendor_certificate = None

        return response.sensor_certificate, vendor_certificate

    def _check_and_get_certificates(self, auth_id, ca_rsa_key):
        sensor_certificate_bytes, vendor_certificate_bytes = self._get_certificates(auth_id)
        vendor_certificate = None
        sensor_certificate = None

        if vendor_certificate_bytes:
            vendor_certificate = RSAPublicCertificate.get_and_validate_certificate_from_bytes(ca_rsa_key, vendor_certificate_bytes)
            self.__logger.log(self.__verbosity, str(vendor_certificate))
            sensor_ca_rsa_key = vendor_certificate.rsa_public_key
        else:
            sensor_ca_rsa_key = ca_rsa_key
        sensor_certificate = RSAPublicCertificate.get_and_validate_certificate_from_bytes(sensor_ca_rsa_key, sensor_certificate_bytes)
        self.__logger.log(self.__verbosity, str(sensor_certificate))

        return sensor_certificate, vendor_certificate


    def _set_rsa_session(self, public_key, master_secret, salt):
        """Run SetSessionKeys command"""

        params = HostCommandSetSessionKeysParams()
        params.videoAuthCipherMode = self.__context.video_auth_cipher_mode.value
        params.videoAuthKeyType = self.__context.video_auth_key_type.value
        params.controlChannelCipherMode = self.__context.control_channel_cipher_mode.value
        params.controlChannelKeyType = self.__context.control_channel_key_type.value
        params.sessionParamsCipher = self.__context.session_params_cipher.value

        params.hostSalt = salt

        master_key_data = self._build_session_master_secret(master_secret)
        params.encryptedParamsSizeWords = len(master_key_data)//2
        params.encryptedParams = self.__crypto.rsa_oaep_encrypt(public_key, master_key_data)
        response = HostCommandSetSessionKeysResponse()

        result = self._execute_command(HostCommandCode.SET_SESSION_KEYS, params, response)

        return bytes(response.sensorSalt), bytes(response.videoAuthGMACIV), bytes(response.controlChannelNonce)

    def _set_psk_session(self, salt):
        # Issue SetPskSessionKeys command

        response = HostCommandSetPskSessionKeys(self.__camera).execute(
            videoAuthCipherMode = self.__context.video_auth_cipher_mode,
            videoAuthKeyType = self.__context.video_auth_key_type,
            controlChannelCipherMode = self.__context.control_channel_cipher_mode,
            controlChannelKeyType = self.__context.control_channel_key_type, hostSalt = salt)

        return bytes(response.sensorSalt), bytes(response.videoAuthGMACIV), bytes(response.controlChannelNonce)

    def _set_ephemeral_master_secret(self, key):
        # Issue SetPskSessionKeys command
        params = HostCommandSetEthemeralMasterSecretParams()
        key_type = self._get_type_from_size(len(key))
        params.masterSecretKeyType = key_type.value
        params.masterSecret = key
        response = HostCommandSetEthemeralMasterSecretResponse()

        result = self._execute_command(HostCommandCode.SET_EPHEMERAL_MASTER_SECRET, params, response)

    def _execute_command(self, command_code, params, response, timeout_usec=50000000):
        # executre a command and return the result
        self.__logger.log(self.__verbosity, 'Executing {} command...'.format(str(command_code)))
        result = self.__command_helper.execute(command_code, params, response, timeout_usec = timeout_usec, verbosity = self.__verbosity)
        if Error.SUCCESS != result:
            self.__logger.log(self.__verbosity, 'Operation failed with error {}'.format(result.name))
            raise Exception('Operation failed with error {}'.format(result.name))
        return result


if __name__ == "__main__":
    print("Just testing...")

