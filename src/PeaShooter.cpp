#include "PeaShooter.h"

PeaShooter::PeaShooter(Resources& res, int x, int y)
    : Plant(res, x, y, 300, 100, "PeaShooter") {
    // Gọi hàm này để nạp reanim cho Peashooter (thay đổi đường dẫn nếu cần)
    getResources(res.GetAssetPath("assets/reanim/PeaShooter.reanim"));
    m_fireRate = 2.08f; // Mỗi 1.5s bắn 1 viên đạn
    m_fireTimer = 0.0f;
}

PeaShooter::~PeaShooter() {
}

void PeaShooter::update(float deltaTime, std::vector<Projectile>& outProjectiles) {
    // Cập nhật animation
    m_anim.Update(deltaTime);

    // Xử lý logic bắn đạn
    if (m_anim.GetCurrentAnimName() == "anim_shooting" && m_anim.GetCurrentFrame() == 59 && did_shoot == 0) {
        // Bắn đạn
        Texture2D tex = res.GetTexture("ProjectilePea"); 
        // Tọa độ bắn có thể tuỳ chỉnh đôi chút để ra đúng vị trí miệng
        outProjectiles.push_back(Projectile(m_x + 60, m_y + 35, 400.0f, tex));
        did_shoot = 1;
    }

    if (m_anim.GetCurrentFrame() == 60) {
        did_shoot = 0;
    }
}

void PeaShooter::draw() {
    // Vẽ animation tại vị trí của cây
    m_anim.Draw(m_x, m_y, 1.6f); 
}
