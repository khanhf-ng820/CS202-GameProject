#pragma once
#include "Plant.h"

enum class SquashState {
    IDLE,
    AIMING,
    JUMPING,
    SQUASHING,
    DONE
};

class Squash : public Plant {
private:
    SquashState m_state = SquashState::IDLE;
    float m_aimTimer = 0.0f;
    float m_jumpTimer = 0.0f;
    float m_squashTimer = 0.0f;
    float m_targetX = 0.0f;
    float m_targetY = 0.0f;
    float m_startX = 0.0f;
    float m_jumpHeight = 0.0f;
    bool m_hasDealtDamage = false;

public:
    Squash(Resources& res, int x, int y);
    ~Squash() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isDead() const override;
    bool isIdle() const { return m_state == SquashState::IDLE; }
    bool isSquashing() const { return m_state == SquashState::SQUASHING; }
    void setTargetZombie(float x, float y);
    float getTargetX() const { return m_targetX; }
    float getTargetY() const { return m_targetY; }
    bool hasDealtDamage() const { return m_hasDealtDamage; }
    void markDamageDealt() { m_hasDealtDamage = true; }
};
