# ESP32-S3 Terminal 固件烧录包

本烧录包包含所有必要文件，无需安装 ESP-IDF 环境。

- 只需双击运行 `flash.bat` 即可完成烧录。

## 硬件配置

- 芯片：ESP32-S3 N16R8（16MB Flash，8MB PSRAM）
- 显示屏：ST7789 320x240 SPI LCD
- 音频输出：NS4168 I2S 功放
- 音频输入：MSM261S4030H0R I2S 麦克风
- 按键：GPIO0（Boot 按钮）

## 引脚定义

### 显示屏

- MOSI = 7
- CS = 15
- RST = 4
- BL = 5
- SCK = 16
- DC = 6

### 麦克风（I2S）

- BCLK = 11
- WS = 13
- DATA = 12

### 扬声器（I2S）

- BCLK = 40
- LRCLK = 41
- DATA = 39

### SD 卡（SPI）

- CS = 9
- MOSI = 10
- MISO = 17
- SCK = 14

## 烧录步骤

### 方法一：使用烧录脚本（推荐，无需安装任何软件）

1. 连接 ESP32-S3 开发板到电脑 USB 口
2. 双击运行 `flash.bat`
3. 从列表中选择对应的串口编号
4. 等待烧录完成

### 方法二：手动使用 esptool（高级用户）

```bat
esptool.exe --chip esp32s3 --port COM3 --baud 921600 ^
  --before default_reset --after hard_reset write_flash ^
  --flash_mode dio --flash_freq 80m --flash_size 16MB ^
  0x0 bootloader.bin ^
  0x8000 partition-table.bin ^
  0xd000 ota_data_initial.bin ^
  0x20000 xiaozhi.bin ^
  0x800000 generated_assets.bin
```

> 注意：将 `COM3` 替换为实际串口号。

## 文件说明

### 固件文件

- `bootloader.bin`：ESP32-S3 引导加载程序（0x0）
- `partition-table.bin`：分区表（0x8000）
- `ota_data_initial.bin`：OTA 数据初始化（0xd000）
- `xiaozhi.bin`：主固件（0x20000，约 2.7MB）
- `generated_assets.bin`：资源文件（字体/表情/唤醒词）（0x800000，约 1.5MB）

### 工具文件

- `esptool.exe`：固件烧录工具（独立可执行文件）
- `python.exe`：Python 运行时（esptool 依赖）
- `flash.bat`：自动烧录脚本
- `README.txt`：本说明文件

总计 Flash 使用：约 4.2MB / 16MB

## 故障排查

### 问题：找不到串口

解决：

- 检查 USB 线是否支持数据传输
- 安装 CH340/CP2102 等 USB 转串口驱动
- 在设备管理器中确认串口存在

### 问题：烧录失败（“连接超时”）

解决：

- 手动进入下载模式：
  1. 按住 **BOOT** 按钮
  2. 短按 **RESET** 按钮
  3. 松开 **BOOT** 按钮
- 降低波特率（`--baud 115200`）
- 更换 USB 口 / USB 线

### 问题：烧录成功但无显示

解决：

- 检查显示屏连接和引脚定义
- 使用串口监视器查看日志（115200 波特率）
- 检查背光 GPIO5 是否工作

### 问题：无音频输出

解决：

- 检查 I2S 引脚连接
- 检查 NS4168 功放供电
- 查看串口日志中的音频初始化信息

## 更多信息

- 项目地址：https://github.com/CaddonThaw/xiaozhi-esp32
- 固件版本：v2.1.0
- 构建时间：2025-12-27
- ESP-IDF：v5.5.1

### 功能特性

- 离线唤醒词检测（“你好小智”）
- 实时语音识别（ASR）
- 大语言模型对话（LLM）
- 语音合成播放（TTS）
- 回声消除（AEC）
- MCP 协议支持
- OTA 远程升级

如遇问题请查阅项目文档或提交 Issue。
