#include "Melonpult.h"
#include "AudioManager.h"

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

Melonpult::Melonpult(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 300, "Melonpult") {
    // Load reanim for Melonpult
    getResources(res.GetAssetPath("assets/reanim/Melonpult.reanim"));
}

Melonpult::~Melonpult() {
}

void Melonpult::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    int currentFrame = m_anim.GetCurrentFrame();
    bool isShooting = (m_anim.GetCurrentAnimName() == "anim_shooting");

    if (isShooting) {
        if (currentFrame >= 49) {
            // Hide the melon in the basket after launch
            m_anim.SetTrackVisible("Melonpult_mellon", false);
        } else {
            // Show before launch
            m_anim.SetTrackVisible("Melonpult_mellon", true);
        }
    } else {
        // Show by default in other animations (like idle)
        m_anim.SetTrackVisible("Melonpult_mellon", true);
        if (m_anim.GetCurrentAnimName() != "anim_idle"){
            m_anim.SetAnimation("anim_idle");
        }
    }

    // Shoot melon when shooting animation reaches the launch frame (frame 49)
    if (isShooting && currentFrame == 49 && did_shoot == false) {
        Texture2D tex = res.GetTexture("Melonpult_melon");
        const float travelDistance = std::max(0.0f, distance);
        const float launchSpeed = std::max(300.0f, ComputeLobbedArcLength(travelDistance, 150.0f)) * 0.5f;

        // Adjust spawn position
        Projectile melonProj(m_x + 90, m_y - 60, launchSpeed, tex, false, true, 1.0f, 80, &res, travelDistance); // Parabolic trajectory
        melonProj.setMelon(true);
        outProjectiles.push_back(melonProj);
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/throw.ogg"));
        did_shoot = true;
    }

    if (currentFrame == 50) {
        did_shoot = false;
    }
}

void Melonpult::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
