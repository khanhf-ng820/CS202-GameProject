#pragma once
#include "Projectile.h"


void Projectile::update(float dt) {
    m_x += m_speed * dt;
    if (m_isLobbed) {
        float progress = (m_x - m_startX) / m_range;
        if (progress >= 1.0f) {
            m_active = false; // Chạm đất/bia bắn thì tắt
        } else {
            // Tính tọa độ Y theo đường cong Sin (1 nửa chu kỳ từ 0 -> PI)
            m_y = m_startY - m_maxHeight * sinf(progress * 3.14159265f);
        }
    }
    if (m_x > 1300) {
        m_active = false;
    }
    efftrailing.timing(dt);
    efftrailing.setter(m_x, m_y + (m_tex.height * m_scale) / 2.0f); // Đặt hiệu ứng theo vị trí của đạn
}

void Projectile::draw() const {
    if (m_active && m_tex.id != 0) {
        DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, m_scale, WHITE);
    }
    efftrailing.draw();
}