@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x86_x64 >nul 2>&1
echo MSVC_CL=%VCToolsInstallDir%
where cl
where nmake
tools\cmake\cmake-3.30.2-windows-x86_64\bin\cmake.exe -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 exit /b 1
tools\cmake\cmake-3.30.2-windows-x86_64\bin\cmake.exe --build build
