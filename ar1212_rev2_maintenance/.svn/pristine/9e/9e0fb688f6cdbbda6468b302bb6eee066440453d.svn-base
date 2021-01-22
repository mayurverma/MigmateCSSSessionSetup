####################################################################################
# Copyright 2019 ON Semiconductor.  All rights reserved.
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
#
# Makefile for the User Guide Examples built using OpenSSL
#
####################################################################################

BUILD_COMPONENT	:= UserGuideExamples
BUILD_COMPONENT_ROOTDIR := doc/$(BUILD_COMPONENT)

OPENSSL_ROOTDIR := /home/jtmzyj/Projects/AR0820/openssl-1.1.1a_install

INC_PATHS       += -Idoc/$(BUILD_COMPONENT)/inc
INC_PATHS		+= -I$(OPENSSL_ROOTDIR)/include

LIB_PATHS		+= -L$(OPENSSL_ROOTDIR)/lib

CFLAGS			+= $(INC_PATHS)

help::
	$(QUIET) echo Builds the OpenSSL examples for: $(BUILD_COMPONENT)

all:	rsa_encrypt rsa_verify aes_gcm_encrypt aes_ccm_encrypt rsa_keygen hkdf
#all:	hkdf

rsa_encrypt rsa_verify aes_gcm_encrypt aes_ccm_encrypt rsa_keygen hkdf:
	gcc $(CFLAGS) $(LIB_PATHS) -o doc/$(BUILD_COMPONENT)/output/$@ doc/$(BUILD_COMPONENT)/src/$@.c -lcrypto -std=c99

clean:
	rm -f doc/$(BUILD_COMPONENT)/output/*

test_rsa_encrypt:
	doc/$(BUILD_COMPONENT)/output/rsa_encrypt
	python doc/$(BUILD_COMPONENT)/test/rsa_encrypt_test.py

test_rsa_verify:
	python doc/$(BUILD_COMPONENT)/test/rsa_verify_test.py
	doc/$(BUILD_COMPONENT)/output/rsa_verify

test_aes_gcm_encrypt:
	python doc/$(BUILD_COMPONENT)/test/aes_gcm_test.py
	doc/$(BUILD_COMPONENT)/output/aes_gcm_encrypt

test_aes_ccm_encrypt:
	python doc/$(BUILD_COMPONENT)/test/aes_ccm_test.py
	doc/$(BUILD_COMPONENT)/output/aes_ccm_encrypt

test_rsa_keygen:
	rm -f doc/$(BUILD_COMPONENT)/output/*.pem
	doc/$(BUILD_COMPONENT)/output/rsa_keygen

test_hkdf:
	python doc/$(BUILD_COMPONENT)/test/hkdf_test.py
	doc/$(BUILD_COMPONENT)/output/hkdf

