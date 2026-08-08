#include "SpikeRock.h"

SpikeRock::SpikeRock(Resources& res, int x, int y)
    : Plant(res, x, y, 450, 125, "SpikeRock") {
    getResources(res.GetAssetPath("assets/reanim/SpikeRock.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

SpikeRock::~SpikeRock() {
}

void SpikeRock::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();
    if (m_isAttacking && currentAnim != "anim_attack") {
        m_anim.SetAnimation("anim_attack");
    } else if (!m_isAttacking && currentAnim == "anim_attack") {
        m_anim.SetAnimation("anim_idle");
    }
}

void SpikeRock::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
