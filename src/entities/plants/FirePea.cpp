#include "FirePea.h"

FirePea::FirePea(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 175, "FirePea") {
    // FirePea uses the PeaShooter reanim for its body structure, but shoots flaming peas
    getResources(res.GetAssetPath("assets/reanim/PeaShooter.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_head_idle");
}

FirePea::~FirePea() {
}

void FirePea::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    // Shoot a fire pea when the shooting animation reaches the launch frame
    if (currentAnim == "anim_shooting" && m_anim.GetCurrentFrame() >= 65 && did_shoot == false) {
        Texture2D tex = res.GetTexture("FirePea"); 
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 15, 450.0f, tex)); // Fire pea travels slightly faster
        did_shoot = true;
    }

    if (m_anim.GetCurrentFrame() >= 77) {
        did_shoot = false;
    }
}

void FirePea::draw() {
    m_anim.Draw(m_x, m_y, 1.0f, Color{255, 140, 60, 255}); // Orange-fiery tint
}
