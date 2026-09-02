#pragma once
#include "resources.h"
#include "AudioManager.h"
#include "ProfileManager.h"
#include <string>

// Facade Pattern unifying core engine subsystems (Resources, AudioManager, ProfileManager)
class GameEngineFacade {
public:
    static void PlaySFX(const std::string& soundAsset) {
        std::string fullPath = Resources::GetInstance().GetAssetPath(soundAsset);
        AudioManager::GetInstance().PlaySoundEffect(fullPath);
    }

    static Texture2D GetTexture(const std::string& name) {
        return Resources::GetInstance().GetTexture(name);
    }

    static ReanimDefinition LoadReanim(const std::string& filePath) {
        return Resources::GetInstance().LoadReanim(filePath);
    }

    static ProfileManager& GetProfile() {
        return ProfileManager::GetInstance();
    }
};
