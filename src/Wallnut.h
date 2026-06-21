#pragma once
#include "Plant.h"

class Wallnut : public Plant {
public:
    Wallnut(Resources& res, int x, int y);
    ~Wallnut() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
