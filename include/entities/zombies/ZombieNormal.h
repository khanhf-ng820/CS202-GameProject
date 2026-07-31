#pragma once
#include "Zombie.h"

#include <vector>


class ZombieNormal : public Zombie {
public:
    ZombieNormal(Resources& res, float x, float y);
    ~ZombieNormal() override;

    void update(float deltaTime) override;
    void draw() override;
    void takeDamage(int damage) override;

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
};
