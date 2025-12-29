#include "sd_music_player.h"
#include "application.h"
#include "audio/audio_service.h"
#include "device_state.h"
#include <esp_log.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <algorithm>
#include <errno.h>
#include <cstring>

#define TAG "SdMusicPlayer"
#define MUSIC_DIR "/sdcard/music"

SdMusicPlayer::SdMusicPlayer() : is_playing_(false), interrupt_playback_(false) {
    // 在构造函数中注册MCP工具
    auto& mcp_server = McpServer::GetInstance();

    // 注册列出音乐文件的工具
    mcp_server.AddTool("self.music.list",
        "List all music files available on the SD card. "
        "Returns a list of OGG/Opus format audio files in /sdcard/music directory.",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            return HandleListMusic(properties);
        });

    // 注册播放音乐的工具
    mcp_server.AddTool("self.music.play",
        "Play a music file from SD card. "
        "The file must be in OGG/Opus format and located in /sdcard/music directory. "
        "Only one music file can be played at a time.",
        PropertyList({
            Property("filename", kPropertyTypeString)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            return HandlePlayMusic(properties);
        });

    // 注册停止播放的工具
    mcp_server.AddTool("self.music.stop",
        "Stop the currently playing music.",
        PropertyList(),
        [this](const PropertyList& properties) -> ReturnValue {
            return HandleStopMusic(properties);
        });

    ESP_LOGI(TAG, "SD Music Player MCP tools registered");
}

SdMusicPlayer::~SdMusicPlayer() {
}

ReturnValue SdMusicPlayer::HandleListMusic(const PropertyList& properties) {
    auto files = GetMusicFiles();
    
    if (files.empty()) {
        return "No music files found. Please place OGG/Opus files in /sdcard/music directory.";
    }

    std::string result = "Available music files:\n";
    for (size_t i = 0; i < files.size(); i++) {
        result += std::to_string(i + 1) + ". " + files[i] + "\n";
    }
    
    return result;
}

// 播放任务参数
struct PlayTaskParams {
    SdMusicPlayer* player;
    std::string filename;
};

// 播放任务（在后台执行同步播放）
void play_music_task(void* param) {
    auto* params = static_cast<PlayTaskParams*>(param);
    
    ESP_LOGI(TAG, "Play task started for: %s", params->filename.c_str());
    
    // 立即设置Speaking状态，通知服务器正在播放
    auto& app = Application::GetInstance();
    app.SetDeviceState(kDeviceStateSpeaking);
    ESP_LOGI(TAG, "Set to Speaking state immediately");
    
    // 执行同步播放
    params->player->PlayMusicFile(params->filename);
    
    delete params;
    vTaskDelete(nullptr);
}

ReturnValue SdMusicPlayer::HandlePlayMusic(const PropertyList& properties) {
    std::string filename = properties["filename"].value<std::string>();
    
    if (filename.empty()) {
        return "Error: filename parameter is required";
    }

    // 如果正在播放，先停止
    if (is_playing_) {
        StopCurrentMusic();
    }

    // 创建播放任务参数
    auto* params = new PlayTaskParams{this, filename};
    
    // 创建后台任务执行播放（播放本身是同步阻塞的）
    BaseType_t result = xTaskCreatePinnedToCore(
        play_music_task,
        "play_music",
        8192,
        params,
        5,
        nullptr,
        1
    );
    
    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create play task");
        delete params;
        return "Failed to start playback task";
    }
    
    // 返回成功消息，Application会检查IsMusicPlaying()保持Speaking状态
    current_playing_ = filename;
    is_playing_ = true;
    ESP_LOGI(TAG, "MCP returning success message");
    return "Started playing: " + filename;
}

ReturnValue SdMusicPlayer::HandleStopMusic(const PropertyList& properties) {
    if (!is_playing_) {
        return "No music is currently playing";
    }

    StopCurrentMusic();
    std::string stopped_file = current_playing_;
    current_playing_.clear();
    is_playing_ = false;
    
    // 返回空字符串，避免AI说话
    return "";
}

std::vector<std::string> SdMusicPlayer::GetMusicFiles() {
    std::vector<std::string> files;
    
    ESP_LOGI(TAG, "Opening directory: %s", MUSIC_DIR);
    DIR* dir = opendir(MUSIC_DIR);
    
    if (dir == nullptr) {
        ESP_LOGE(TAG, "Failed to open music directory: %s (errno=%d)", MUSIC_DIR, errno);
        
        // 检查SD卡根目录是否可访问
        DIR* root = opendir("/sdcard");
        if (root == nullptr) {
            ESP_LOGE(TAG, "SD card root directory not accessible!");
        } else {
            ESP_LOGI(TAG, "SD card root is accessible, trying to create music directory");
            closedir(root);
            // 尝试创建目录
            if (mkdir(MUSIC_DIR, 0755) == 0) {
                ESP_LOGI(TAG, "Created music directory successfully");
            } else {
                ESP_LOGE(TAG, "Failed to create music directory (errno=%d)", errno);
            }
        }
        return files;
    }

    ESP_LOGI(TAG, "Directory opened successfully, scanning for .ogg files");
    struct dirent* entry;
    int total_files = 0;
    while ((entry = readdir(dir)) != nullptr) {
        total_files++;
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // d_name contains UTF-8 encoded long filename when CONFIG_FATFS_LFN_HEAP is enabled
        // No need for conversion, just use it directly
        std::string filename = entry->d_name;
        
        ESP_LOGD(TAG, "Found file: %s (type: %d)", filename.c_str(), entry->d_type);
        
        // 只列出.ogg文件（不区分大小写）
        if (filename.length() > 4) {
            std::string ext = filename.substr(filename.length() - 4);
            // 转换为小写进行比较
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".ogg") {
                ESP_LOGI(TAG, "  -> OGG file: %s", filename.c_str());
                files.push_back(filename);
            }
        }
    }
    
    closedir(dir);
    std::sort(files.begin(), files.end());
    
    ESP_LOGI(TAG, "Scan complete: found %d .ogg files out of %d total files", files.size(), total_files);
    return files;
}

