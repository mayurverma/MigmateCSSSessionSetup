@echo off
REM #######################################################################################
REM #	Copyright 2018 ON Semiconductor.  All rights reserved.
REM #
REM #	This software and/or documentation is licensed by ON Semiconductor under limited
REM #	terms and conditions.  The terms and conditions pertaining to the software and/or
REM #	documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
REM #	("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
REM #	Do not use this software and/or documentation unless you have carefully read and
REM #	you agree to the limited terms and conditions.  By using this software and/or
REM #	documentation, you agree to the limited terms and conditions.
REM ########################################################################################
REM # This script sets the CSS development environment for all developers. It starts with
REM # the Windows 10 default path and then prefixes/appends path elements as appropriate.
REM #
REM # NOTE: Current PATH is ignored to ensure consistent content and search order.
REM ########################################################################################

set "PATH_TO_SCRIPT=%~dp0%"

REM Start with Windows 10 default path and prefix/append elements as appropriate
set "PATH=%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem;%SystemRoot%\System32\WindowsPowerShell\v1.0\"

REM Setup environment for Python
if exist "C:\Python36\python.exe" (
    echo Adding Python36 to your PATH environment variable
    set "PATH=%PATH%;C:\Python36"
) else (
    echo ERROR: Python36 not found
    REM exit /B 1
)

REM Setup environment for MSYS2 shell
if exist "C:\MinGW\msys\1.0\bin\sh.exe" (
    echo Adding MSYS2 to your PATH environment variable
    set "PATH=%PATH%;C:\MinGW\msys\1.0\bin"
) else (
    echo ERROR: MSYS2 not found
    REM exit /B 1
)

REM Setup environment for MinGW32 gcc compiler
if exist "C:\MinGW\bin\gcc.exe" (
    echo Adding MinGW32 to your PATH environment variable
    set "PATH=%PATH%;C:\MinGW\bin"
) else (
    echo ERROR: MinGW32 compiler not found
    REM exit /B 1
)

REM Setup environment for ARM DS-5 compiler (optional)
set ARMLMD_LICENSE_FILE=28100@licpx2;28100@licg11
set "ARM_TOOLS=C:\Program Files\DS-5 v5.27.1\sw\ARMCompiler5.06u5"
if exist "C:\Program Files\DS-5 v5.27.1\bin\eclipse.exe" (
    echo Adding ARM DS-5 to your PATH environment variable
    set "PATH=%PATH%;%ARM_TOOLS%\bin;C:\Program Files\DS-5 v5.27.1\bin"
)

REM Setup environment for ARM-GNU compiler (optional)
set MGLS_LICENSE_FILE=1717@licg22;1717@licg32;1717@licg12
set "ARM_GNU_TOOLS=C:\mgc\embedded\toolchains\arm-none-eabi.2016.11"
if exist "%ARM_GNU_TOOLS%\bin\arm-none-eabi-gcc.exe" (
    echo Adding ARM_GNU_TOOLS to your PATH environment variable
    set "PATH=%PATH%;%ARM_GNU_TOOLS%\bin"
)

REM Setup environment for PuTTY
if exist "C:\Program Files\PuTTY\putty.exe" (
    echo Adding PuTTY to your PATH environment variable
    set "PATH=%PATH%;C:\Program Files\PuTTY"
) else (
    echo ERROR: PuTTY not found
    REM exit /B 1
)

REM Setup environment for 7-Zip
if exist "C:\Program Files\7-Zip\7z.exe" (
    echo Adding 7-Zip to your PATH environment variable
    set "PATH=%PATH%;C:\Program Files\7-Zip"
) else (
    echo ERROR: 7-Zip not found
    REM exit /B 1
)

REM Setup environment for Doxygen
if exist "%PATH_TO_SCRIPT%contrib\doxygen\doxygen.exe" (
    echo Adding Doxygen to your PATH environment variable
    set "PATH=%PATH%;%PATH_TO_SCRIPT%contrib\doxygen"
) else (
    echo ERROR: Doxygen not found
    REM exit /B 1
)

set PATH_TO_SCRIPT=

@echo on
