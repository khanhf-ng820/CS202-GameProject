#pragma once
#include "Plant.h"

class Repeater : public Plant {
private:
    float m_shootTimer = 0.0f;
    float m_shootInterval = 1.5f;
    bool did_shoot = false;

public:
    Repeater(Resources& res, int x, int y);
    ~Repeater() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
