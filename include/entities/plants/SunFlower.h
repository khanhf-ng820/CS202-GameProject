#pragma once
#include "Plant.h"
#include "SunItem.h"

class SunFlower : public Plant {
private:
    float m_sunProduceTimer = 0.0f;
    float m_sunProduceInterval = 5.0f; // Spawns sun every 5 seconds

public:
    SunFlower(Resources& res, int x, int y);
    ~SunFlower() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
