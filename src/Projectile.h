#pragma once
#include "raylib.h"

class Projectile {
private:
    float m_x, m_y;
    float m_speed;
    Texture2D m_tex;
    bool m_active;
    bool m_isSnow;

public:
    Projectile(float x, float y, float speed, Texture2D tex, bool isSnow = false) 
        : m_x(x), m_y(y), m_speed(speed), m_tex(tex), m_active(true), m_isSnow(isSnow) {}

    void update(float dt) {
        m_x += m_speed * dt;
        if (m_x > 1300) m_active = false;
    }

    void draw() const {
        if (m_active && m_tex.id != 0) {
            DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, 1.0f, WHITE);
        }
    }

    bool isActive() const { return m_active; }
};
