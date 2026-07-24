#include "PeaShooter.h"

PeaShooter::PeaShooter(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 100, "PeaShooter") {
    // Gọi hàm này để nạp reanim cho Peashooter
    getResources(res.GetAssetPath("assets/reanim/PeaShooter.reanim"));
    m_anim.SetBaseAnimation("anim_idle");
    m_anim.SetAnimation("anim_head_idle");
    m_fireRate = 2.08f;
    m_fireTimer = 0.0f;
}

PeaShooter::~PeaShooter() {
}

void PeaShooter::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    // Cập nhật animation
    m_anim.Update(deltaTime);

    std::string currentAnim = m_anim.GetCurrentAnimName();

    // Nếu người dùng bấm "anim_idle" trên UI, tự động chuyển sang "anim_head_idle"
    if (currentAnim == "anim_idle") {
        m_anim.SetAnimation("anim_head_idle");
        currentAnim = "anim_head_idle";
    }

    // Xử lý logic bắn đạn
    if (currentAnim == "anim_shooting" && m_anim.GetCurrentFrame() == 65 && !did_shoot) {
        // Bắn đạn
        Texture2D tex = res.GetTexture("ProjectilePea"); 
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 15, 400.0f, tex));
        did_shoot = 1;
    }

    if (m_anim.GetCurrentFrame() == 66)
        did_shoot = 0;
}

void PeaShooter::draw() {
    // Vẽ animation tại vị trí của cây
    m_anim.Draw(m_x, m_y, 1.0f); 
}
