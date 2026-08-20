#include "SunItem.h"
#include <cmath>
#include <algorithm>

SunItem::SunItem(float x, float y, Texture2D tex)
    : m_x(x), m_y(y), m_startX(x), m_startY(y), m_targetY(y + 50.0f), m_tex(tex),
      m_lifetime(0.0f), m_active(true), m_isSkySun(false),
      m_isCollecting(false), m_collectStartX(x), m_collectStartY(y),
      m_destX(20.0f), m_destY(15.0f), m_collectTimer(0.0f),
      m_collectDuration(0.75f), m_arrived(false), m_arrivalHandled(false) {
}

SunItem::SunItem(float x, float startY, float targetY, Texture2D tex)
    : m_x(x), m_y(startY), m_startX(x), m_startY(startY), m_targetY(targetY), m_tex(tex),
      m_lifetime(0.0f), m_active(true), m_isSkySun(true),
      m_isCollecting(false), m_collectStartX(x), m_collectStartY(startY),
      m_destX(20.0f), m_destY(15.0f), m_collectTimer(0.0f),
      m_collectDuration(0.75f), m_arrived(false), m_arrivalHandled(false) {
}

SunItem::~SunItem() {
}

void SunItem::update(float dt) {
    if (m_isCollecting) {
        m_collectTimer += dt;
        float t = m_collectTimer / m_collectDuration;
        if (t >= 1.0f) {
            t = 1.0f;
            m_x = m_destX;
            m_y = m_destY;
            m_arrived = true;
            m_active = false;
        } else {
            // Smooth natural curve (ease-in-out / smoothstep) towards the top-left SeedBank
            float ease = t * t * (3.0f - 2.0f * t);
            m_x = m_collectStartX + (m_destX - m_collectStartX) * ease;
            m_y = m_collectStartY + (m_destY - m_collectStartY) * ease;
        }
        return;
    }

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
        if (m_isCollecting) {
            float t = std::clamp(m_collectTimer / m_collectDuration, 0.0f, 1.0f);
            float scale = 1.0f - 0.2f * t; // Smoothly scales down from 1.0x to 0.8x into the counter
            float srcW = (float)m_tex.width;
            float srcH = (float)m_tex.height;
            DrawTexturePro(
                m_tex,
                { 0.0f, 0.0f, srcW, srcH },
                { m_x, m_y, srcW * scale, srcH * scale },
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        } else {
            DrawTextureEx(m_tex, {m_x, m_y}, 0.0f, 1.0f, WHITE);
        }
    }
}

bool SunItem::isActive() const {
    return m_active;
}

bool SunItem::isClicked(Vector2 mousePos) const {
    if (!m_active || m_isCollecting) return false;
    float width = (m_tex.id != 0) ? (float)m_tex.width : 70.0f;
    float height = (m_tex.id != 0) ? (float)m_tex.height : 70.0f;
    Rectangle bounds = { m_x, m_y, width, height };
    return CheckCollisionPointRec(mousePos, bounds);
}

void SunItem::collect(float destX, float destY) {
    if (m_isCollecting || !m_active) return;
    m_isCollecting = true;
    m_collectStartX = m_x;
    m_collectStartY = m_y;
    m_destX = destX;
    m_destY = destY;
    m_collectTimer = 0.0f;
    m_collectDuration = 0.75f;
    m_arrived = false;
    m_arrivalHandled = false;
}

bool SunItem::hasArrived() {
    if (m_arrived && !m_arrivalHandled) {
        m_arrivalHandled = true;
        return true;
    }
    return false;
}
