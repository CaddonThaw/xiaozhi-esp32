#ifndef _SD_MUSIC_PLAYER_H_
#define _SD_MUSIC_PLAYER_H_

#include <string>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "mcp_server.h"

class SdMusicPlayer {
public:
    SdMusicPlayer();
    ~SdMusicPlayer();

    // Friend function for async playback task
    friend void music_playback_task(void* param);
    friend void play_music_task(void* param);  // 新的后台播放任务

public:
    // 检查是否正在播放
    bool IsPlaying() const { return is_playing_; }
    
    // 停止当前播放
    void StopCurrentMusic();

private:
    // MCP工具回调函数
    ReturnValue HandleListMusic(const PropertyList& properties);
    ReturnValue HandlePlayMusic(const PropertyList& properties);
    ReturnValue HandleStopMusic(const PropertyList& properties);

    // 获取SD卡中的音乐文件列表
    std::vector<std::string> GetMusicFiles();
    
    // 播放指定的音乐文件
    bool PlayMusicFile(const std::string& filename);

    std::string current_playing_;
    bool is_playing_;

    volatile bool interrupt_playback_;   // 中断播放标志
};

#endif // _SD_MUSIC_PLAYER_H_
