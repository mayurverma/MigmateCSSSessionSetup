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
#
# Toolchain-selector makefile
#
####################################################################################

# Define the distutils that are common to both toolchains
ifeq ($(BUILD_HOST),windows)
	AR			:= ar.exe
	ASTYLE		:= contrib/astyle/AStyle.exe
	CAT			:= cat.exe
	CP			:= cp.exe
	DOS2UNIX	:= dos2unix.exe
	GZIP		:= gzip.exe
	GUNZIP		:= gzip.exe --decompress
	MD5SUM		:= md5sum.exe
	MKDIR		:= mkdir.exe -p
	PYTHON		:= python.exe
	RM			:= rm.exe -f
	RMDIR		:= rm.exe -rf
	SHELL		:= sh.exe
	TAR			:= tar.exe
	UNCRUSTIFY	:= contrib/uncrustify/uncrustify.exe
	UNZIP		:= 7z.exe x -bso0 -aoa
	ZIP			:= 7z.exe a -tzip -bso0
else
	AR			:= ar
	ASTYLE		:= /proj/AR1212/contrib/astyle/bin/astyle
	CAT			:= cat
	CP			:= cp
	DOS2UNIX	:= dos2unix
	GZIP		:= gzip
	GUNZIP		:= gunzip
	MD5SUM		:= md5sum
	MKDIR		:= mkdir -p
	PYTHON		?= python3
	RM			:= rm -f
	RMDIR		:= rm -rf
	SHELL		:= /bin/sh
	TAR			:= tar
	UNCRUSTIFY	:= /proj/AR1212/contrib/uncrustify/bin/uncrustify
	UNZIP		:= unzip -oq
	ZIP			:= zip -9qy@
endif

ifeq ($(BUILD_TOOLCHAIN), )
$(error BUILD_TOOLCHAIN not specfied - either mingw32, arm-none-eabi-gcc or armcc)
endif

ifeq ($(BUILD_TOOLCHAIN),mingw32)
include mingw32.mk

else ifeq ($(BUILD_TOOLCHAIN),arm-none-eabi-gcc)
include arm-none-eabi-gcc.mk

else ifeq ($(BUILD_TOOLCHAIN),armcc)
include armcc.mk

else
$(error BUILD_TOOLCHAIN $(BUILD_TOOLCHAIN) is not supported)
endif

.PHONY: help lib exe create_output_dir

create_output_dir:
	$(QUIET) $(MKDIR) $(BUILD_OUTPUT_DIR)

lib: create_output_dir $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)

exe: create_output_dir $(BUILD_OUTPUT_DIR)/$(BUILD_TARGET)

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    exe                Build executable images"
	$(QUIET) echo "    lib                Build libraries"

