#pragma once

#include "Zombie.h"
#include <string>
#include <vector>

class NewspaperZombie : public Zombie {
public:
    NewspaperZombie(Resources& res, float x, float y);
    ~NewspaperZombie();

    void takeDamage(float damage) override;
    void update(float deltaTime) override;
    void draw() override;

private:
    bool m_hasLostNewspaper = false;
    bool m_isGasping = false;
    bool m_hasLostArm = false;
    bool m_hasSpawnedDeathParts = false;
    std::vector<FallingPart> m_fallingParts;
};
