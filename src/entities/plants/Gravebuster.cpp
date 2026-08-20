#include "Gravebuster.h"
#include "AudioManager.h"

Gravebuster::Gravebuster(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 75, "Gravebuster"),
      m_hasLanded(false), m_eatTimer(0.0f), m_eatDuration(4.5f),
      m_chompSoundTimer(0.0f), m_finished(false) {
    getResources(res.GetAssetPath("assets/reanim/Gravebuster.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_land");
}

Gravebuster::~Gravebuster() {
}

void Gravebuster::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    // Transition from landing to idle chewing animation
    if (!m_hasLanded) {
        if (m_anim.GetCurrentAnimName() == "anim_land") {
            if (m_anim.GetCurrentFrame() >= m_anim.GetEndFrame() - 1) {
                m_anim.SetAnimation("anim_idle");
                m_hasLanded = true;
            }
        } else {
            m_hasLanded = true;
        }
    }

    m_eatTimer += deltaTime;
    m_chompSoundTimer += deltaTime;
    if (m_chompSoundTimer >= 0.45f) {
        m_chompSoundTimer = 0.0f;
        AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/gravebusterchomp.ogg"));
    }

    if (m_eatTimer >= m_eatDuration) {
        m_finished = true;
        m_hp = 0; // Mark dead so it despawns after eating
    }
}

void Gravebuster::draw() {
    float eatProgress = getEatProgress();
    // Gravebuster starts high above the tombstone and slides down +20px until its peak aligns with the tombstone peak
    float chewDownY = (float)m_y + eatProgress * 20.0f;
    m_anim.Draw((float)m_x, chewDownY, 1.15f);
}
