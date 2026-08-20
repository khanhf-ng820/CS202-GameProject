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
        m_anim.SetBaseAnimation("anim_mashed");
        m_anim.SetAnimation("anim_mashed");
        m_explodeTimer = 0.0f;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/potato_mine.ogg"));
    }
}

void PotatoMine::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    switch (m_state) {
        case PotatoMineState::UNARMED:
            m_armTimer += deltaTime;
            if (m_armTimer >= ARM_DELAY) {
                m_state = PotatoMineState::RISING;
                m_anim.SetBaseAnimation("anim_rise");
                m_anim.SetAnimation("anim_rise");
            }
            break;

        case PotatoMineState::RISING:
            if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
                m_state = PotatoMineState::ARMED;
                m_anim.SetBaseAnimation("anim_armed");
                m_anim.SetAnimation("anim_armed");
            }
            break;

        case PotatoMineState::ARMED:
            // Waiting for zombie to trigger — handled by Level1::updateCollisions
            break;

        case PotatoMineState::EXPLODING:
            m_explodeTimer += deltaTime;
            if (m_explodeTimer >= 1.0f) {
                m_state = PotatoMineState::DONE;
                takeDamage(m_hp); // Mark as dead after full anim_mashed sequence plays
            }
            break;

        case PotatoMineState::DONE:
            break;
    }
}

void PotatoMine::draw() {
    if (m_state == PotatoMineState::EXPLODING) {
        Texture2D mashedTex = res.GetTexture("PotatoMine_mashed");
        if (mashedTex.id != 0) {
            float progress = m_explodeTimer / 1.0f; // 0.0 -> 1.0
            if (progress > 1.0f) progress = 1.0f;

            unsigned char alpha = 255;
            if (progress > 0.7f) {
                alpha = (unsigned char)(255 * (1.0f - progress) / 0.3f);
            }
            Color tint = { 255, 255, 255, alpha };

            // Draw mashed potato "SPUDOW!" with scale 0.8f
            DrawTextureEx(mashedTex, Vector2{ (float)m_x + 6.0f, (float)m_y + 24.0f }, 0.0f, 0.8f, tint);
        }
    } else if (m_state != PotatoMineState::DONE) {
        m_anim.Draw(m_x, m_y, 0.8f);
    }
}

bool PotatoMine::isDead() const {
    return m_state == PotatoMineState::DONE;
}
