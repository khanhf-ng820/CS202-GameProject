#pragma once
#include "raylib.h"
#include <string>

enum class MusicTrack {
    None,
    MainMenu,
    ShopMenu
};

class AudioManager {
public:
    static AudioManager& GetInstance();

    void Init();
    void Close();

    void PlayMusic(MusicTrack track);
    void StopMusic();
    void Update();

    void SetMusicVolume(float volume);
    float GetMusicVolume() const { return m_volume; }
    MusicTrack GetCurrentTrack() const { return m_currentTrack; }

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    MusicTrack m_currentTrack = MusicTrack::None;
    Music m_currentMusic = { 0 };
    bool m_isAudioInit = false;
    bool m_isMusicLoaded = false;
    float m_volume = 1.0f; // in range [0.0f, 1.0f]
};
