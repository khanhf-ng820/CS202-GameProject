#include "Gravebuster.h"

Gravebuster::Gravebuster(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 75, "Gravebuster") {
    getResources(res.GetAssetPath("assets/reanim/Gravebuster.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_land");
}

Gravebuster::~Gravebuster() {
}

void Gravebuster::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    // Transition from landing to idle animation
    if (!m_hasLanded && m_anim.GetCurrentAnimName() == "anim_land") {
        if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
            m_anim.SetAnimation("anim_idle");
            m_hasLanded = true;
        }
    }
    // Grave-eating functionality to be added when grave system is implemented
}

void Gravebuster::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
