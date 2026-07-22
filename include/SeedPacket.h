#pragma once
#include "raylib.h"
#include "resources.h"
#include <string>

class SeedPacket {
private:
    std::string m_plantType;
    int m_sunCost;
    float m_cooldownMax;
    float m_cooldownTimer;
    std::string m_textureName; // e.g. "PEASHOOTER", "SUNFLOWER"
    Rectangle m_bounds;

public:
    SeedPacket(const std::string& plantType, int sunCost, float cooldownMax, const std::string& textureName, Rectangle bounds);

    void update(float dt);
    void draw(Resources& res, int currentSun, bool isSelected) const;

    bool isClicked(Vector2 mousePos) const;
    bool isReady(int currentSun) const;
    void triggerCooldown();

    std::string getPlantType() const { return m_plantType; }
    int getSunCost() const { return m_sunCost; }
    float getCooldownTimer() const { return m_cooldownTimer; }
    Rectangle getBounds() const { return m_bounds; }
    std::string getTextureName() const { return m_textureName; }
};
