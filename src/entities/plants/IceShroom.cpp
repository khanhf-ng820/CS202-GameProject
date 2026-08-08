#include "IceShroom.h"

IceShroom::IceShroom(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 75, "IceShroom") {
    getResources(res.GetAssetPath("assets/reanim/IceShroom.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

IceShroom::~IceShroom() {
}

void IceShroom::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    if (m_state == IceShroomState::IDLE) {
        m_idleTimer += deltaTime;
        // Auto-activate after a brief delay (like CherryBomb's swell)
        if (m_idleTimer >= 0.8f) {
            m_state = IceShroomState::FREEZING;
            m_freezeEffectTimer = 0.0f;
        }
    } else if (m_state == IceShroomState::FREEZING) {
        m_freezeEffectTimer += deltaTime;
        if (m_freezeEffectTimer >= 1.0f) {
            m_state = IceShroomState::DONE;
            takeDamage(m_hp); // Mark as dead
        }
    }
}

void IceShroom::draw() {
    if (m_state == IceShroomState::FREEZING) {
        // Draw with a blue tint during freeze effect
        m_anim.Draw(m_x, m_y, 1.0f);

        // Draw freeze overlay effect expanding outward
        float progress = m_freezeEffectTimer / 1.0f;
        unsigned char alpha = (unsigned char)(180 * (1.0f - progress));
        DrawCircle(m_x + 40, m_y + 40, 400.0f * progress, ColorAlpha(SKYBLUE, alpha / 255.0f));
    } else if (m_state != IceShroomState::DONE) {
        m_anim.Draw(m_x, m_y, 1.0f);
    }
}

bool IceShroom::isDead() const {
    return m_state == IceShroomState::DONE;
}
