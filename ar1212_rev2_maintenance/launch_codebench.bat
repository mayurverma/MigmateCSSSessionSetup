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

REM Sets-up license path for Mentor Sourcery CodeBench, then launches it.
set "CODEBENCH_EXE=C:\mgc\embedded\codebench\bin\sourcerygxx-ide.exe"
set MGLS_LICENSE_FILE=1717@licg22;1717@licg32;1717@licg12

echo Adding MSYS_HOME environment variable ^(used by eclipse^)
set "MSYS_HOME=C:\msys64\usr"

set PATH_TO_SCRIPT=

start %CODEBENCH_EXE%

set CODEBENCH_EXE=

