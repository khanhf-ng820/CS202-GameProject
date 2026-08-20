#include "Wallnut.h"

Wallnut::Wallnut(Resources& res, int x, int y)
    : Plant(res, x, y, 4000, 50, "Wallnut") {
    // Load reanim for Wallnut
    getResources(res.GetAssetPath("assets/reanim/Wallnut.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Wallnut::~Wallnut() {
}

void Wallnut::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    // Wallnut Max HP = 4000
    // Stage 1 (Healthy): HP > 2666 (2/3) -> Normal texture
    // Stage 2 (Cracked 1): 1333 < HP <= 2666 (1/3 to 2/3) -> Minor cracks
    // Stage 3 (Cracked 2): HP <= 1333 (<= 1/3) -> Heavy cracks
    if (m_hp <= 1333) {
        m_anim.OverrideTrackImage("anim_face", "WALLNUT_CRACKED2");
    } else if (m_hp <= 2666) {
        m_anim.OverrideTrackImage("anim_face", "WALLNUT_CRACKED1");
    } else {
        m_anim.ClearTrackImageOverride("anim_face");
    }
}

void Wallnut::draw() {
    m_anim.Draw(m_x, m_y, 1.0f); 
}

