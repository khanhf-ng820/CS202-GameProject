#pragma once
#include "Plant.h"

class Melonpult : public Plant {
private:
    bool did_shoot = false;

public:
    Melonpult(Resources& res, int x, int y);
    ~Melonpult() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
