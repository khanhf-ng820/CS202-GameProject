#pragma once
#include "Plant.h"

class CherryBomb : public Plant {
private:
    bool did_explode = false;
    bool is_exploding_effect = false;
    float explosion_timer = 0.0f;

public:
    CherryBomb(Resources& res, int x, int y);
    ~CherryBomb() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
