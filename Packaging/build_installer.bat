@echo off
chcp 65001 >nul
echo ==========================================
echo Power Game - Windows Installer Builder
echo ==========================================
echo.

REM Check for Inno Setup
set INNO_SETUP="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if not exist %INNO_SETUP% (
    set INNO_SETUP="C:\Program Files\Inno Setup 6\ISCC.exe"
)
if not exist %INNO_SETUP% (
    echo Error: Inno Setup 6 not found!
    echo Please download and install Inno Setup 6 from:
    echo https://jrsoftware.org/isdl.php
    pause
    exit /b 1
)

echo Found Inno Setup at: %INNO_SETUP%
echo.

REM Create release directory
if not exist "..\Release" mkdir "..\Release"

REM Build the installer
echo Building installer...
%INNO_SETUP% "PowerGame.iss"

if %ERRORLEVEL% neq 0 (
    echo.
    echo Error: Failed to build installer!
    pause
    exit /b 1
)

echo.
echo ==========================================
echo Installer built successfully!
echo Output: ..\Release\PowerGame-Setup-1.0.0.exe
echo ==========================================
pause
