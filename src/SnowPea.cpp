#include "SnowPea.h"

SnowPea::SnowPea(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 175, "SnowPea") {
    // Nạp reanim cho SnowPea
    getResources(res.GetAssetPath("assets/reanim/SnowPea.reanim"));
    m_fireRate = 1.5f; // Thời gian hồi đạn chuẩn
    m_fireTimer = 0.0f;
}

SnowPea::~SnowPea() {
}

void SnowPea::update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) {
    // Cập nhật animation
    m_anim.Update(deltaTime);

    // Xử lý logic bắn đạn
    m_fireTimer += deltaTime;
    if (m_fireTimer >= m_fireRate) {
        if (m_anim.GetCurrentAnimName() == "anim_shooting" && m_anim.GetCurrentFrame() == 59 && did_shoot == 0) {
            // Bắn đạn băng (ProjectileSnowPea)
            Texture2D tex = res.GetTexture("ProjectileSnowPea"); 
            // Tọa độ bắn có thể tuỳ chỉnh đôi chút để ra đúng vị trí miệng
            outProjectiles.push_back(Projectile(m_x + 60, m_y + 35, 400.0f, tex, true));
            did_shoot = 1;
        }

        if (m_anim.GetCurrentFrame() == 60) {
            did_shoot = 0;
            m_fireTimer = 0.0f; // Reset bộ đếm khi hoàn tất bắn đạn
        }
    }
}

void SnowPea::draw() {
    // Scale 1.6f cho phù hợp màn hình
    m_anim.Draw(m_x, m_y, 1.6f);
}
