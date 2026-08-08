#pragma once
#include "Plant.h"

enum class IceShroomState {
    IDLE,
    FREEZING,
    DONE
};

class IceShroom : public Plant {
private:
    IceShroomState m_state = IceShroomState::IDLE;
    float m_idleTimer = 0.0f;
    float m_freezeEffectTimer = 0.0f;
    bool m_hasDealtDamage = false;

public:
    IceShroom(Resources& res, int x, int y);
    ~IceShroom() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isDead() const override;
    bool isFreezing() const { return m_state == IceShroomState::FREEZING; }
    bool hasDealtDamage() const { return m_hasDealtDamage; }
    void markDamageDealt() { m_hasDealtDamage = true; }
};
