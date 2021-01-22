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
# Makefile for CC312HalPal component
#
####################################################################################


CFLAGS += -DCC_IOT -D__ARM_DS5__
CFLAGS += -DMBEDTLS_CONFIG_FILE='<config-cc312-ar0820.h>'
CFLAGS += -DCC_CONFIG_TRNG_MODE=0

INC_PATHS	+= -Icontrib/sw-cc312/inc
INC_PATHS	+= -Icontrib/sw-cc312-sbrom/inc
INC_PATHS	+= -Isrc/CC312HalPal/inc

ASM_SRC +=

C_SRC			+= cc_hal.c cc_pal.c cc_pal_mutex.c cc_pal_pm.c
C_SRC			+= cc_pal_interrupt_ctrl.c cc_pal_mem.c cc_pal_buff_attr.c cc_pal_abort_plat.c cc_pal_trng.c
C_SRC			+= cc_pal_apbc.c
ifneq ($(BUILD_TYPE), release)
   CFLAGS += -DDEBUG
   C_SRC		+= cc_pal_log.c
endif
C_SRC			+= bsv_otp_api.c

VPATH += src/CC312HalPal/src

# Select the correct SVC exception handler
ifneq ($(findstring $(BUILD_CONFIG), barebones_armcc semihosted_armcc), )
VPATH += src/CC312HalPal/src/ARMCC
endif

