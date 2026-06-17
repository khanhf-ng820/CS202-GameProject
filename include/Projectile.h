#pragma once
#include "raylib.h"
#include <cmath>
#include <iostream>

class Projectile {
private:
    float m_x, m_y;
    float m_startX, m_startY;
    float m_speed;
    Texture2D m_tex;
    bool m_active;
    bool m_isSnow;
    bool m_isLobbed;
    float m_range;
    float m_maxHeight;

public:
    Projectile(float x, float y, float speed, Texture2D tex, bool isSnow = false, bool isLobbed = false) 
        : m_x(x), m_y(y), m_startX(x), m_startY(y), m_speed(speed), m_tex(tex), m_active(true), m_isSnow(isSnow), m_isLobbed(isLobbed) {
        m_range = 500.0f;     // Tầm bắn xa của đạn cầu vồng
        m_maxHeight = 150.0f; // Chiều cao tối đa vòng cung
    }

    void update(float dt) {
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
    }

    void draw() const {
        if (m_active && m_tex.id != 0) {
            DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, 1.0f, WHITE);
        }
    }

    bool isActive() const { return m_active; }
};
