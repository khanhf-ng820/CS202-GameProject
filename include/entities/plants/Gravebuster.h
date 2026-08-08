#pragma once
#include "Plant.h"

class Gravebuster : public Plant {
private:
    bool m_hasLanded = false;

public:
    Gravebuster(Resources& res, int x, int y);
    ~Gravebuster() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;
};
