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
    for (auto& pair : m_sounds) {
        UnloadSound(pair.second);
    }
    m_sounds.clear();
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

void AudioManager::SetSoundVolume(float volume) {
    m_sfxVolume = volume;
    for (auto& pair : m_sounds) {
        ::SetSoundVolume(pair.second, m_sfxVolume);
    }
}

void AudioManager::PlaySoundEffect(const std::string& soundPath) {
    if (!m_isAudioInit || soundPath.empty()) return;
    auto it = m_sounds.find(soundPath);
    if (it == m_sounds.end()) {
        if (FileExists(soundPath.c_str())) {
            Sound sound = LoadSound(soundPath.c_str());
            if (sound.stream.buffer != nullptr) {
                ::SetSoundVolume(sound, m_sfxVolume);
                m_sounds[soundPath] = sound;
                ::PlaySound(sound);
            }
        }
    } else {
        ::SetSoundVolume(it->second, m_sfxVolume);
        ::PlaySound(it->second);
    }
}

void AudioManager::StopSoundEffect(const std::string& soundPath) {
    if (!m_isAudioInit || soundPath.empty()) return;
    auto it = m_sounds.find(soundPath);
    if (it != m_sounds.end()) {
        ::StopSound(it->second);
    }
}
