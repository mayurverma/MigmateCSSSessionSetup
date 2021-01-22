These files represent the reference C code to accompany the AR0825 User Guide.
The code is purely for reference and performs no error checking or corner-case
checks.

The code includes a CppUtest wrapper. The I2cWriteReg16 and I2cReadReg16 functions
are mocked to allow the host command functions to be exercised. Note these unit
tests must be run under Windows

To build (from root of checkout):
\> make -r BUILD_CONFIG=cpputest_mingw32 -C doc\UserGuideExamples\test clean all

To run (from root of checkout):
\> make -r BUILD_CONFIG=cpputest_mingw32 -C doc\UserGuideExamples\test test

----

The code also includes examples for RSA, HKDF and AES-CCM/GCM using OpenSSL. The examples
are built with OpenSSL 1.1.1a which is installed in /home/jtmzyj/Projects/AR0820/openssl-1.1.1a_install

To build the OpenSSL examples on Linux (from root of checkout):
% make -f doc/UserGuideExamples/test/openssl_examples.mk clean all

Each OpenSSL example has a 'test' script written in Python (uses the CryptoHelper
class in <root>/scripts). The tests are not self-checking and require the operator
to confirm pass/fail.

You need to setup your Bash environment (where <checkout_root> is the root of your AR0820 svn checkout):
% export LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):/home/jtmzyj/Projects/AR0820/openssl-1.1.1a_install/lib
% export PYTHONPATH=$(PYTHONPATH):<checkout_root>/contrib/pycryptodome/linux/Crypto:<checkout_root>scripts

To run (from root of checkout):
% make -f doc/UserGuideExamples/test/openssl_examples.mk test_hkdf
% make -f doc/UserGuideExamples/test/openssl_examples.mk test_aes_ccm_encrypt
% make -f doc/UserGuideExamples/test/openssl_examples.mk test_aes_gcm_encrypt
% make -f doc/UserGuideExamples/test/openssl_examples.mk test_rsa_encrypt
% make -f doc/UserGuideExamples/test/openssl_examples.mk test_rsa_verify
