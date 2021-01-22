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

call "%PATH_TO_SCRIPT%"\eclipse_config.bat

REM Default to Eclipse "Oxygen"
IF NOT DEFINED ECLIPSE_VERSION set "ECLIPSE_VERSION=DS-5 v5.27.1"
set "ECLIPSE_EXE=C:\"Program Files"\"%ECLIPSE_VERSION%"\bin\eclipse.exe"

REM For MSYS2 change where MSYS_HOME points to to make it compatible with DS5 eclipse.
REM DS5 prefixes its directoties to our configured paths which causes the wrong make executable
REM to be invoked. CDT further prefixes the PATH variable with ${MINGW_HOME}\bin;${MSYS_HOME}\bin
REM as MSYS2 bin directory in in fact in C:\msys64\usr (not at top level i.e. MSYS_HOME) setting
REM MSYS_HOME to C:\msys64\usr will ensure make from MSYS2 is called.
echo Adding MSYS_HOME environment variable ^(used by eclipse^)
set "MSYS_HOME=C:\msys64\usr"

set PATH_TO_SCRIPT=

start %ECLIPSE_EXE%

set ECLIPSE_EXE=

