#include "SunItem.h"
#include <cmath>

SunItem::SunItem(float x, float y, Texture2D tex)
    : m_x(x), m_y(y), m_startX(x), m_startY(y), m_targetY(y + 50.0f), m_tex(tex), m_lifetime(0.0f), m_active(true), m_isSkySun(false) {
}

SunItem::SunItem(float x, float startY, float targetY, Texture2D tex)
    : m_x(x), m_y(startY), m_startX(x), m_startY(startY), m_targetY(targetY), m_tex(tex), m_lifetime(0.0f), m_active(true), m_isSkySun(true) {
}

SunItem::~SunItem() {
}

void SunItem::update(float dt) {
    m_lifetime += dt;
    if (!m_isSkySun) {
        if (m_lifetime < 1.0f) {
            m_y += 50.0f * dt;
            m_x += 20.0f * dt;
        } else {
            m_y = m_startY + 50.0f + sinf(m_lifetime * 3.0f) * 5.0f;
        }
        if (m_lifetime > 7.0f) {
            m_active = false;
        }
    } else {
        if (m_y < m_targetY) {
            m_y += 80.0f * dt;
            if (m_y > m_targetY) m_y = m_targetY;
        } else {
            m_y = m_targetY + sinf(m_lifetime * 3.0f) * 3.0f;
        }
        if (m_lifetime > 12.0f) {
            m_active = false;
        }
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

bool SunItem::isClicked(Vector2 mousePos) const {
    if (!m_active) return false;
    float width = (m_tex.id != 0) ? (float)m_tex.width : 70.0f;
    float height = (m_tex.id != 0) ? (float)m_tex.height : 70.0f;
    Rectangle bounds = { m_x, m_y, width, height };
    return CheckCollisionPointRec(mousePos, bounds);
}

void SunItem::collect() {
    m_active = false;
}
