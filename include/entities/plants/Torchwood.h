#pragma once
#include "Plant.h"

class Torchwood : public Plant {
public:
    Torchwood(Resources& res, int x, int y);
    ~Torchwood() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
