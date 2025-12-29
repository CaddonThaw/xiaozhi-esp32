@echo off
chcp 65001 >nul
title 下载 FFmpeg 便携版

echo.
echo ============================================================
echo              下载 FFmpeg 便携版
echo ============================================================
echo.

REM 检查是否已存在 ffmpeg
if exist "ffmpeg\bin\ffmpeg.exe" (
    echo ✅ FFmpeg 已存在，无需重新下载
    echo.
    pause
    exit /b 0
)

echo 正在下载 FFmpeg 便携版...
echo 文件大小约 90MB，请耐心等待...
echo.

REM 创建 ffmpeg 目录
if not exist "ffmpeg" mkdir ffmpeg

REM 下载 FFmpeg
powershell -Command "Invoke-WebRequest -Uri 'https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.zip' -OutFile 'ffmpeg.zip' -UseBasicParsing"

if not exist "ffmpeg.zip" (
    echo.
    echo [错误] 下载失败，请检查网络连接
    echo.
    pause
    exit /b 1
)

echo ✅ 下载完成
echo 正在解压...

REM 解压文件
powershell -Command "Expand-Archive -Path 'ffmpeg.zip' -DestinationPath 'ffmpeg_temp' -Force"

echo 正在安装...

REM 移动 bin 目录
for /d %%i in (ffmpeg_temp\ffmpeg-*) do (
    if exist "%%i\bin" (
        xcopy "%%i\bin" "ffmpeg\bin\" /E /I /Y >nul
    )
)

REM 清理临时文件
if exist "ffmpeg_temp" rmdir /s /q "ffmpeg_temp"
if exist "ffmpeg.zip" del /f /q "ffmpeg.zip"

echo.
echo ============================================================
echo ✅ FFmpeg 安装完成！现在可以使用转换工具了
echo ============================================================
echo.

REM 验证安装
if exist "ffmpeg\bin\ffmpeg.exe" (
    echo [验证] FFmpeg 安装成功
    ffmpeg\bin\ffmpeg.exe -version | findstr "ffmpeg version"
) else (
    echo [警告] 安装可能有问题，请检查 ffmpeg\bin 目录
)

echo.
pause
