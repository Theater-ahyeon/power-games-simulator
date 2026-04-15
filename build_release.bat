@echo off
chcp 65001 >nul
echo ==========================================
echo Power Game - Release Build Script
echo ==========================================
echo.

REM Check for Visual Studio
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Visual Studio compiler not found!
    echo Please run this script from a Visual Studio Developer Command Prompt.
    pause
    exit /b 1
)

REM Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: CMake not found!
    echo Please install CMake and add it to PATH.
    pause
    exit /b 1
)

set PROJECT_ROOT=%~dp0
set BUILD_DIR=%PROJECT_ROOT%Build
set RELEASE_DIR=%PROJECT_ROOT%Release

echo Creating build directory...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

echo.
echo Configuring CMake for Release build...
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="%RELEASE_DIR%" "%PROJECT_ROOT%"

if %ERRORLEVEL% neq 0 (
    echo.
    echo Error: CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building project...
cmake --build . --config Release --parallel

if %ERRORLEVEL% neq 0 (
    echo.
    echo Error: Build failed!
    pause
    exit /b 1
)

echo.
echo Installing to Release directory...
cmake --install . --config Release

echo.
echo ==========================================
echo Release build completed successfully!
echo Executable: %RELEASE_DIR%\bin\PowerGame.exe
echo ==========================================
pause
