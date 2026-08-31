#pragma once
#include "Plant.h"

class Plantern : public Plant {
public:
    Plantern(Resources& res, int x, int y);
    ~Plantern() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
