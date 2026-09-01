#include "SunItem.h"
#include "resources.h"
#include <cmath>
#include <algorithm>

static Texture2D EnsureValidSunTexture(Texture2D tex) {
    if (tex.id != 0) return tex;
    Resources& res = Resources::GetInstance();
    Texture2D validTex = res.GetTexture("Sun3");
    if (validTex.id == 0) validTex = res.GetTexture("Sun2");
    if (validTex.id == 0) validTex = res.GetTexture("Sun1");
    return validTex;
}

SunItem::SunItem(float x, float y, Texture2D tex)
    : m_x(x), m_y(y), m_startX(x), m_startY(y), m_targetY(y + 50.0f), m_tex(EnsureValidSunTexture(tex)),
      m_lifetime(0.0f), m_active(true), m_isSkySun(false),
      m_isCollecting(false), m_collectStartX(x), m_collectStartY(y),
      m_destX(20.0f), m_destY(15.0f), m_collectTimer(0.0f),
      m_collectDuration(0.75f), m_arrived(false), m_arrivalHandled(false) {
}

SunItem::SunItem(float x, float startY, float targetY, Texture2D tex)
    : m_x(x), m_y(startY), m_startX(x), m_startY(startY), m_targetY(targetY), m_tex(EnsureValidSunTexture(tex)),
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
            m_y += 38.0f * dt; // Slow, gentle floating drop from sky
            if (m_y > m_targetY) m_y = m_targetY;
        } else {
            m_y = m_targetY + sinf(m_lifetime * 3.0f) * 3.0f;
        }
        if (m_lifetime > 15.0f) {
            m_active = false;
        }
    }
}

void SunItem::draw() const {
    if (!m_active) return;

    Resources& res = Resources::GetInstance();
    Texture2D tex1 = res.GetTexture("SUN1");
    Texture2D tex3 = res.GetTexture("SUN3");

    if (tex1.id == 0) tex1 = m_tex;
    if (tex3.id == 0) tex3 = m_tex;

    if (tex1.id != 0 || tex3.id != 0) {
        float scale = 1.0f;
        if (m_isCollecting) {
            float t = std::clamp(m_collectTimer / m_collectDuration, 0.0f, 1.0f);
            scale = 1.0f - 0.2f * t;
        }

        float w3 = (tex3.id != 0) ? (float)tex3.width * scale : 75.0f * scale;
        float h3 = (tex3.id != 0) ? (float)tex3.height * scale : 75.0f * scale;
        float w1 = (tex1.id != 0) ? (float)tex1.width * scale : w3;
        float h1 = (tex1.id != 0) ? (float)tex1.height * scale : h3;

        float refW = std::max(w1, w3);
        float refH = std::max(h1, h3);

        // 1. Draw Sun3 (outer ray background layer)
        if (tex3.id != 0) {
            float offsetX3 = (refW - w3) / 2.0f;
            float offsetY3 = (refH - h3) / 2.0f;
            DrawTexturePro(
                tex3,
                { 0.0f, 0.0f, (float)tex3.width, (float)tex3.height },
                { m_x + offsetX3, m_y + offsetY3, w3, h3 },
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }

        // 2. Draw Sun1 overlaid on top of Sun3 (centered core layer)
        if (tex1.id != 0) {
            float offsetX1 = (refW - w1) / 2.0f;
            float offsetY1 = (refH - h1) / 2.0f;
            DrawTexturePro(
                tex1,
                { 0.0f, 0.0f, (float)tex1.width, (float)tex1.height },
                { m_x + offsetX1, m_y + offsetY1, w1, h1 },
                { 0.0f, 0.0f },
                0.0f,
                WHITE
            );
        }
    } else {
        // Fallback: draw bright golden sun circle if texture is missing
        DrawCircle((int)m_x + 35, (int)m_y + 35, 30.0f, GOLD);
        DrawCircle((int)m_x + 35, (int)m_y + 35, 24.0f, YELLOW);
    }
}

bool SunItem::isActive() const {
    return m_active;
}

bool SunItem::isClicked(Vector2 mousePos) const {
    if (!m_active || m_isCollecting) return false;
    Resources& res = Resources::GetInstance();
    Texture2D tex1 = res.GetTexture("SUN1");
    Texture2D tex3 = res.GetTexture("SUN3");
    if (tex1.id == 0) tex1 = m_tex;

    float w1 = (tex1.id != 0) ? (float)tex1.width : 75.0f;
    float h1 = (tex1.id != 0) ? (float)tex1.height : 75.0f;
    float w3 = (tex3.id != 0) ? (float)tex3.width : 75.0f;
    float h3 = (tex3.id != 0) ? (float)tex3.height : 75.0f;

    float width = std::max({ 75.0f, w1, w3 });
    float height = std::max({ 75.0f, h1, h3 });

    // Generous hitbox padded by 15px on all sides for smooth easy clicking
    Rectangle bounds = { m_x - 15.0f, m_y - 15.0f, width + 30.0f, height + 30.0f };
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
