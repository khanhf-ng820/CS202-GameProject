#pragma once
#include "AudioManager.h"
#include <string>

// Target interface for audio subsystem adapter
class IAudioEngine {
public:
    virtual ~IAudioEngine() = default;
    virtual void playSound(const std::string& soundAsset) = 0;
};

// Adapter Pattern adapting Raylib/AudioManager API to IAudioEngine interface
class RaylibAudioAdapter : public IAudioEngine {
public:
    void playSound(const std::string& soundAsset) override {
        AudioManager::GetInstance().PlaySoundEffect(soundAsset);
    }
};
