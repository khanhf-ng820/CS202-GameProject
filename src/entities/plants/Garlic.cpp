#include "Garlic.h"

Garlic::Garlic(Resources& res, int x, int y)
    : Plant(res, x, y, 400, 50, "Garlic") {
    getResources(res.GetAssetPath("assets/reanim/Garlic.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Garlic::~Garlic() {
}

void Garlic::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    if (m_redirectCooldown > 0.0f) {
        m_redirectCooldown -= deltaTime;
    }

    // Show cracked body states based on remaining HP
    float hpRatio = m_hp / m_maxHp;
    if (hpRatio <= 0.33f) {
        m_anim.OverrideTrackImage("Garlic_body1", "Garlic_body3");
    } else if (hpRatio <= 0.66f) {
        m_anim.OverrideTrackImage("Garlic_body1", "Garlic_body2");
    } else {
        m_anim.ClearTrackImageOverride("Garlic_body1");
    }
}

void Garlic::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
