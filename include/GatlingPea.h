#pragma once
#include "Plant.h"

class GatlingPea : public Plant {
private:
    bool did_shoot = false;
    float m_shootTimer = 0.0f;
    float m_shootInterval = 1.5f; // Thời gian idle trước khi bắn tiếp

public:
    GatlingPea(Resources& res, int x, int y);
    ~GatlingPea() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
