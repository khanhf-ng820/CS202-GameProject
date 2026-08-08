#pragma once
#include "Zombie.h"

class FlagZombie : public Zombie {
public:
    FlagZombie(Resources& res, float x, float y);
    ~FlagZombie() override;

    void takeDamage(float damage) override;
    void update(float dt) override;
    void draw() override;

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
};
