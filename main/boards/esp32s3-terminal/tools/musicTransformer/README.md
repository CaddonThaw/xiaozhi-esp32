# 音频格式转换工具

将各种音频格式（MP3, WAV, FLAC等）转换为 ESP32 小智音箱支持的 OGG/Opus 格式。

## 📁 目录结构

```
music-transformer/
├── music/              # 放入待转换的音频文件
├── output/             # 转换后的 OGG 文件
├── convert_to_ogg.py   # Python 转换脚本
├── 转换音乐.bat         # Windows 一键转换
└── README.md           # 使用说明
```

## 🚀 快速开始

### 首次使用（仅需一次）：

1. 双击运行 `下载FFmpeg.bat` （自动下载便携版 FFmpeg）
2. 等待下载完成（约 90MB）

### 转换音频：

1. 将音频文件放入 `music` 文件夹
2. 双击运行 `转换音乐.bat`
3. 转换后的文件在 `output` 文件夹

> **💡 提示：** 工具已集成便携版 FFmpeg，无需手动安装！

## 📋 系统要求

### 必须安装：

**Python 3.7+**
- 下载：https://www.python.org/downloads/
- 安装时勾选 "Add Python to PATH"

### FFmpeg（自动下载，无需手动安装）

工具会自动下载 FFmpeg 便携版到本地目录，完全无需配置系统环境变量！

如果自动下载失败，也可以手动下载：
1. 访问：https://www.gyan.dev/ffmpeg/builds/
2. 下载 `ffmpeg-release-essentials.zip`
3. 解压后将 `bin` 文件夹复制到工具的 `ffmpeg\bin` 目录

### 验证安装：

```bash
python --version
# FFmpeg 无需验证，工具会自动处理
```

## 🎵 支持的格式

**输入格式：**
- MP3
- WAV
- FLAC
- M4A
- AAC
- WMA
- OGG
- OPUS

**输出格式：**
- OGG (Opus 编码)

## ⚙️ 配置选项

编辑 `convert_to_ogg.py` 中的配置：

```python
INPUT_DIR = "music"          # 输入文件夹
OUTPUT_DIR = "output"        # 输出文件夹
BITRATE = "64k"              # 音频比特率
```

### 比特率建议：

| 比特率 | 音质 | 文件大小 | 推荐场景 |
|--------|------|----------|----------|
| 48k    | 一般 | 小       | 语音、播客 |
| 64k    | 良好 | 中等     | 一般音乐（默认） |
| 96k    | 优秀 | 较大     | 高品质音乐 |
| 128k   | 极佳 | 大       | 无损音质要求 |

## 📝 使用流程

1. **准备音频文件**
   ```
   将 MP3/WAV 等文件放入 music/ 文件夹
   ```

2. **执行转换**
   ```
   双击 转换音乐.bat
   ```

3. **获取结果**
   ```
   转换后的 OGG 文件在 output/ 文件夹
   ```

4. **拷贝到 SD 卡**
   ```
   将 output/ 中的 .ogg 文件拷贝到 SD 卡根目录
   ```

## 🎯 常见问题

### Q: 提示"未找到 FFmpeg"？
A: 需要安装 FFmpeg 并添加到系统 PATH：
   解决方案（任选其一）：
   1. **推荐：** 运行 `下载FFmpeg.bat` 自动下载便携版
   2. 手动下载：
      - 访问：https://www.gyan.dev/ffmpeg/builds/
      - 下载 `ffmpeg-release-essentials.zip`
      - 解压后将 `bin` 文件夹复制到 `ffmpeg\bin` 目录
### Q: 转换后音质不好？
A: 修改 `convert_to_ogg.py` 中的 `BITRATE`：
   ```python
   BITRATE = "96k"  # 或 "128k"
   ```

### Q: 转换速度慢？
A: Opus 编解码较复杂，这是正常的。可以：
   - 降低 `compression_level` (脚本中修改)
   - 使用性能更好的电脑
   - 批量转换时等待即可

### Q: 支持批量转换吗？
A: 支持！将所有文件放入 `music` 文件夹，脚本会自动批量转换。

## 🔧 高级用法

### 修改压缩级别

编辑 `convert_to_ogg.py`，找到：
```python
'-compression_level', '10',  # 0-10，越高质量越好但越慢
```

建议值：
- `0-3`: 快速转换，质量一般
- `5-7`: 平衡速度和质量
- `8-10`: 最佳质量，速度较慢（默认）

### 自定义输出文件名

脚本会自动保持原文件名，仅修改扩展名为 `.ogg`

## 📌 注意事项

1. **文件命名：** 避免使用特殊字符和中文空格
2. **文件大小：** OGG 格式会比 MP3 略小或相当
3. **音质损失：** 从有损格式转 OGG 会有轻微质量损失
4. **SD 卡兼容：** 转换后的文件可直接用于 ESP32 小智音箱

## 📞 技术支持

如有问题，请检查：
1. Python 和 FFmpeg 是否正确安装
2. 输入文件格式是否支持
3. 查看错误信息并搜索解决方案

---

**开发者：** CaddonThaw  
**更新日期：** 2025-01-28
