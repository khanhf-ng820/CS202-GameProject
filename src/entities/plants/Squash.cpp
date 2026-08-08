#include "Squash.h"
#include <cmath>

Squash::Squash(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 50, "Squash") {
    getResources(res.GetAssetPath("assets/reanim/Squash.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
    m_startX = (float)x;
}

Squash::~Squash() {
}

void Squash::setTargetZombie(float x, float y) {
    if (m_state == SquashState::IDLE) {
        m_targetX = x;
        m_targetY = y;
        m_state = SquashState::AIMING;
        m_aimTimer = 0.0f;
        m_anim.SetAnimation("anim_lookright");
    }
}

void Squash::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    switch (m_state) {
        case SquashState::IDLE:
            // Waiting for Level1 to call setTargetZombie()
            break;

        case SquashState::AIMING:
            m_aimTimer += deltaTime;
            if (m_aimTimer >= 0.4f) {
                m_state = SquashState::JUMPING;
                m_jumpTimer = 0.0f;
                m_anim.SetAnimation("anim_jumpup");
            }
            break;

        case SquashState::JUMPING: {
            m_jumpTimer += deltaTime;
            float jumpDuration = 0.5f;
            float t = m_jumpTimer / jumpDuration;
            if (t >= 1.0f) t = 1.0f;

            // Lerp X position toward target
            m_x = (int)(m_startX + (m_targetX - m_startX) * t);

            // Parabolic Y offset for jump arc (jump up then come down)
            m_jumpHeight = -200.0f * 4.0f * t * (1.0f - t); // peaks at -200px at t=0.5

            if (t >= 1.0f) {
                m_state = SquashState::SQUASHING;
                m_squashTimer = 0.0f;
                m_jumpHeight = 0.0f;
                m_anim.SetAnimation("anim_jumpdown");
            }
            break;
        }

        case SquashState::SQUASHING:
            m_squashTimer += deltaTime;
            if (m_squashTimer >= 0.5f) {
                m_state = SquashState::DONE;
                takeDamage(m_hp); // Squash dies after squashing
            }
            break;

        case SquashState::DONE:
            break;
    }
}

void Squash::draw() {
    if (m_state != SquashState::DONE) {
        m_anim.Draw(m_x, m_y + (int)m_jumpHeight, 1.0f);
    }
}

bool Squash::isDead() const {
    return m_state == SquashState::DONE;
}
