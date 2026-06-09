@echo off
setlocal

echo ========================================
echo   AmusementPark3D - MSVC Build Script
echo ========================================
echo.

:: Set up MSVC environment
set VCVARS="C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %VCVARS% (
    echo ERROR: Visual Studio Build Tools 2022 not found.
    echo Install from: https://visualstudio.microsoft.com/
    pause
    exit /b 1
)

echo [1/3] Setting up MSVC x64 environment...
call %VCVARS% x64 >nul 2>&1

:: Use portable CMake if available, else system cmake
set CMAKE_EXE=cmake
if exist "tools\cmake\cmake-3.30.2-windows-x86_64\bin\cmake.exe" (
    set CMAKE_EXE=tools\cmake\cmake-3.30.2-windows-x86_64\bin\cmake.exe
)

:: Clean old build
if exist build rmdir /s /q build

echo [2/3] Configuring with CMake (NMake Makefiles)...
%CMAKE_EXE% -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

echo [3/3] Building...
%CMAKE_EXE% --build build
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   BUILD SUCCESSFUL!
echo   Executable: build\bin\AmusementPark3D.exe
echo ========================================
echo.

:: Run the application
echo Starting AmusementPark3D...
cd build\bin
AmusementPark3D.exe

endlocal