bool SdMusicPlayer::PlayMusicFile(const std::string& filename) {
    auto& app = Application::GetInstance();
    auto& audio_service = app.GetAudioService();
    
    std::string filepath = std::string(MUSIC_DIR) + "/" + filename;
    
    // 检查文件
    struct stat st;
    if (stat(filepath.c_str(), &st) != 0) {
        ESP_LOGW(TAG, "Music file not found: %s", filepath.c_str());
        return false;
    }

    ESP_LOGI(TAG, "Playing music: %s (%.2f MB)", filename.c_str(), st.st_size / 1024.0 / 1024.0);

    const long MAX_FILE_SIZE = 5 * 1024 * 1024;
    if (st.st_size > MAX_FILE_SIZE) {
        ESP_LOGE(TAG, "File too large: %.2f MB", st.st_size / 1024.0 / 1024.0);
        return false;
    }

    // 分配PSRAM
    char* buffer = (char*)heap_caps_malloc(st.st_size, MALLOC_CAP_SPIRAM);
    if (!buffer) {
        ESP_LOGE(TAG, "Failed to allocate PSRAM");
        return false;
    }

    // 读取文件
    FILE* file = fopen(filepath.c_str(), "rb");
    if (!file) {
        heap_caps_free(buffer);
        return false;
    }
    size_t bytes_read = fread(buffer, 1, st.st_size, file);
    fclose(file);
    
    if (bytes_read != (size_t)st.st_size) {
        heap_caps_free(buffer);
        return false;
    }

    // 清空队列
    audio_service.ResetDecoder();
    
    // 清除中断标志
    interrupt_playback_ = false;
    is_playing_ = true;
    current_playing_ = filename;

    // 同步播放（参考PlaySound）
    const uint8_t* buf = (const uint8_t*)buffer;
    size_t size = bytes_read;
    size_t offset = 0;

    auto find_page = [&](size_t start) -> size_t {
        for (size_t i = start; i + 4 <= size; ++i) {
            if (buf[i] == 'O' && buf[i+1] == 'g' && buf[i+2] == 'g' && buf[i+3] == 'S') 
                return i;
        }
        return (size_t)-1;
    };

    bool seen_head = false, seen_tags = false;
    int sample_rate = 24000;

    while (true) {
        // 检查中断
        if (interrupt_playback_) {
            ESP_LOGI(TAG, "Playback interrupted");
            break;
        }

        size_t pos = find_page(offset);
        if (pos == (size_t)-1) break;
        offset = pos;
        if (offset + 27 > size) break;

        const uint8_t* page = buf + offset;
        uint8_t segs = page[26];
        size_t seg_off = offset + 27;
        if (seg_off + segs > size) break;

        size_t body_size = 0;
        for (size_t i = 0; i < segs; ++i) body_size += page[27 + i];
        size_t body_off = seg_off + segs;
        if (body_off + body_size > size) break;

        size_t cur = body_off, seg_idx = 0;
        while (seg_idx < segs) {
            if (interrupt_playback_) break;

            size_t pkt_len = 0, pkt_start = cur;
            bool continued;
            do {
                uint8_t l = page[27 + seg_idx++];
                pkt_len += l;
                cur += l;
                continued = (l == 255);
            } while (continued && seg_idx < segs);

            if (pkt_len == 0) continue;
            const uint8_t* pkt = buf + pkt_start;

            if (!seen_head) {
                if (pkt_len >= 19 && memcmp(pkt, "OpusHead", 8) == 0) {
                    seen_head = true;
                    if (pkt_len >= 16) {
                        sample_rate = pkt[12] | (pkt[13] << 8) | (pkt[14] << 16) | (pkt[15] << 24);
                        ESP_LOGI(TAG, "Sample rate: %d", sample_rate);
                    }
                }
                continue;
            }
            if (!seen_tags) {
                if (pkt_len >= 8 && memcmp(pkt, "OpusTags", 8) == 0) seen_tags = true;
                continue;
            }

            // 同步推送（wait=true阻塞）
            auto packet = std::make_unique<AudioStreamPacket>();
            packet->sample_rate = sample_rate;
            packet->frame_duration = 60;
            packet->payload.assign(pkt, pkt + pkt_len);
            audio_service.PushPacketToDecodeQueue(std::move(packet), true);
        }

        offset = body_off + body_size;
    }

    heap_caps_free(buffer);
    
    // 播放完成
    is_playing_ = false;
    current_playing_.clear();
    
    // 切换到Listening状态
    app.SetDeviceState(kDeviceStateListening);
    ESP_LOGI(TAG, "Music playback finished");
    
    return true;
}

void SdMusicPlayer::StopCurrentMusic() {
    if (!is_playing_) {
        return;
    }
    
    ESP_LOGI(TAG, "Stopping music playback");
    
    // 设置中断标志
    interrupt_playback_ = true;
    
    // 清空解码队列
    auto& app = Application::GetInstance();
    auto& audio_service = app.GetAudioService();
    audio_service.ResetDecoder();
    
    is_playing_ = false;
    current_playing_.clear();
    
    ESP_LOGI(TAG, "Music playback stopped");
}
