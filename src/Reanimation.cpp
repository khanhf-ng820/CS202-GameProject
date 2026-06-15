#include "Reanimation.h"
#include "rlgl.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Reanimation::Reanimation(const ReanimDefinition& def, const Resources& resources) {
    SetResources(def, resources);
}

void Reanimation::SetResources(const ReanimDefinition& def, const Resources& resources) {
    m_def = def;
    m_resources = &resources;
    PopulateAnimations();
    // Default to first animation if available
    if (!m_anims.empty()) {
        m_currentAnimIndex = 0;
        m_currentFrameFloat = (float)m_anims[m_currentAnimIndex].startFrame;
    }
}

void Reanimation::PopulateAnimations() {
    m_anims.clear();
    for (const auto& track : m_def.tracks) {
        // If it is an animation control track (has no image names in any keyframe)
        bool isControlTrack = true;
        for (const auto& kf : track.keyframes) {
            if (!kf.imageName.empty()) {
                isControlTrack = false;
                break;
            }
        }
        
        // Also check if its name starts with "anim_"
        if (isControlTrack && track.name.rfind("anim_", 0) == 0) {
            // Find ranges where kf.f is not -1
            int start = -1;
            for (size_t i = 0; i < track.keyframes.size(); ++i) {
                bool active = (track.keyframes[i].f != -1);
                if (active && start == -1) {
                    start = (int)i;
                } else if (!active && start != -1) {
                    m_anims.push_back({track.name, start, (int)i - 1});
                    start = -1;
                }
            }
            if (start != -1) {
                m_anims.push_back({track.name, start, (int)track.keyframes.size() - 1});
            }
        }
    }

    // If no animations found, create a default range for the whole file
    if (m_anims.empty()) {
        m_anims.push_back({"All Frames", 0, m_def.maxFrames - 1});
    }
}

void Reanimation::Update(float dt) {
    if (m_isPaused || m_anims.empty()) {
        return;
    }

    float step = dt * m_def.fps * m_speedMultiplier;
    m_currentFrameFloat += step;

    if (m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size()) {
        m_baseFrameFloat += step;
        int baseStart = m_anims[m_baseAnimIndex].startFrame;
        int baseEnd = m_anims[m_baseAnimIndex].endFrame;
        if (m_baseFrameFloat > (float)baseEnd) {
            m_baseFrameFloat = (float)baseStart;
        } else if (m_baseFrameFloat < (float)baseStart) {
            m_baseFrameFloat = (float)baseStart;
        }
    }

    int start = m_anims[m_currentAnimIndex].startFrame;
    int end = m_anims[m_currentAnimIndex].endFrame;

    if (m_currentFrameFloat > (float)end) {
        m_currentFrameFloat = (float)start; // Loop back
    } else if (m_currentFrameFloat < (float)start) {
        m_currentFrameFloat = (float)start;
    }
}

void Reanimation::Draw(float x, float y, float scale) const {
    Draw(x, y, scale, WHITE);
}

