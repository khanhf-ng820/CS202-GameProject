#pragma once
#include "Plant.h"

class SnowPea : public Plant {
private:
    float m_fireRate;
    float m_fireTimer;
    bool did_shoot = 0;

public:
    SnowPea(Resources& res, int x, int y);
    ~SnowPea() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
