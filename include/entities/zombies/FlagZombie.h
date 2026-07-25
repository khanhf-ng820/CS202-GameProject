#pragma once
#include "Zombie.h"

class FlagZombie : public Zombie {
public:
    FlagZombie(Resources& res, float x, float y);
    ~FlagZombie() override;

    void takeDamage(int damage) override;
    bool isFinished() const override { return m_hp <= 0 && m_deathTimer >= 1.5f; }
    void update(float dt) override;
    void draw() override;

private:
    std::vector<FallingPart> m_fallingParts;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
    float m_deathTimer = 0.0f;
};
