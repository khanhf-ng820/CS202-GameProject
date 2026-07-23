#include "Jalapeno.h"

Jalapeno::Jalapeno(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 125, "Jalapeno") {
    getResources(res.GetAssetPath("assets/reanim/Jalapeno.reanim"));
    m_anim.SetAnimation("anim_explode");
    
    // Load the fire reanim
    m_fireAnim.SetResources(res.LoadReanim(res.GetAssetPath("assets/reanim/fire.reanim")), res);
    m_fireAnim.SetAnimation("anim_flame");
}

Jalapeno::~Jalapeno() {
}

void Jalapeno::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    if (m_state == JalapenoState::EXPLODING_SWELL) {
        m_anim.Update(deltaTime);
        if (m_anim.GetCurrentAnimName() == "anim_explode" && m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
            m_state = JalapenoState::EXPLODING_FIRE;
            m_fireAnim.SetAnimation("anim_flame");
            m_fireDurationTimer = 0.0f;
        }
    } else if (m_state == JalapenoState::EXPLODING_FIRE) {
        m_fireAnim.Update(deltaTime);
        m_fireDurationTimer += deltaTime;
        if (m_fireDurationTimer >= 2.0f) { // Keep burning for 2.0 seconds
            m_state = JalapenoState::DONE;
            takeDamage(m_hp); // Mark as dead
        }
    }
}

void Jalapeno::draw() {
    if (m_state == JalapenoState::EXPLODING_SWELL) {
        m_anim.Draw(m_x, m_y, 1.0f);
    } else if (m_state == JalapenoState::EXPLODING_FIRE) {
        // Draw the fire wave spanning across the screen row (X from 0 to 1280)
        for (int i = 0; i < 12; ++i) {
            m_fireAnim.Draw(i * 110 - 20, m_y - 45, 1.6f);
        }
    }
}

bool Jalapeno::isDead() const {
    return m_state == JalapenoState::DONE;
}
