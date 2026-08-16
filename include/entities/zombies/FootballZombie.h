#pragma once
#include "Zombie.h"
#include <vector>

class FootballZombie : public Zombie {
private:
    bool m_hasLostHelmet = false;
    bool m_hasLostArm = false;
    bool m_hasSpawnedDeathParts = false;
    std::vector<FallingPart> m_fallingParts;

public:
    FootballZombie(Resources& res, float x, float y);
    ~FootballZombie() override;

    void update(float deltaTime) override;
    void draw() override;
    void takeDamage(float damage) override;
};
