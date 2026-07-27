#include "Reanimation.h"
#include "rlgl.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <map>

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
        m_currentFrameFloat = GetLoopStartTime(0);
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
        m_currentFrameFloat = GetLoopStartTime(m_currentAnimIndex); // Loop back
    } else if (m_currentFrameFloat < (float)start) {
        m_currentFrameFloat = GetLoopStartTime(m_currentAnimIndex);
    }
}

ReanimKeyframe Reanimation::GetInterpolatedKeyframe(const ReanimTrack& track, float frameFloat, int animIndex) const {
    if (track.keyframes.empty()) return ReanimKeyframe{};

    int f1 = (int)floorf(frameFloat);
    if (f1 < 0) f1 = 0;
    if (f1 >= (int)track.keyframes.size()) f1 = (int)track.keyframes.size() - 1;

    float fraction = frameFloat - (float)f1;
    if (fraction <= 0.0001f) {
        return track.keyframes[f1];
    }

    int startFrame = 0;
    int endFrame = (int)track.keyframes.size() - 1;
    int loopStartFrame = 0;

    if (animIndex >= 0 && animIndex < (int)m_anims.size()) {
        startFrame = m_anims[animIndex].startFrame;
        endFrame = m_anims[animIndex].endFrame;
        loopStartFrame = (int)GetLoopStartTime(animIndex);
    }

    int f2 = f1 + 1;
    if (f2 > endFrame) {
        f2 = (loopStartFrame >= startFrame && loopStartFrame <= endFrame) ? loopStartFrame : startFrame;
    }

    if (f2 < 0 || f2 >= (int)track.keyframes.size()) {
        return track.keyframes[f1];
    }

    const auto& kf1 = track.keyframes[f1];
    const auto& kf2 = track.keyframes[f2];

    if (kf1.f == -1 || kf2.f == -1) {
        return kf1;
    }

    ReanimKeyframe result = kf1;
    result.x = kf1.x + (kf2.x - kf1.x) * fraction;
    result.y = kf1.y + (kf2.y - kf1.y) * fraction;
    result.sx = kf1.sx + (kf2.sx - kf1.sx) * fraction;
    result.sy = kf1.sy + (kf2.sy - kf1.sy) * fraction;
    result.kx = kf1.kx + (kf2.kx - kf1.kx) * fraction;
    result.ky = kf1.ky + (kf2.ky - kf1.ky) * fraction;

    return result;
}

void Reanimation::Draw(float x, float y, float scale) const {
    Draw(x, y, scale, WHITE);
}

