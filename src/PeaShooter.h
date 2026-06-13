#pragma once
#include "Plant.h"

class PeaShooter : public Plant {
private:
    float m_fireRate;
    float m_fireTimer;
    bool did_shoot = 0;

public:
    PeaShooter(Resources& res, int x, int y);
    ~PeaShooter() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles) override;
    void draw() override;
};
