#include "GatlingPea.h"
#include "AudioManager.h"

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

    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    if (currentAnim == "anim_shooting") {
        int frame = m_anim.GetCurrentFrame();
        if ((frame == 65 || frame == 71 || frame == 77 || frame == 83) && !did_shoot) {
            Texture2D tex = res.GetTexture("ProjectilePea"); 
            outProjectiles.push_back(Projectile(m_x + 46, m_y + 12, 400.0f, tex));
            AudioManager::GetInstance().PlaySoundEffect(res.GetAssetPath("assets/sounds/throw.ogg"));
            did_shoot = true;
        }

        if (frame == 66 || frame == 72 || frame == 78 || frame == 84) {
            did_shoot = false;
        }
    }
}

void GatlingPea::draw() {
    m_anim.Draw(m_x, m_y, 1.0f); 
}