void Reanimation::Draw(float x, float y, float scale, Color tint) const {
    int currentFrame = GetCurrentFrame();

    for (const auto& track : m_def.tracks) {
        // Check track visibility
        auto it = m_trackVisibility.find(track.name);
        if (it != m_trackVisibility.end() && !it->second) {
            continue;
        }

        if (currentFrame < 0 || currentFrame >= (int)track.keyframes.size()) {
            continue;
        }

        const ReanimKeyframe* kf_ptr = nullptr;

        if (currentFrame >= 0 && currentFrame < (int)track.keyframes.size()) {
            const auto& kf_overlay = track.keyframes[currentFrame];
            if (kf_overlay.f != -1 && !kf_overlay.imageName.empty()) {
                kf_ptr = &kf_overlay;
            }
        }

        // Fallback to base animation if overlay frame is hidden
        if (!kf_ptr && m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size()) {
            int baseFrame = (int)m_baseFrameFloat;
            if (baseFrame >= 0 && baseFrame < (int)track.keyframes.size()) {
                const auto& kf_base = track.keyframes[baseFrame];
                if (kf_base.f != -1 && !kf_base.imageName.empty()) {
                    kf_ptr = &kf_base;
                }
            }
        }

        if (!kf_ptr) {
            continue;
        }

        const auto& kf = *kf_ptr;

        Texture2D tex = m_resources->GetTexture(kf.imageName);
        if (tex.id == 0) {
            continue; // Texture not loaded or not found
        }

        // Compute transform matrix for this track
        float radX = kf.kx * DEG2RAD;
        float radY = kf.ky * DEG2RAD;

        float m00 = kf.sx * cosf(radX);
        float m10 = kf.sx * sinf(radX);
        float m01 = -kf.sy * sinf(radY);
        float m11 = kf.sy * cosf(radY);
        
        // Root translation + track local translation
        float tx = x + kf.x * scale;
        float ty = y + kf.y * scale;

        // Apply scale to matrix components
        float finalM00 = m00 * scale;
        float finalM10 = m10 * scale;
        float finalM01 = m01 * scale;
        float finalM11 = m11 * scale;

        // Build 4x4 column-major matrix for Raylib/OpenGL
        float mat[16] = {
            finalM00, finalM10, 0.0f, 0.0f,
            finalM01, finalM11, 0.0f, 0.0f,
            0.0f,     0.0f,     1.0f, 0.0f,
            tx,       ty,       0.0f, 1.0f
        };

        rlPushMatrix();
        rlMultMatrixf(mat);

        // Draw texture at local origin (0, 0)
        DrawTexture(tex, 0, 0, tint);

        rlPopMatrix();
    }
}

void Reanimation::Draw(Vector2 position, float scale) const {
    Draw(position.x, position.y, scale, WHITE);
}

void Reanimation::Draw(Vector2 position, float scale, Color tint) const {
    Draw(position.x, position.y, scale, tint);
}

void Reanimation::SetAnimation(const std::string& animName) {
    for (size_t i = 0; i < m_anims.size(); ++i) {
        if (m_anims[i].name == animName) {
            SetAnimationIndex((int)i);
            return;
        }
    }
}

void Reanimation::SetAnimationIndex(int index) {
    if (index >= 0 && index < (int)m_anims.size()) {
        m_currentAnimIndex = index;
        m_currentFrameFloat = (float)m_anims[index].startFrame;
    }
}

void Reanimation::SetBaseAnimation(const std::string& animName) {
    for (size_t i = 0; i < m_anims.size(); ++i) {
        if (m_anims[i].name == animName) {
            SetBaseAnimationIndex((int)i);
            return;
        }
    }
}

void Reanimation::SetBaseAnimationIndex(int index) {
    if (index >= 0 && index < (int)m_anims.size()) {
        m_baseAnimIndex = index;
        m_baseFrameFloat = (float)m_anims[index].startFrame;
    }
}

void Reanimation::SetSpeed(float speed) {
    m_speedMultiplier = speed;
}

float Reanimation::GetSpeed() const {
    return m_speedMultiplier;
}

void Reanimation::SetPaused(bool paused) {
    m_isPaused = paused;
}

bool Reanimation::IsPaused() const {
    return m_isPaused;
}

void Reanimation::TogglePause() {
    m_isPaused = !m_isPaused;
}

int Reanimation::GetCurrentFrame() const {
    return (int)m_currentFrameFloat;
}

int Reanimation::GetEndFrame() const {
    if (m_anims.empty() || m_currentAnimIndex >= (int)m_anims.size()) {
        return 0;
    }
    return m_anims[m_currentAnimIndex].endFrame;
}

std::string Reanimation::GetCurrentAnimName() const {
    if (m_anims.empty() || m_currentAnimIndex >= (int)m_anims.size()) {
        return "";
    }
    return m_anims[m_currentAnimIndex].name;
}

std::string Reanimation::GetFormattedAnimName() const {
    return m_resources->FormatAnimName(GetCurrentAnimName());
}

const std::vector<AnimationRange>& Reanimation::GetAnimations() const {
    return m_anims;
}

int Reanimation::GetCurrentAnimIndex() const {
    return m_currentAnimIndex;
}

void Reanimation::SetTrackVisible(const std::string& trackName, bool visible) {
    m_trackVisibility[trackName] = visible;
}

void Reanimation::SetFrame(float frame) {
    m_currentFrameFloat = frame;
}
