@echo off
echo ========================================
echo   PowerGame Build Script
echo ========================================
echo.

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake not found. Please install CMake 3.20+
    echo Download from: https://cmake.org/download/
    echo.
    pause
    exit /b 1
)

where cl >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo Found MSVC compiler
    set GENERATOR=
) else (
    where g++ >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        echo Found MinGW g++ compiler
        set GENERATOR=-G "MinGW Makefiles"
    ) else (
        echo ERROR: No C++ compiler found.
        echo Please install one of:
        echo   - Visual Studio 2019+ with C++ development tools
        echo   - MinGW-w64 with g++
        echo   - LLVM Clang
        echo.
        pause
        exit /b 1
    )
)

echo Configuring project...
cmake -B build %GENERATOR% -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build successful!
echo   Executable: build\bin\PowerGame.exe
echo ========================================
echo.
pause
