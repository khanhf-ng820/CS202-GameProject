#include "Caltrop.h"

Caltrop::Caltrop(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 100, "Caltrop") {
    getResources(res.GetAssetPath("assets/reanim/Caltrop.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Caltrop::~Caltrop() {
}

void Caltrop::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    // Switch between idle and attack animation based on whether a zombie is on top
    std::string currentAnim = m_anim.GetCurrentAnimName();
    if (m_isAttacking && currentAnim != "anim_attack") {
        m_anim.SetAnimation("anim_attack");
    } else if (!m_isAttacking && currentAnim == "anim_attack") {
        m_anim.SetAnimation("anim_idle");
    }
}

void Caltrop::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
