#include "Cornpult.h"
#include <cstdlib> // For rand()

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
        // PVZ Cornpult stuns with butter (25% chance) or shoots normal kernel (75% chance)
        bool isButter = (rand() % 4 == 0); 
        Texture2D tex;
        tex = res.GetTexture("Cornpult_butter");

        // Adjust spawn position to the basket location of Cornpult (x + 90, y - 60)
        outProjectiles.push_back(Projectile(m_x + 90, m_y - 60, 400.0f, tex, isButter, true));
        did_shoot = true;
    }

    if (currentFrame == 37) {
        did_shoot = false;
    }
}

void Cornpult::draw() {
    // Draw animation with appropriate scale (1.0f matches PeaShooter)
    m_anim.Draw(m_x, m_y, 1.0f);
}
