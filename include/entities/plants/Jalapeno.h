#pragma once
#include "Plant.h"

enum class JalapenoState {
    EXPLODING_SWELL,
    EXPLODING_FIRE,
    DONE
};

class Jalapeno : public Plant {
private:
    JalapenoState m_state = JalapenoState::EXPLODING_SWELL;
    Reanimation m_fireAnim;
    float m_fireDurationTimer = 0.0f;
    bool m_hasDealtDamage = false;

public:
    Jalapeno(Resources& res, int x, int y);
    ~Jalapeno() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isDead() const override;
    bool isExplodingFire() const { return m_state == JalapenoState::EXPLODING_FIRE; }
    bool hasDealtDamage() const { return m_hasDealtDamage; }
    void markDamageDealt() { m_hasDealtDamage = true; }
};
