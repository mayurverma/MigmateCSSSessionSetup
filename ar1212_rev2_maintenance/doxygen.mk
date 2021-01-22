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
# Common makefile for Doxygen
#
####################################################################################

# Doxygen directories
DOXYGEN_TOOL_DIR	:= contrib/doxygen

# Doxygen output is for the whole workspace independent of build configuration
BUILD_DOCS_DIR		:= output/doc

# Doxygen tools
ifeq ($(BUILD_HOST),windows)
DOXYGEN				:= $(DOXYGEN_TOOL_DIR)/doxygen.exe
else
DOXYGEN				:= $(DOXYGEN_TOOL_DIR)/doxygen_linux
endif

# Doxygen configuration files
DOXYFILE_TEMPLATE	:= doc/Doxyfile.tmpl
DOXYFILE			:= $(BUILD_DOCS_DIR)/doxyfile

.PHONY: docs docs_clean doxyfile

# Generate the Doxygen configuration file by copying the template and then
# appending any makefile specific settings onto the end of the file
doxyfile:
	$(QUIET) echo Generating Doxygen configuration file...
	$(QUIET) $(MKDIR) $(BUILD_DOCS_DIR)
	$(QUIET) $(CP) $(DOXYFILE_TEMPLATE) $(DOXYFILE)
	$(QUIET) echo OUTPUT_DIRECTORY = $(BUILD_DOCS_DIR) >> $(DOXYFILE)

# Generate public documentation (excludes configuration files)
docs: doxyfile
	$(QUIET) echo Appending Doxygen configuration file...
	$(QUIET) echo Generating documentation...
	$(QUIET) $(DOXYGEN) $(DOXYFILE)

docs_clean:
	$(QUIET) echo Doxygen clean...
	-$(QUIET) $(RMDIR) $(BUILD_DOCS_DIR)

help::
	$(QUIET) echo "    --"
	$(QUIET) echo "    docs               Build Doxygen documentation"
	$(QUIET) echo "    docs_clean         Remove all Doxygen artefacts"

