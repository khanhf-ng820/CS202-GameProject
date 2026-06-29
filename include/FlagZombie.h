#pragma once
#include "Zombie.h"

class FlagZombie : public Zombie {
public:
    FlagZombie(Resources& res, float x, float y);
    ~FlagZombie() override;

    void update(float deltaTime) override;
    void draw() override;
};
