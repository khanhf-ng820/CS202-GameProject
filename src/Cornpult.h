#pragma once
#include "Plant.h"

class Cornpult : public Plant {
private:
    bool did_shoot = false;

public:
    Cornpult(Resources& res, int x, int y);
    ~Cornpult() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles) override;
    void draw() override;
};
