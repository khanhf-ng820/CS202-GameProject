#pragma once
#include "Plant.h"

class Garlic : public Plant {
private:
    float m_redirectCooldown = 0.0f; // Cooldown between lane redirects

public:
    Garlic(Resources& res, int x, int y);
    ~Garlic() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
