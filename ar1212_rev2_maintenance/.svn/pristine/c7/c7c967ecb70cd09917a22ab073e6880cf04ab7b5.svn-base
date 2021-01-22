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
# Target-device makefile
#
####################################################################################

#
# Guards
#
ifeq ($(BUILD_TARGET_DEVICE), )
$(error BUILD_TARGET_DEVICE not specfied - either 'pc', 'asic', or 'proto')
endif

ifneq ($(BUILD_TARGET_DEVICE), proto)
ifeq ($(BUILD_TARGET_DEVICE_VERSION),)
$(error BUILD_TARGET_DEVICE_VERSION is not defined)
endif

else
ifeq ($(BUILD_TARGET_PROTO_DEVICE),)
$(error BUILD_TARGET_PROTO_DEVICE is not defined)
endif
ifeq ($(BUILD_TARGET_PROTO_DEVICE_VERSION),)
$(error BUILD_TARGET_PROTO_DEVICE_VERSION is not defined)
endif
endif # ifneq ($(BUILD_TARGET_DEVICE), proto)

#
# Export the target build configuration to C/C++
#
CFLAGS		+= -DBUILD_TARGET_DEVICE_$(strip $(BUILD_TARGET_DEVICE))
CPPFLAGS	+= -DBUILD_TARGET_DEVICE_$(strip $(BUILD_TARGET_DEVICE))

ifneq ($(BUILD_TARGET_DEVICE), proto)
# pc and asic target devices
CFLAGS		+= -DBUILD_TARGET_DEVICE_VERSION_$(strip $(BUILD_TARGET_DEVICE_VERSION))
CPPFLAGS	+= -DBUILD_TARGET_DEVICE_VERSION_$(strip $(BUILD_TARGET_DEVICE_VERSION))

else
# proto target devices
CFLAGS		+= -DBUILD_TARGET_PROTO_DEVICE_$(strip $(BUILD_TARGET_PROTO_DEVICE))
CPPFLAGS	+= -DBUILD_TARGET_PROTO_DEVICE_$(strip $(BUILD_TARGET_PROTO_DEVICE))

CFLAGS		+= -DBUILD_TARGET_PROTO_DEVICE_VERSION_$(strip $(BUILD_TARGET_PROTO_DEVICE_VERSION))
CPPFLAGS	+= -DBUILD_TARGET_PROTO_DEVICE_VERSION_$(strip $(BUILD_TARGET_PROTO_DEVICE_VERSION))
endif

#
# Configure the correct RTL headers include path
#
# TODO: <PH>: for now we use the same headers, at some point these should be separated into
#             asic and proto headers, with versions...
ifeq ($(BUILD_TARGET_DEVICE), asic)
CFLAGS		+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
ASFLAGS		+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
CPPFLAGS	+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
endif

ifeq ($(BUILD_TARGET_DEVICE), proto)
CFLAGS		+= -Icontrib/target/$(BUILD_TARGET_PROTO_DEVICE_VERSION)/inc
ASFLAGS		+= -Icontrib/target/$(BUILD_TARGET_PROTO_DEVICE_VERSION)/inc
CPPFLAGS	+= -Icontrib/target/$(BUILD_TARGET_PROTO_DEVICE_VERSION)/inc
endif

ifeq ($(BUILD_TARGET_DEVICE), pc)
CFLAGS		+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
ASFLAGS		+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
CPPFLAGS	+= -Icontrib/target/$(BUILD_TARGET_DEVICE_VERSION)/inc
endif

