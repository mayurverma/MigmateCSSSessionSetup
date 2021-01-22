@echo off

echo ---------------------------------------
echo STEP 1 : Checking style violations
echo ---------------------------------------
call make -r BUILD_CONFIG=cpputest_mingw32 bare check-style
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Style check failed
    exit /B 1
)

echo.
echo ---------------------------------------
echo STEP 2 : Building documentation
echo ---------------------------------------
call make -r BUILD_CONFIG=cpputest_mingw32 docs_clean docs
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Doxygen build failed
    exit /B 1
)

echo.
echo ---------------------------------------
echo STEP 3 : Running unit tests
echo ---------------------------------------
call make -r BUILD_CONFIG=cpputest_mingw32 clean unittest
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Unit tests failed
    exit /B 1
)

echo.
echo ---------------------------------------
echo STEP 4 : Building system ROM
echo ---------------------------------------
call make -r -C src/Rom BUILD_CONFIG=barebones_armcc clean dist
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: System ROM build failed
    exit /B 1
)

echo.
echo ---------------------------------------
echo STEP 5 : Build Patch
echo ---------------------------------------
call make -r -C src/Patch BUILD_CONFIG=barebones_armcc clean dist
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Patch build failed
    exit /B 1
)

echo.
echo ---------------------------------------
echo STEP 6 : Building Applications
echo ---------------------------------------


call make -r -C src/VerificationApp BUILD_CONFIG=barebones_armcc clean dist
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: VerificationApp build failed
    exit /B 1
)

echo.
echo Pre-commit checks passed!
