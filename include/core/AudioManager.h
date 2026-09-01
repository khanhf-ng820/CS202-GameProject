#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

enum class MusicTrack {
    None,
    MainMenu,
    ShopMenu,
    Vasebreaker,
    DayLevel,
    NightLevel,
    ChooseYourSeeds,
    Bowling
};

class AudioManager {
public:
    static AudioManager& GetInstance();

    void Init();
    void Close();

    void PlayMusic(MusicTrack track);
    void StopMusic();
    void Update();

    void PlaySoundEffect(const std::string& soundPath);
    void StopSoundEffect(const std::string& soundPath);

    void SetMusicVolume(float volume);
    float GetMusicVolume() const { return m_volume; }

    void SetSoundVolume(float volume);
    float GetSoundVolume() const { return m_sfxVolume; }

    MusicTrack GetCurrentTrack() const { return m_currentTrack; }

private:
    AudioManager() = default;
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    MusicTrack m_currentTrack = MusicTrack::None;
    Music m_currentMusic = { 0 };
    std::unordered_map<std::string, Sound> m_sounds;
    bool m_isAudioInit = false;
    bool m_isMusicLoaded = false;
    float m_volume = 1.0f; // in range [0.0f, 1.0f]
    float m_sfxVolume = 1.0f; // in range [0.0f, 1.0f]
};

