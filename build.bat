@echo off
REM Glint Build Script for Windows

setlocal enabledelayedexpansion

REM Configuration
set "CC=gcc"
REM Use -std=c23 for strict C23, or -std=gnu23 for GNU extensions
if "%GLINT_USE_GNU%"=="1" (
    set "STD_FLAG=-std=gnu23"
) else (
    set "STD_FLAG=-std=c23"
)
set "CFLAGS=%STD_FLAG% -Wall -Wextra -D_GNU_SOURCE -Ibase -Itests"
set "BUILD_DIR=build"
set "TEST_DIR=tests"

REM Colors (Windows 10+)
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

echo %BLUE%=== Glint Build System ===%NC%
echo %BLUE%Platform: Windows%NC%
echo %BLUE%Compiler: %CC%%NC%
echo.

REM Create build directories
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BUILD_DIR%\tests" mkdir "%BUILD_DIR%\tests"

REM Parse command
set "COMMAND=%1"
if "%COMMAND%"=="" set "COMMAND=build"

if /I "%COMMAND%"=="build" goto :build
if /I "%COMMAND%"=="test" goto :test
if /I "%COMMAND%"=="all" goto :all
if /I "%COMMAND%"=="clean" goto :clean
if /I "%COMMAND%"=="help" goto :help
if /I "%COMMAND%"=="-h" goto :help
if /I "%COMMAND%"=="--help" goto :help

echo %RED%Unknown command: %COMMAND%%NC%
echo.
goto :help

:build
echo.
echo %YELLOW%^>^>^> Building Tests%NC%
echo.

set "FAILED=0"

for /D %%T in ("%TEST_DIR%\g_test_*") do (
    set "TEST_NAME=%%~nxT"
    call :compile_test "!TEST_NAME!"
    if errorlevel 1 set /A FAILED+=1
)

echo.
if %FAILED%==0 (
    echo %GREEN%All tests compiled successfully!%NC%
    exit /b 0
) else (
    echo %RED%%FAILED% test(s) failed to compile%NC%
    exit /b 1
)

:test
echo.
echo %YELLOW%^>^>^> Running Tests%NC%
echo.

set "PASSED=0"
set "FAILED=0"
set "FILTER=%2"

for %%T in ("%BUILD_DIR%\tests\g_test_*.exe") do (
    set "TEST_NAME=%%~nT"
    call :run_test "%%T" "!FILTER!"
    if errorlevel 1 (
        set /A FAILED+=1
    ) else (
        set /A PASSED+=1
    )
)

echo.
echo %BLUE%=== Test Summary ===%NC%
echo %GREEN%Passed: %PASSED%%NC%
echo %RED%Failed: %FAILED%%NC%

if %FAILED%==0 (
    echo.
    echo %GREEN%All tests passed! ✓%NC%
    exit /b 0
) else (
    echo.
    echo %RED%Some tests failed ✗%NC%
    exit /b 1
)

:all
call :build
if errorlevel 1 exit /b 1
call :test %2
exit /b %errorlevel%

:clean
echo.
echo %YELLOW%^>^>^> Cleaning Build Directory%NC%
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo %GREEN%Build directory cleaned%NC%
) else (
    echo Build directory already clean
)
exit /b 0

:help
echo Usage: build.bat [command] [options]
echo.
echo Commands:
echo   build          Compile all tests (default)
echo   test           Run all tests
echo   all            Build and run all tests
echo   clean          Remove build directory
echo   help           Show this help message
echo.
echo Options:
echo   [filter]       When running tests, filter by name
echo.
echo Examples:
echo   build.bat build
echo   build.bat test
echo   build.bat test math      # Run only tests matching 'math'
echo   build.bat all
exit /b 0

REM Function to compile a test
:compile_test
set "TEST_NAME=%~1"
set "TEST_PATH=%TEST_DIR%\%TEST_NAME%"
set "OUTPUT=%BUILD_DIR%\tests\%TEST_NAME%.exe"

echo %BLUE%Compiling %TEST_NAME%...%NC%

if not exist "%TEST_PATH%\%TEST_NAME%.c" (
    echo %RED%✗ Test file not found: %TEST_PATH%\%TEST_NAME%.c%NC%
    exit /b 1
)

REM Compile (suppress warnings for cleaner output)
%CC% %CFLAGS% ^
    "%TEST_DIR%\g_test_main.c" ^
    "%TEST_PATH%\%TEST_NAME%.c" ^
    -o "%OUTPUT%" ^
    -lm >nul 2>&1

if exist "%OUTPUT%" (
    echo %GREEN%✓ %TEST_NAME% compiled successfully%NC%
    exit /b 0
) else (
    echo %RED%✗ %TEST_NAME% compilation failed%NC%
    REM Recompile with output to show errors
    %CC% %CFLAGS% ^
        "%TEST_DIR%\g_test_main.c" ^
        "%TEST_PATH%\%TEST_NAME%.c" ^
        -o "%OUTPUT%" ^
        -lm
    exit /b 1
)

REM Function to run a test
:run_test
set "TEST_BINARY=%~1"
set "FILTER=%~2"

if not exist "%TEST_BINARY%" (
    echo %RED%✗ Test binary not found: %TEST_BINARY%%NC%
    exit /b 1
)

set "TEST_NAME=%~n1"
echo.
echo %BLUE%Running %TEST_NAME%...%NC%

if "%FILTER%"=="" (
    "%TEST_BINARY%"
) else (
    "%TEST_BINARY%" "%FILTER%"
)

if errorlevel 1 (
    echo %RED%✗ %TEST_NAME% failed%NC%
    exit /b 1
) else (
    echo %GREEN%✓ %TEST_NAME% passed%NC%
    exit /b 0
)
