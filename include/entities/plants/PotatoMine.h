#pragma once
#include "Plant.h"

enum class PotatoMineState {
    UNARMED,
    RISING,
    ARMED,
    EXPLODING,
    DONE
};

struct PotatoChunkParticle {
    float x, y;
    float vx, vy;
    float rotation;
    float rotSpeed;
    float scale;
    int frameIndex;
    float alpha;
};

class PotatoMine : public Plant {
private:
    PotatoMineState m_state = PotatoMineState::UNARMED;
    float m_armTimer = 0.0f;
    float m_explodeTimer = 0.0f;
    bool m_hasDealtDamage = false;
    static constexpr float ARM_DELAY = 15.0f; // Seconds before the mine becomes active
    std::vector<PotatoChunkParticle> m_particles;

public:
    PotatoMine(Resources& res, int x, int y);
    ~PotatoMine() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isDead() const override;
    bool isArmed() const { return m_state == PotatoMineState::ARMED; }
    bool isExploding() const { return m_state == PotatoMineState::EXPLODING; }
    void triggerExplode();
    bool hasDealtDamage() const { return m_hasDealtDamage; }
    void markDamageDealt() { m_hasDealtDamage = true; }
};
