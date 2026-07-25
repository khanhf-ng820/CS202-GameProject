#include "Wallnut.h"

Wallnut::Wallnut(Resources& res, int x, int y)
    : Plant(res, x, y, 4000, 50, "Wallnut") {
    // Load reanim for Wallnut
    getResources(res.GetAssetPath("assets/reanim/Wallnut.reanim"));
    
    // Add custom cracked pseudo-animations based on anim_idle
    m_anim.AddCustomAnimation("anim_cracked1", "anim_idle");
    m_anim.AddCustomAnimation("anim_cracked2", "anim_idle");

    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Wallnut::~Wallnut() {
}

void Wallnut::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();
    if (currentAnim == "anim_cracked1") {
        m_anim.OverrideTrackImage("Wallnut_body", "Wallnut_cracked1");
    } else if (currentAnim == "anim_cracked2") {
        m_anim.OverrideTrackImage("Wallnut_body", "Wallnut_cracked2");
    } else {
        m_anim.ClearTrackImageOverride("Wallnut_body");
    }
}

void Wallnut::draw() {
    // Vẽ animation tại vị trí của cây
    m_anim.Draw(m_x, m_y, 1.0f); 
}
