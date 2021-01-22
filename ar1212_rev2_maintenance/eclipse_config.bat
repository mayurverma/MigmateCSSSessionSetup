@echo off
REM #######################################################################################
REM #   Copyright 2019 ON Semiconductor.  All rights reserved.
REM #
REM #   This software and/or documentation is licensed by ON Semiconductor under limited
REM #   terms and conditions.  The terms and conditions pertaining to the software and/or
REM #   documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
REM #   ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
REM #   Do not use this software and/or documentation unless you have carefully read and
REM #   you agree to the limited terms and conditions.  By using this software and/or
REM #   documentation, you agree to the limited terms and conditions.
REM ########################################################################################
set "PATH_TO_SCRIPT=%~dp0%"

call "%PATH_TO_SCRIPT%"\setenv.bat

REM Default to Eclipse "Klocwork executable"
IF NOT DEFINED KW_ECLIPSE_ENGINE set "KW_ECLIPSE_ENGINE=C:\Users\\${USERNAME}\.p2\pool\plugins\com.klocwork.eclipse.engine.win32_12.3.0.v0000810\bin"

set "PROJECT_DEFAULT_PATH=%PATH%"

set PATH_TO_SCRIPT=

