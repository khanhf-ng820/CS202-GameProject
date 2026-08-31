#pragma once
#include "Plant.h"
#include "SunItem.h"

class SunFlower : public Plant {
private:
    float m_sunProduceTimer = 17.0f;    // Initial sun spawns after ~7s, subsequent suns every 24s
    float m_sunProduceInterval = 24.0f; // Spawns sun every 24 seconds (authentic PvZ timing)

public:
    SunFlower(Resources& res, int x, int y);
    ~SunFlower() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
