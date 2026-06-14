#include "Cornpult.h"
#include <cstdlib> // For rand()

Cornpult::Cornpult(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 100, "Cornpult") {
    // Load reanim for Cornpult
    getResources(res.GetAssetPath("assets/reanim/Cornpult.reanim"));
}

Cornpult::~Cornpult() {
}

void Cornpult::update(float deltaTime, std::vector<Projectile>& outProjectiles) {
    // Update animation
    m_anim.Update(deltaTime);

    // Shoot kernel or butter when shooting animation reaches the launch frame (index 205 or later)
    if (m_anim.GetCurrentFrame() == 36 && did_shoot == false) {
        // PVZ Cornpult stuns with butter (25% chance) or shoots normal kernel (75% chance)
        bool isButter = (rand() % 4 == 0); 
        Texture2D tex;
        tex = res.GetTexture("Cornpult_butter");

        // Adjust spawn position to the basket location of Cornpult (x + 70, y + 20)
        outProjectiles.push_back(Projectile(m_x + 90, m_y - 60, 400.0f, tex, isButter, true));
        //std::cout << "Cornpult shoots a " << (isButter ? "butter" : "kernel") << "!" << std::endl;
        did_shoot = true;

        // Reset did_shoot when the animation loops back to the start (before frame 205)
    }

    //std::cout << "Cornpult Animation Frame: " << m_anim.GetCurrentFrame() << std::endl;

    if (m_anim.GetCurrentFrame() == 37) {
        did_shoot = false;
    }
}

void Cornpult::draw() {
    // Draw animation with appropriate scale (1.6f matches SnowPea and PeaShooter)
    m_anim.Draw(m_x, m_y, 1.6f);
}
