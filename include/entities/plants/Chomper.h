#pragma once
#include "Plant.h"

class Chomper : public Plant {
public:
    Chomper(Resources& res, int x, int y);
    ~Chomper() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
