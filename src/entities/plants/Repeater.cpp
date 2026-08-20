#include "Repeater.h"
#include "AudioManager.h"

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

    if (currentAnim == "anim_shooting") {
        int frame = m_anim.GetCurrentFrame();
        if ((frame == 65 || frame == 68) && !did_shoot) {
            Texture2D tex = res.GetTexture("ProjectilePea"); 
            outProjectiles.push_back(Projectile(m_x + 46, m_y + 12, 400.0f, tex));
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/throw.ogg"));
            did_shoot = true;
        }

        if (frame == 66 || frame == 69) {
            did_shoot = false;
        }
    }
}

void Repeater::draw() {
    m_anim.Draw(m_x, m_y, 1.0f); 
}
