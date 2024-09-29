@echo off

REM If Git is not found, exit with an error message
where git >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Git is not installed or not found in PATH.
    exit /b 1
)

REM If CMake is not found, exit with an error message
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo CMake is not installed or not found in PATH.
    exit /b 1
)

REM Check if "clean" argument is passed
if "%1"=="clean" (
    echo Cleaning build directory...
    if exist "build" (
        rmdir /S /Q "build"
    )
)

REM Checkout git submodules recursively
git submodule init
git submodule update --init --recursive

REM Create the "build" directory
if not exist build (
    mkdir build
)

REM Run CMake to generate Visual Studio 17 2022 solution files with x64 architecture
cmake -S . -B build -A x64 -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
