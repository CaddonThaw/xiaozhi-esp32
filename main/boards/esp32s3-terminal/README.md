# ESP32S3 Terminal Board

## 硬件规格

- **主控**: ESP32-S3-WROOM-1 N16R8 (16MB Flash, 8MB PSRAM)
- **屏幕**: ST7789 320x240 TFT LCD (SPI接口)
- **麦克风**: MSM261S4030H0R (I2S MEMS麦克风)
- **扬声器**: NS4168 (I2S数字功放)

## 引脚配置

### 显示屏 (ST7789)
- LCD_SCK: GPIO 15
- LCD_MOSI: GPIO 7
- LCD_DC: GPIO 6
- LCD_CS: GPIO 4
- LCD_RST: GPIO 5
- LCD_BL: GPIO 16

### 扬声器 (NS4168)
- LRCLK: GPIO 41
- BCLK: GPIO 40
- SDATA: GPIO 39

### 麦克风 (MSM261S4030H0R)
- WS: GPIO 13
- SCK: GPIO 11
- SD: GPIO 12

### SD卡 (SDSPI模式)
- CS: GPIO 9
- MOSI: GPIO 10
- MISO: GPIO 17
- SCK: GPIO 14

### 按钮
- BOOT: GPIO 0

## 功能特性

### SD卡音乐播放
本板支持通过SD卡播放OGG/Opus格式的音乐文件。

#### 使用方法:
1. 格式化SD卡为FAT32格式
2. 将OGG/Opus格式的音乐文件直接放在SD卡根目录 (转换工具在tool/   music_convert)
3. 插入SD卡到板子

**修改音乐目录：** 如需更改音乐存放路径，编辑 `main/boards/common/sd_music_player.cc` 中的 `MUSIC_DIR` 宏定义
5. 通过AI语音指令控制播放:
   - "列出SD卡中的音乐" - 查看可播放的音乐列表
   - "播放[音乐文件名]" - 播放指定的音乐文件
   - "停止播放音乐" - 停止当前播放

#### 支持的AI指令:
- `self.music.list` - 列出所有可用的音乐文件
- `self.music.play` - 播放指定的音乐文件
- `self.music.stop` - 停止当前播放

## 编译说明

### 使用 idf.py 手动编译

1. 设置目标芯片:
```bash
idf.py set-target esp32s3
```

2. 配置项目:
```bash
idf.py menuconfig
```
在菜单中选择: `Xiaozhi Assistant` -> `Board Type` -> `ESP32S3 Terminal`

3. 编译:
```bash
idf.py build
```

4. 烧录:
```bash
idf.py flash monitor
```

### 使用 release.py 脚本自动编译

```bash
python scripts/release.py esp32s3-terminal
```
