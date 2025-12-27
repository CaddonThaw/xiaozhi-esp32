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

### 按钮
- BOOT: GPIO 0

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
