@echo off
setlocal enabledelayedexpansion

echo ============================================
echo  AmusementPark3D - Setup Script
echo ============================================
echo.

:: Check for required tools
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake not found. Please install CMake 3.20+ and add to PATH.
    pause
    exit /b 1
)

where git >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: Git not found. Please install Git and add to PATH.
    pause
    exit /b 1
)

:: Download stb_image.h
echo [1/3] Downloading stb_image.h...
if not exist "vendor\stb" mkdir "vendor\stb"
powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/nothings/stb/master/stb_image.h' -OutFile 'vendor\stb\stb_image.h'"
if %ERRORLEVEL% neq 0 (
    echo WARNING: Could not download stb_image.h. Using bundled version.
)

:: Download GLAD (OpenGL 4.6 Core Profile)
echo [2/3] Setting up GLAD...
if not exist "vendor\glad\include\glad" mkdir "vendor\glad\include\glad"
if not exist "vendor\glad\include\KHR" mkdir "vendor\glad\include\KHR"
if not exist "vendor\glad\src" mkdir "vendor\glad\src"

:: Download GLAD files from glad.dav1d.de generated files
:: Using pre-generated GLAD for OpenGL 4.6 Core
powershell -Command "Invoke-WebRequest -Uri 'https://glad.dav1d.de/generated/tmpZqVPF_/glad.zip' -OutFile 'vendor\glad\glad.zip'" 2>nul
if %ERRORLEVEL% neq 0 (
    echo NOTE: Could not download GLAD from web. Using bundled GLAD files.
    echo Please ensure vendor\glad\src\glad.c and vendor\glad\include\glad\glad.h exist.
)

:: Create asset directories
echo [3/3] Creating asset directories...
if not exist "assets\textures" mkdir "assets\textures"
if not exist "assets\models" mkdir "assets\models"
if not exist "assets\audio" mkdir "assets\audio"
if not exist "assets\shaders" mkdir "assets\shaders"

echo.
echo Setup complete!
echo.
echo To build the project:
echo   1. Run build.bat
echo   OR manually:
echo   cmake -B build -G "Visual Studio 17 2022" -A x64
echo   cmake --build build --config Release
echo.
pause
