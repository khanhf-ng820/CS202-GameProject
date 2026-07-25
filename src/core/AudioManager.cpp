#include "AudioManager.h"
#include "resources.h"

AudioManager& AudioManager::GetInstance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::Init() {
    if (!m_isAudioInit) {
        InitAudioDevice();
        m_isAudioInit = true;
    }
}

void AudioManager::Close() {
    StopMusic();
    if (m_isAudioInit) {
        CloseAudioDevice();
        m_isAudioInit = false;
    }
}

void AudioManager::PlayMusic(MusicTrack track) {
    if (track == m_currentTrack && m_isMusicLoaded) {
        return;
    }

    StopMusic();

    m_currentTrack = track;
    if (track == MusicTrack::None) {
        return;
    }

    Resources& res = Resources::GetInstance();
    std::string soundPath;

    if (track == MusicTrack::MainMenu) {
        soundPath = res.GetAssetPath("assets/sounds/main_menu.ogg");
    } else if (track == MusicTrack::ShopMenu) {
        soundPath = res.GetAssetPath("assets/sounds/shop.ogg");
    }

    if (!soundPath.empty()) {
        m_currentMusic = LoadMusicStream(soundPath.c_str());
        if (m_currentMusic.ctxData != nullptr) {
            m_isMusicLoaded = true;
            m_currentMusic.looping = true;
            ::SetMusicVolume(m_currentMusic, m_volume);
            PlayMusicStream(m_currentMusic);
        }
    }
}

void AudioManager::StopMusic() {
    if (m_isMusicLoaded) {
        StopMusicStream(m_currentMusic);
        UnloadMusicStream(m_currentMusic);
        m_isMusicLoaded = false;
        m_currentMusic = { 0 };
    }
    m_currentTrack = MusicTrack::None;
}

void AudioManager::Update() {
    if (m_isMusicLoaded) {
        UpdateMusicStream(m_currentMusic);
    }
}

void AudioManager::SetMusicVolume(float volume) {
    m_volume = volume;
    if (m_isMusicLoaded) {
        ::SetMusicVolume(m_currentMusic, m_volume);
    }
}
