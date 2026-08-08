#include "PotatoMine.h"
#include "AudioManager.h"

PotatoMine::PotatoMine(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 25, "PotatoMine") {
    getResources(res.GetAssetPath("assets/reanim/PotatoMine.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle"); // Unarmed/buried state
}

PotatoMine::~PotatoMine() {
}

void PotatoMine::triggerExplode() {
    if (m_state == PotatoMineState::ARMED) {
        m_state = PotatoMineState::EXPLODING;
        m_anim.SetAnimation("anim_mashed");
        m_explodeTimer = 0.0f;
    }
}

void PotatoMine::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    switch (m_state) {
        case PotatoMineState::UNARMED:
            m_armTimer += deltaTime;
            if (m_armTimer >= ARM_DELAY) {
                m_state = PotatoMineState::RISING;
                m_anim.SetAnimation("anim_rise");
            }
            break;

        case PotatoMineState::RISING:
            if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
                m_state = PotatoMineState::ARMED;
                m_anim.SetAnimation("anim_armed");
            }
            break;

        case PotatoMineState::ARMED:
            // Waiting for zombie to trigger — handled by Level1::updateCollisions
            break;

        case PotatoMineState::EXPLODING:
            m_explodeTimer += deltaTime;
            if (m_explodeTimer >= 1.4f) {
                m_state = PotatoMineState::DONE;
                takeDamage(m_hp); // Mark as dead after full anim_mashed sequence plays
            }
            break;

        case PotatoMineState::DONE:
            break;
    }
}

void PotatoMine::draw() {
    if (m_state != PotatoMineState::DONE) {
        m_anim.Draw(m_x, m_y, 1.0f);
    }
}

bool PotatoMine::isDead() const {
    return m_state == PotatoMineState::DONE;
}
