#include "GatlingPea.h"

GatlingPea::GatlingPea(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 250, "GatlingPea") {
    getResources(res.GetAssetPath("assets/reanim/GatlingPea.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_head_idle");
}

GatlingPea::~GatlingPea() {
}

void GatlingPea::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    // Nếu người dùng bấm "anim_idle" trên UI, tự động chuyển sang "anim_head_idle"
    // vì anim_idle trong file reanim chỉ chứa phần thân, không có đầu.
    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    if (currentAnim == "anim_shooting") {
        if (m_anim.GetCurrentFrame() == 59 && did_shoot == false) {
            Texture2D tex = res.GetTexture("ProjectilePea"); 
            outProjectiles.push_back(Projectile(m_x + 60, m_y + 35, 400.0f, tex));
            outProjectiles.push_back(Projectile(m_x + 15, m_y + 35, 400.0f, tex));
            outProjectiles.push_back(Projectile(m_x - 30, m_y + 35, 400.0f, tex));
            outProjectiles.push_back(Projectile(m_x - 75, m_y + 35, 400.0f, tex));
            did_shoot = true;
        }

        if (m_anim.GetCurrentFrame() == 60) {
            did_shoot = false;
        }
    }
}

void GatlingPea::draw() {
    m_anim.Draw(m_x, m_y, 1.6f); 
}