void Reanimation::Draw(float x, float y, float scale, Color tint) const {
    int currentFrame = GetCurrentFrame();

    float stem_offset_x = 0.0f;
    float stem_offset_y = 0.0f;

    if (m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size() && m_currentAnimIndex != m_baseAnimIndex) {
        int baseStart = m_anims[m_baseAnimIndex].startFrame;
        int baseFrame = (int)m_baseFrameFloat;
        
        const ReanimTrack* stemTrack = nullptr;
        for (const auto& tr : m_def.tracks) {
            if (tr.name == "anim_stem") {
                stemTrack = &tr;
                break;
            }
        }
        if (!stemTrack) {
            for (const auto& tr : m_def.tracks) {
                if (tr.name == "stalk_top" || tr.name == "stalk_bottom" || tr.name == "body") {
                    stemTrack = &tr;
                    break;
                }
            }
        }
        
        int currentStart = m_anims[m_currentAnimIndex].startFrame;
        int currentEnd = m_anims[m_currentAnimIndex].endFrame;
        bool stemIsConstantInCurrent = true;
        if (stemTrack && currentStart >= 0 && currentEnd < (int)stemTrack->keyframes.size() && currentStart <= currentEnd) {
            const auto& firstKf = stemTrack->keyframes[currentStart];
            for (int fIdx = currentStart + 1; fIdx <= currentEnd; ++fIdx) {
                const auto& kfTest = stemTrack->keyframes[fIdx];
                if (kfTest.x != firstKf.x || kfTest.y != firstKf.y) {
                    stemIsConstantInCurrent = false;
                    break;
                }
            }
        }

        if (stemIsConstantInCurrent && stemTrack && baseStart >= 0 && baseStart < (int)stemTrack->keyframes.size() &&
            baseFrame >= 0 && baseFrame < (int)stemTrack->keyframes.size()) {
            ReanimKeyframe interpolatedStem = GetInterpolatedKeyframe(*stemTrack, m_baseFrameFloat, m_baseAnimIndex);
            stem_offset_x = interpolatedStem.x - stemTrack->keyframes[baseStart].x;
            stem_offset_y = interpolatedStem.y - stemTrack->keyframes[baseStart].y;
        }
    }

    for (const auto& track : m_def.tracks) {
        // Check track visibility
        auto it = m_trackVisibility.find(track.name);
        if (it != m_trackVisibility.end() && !it->second) {
            continue;
        }

        if (currentFrame < 0 || currentFrame >= (int)track.keyframes.size()) {
            continue;
        }

        ReanimKeyframe kf_holder;
        bool hasKeyframe = false;
        bool usedOverlayTrack = false;
        std::string resolvedImageName;

        // Determine if the current overlay animation actually animates this track
        bool overlayIsConstant = false;
        if (m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size() && m_currentAnimIndex != m_baseAnimIndex) {
            int startF = m_anims[m_currentAnimIndex].startFrame;
            int endF = m_anims[m_currentAnimIndex].endFrame;
            if (startF >= 0 && endF < (int)track.keyframes.size() && startF <= endF) {
                overlayIsConstant = true;
                const auto& firstKf = track.keyframes[startF];
                for (int fIdx = startF + 1; fIdx <= endF; ++fIdx) {
                    const auto& kfTest = track.keyframes[fIdx];
                    if (kfTest.x != firstKf.x || kfTest.y != firstKf.y ||
                        kfTest.kx != firstKf.kx || kfTest.ky != firstKf.ky ||
                        kfTest.sx != firstKf.sx || kfTest.sy != firstKf.sy ||
                        kfTest.f != firstKf.f || kfTest.imageName != firstKf.imageName) {
                        overlayIsConstant = false;
                        break;
                    }
                }
            }
        }

        // Try overlay (current animation) frame if this track is animated by overlay
        if (!overlayIsConstant && currentFrame >= 0 && currentFrame < (int)track.keyframes.size()) {
            if (track.keyframes[currentFrame].f != -1) {
                kf_holder = GetInterpolatedKeyframe(track, m_currentFrameFloat, m_currentAnimIndex);
                if (kf_holder.f != -1) {
                    hasKeyframe = true;
                    usedOverlayTrack = true;
                    // Resolve image name: if this frame has one, use it;
                    // otherwise search backwards for the most recent image name
                    if (!track.keyframes[currentFrame].imageName.empty()) {
                        resolvedImageName = track.keyframes[currentFrame].imageName;
                    } else {
                        for (int j = currentFrame - 1; j >= 0; --j) {
                            if (!track.keyframes[j].imageName.empty()) {
                                resolvedImageName = track.keyframes[j].imageName;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Fallback to base animation if overlay frame is hidden or overlay track is un-animated
        if ((!hasKeyframe || resolvedImageName.empty()) && m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size()) {
            int baseFrame = (int)m_baseFrameFloat;
            if (baseFrame >= 0 && baseFrame < (int)track.keyframes.size()) {
                if (track.keyframes[baseFrame].f != -1) {
                    kf_holder = GetInterpolatedKeyframe(track, m_baseFrameFloat, m_baseAnimIndex);
                    if (kf_holder.f != -1) {
                        hasKeyframe = true;
                        usedOverlayTrack = false;
                        if (resolvedImageName.empty()) {
                            if (!track.keyframes[baseFrame].imageName.empty()) {
                                resolvedImageName = track.keyframes[baseFrame].imageName;
                            } else {
                                for (int j = baseFrame - 1; j >= 0; --j) {
                                    if (!track.keyframes[j].imageName.empty()) {
                                        resolvedImageName = track.keyframes[j].imageName;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!hasKeyframe || resolvedImageName.empty()) {
            continue;
        }

        // Apply track image override if any
        auto imgOverrideIt = m_trackImageOverrides.find(track.name);
        if (imgOverrideIt != m_trackImageOverrides.end()) {
            resolvedImageName = imgOverrideIt->second;
        }

        ReanimKeyframe kf = kf_holder;
        if (m_currentAnimIndex != m_baseAnimIndex && !overlayIsConstant && usedOverlayTrack) {
            kf.x += stem_offset_x;
            kf.y += stem_offset_y;
        }

        Texture2D tex = m_resources->GetTexture(resolvedImageName);
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
        m_currentFrameFloat = GetLoopStartTime(index);
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

void Reanimation::OverrideTrackImage(const std::string& trackName, const std::string& imageName) {
    m_trackImageOverrides[trackName] = imageName;
}

void Reanimation::ClearTrackImageOverride(const std::string& trackName) {
    m_trackImageOverrides.erase(trackName);
}

void Reanimation::AddCustomAnimation(const std::string& newAnimName, const std::string& baseAnimName) {
    for (const auto& anim : m_anims) {
        if (anim.name == baseAnimName) {
            m_anims.push_back({newAnimName, anim.startFrame, anim.endFrame});
            return;
        }
    }
}

float Reanimation::GetLoopStartTime(int animIndex) const {
    if (animIndex < 0 || animIndex >= (int)m_anims.size()) return 0.0f;
    
    int startFrame = m_anims[animIndex].startFrame;
    int endFrame = m_anims[animIndex].endFrame;

    std::map<int, int> snapCount;
    for (const auto& track : m_def.tracks) {
        for (int i = startFrame; i <= endFrame && i < (int)track.keyframes.size(); ++i) {
            if (track.keyframes[i].f != -1) {
                snapCount[i]++;
                break; // only take the first visible frame of each track
            }
        }
    }

    if (snapCount.empty()) return (float)startFrame;

    int bestSnap = startFrame;
    int bestCount = 0;
    for (const auto& [snap, count] : snapCount) {
        if (count > bestCount) {
            bestCount = count;
            bestSnap = snap;
        }
    }
    return (float)bestSnap;
}

Rectangle Reanimation::GetTrackBounds(const std::string& trackName, float x, float y, float scale) const {
    int currentFrame = GetCurrentFrame();

    for (const auto& track : m_def.tracks) {
        if (track.name != trackName) continue;

        // Check track visibility
        auto visIt = m_trackVisibility.find(track.name);
        if (visIt != m_trackVisibility.end() && !visIt->second) {
            return {0, 0, 0, 0};
        }

        if (currentFrame < 0 || currentFrame >= (int)track.keyframes.size()) {
            return {0, 0, 0, 0};
        }

        ReanimKeyframe kf_holder;
        bool hasKeyframe = false;
        std::string resolvedImageName;

        // Try current frame
        if (currentFrame >= 0 && currentFrame < (int)track.keyframes.size() && track.keyframes[currentFrame].f != -1) {
            kf_holder = GetInterpolatedKeyframe(track, m_currentFrameFloat, m_currentAnimIndex);
            if (kf_holder.f != -1) {
                hasKeyframe = true;
                if (!track.keyframes[currentFrame].imageName.empty()) {
                    resolvedImageName = track.keyframes[currentFrame].imageName;
                } else {
                    for (int j = currentFrame - 1; j >= 0; --j) {
                        if (!track.keyframes[j].imageName.empty()) {
                            resolvedImageName = track.keyframes[j].imageName;
                            break;
                        }
                    }
                }
            }
        }

        // Fallback to base animation
        if ((!hasKeyframe || resolvedImageName.empty()) && m_baseAnimIndex >= 0 && m_baseAnimIndex < (int)m_anims.size()) {
            int baseFrame = (int)m_baseFrameFloat;
            if (baseFrame >= 0 && baseFrame < (int)track.keyframes.size() && track.keyframes[baseFrame].f != -1) {
                kf_holder = GetInterpolatedKeyframe(track, m_baseFrameFloat, m_baseAnimIndex);
                if (kf_holder.f != -1) {
                    hasKeyframe = true;
                    if (resolvedImageName.empty()) {
                        if (!track.keyframes[baseFrame].imageName.empty()) {
                            resolvedImageName = track.keyframes[baseFrame].imageName;
                        } else {
                            for (int j = baseFrame - 1; j >= 0; --j) {
                                if (!track.keyframes[j].imageName.empty()) {
                                    resolvedImageName = track.keyframes[j].imageName;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!hasKeyframe || resolvedImageName.empty()) {
            return {0, 0, 0, 0};
        }

        // Apply track image override if any
        auto imgOverrideIt = m_trackImageOverrides.find(track.name);
        if (imgOverrideIt != m_trackImageOverrides.end()) {
            resolvedImageName = imgOverrideIt->second;
        }

        ReanimKeyframe kf = kf_holder;

        Texture2D tex = m_resources->GetTexture(resolvedImageName);
        if (tex.id == 0) {
            return {0, 0, 0, 0};
        }

        // Compute transform (same as Draw)
        float radX = kf.kx * DEG2RAD;
        float radY = kf.ky * DEG2RAD;

        float m00 = kf.sx * cosf(radX) * scale;
        float m10 = kf.sx * sinf(radX) * scale;
        float m01 = -kf.sy * sinf(radY) * scale;
        float m11 = kf.sy * cosf(radY) * scale;

        float tx = x + kf.x * scale;
        float ty = y + kf.y * scale;

        // Transform the four corners of the texture
        float w = (float)tex.width;
        float h = (float)tex.height;

        float corners[4][2] = {
            { tx,                    ty },                     // (0,0)
            { tx + m00 * w,          ty + m10 * w },           // (w,0)
            { tx + m01 * h,          ty + m11 * h },           // (0,h)
            { tx + m00 * w + m01 * h, ty + m10 * w + m11 * h } // (w,h)
        };

        // Compute axis-aligned bounding box
        float minX = corners[0][0], maxX = corners[0][0];
        float minY = corners[0][1], maxY = corners[0][1];
        for (int i = 1; i < 4; ++i) {
            if (corners[i][0] < minX) minX = corners[i][0];
            if (corners[i][0] > maxX) maxX = corners[i][0];
            if (corners[i][1] < minY) minY = corners[i][1];
            if (corners[i][1] > maxY) maxY = corners[i][1];
        }

        return { minX, minY, maxX - minX, maxY - minY };
    }

    return {0, 0, 0, 0};  // Track not found
}

