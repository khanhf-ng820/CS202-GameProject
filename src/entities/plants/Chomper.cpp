#include "Chomper.h"
#include "Zombie.h"
#include "AudioManager.h"

Chomper::Chomper(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 150, "Chomper") {
    getResources(res.GetAssetPath("assets/reanim/Chomper.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_idle");
}

Chomper::~Chomper() {
}

void Chomper::devourZombie(Zombie* z) {
    if (m_state == ChomperState::IDLE) {
        m_state = ChomperState::BITING;
        m_targetZombie = z;
        m_stateTimer = 0.0f;
        m_anim.SetAnimation("anim_bite");
    }
}

void Chomper::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);
    m_stateTimer += deltaTime;

    switch (m_state) {
        case ChomperState::IDLE:
            if (m_anim.GetCurrentAnimName() != "anim_idle") {
                m_anim.SetAnimation("anim_idle");
            }
            break;

        case ChomperState::BITING:
            if (m_anim.GetCurrentAnimName() != "anim_bite") {
                m_anim.SetAnimation("anim_bite");
            }
            // Devour target zombie right at the 2nd second of bite animation when mouth closes
            // Devour target zombie right at the moment of bite animation when mouth snaps shut
            if (m_stateTimer >= 1.5f && m_targetZombie && !m_targetZombie->isDevoured()) {
                m_targetZombie->devour();
                m_targetZombie = nullptr;
                AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/bigchomp.ogg"));
            }
            if (m_stateTimer >= 2.0f) {
                m_state = ChomperState::CHEWING;
                m_stateTimer = 0.0f;
                m_anim.SetAnimation("anim_chew");
            }
            break;

        case ChomperState::CHEWING:
            if (m_anim.GetCurrentAnimName() != "anim_chew") {
                m_anim.SetAnimation("anim_chew");
            }
            if (m_stateTimer >= CHEW_DURATION) {
                m_state = ChomperState::SWALLOWING;
                m_stateTimer = 0.0f;
                m_anim.SetAnimation("anim_swallow");
            }
            break;

        case ChomperState::SWALLOWING:
            if (m_anim.GetCurrentAnimName() != "anim_swallow") {
                m_anim.SetAnimation("anim_swallow");
            }
            if (m_stateTimer >= 2.3f) {
                m_state = ChomperState::IDLE;
                m_stateTimer = 0.0f;
                m_anim.SetAnimation("anim_idle");
            }
            break;
    }
}

void Chomper::draw() {
    m_anim.Draw(m_x, m_y, 1.0f);
}
