#include "SeedPacket.h"
#include <iostream>

SeedPacket::SeedPacket(const std::string& plantType, int sunCost, float cooldownMax, const std::string& textureName, Rectangle bounds)
    : m_plantType(plantType), m_sunCost(sunCost), m_cooldownMax(cooldownMax),
      m_cooldownTimer(0.0f), m_textureName(textureName), m_bounds(bounds) {
}

void SeedPacket::update(float dt) {
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= dt;
        if (m_cooldownTimer < 0.0f) {
            m_cooldownTimer = 0.0f;
        }
    }
}

void SeedPacket::draw(Resources& res, int currentSun, bool isSelected) const {
    Texture2D tex = res.GetTexture(m_textureName);
    
    if (tex.id != 0) {
        DrawTexturePro(
            tex,
            { 0.0f, 0.0f, (float)tex.width, (float)tex.height },
            m_bounds,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        // Fallback text if packet image is not found
        DrawRectangleRec(m_bounds, LIGHTGRAY);
        DrawText(m_plantType.c_str(), (int)m_bounds.x + 2, (int)m_bounds.y + 10, 10, BLACK);
    }

    // Check if affordable and off cooldown
    bool affordable = (currentSun >= m_sunCost);
    bool offCooldown = (m_cooldownTimer <= 0.0f);

    if (!affordable) {
        // Dark overlay for unaffordable state (35% opacity shade)
        DrawRectangleRec(m_bounds, ColorAlpha(BLACK, 0.35f));
    }

    // Cooldown timer sweep overlay
    if (m_cooldownTimer > 0.0f && m_cooldownMax > 0.0f) {
        float ratio = m_cooldownTimer / m_cooldownMax;
        float cdHeight = m_bounds.height * ratio;
        Rectangle cdRect = { m_bounds.x, m_bounds.y, m_bounds.width, cdHeight };
        DrawRectangleRec(cdRect, ColorAlpha(BLACK, 0.45f));
    }

    // Draw Sun cost text at bottom right of packet
    std::string costStr = std::to_string(m_sunCost);
    int fontSize = 12;
    int textWidth = MeasureText(costStr.c_str(), fontSize);
    int textX = (int)(m_bounds.x + m_bounds.width - textWidth - 3);
    int textY = (int)(m_bounds.y + m_bounds.height - fontSize - 2);

    // Text shadow + text
    DrawText(costStr.c_str(), textX + 1, textY + 1, fontSize, BLACK);
    DrawText(costStr.c_str(), textX, textY, fontSize, affordable ? WHITE : RED);

    // Draw selection highlight frame if currently selected
    if (isSelected) {
        DrawRectangleLinesEx(m_bounds, 2.5f, GOLD);
    } else {
        DrawRectangleLinesEx(m_bounds, 1.0f, ColorAlpha(WHITE, 0.3f));
    }
}

bool SeedPacket::isClicked(Vector2 mousePos) const {
    return CheckCollisionPointRec(mousePos, m_bounds);
}

bool SeedPacket::isReady(int currentSun) const {
    return (currentSun >= m_sunCost) && (m_cooldownTimer <= 0.0f);
}

void SeedPacket::triggerCooldown() {
    m_cooldownTimer = m_cooldownMax;
}
