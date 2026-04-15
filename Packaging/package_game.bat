@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ==========================================
echo Power Game - Packaging Script
echo ==========================================
echo.

REM Set paths
set PROJECT_ROOT=%~dp0..
set BUILD_DIR=%PROJECT_ROOT%\Build
set RELEASE_DIR=%PROJECT_ROOT%\Release
set CONTENT_DIR=%PROJECT_ROOT%\Content

REM Create directories
echo Creating release directories...
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"
if not exist "%RELEASE_DIR%\Content" mkdir "%RELEASE_DIR%\Content"
if not exist "%RELEASE_DIR%\Docs" mkdir "%RELEASE_DIR%\Docs"

REM Check for built executable
if not exist "%BUILD_DIR%\Release\PowerGame.exe" (
    echo Error: PowerGame.exe not found!
    echo Please build the project first using CMake.
    pause
    exit /b 1
)

echo Copying executable...
copy /Y "%BUILD_DIR%\Release\PowerGame.exe" "%RELEASE_DIR%\"

REM Copy DLLs
echo Copying dependencies...
if exist "%BUILD_DIR%\Release\*.dll" (
    copy /Y "%BUILD_DIR%\Release\*.dll" "%RELEASE_DIR%\"
)

REM Copy SDL2 DLLs if they exist
set SDL2_PATH=C:\vcpkg\installed\x64-windows\bin
if exist "%SDL2_PATH%\SDL2.dll" (
    copy /Y "%SDL2_PATH%\SDL2.dll" "%RELEASE_DIR%\"
)
if exist "%SDL2_PATH%\SDL2_image.dll" (
    copy /Y "%SDL2_PATH%\SDL2_image.dll" "%RELEASE_DIR%\"
)
if exist "%SDL2_PATH%\SDL2_ttf.dll" (
    copy /Y "%SDL2_PATH%\SDL2_ttf.dll" "%RELEASE_DIR%\"
)
if exist "%SDL2_PATH%\SDL2_mixer.dll" (
    copy /Y "%SDL2_PATH%\SDL2_mixer.dll" "%RELEASE_DIR%\"
)

REM Copy Content
echo Copying game content...
if exist "%CONTENT_DIR%" (
    xcopy /E /I /Y "%CONTENT_DIR%\*" "%RELEASE_DIR%\Content\"
)

REM Copy Documentation
echo Copying documentation...
copy /Y "%PROJECT_ROOT%\README.md" "%RELEASE_DIR%\"
copy /Y "%PROJECT_ROOT%\README_中文.md" "%RELEASE_DIR%\"
copy /Y "%PROJECT_ROOT%\LICENSE" "%RELEASE_DIR%\"

if exist "%PROJECT_ROOT%\Docs" (
    xcopy /E /I /Y "%PROJECT_ROOT%\Docs\*" "%RELEASE_DIR%\Docs\"
)

REM Create batch file for running game
echo Creating launcher...
(
echo @echo off
echo chcp 65001 ^>nul
echo cd /d "%%~dp0"
echo start "" "PowerGame.exe"
) > "%RELEASE_DIR%\Play Power Game.bat"

echo.
echo ==========================================
echo Packaging complete!
echo Release directory: %RELEASE_DIR%
echo.
echo Contents:
dir /B "%RELEASE_DIR%"
echo ==========================================
pause
