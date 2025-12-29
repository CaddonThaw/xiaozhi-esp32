@echo off
echo ====================================
echo  ESP32-S3 Terminal Release Flash Tool
echo ====================================
echo.
echo This tool is for flashing CI release firmware
echo File: xiaozhi_esp32s3-terminal_vX.X.X.bin
echo.

echo Scanning available serial ports...
echo.

setlocal enabledelayedexpansion
set count=0

for /f "tokens=*" %%a in ('wmic path Win32_PnPEntity where "Caption like '%%(COM%%'" get Caption 2^>nul ^| findstr "COM"') do (
    set /a count+=1
    set "port[!count!]=%%a"
)

if %count%==0 (
    echo No serial port found!
    echo.
    echo Please check:
    echo   1. Is ESP32-S3 connected to computer
    echo   2. Is USB driver installed correctly
    echo   3. Is device being used by other program
    echo.
    pause
    exit /b 1
)

echo Available serial ports:
echo.
for /l %%i in (1,1,%count%) do (
    echo [%%i] !port[%%i]!
)
echo.

set /p CHOICE="Please select port number: "

if not defined CHOICE (
    echo ERROR: No selection!
    pause
    exit /b 1
)

if %CHOICE% lss 1 (
    echo ERROR: Invalid selection!
    pause
    exit /b 1
)

if %CHOICE% gtr %count% (
    echo ERROR: Invalid selection!
    pause
    exit /b 1
)

set "SELECTED=!port[%CHOICE%]!"

REM Extract COM port number from selection
for /f "tokens=2 delims=()" %%a in ("!SELECTED!") do set "PORT=%%a"

echo.
echo Selected: !port[%CHOICE%]!
echo Port: !PORT!

if "!PORT!"=="" (
    echo ERROR: Cannot identify port number!
    pause
    exit /b 1
)

REM Find the release firmware file
set "FIRMWARE="
for %%f in (xiaozhi_esp32s3-terminal_v*.bin) do (
    set "FIRMWARE=%%f"
    goto :foundfirmware
)

:foundfirmware
if "!FIRMWARE!"=="" (
    echo.
    echo ERROR: Cannot find release firmware file!
    echo Expected file: xiaozhi_esp32s3-terminal_vX.X.X.bin
    echo.
    echo Please download the firmware from GitHub Release
    echo and place it in this folder.
    echo.
    pause
    exit /b 1
)

echo.
echo Found firmware: !FIRMWARE!

echo.
set /p ERASE="Erase entire Flash? (y/N): "

if /i "!ERASE!"=="y" (
    echo.
    echo ====================================
    echo Erasing Flash...
    echo ====================================
    echo.

    .\esptool.exe --chip esp32s3 --port !PORT! erase_flash

    if !ERRORLEVEL! EQU 0 (
        echo Flash erase completed!
    ) else (
        echo Flash erase failed!
        pause
        exit /b 1
    )
)

echo.
echo ====================================
echo Starting firmware flash to !PORT!...
echo ====================================
echo.
echo Flash parameters:
echo   Chip: ESP32-S3
echo   File: !FIRMWARE!
echo   Address: 0x0
echo   Baud rate: 921600
echo   Flash mode: DIO
echo   Flash freq: 80MHz
echo   Flash size: 16MB
echo.

.\esptool.exe --chip esp32s3 --port !PORT! --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB 0x0 "!FIRMWARE!"

if !ERRORLEVEL! EQU 0 (
    echo.
    echo ====================================
    echo Firmware flash SUCCESS!
    echo ====================================
    echo.
    echo Device will auto-reset and run new firmware
    echo To view logs, connect serial monitor to !PORT!
    echo Baud rate: 115200
) else (
    echo.
    echo ====================================
    echo Firmware flash FAILED! Error code: !ERRORLEVEL!
    echo ====================================
    echo.
    echo Tips:
    echo   1. Is serial port being used by other program
    echo   2. Is USB connection stable
    echo   3. Try manual download mode:
    echo      - Press and hold BOOT button
    echo      - Press RST button briefly
    echo      - Release BOOT button
    echo      - Run this script again
)

endlocal

echo.
pause
