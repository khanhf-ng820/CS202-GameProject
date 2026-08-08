#pragma once
#include "Plant.h"

class Cabbagepult : public Plant {
private:
    bool did_shoot = false;
    float distance = 400.0f;

public:
    Cabbagepult(Resources& res, int x, int y);
    ~Cabbagepult() override;

    void set_distance(float dist) override { distance = dist; }
    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
