#pragma once
#include "Zombie.h"
#include <vector>

class ConeheadZombie : public Zombie {
public:
    ConeheadZombie(Resources& res, float x, float y);
    ~ConeheadZombie() override;

    void update(float deltaTime) override;
    void draw() override;
    void takeDamage(int damage) override;

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
    bool m_hasLostCone = false;
};
