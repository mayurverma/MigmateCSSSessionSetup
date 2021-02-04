# ***********************************************************************************
#  Copyright 2018 ON Semiconductor.  All rights reserved.
#
#  This software and/or documentation is licensed by ON Semiconductor under limited
#  terms and conditions. The terms and conditions pertaining to the software and/or
#  documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
#  ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
#  Do not use this software and/or documentation unless you have carefully read and
#  you agree to the limited terms and conditions. By using this software and/or
#  documentation, you agree to the limited terms and conditions.
#
# *************************************************************************************
#
# System integration test makefile
#
# *************************************************************************************

# Set the system default goal
ifeq ($(.DEFAULT_GOAL),)
	.DEFAULT_GOAL := all
endif

MAKE_OPTIONS 	:= -r --no-print-directory -C../.. -f test/system/system_test.mk

%:
	@$(MAKE) -r $(MAKE_OPTIONS) $@

