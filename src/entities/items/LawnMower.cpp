#include "LawnMower.h"
#include "AudioManager.h"

LawnMower::LawnMower(Resources& res, float x, float y, int row)
    : m_x(x), m_y(y), m_row(row), m_isTriggered(false), m_isFinished(false), m_speed(420.0f) {
    m_anim.SetResources(res.LoadReanim(res.GetAssetPath("assets/reanim/LawnMower.reanim")), res);
    m_anim.SetBaseAnimation("anim_normal");
    m_anim.SetAnimation("anim_normal");
    m_anim.SetPaused(true);
}

void LawnMower::trigger() {
    if (!m_isTriggered && !m_isFinished) {
        m_isTriggered = true;
        m_anim.SetPaused(false);
        Resources& res = Resources::GetInstance();
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/lawnmower.ogg"));
    }
}

void LawnMower::update(float dt) {
    if (m_isFinished) return;

    if (m_isTriggered) {
        m_anim.Update(dt);
        m_x += m_speed * dt;
        if (m_x > 850.0f) {
            m_isFinished = true;
        }
    }
}

void LawnMower::draw() const {
    if (!m_isFinished) {
        m_anim.Draw(m_x, m_y, 1.0f);
    }
}
