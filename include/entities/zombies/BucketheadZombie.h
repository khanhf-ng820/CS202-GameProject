#pragma once
#include "Zombie.h"
#include <vector>

class BucketheadZombie : public Zombie {
public:
    BucketheadZombie(Resources& res, float x, float y);
    ~BucketheadZombie() override;

    void update(float deltaTime) override;
    void draw() override;
    void takeDamage(int damage) override;

    bool isFinished() const override { return m_hp <= 0 && m_deathTimer >= 1.5f; }

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
    bool m_hasLostBucket = false;
    float m_deathTimer = 0.0f;
};
