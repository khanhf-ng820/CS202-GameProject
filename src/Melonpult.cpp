#include "Melonpult.h"

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
    }

    // Shoot melon when shooting animation reaches the launch frame (frame 49)
    if (isShooting && currentFrame == 49 && did_shoot == false) {
        Texture2D tex = res.GetTexture("Melonpult_melon");

        // Adjust spawn position
        outProjectiles.push_back(Projectile(m_x + 90, m_y - 60, 400.0f, tex, false, true, 1.4f)); // Parabolic trajectory
        did_shoot = true;
    }

    if (currentFrame == 50) {
        did_shoot = false;
    }
}

void Melonpult::draw() {
    m_anim.Draw(m_x, m_y, 1.6f);
}
