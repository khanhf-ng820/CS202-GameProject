#pragma once
#include "raylib.h"
#include "reanim.h"
#include "resources.h"
#include <vector>
#include <string>
#include <unordered_map>

class Reanimation {
public:
    Reanimation() = default;
    Reanimation(const ReanimDefinition& def, const Resources& resources);
    void SetResources(const ReanimDefinition& def, const Resources& resources);
    void Update(float dt);
    void Draw(float x, float y, float scale) const;
    void Draw(float x, float y, float scale, Color tint) const;
    void Draw(Vector2 position, float scale) const;
    void Draw(Vector2 position, float scale, Color tint) const;

    // Playback control
    void SetAnimation(const std::string& animName);
    void SetAnimationIndex(int index);
    void SetBaseAnimation(const std::string& animName);
    void SetBaseAnimationIndex(int index);
    void SetSpeed(float speed);
    float GetSpeed() const;
    void SetPaused(bool paused);
    bool IsPaused() const;
    void TogglePause();
    void SetFrame(float frame);

    // Query methods
    int GetCurrentFrame() const;
    int GetEndFrame() const;
    std::string GetCurrentAnimName() const;
    std::string GetFormattedAnimName() const;
    const std::vector<AnimationRange>& GetAnimations() const;
    int GetCurrentAnimIndex() const;
    void SetTrackVisible(const std::string& trackName, bool visible);
    void OverrideTrackImage(const std::string& trackName, const std::string& imageName);
    void ClearTrackImageOverride(const std::string& trackName);
    void AddCustomAnimation(const std::string& newAnimName, const std::string& baseAnimName);

private:
    void PopulateAnimations();

    ReanimDefinition m_def;
    const Resources* m_resources = nullptr;

    float m_currentFrameFloat = 0.0f;
    int m_currentAnimIndex = 0;
    float m_baseFrameFloat = 0.0f;
    int m_baseAnimIndex = -1;
    std::vector<AnimationRange> m_anims;
    float m_speedMultiplier = 1.0f;
    bool m_isPaused = false;
    std::unordered_map<std::string, bool> m_trackVisibility;
    std::unordered_map<std::string, std::string> m_trackImageOverrides;
};
