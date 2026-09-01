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
    } else if (track == MusicTrack::Vasebreaker) {
        soundPath = res.GetAssetPath("assets/sounds/moongrains.ogg");
    } else if (track == MusicTrack::DayLevel) {
        soundPath = res.GetAssetPath("assets/sounds/grasswalk.ogg");
    } else if (track == MusicTrack::NightLevel) {
        soundPath = res.GetAssetPath("assets/sounds/watery_graves.ogg");
    } else if (track == MusicTrack::ChooseYourSeeds) {
        soundPath = res.GetAssetPath("assets/sounds/choose_your_seeds.ogg");
    } else if (track == MusicTrack::Bowling) {
        soundPath = res.GetAssetPath("assets/sounds/wallnut_bowling_bgm.ogg");
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

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace {
Sound LoadSoundTrimmed(const char* fileName, float thresholdRatio = 0.02f) {
    Wave wave = LoadWave(fileName);
    if (wave.data == nullptr || wave.frameCount == 0) {
        return LoadSound(fileName);
    }

    int firstSample = 0;
    int channels = (int)wave.channels;
    if (channels <= 0) channels = 1;

    if (wave.sampleSize == 16) {
        short* samples = (short*)wave.data;
        short threshold = (short)(32767.0f * thresholdRatio);
        for (unsigned int f = 0; f < wave.frameCount; ++f) {
            for (int c = 0; c < channels; ++c) {
                if (std::abs((int)samples[f * channels + c]) > threshold) {
                    firstSample = (int)f;
                    goto found;
                }
            }
        }
    } else if (wave.sampleSize == 32) {
        float* samples = (float*)wave.data;
        for (unsigned int f = 0; f < wave.frameCount; ++f) {
            for (int c = 0; c < channels; ++c) {
                if (std::fabs(samples[f * channels + c]) > thresholdRatio) {
                    firstSample = (int)f;
                    goto found;
                }
            }
        }
    } else if (wave.sampleSize == 8) {
        unsigned char* samples = (unsigned char*)wave.data;
        int threshold = (int)(127.0f * thresholdRatio);
        for (unsigned int f = 0; f < wave.frameCount; ++f) {
            for (int c = 0; c < channels; ++c) {
                if (std::abs((int)samples[f * channels + c] - 128) > threshold) {
                    firstSample = (int)f;
                    goto found;
                }
            }
        }
    }

found:
    if (firstSample > 0 && firstSample < (int)wave.frameCount) {
        Wave trimmed;
        trimmed.frameCount = wave.frameCount - (unsigned int)firstSample;
        trimmed.sampleRate = wave.sampleRate;
        trimmed.sampleSize = wave.sampleSize;
        trimmed.channels = wave.channels;

        unsigned int bytesPerFrame = (unsigned int)wave.channels * (wave.sampleSize / 8);
        unsigned int dataSize = trimmed.frameCount * bytesPerFrame;
        trimmed.data = MemAlloc(dataSize);
        if (trimmed.data != nullptr) {
            memcpy(trimmed.data, (unsigned char*)wave.data + ((size_t)firstSample * bytesPerFrame), dataSize);
            Sound sound = LoadSoundFromWave(trimmed);
            UnloadWave(trimmed);
            UnloadWave(wave);
            return sound;
        }
    }

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}
}

void AudioManager::PlaySoundEffect(const std::string& soundPath) {
    if (!m_isAudioInit || soundPath.empty()) return;
    auto it = m_sounds.find(soundPath);
    if (it == m_sounds.end()) {
        if (FileExists(soundPath.c_str())) {
            Sound sound = LoadSoundTrimmed(soundPath.c_str());
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
