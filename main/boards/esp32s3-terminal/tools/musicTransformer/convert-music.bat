@echo off
chcp 65001 >nul
title Audio to OGG Converter

echo.
echo ============================================================
echo         Audio to OGG Format Converter
echo ============================================================
echo.

REM Check FFmpeg
if not exist "ffmpeg\bin\ffmpeg.exe" (
    echo [INFO] FFmpeg not found, downloading...
    echo.
    call "download-ffmpeg.bat"
    if errorlevel 1 (
        echo.
        echo FFmpeg download failed
        pause
        exit /b
    )
)

REM Check music directory
if not exist "music" (
    echo [INFO] Creating 'music' directory...
    mkdir music
    echo.
    echo Please put audio files in 'music' folder and run again
    echo.
    pause
    exit /b
)

REM Check Python
python --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python not found
    echo.
    echo Download: https://www.python.org/downloads/
    echo.
    pause
    exit /b
)

REM Run conversion
echo Starting conversion...
echo.
python convert_to_ogg.py

if errorlevel 1 (
    echo.
    echo [ERROR] Conversion failed
    echo.
)

pause
