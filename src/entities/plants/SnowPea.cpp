#include "SnowPea.h"

SnowPea::SnowPea(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 175, "SnowPea") {
    // Nạp reanim cho SnowPea
    getResources(res.GetAssetPath("assets/reanim/SnowPea.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_head_idle");
    m_fireRate = 1.5f; // Thời gian hồi đạn chuẩn
    m_fireTimer = 0.0f;
}

SnowPea::~SnowPea() {
}

void SnowPea::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    // Cập nhật animation
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    if (currentAnim == "anim_shooting" && m_anim.GetCurrentFrame() == 65 && !did_shoot) {
        // Bắn đạn
        Texture2D tex = res.GetTexture("ProjectileSnowPea"); 
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 15, 400.0f, tex, true));
        did_shoot = 1;
    }

    if (m_anim.GetCurrentFrame() == 66)
        did_shoot = 0;
}

void SnowPea::draw() {
    // Scale 1.0f cho phù hợp màn hình
    m_anim.Draw(m_x, m_y, 1.0f);
}
