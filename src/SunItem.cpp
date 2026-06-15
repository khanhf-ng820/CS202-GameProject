#include "SunItem.h"
#include <cmath>

SunItem::SunItem(float x, float y, Texture2D tex)
    : m_x(x), m_y(y), m_startX(x), m_startY(y), m_tex(tex), m_lifetime(0.0f), m_active(true) {
}

SunItem::~SunItem() {
}

void SunItem::update(float dt) {
    m_lifetime += dt;
    if (m_lifetime < 1.0f) {
        m_y += 50.0f * dt;
        m_x += 20.0f * dt;
    } else {
        m_y = m_startY + 50.0f + sinf(m_lifetime * 3.0f) * 5.0f;
    }
    if (m_lifetime > 7.0f) {
        m_active = false;
    }
}

void SunItem::draw() const {
    if (m_active && m_tex.id != 0) {
        DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, 1.0f, WHITE);
    }
}

bool SunItem::isActive() const {
    return m_active;
}
