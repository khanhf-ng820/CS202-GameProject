#pragma once
#include "Plant.h"
#include "SunItem.h"

class TwinSunflower : public Plant {
private:
    float m_sunProduceTimer = 0.0f;
    float m_sunProduceInterval = 5.0f; // Produces sun every 5 seconds

public:
    TwinSunflower(Resources& res, int x, int y);
    ~TwinSunflower() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
