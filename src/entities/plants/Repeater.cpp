#include "Repeater.h"

Repeater::Repeater(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 200, "Repeater") {
    // Repeater reuses PeaShooter.reanim visually but shoots 2 peas
    getResources(res.GetAssetPath("assets/reanim/PeaShooter.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_head_idle");
}

Repeater::~Repeater() {
}

void Repeater::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    if (currentAnim == "anim_shooting" && m_anim.GetCurrentFrame() == 65 && !did_shoot) {
        // Bắn đạn
        Texture2D tex = res.GetTexture("ProjectilePea"); 
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 15, 400.0f, tex));
        outProjectiles.push_back(Projectile(m_x + 15, m_y + 15, 400.0f, tex));
        did_shoot = 1;
    }

    if (m_anim.GetCurrentFrame() == 66)
        did_shoot = 0;
}

void Repeater::draw() {
    m_anim.Draw(m_x, m_y, 1.0f); 
}
