#include "Cornpult.h"
#include "AudioManager.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace {
float ComputeLobbedArcLength(float range, float maxHeight) {
    if (range <= 0.0f) {
        return 0.0f;
    }

    constexpr int kSegments = 64;
    constexpr float kPi = 3.14159265f;
    const float step = range / static_cast<float>(kSegments);
    float length = 0.0f;

    for (int i = 0; i < kSegments; ++i) {
        const float x0 = step * static_cast<float>(i);
        const float x1 = x0 + step;
        const float xm = (x0 + x1) * 0.5f;

        auto arcFactor = [range, maxHeight, kPi](float x) {
            const float slope = -(maxHeight * kPi / range) * cosf(kPi * x / range);
            return sqrtf(1.0f + slope * slope);
        };

        length += (arcFactor(x0) + 4.0f * arcFactor(xm) + arcFactor(x1)) * step / 6.0f;
    }

    return length;
}
}  // namespace

Cornpult::Cornpult(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 100, "Cornpult") {
    // Load reanim for Cornpult
    getResources(res.GetAssetPath("assets/reanim/Cornpult.reanim"));
}

Cornpult::~Cornpult() {
}

void Cornpult::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    // Update animation
    m_anim.Update(deltaTime);

    int currentFrame = m_anim.GetCurrentFrame();
    bool isShooting = (m_anim.GetCurrentAnimName() == "anim_shooting");

    if (isShooting) {
        if (currentFrame >= 36) {
            // Hide the kernel and butter in the basket after launch
            m_anim.SetTrackVisible("Cornpult_kernal", false);
            m_anim.SetTrackVisible("Cornpult_butter", false);
        } else {
            // Show them before launch
            m_anim.SetTrackVisible("Cornpult_kernal", true);
            m_anim.SetTrackVisible("Cornpult_butter", true);
        }
    } else {
        // Show by default in other animations (like idle)
        m_anim.SetTrackVisible("Cornpult_kernal", true);
        m_anim.SetTrackVisible("Cornpult_butter", true);
    }

    // Shoot kernel or butter when shooting animation reaches the launch frame (frame 36)
    if (isShooting && currentFrame == 36 && did_shoot == false) {
        bool isButter = (rand() % 4 == 0); 
        Texture2D tex = isButter ? res.GetTexture("Cornpult_butter") : res.GetTexture("Cornpult_kernal");
        if (tex.id == 0) tex = res.GetTexture("Cornpult_butter");

        const float travelDistance = std::max(0.0f, distance);
        const float launchSpeed = std::max(300.0f, ComputeLobbedArcLength(travelDistance, 150.0f)) * 0.5f;
        int dmg = isButter ? 40 : 20;

        Projectile cornProj(m_x + 90, m_y - 60, launchSpeed, tex, isButter, true, 1.0f, dmg, &res, travelDistance);
        cornProj.setButter(isButter);
        outProjectiles.push_back(cornProj);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath(isButter ? "assets/sounds/kernelpult2.ogg" : "assets/sounds/kernelpult.ogg"));
        did_shoot = true;
    }

    if (currentFrame == 37) {
        did_shoot = false;
    }
}

void Cornpult::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
