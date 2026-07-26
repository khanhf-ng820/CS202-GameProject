#pragma once
#include "Plant.h"

class Melonpult : public Plant {
private:
    bool did_shoot = false;
    float distance = 400.0f; // Distance traveled by the melon projectile

public:
    Melonpult(Resources& res, int x, int y);
    ~Melonpult() override;

    void set_distance(float dist) { distance = dist; }
    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
