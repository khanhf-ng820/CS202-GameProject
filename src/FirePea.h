#pragma once
#include "Plant.h"

class FirePea : public Plant {
private:
    bool did_shoot = false;

public:
    FirePea(Resources& res, int x, int y);
    ~FirePea() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
