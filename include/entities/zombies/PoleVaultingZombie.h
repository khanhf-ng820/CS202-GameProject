#pragma once
#include "Zombie.h"
#include <vector>

class PoleVaultingZombie : public Zombie {
public:
    PoleVaultingZombie(Resources& res, float x, float y);
    ~PoleVaultingZombie() override;

    void update(float deltaTime) override;
    void draw() override;
    void takeDamage(float damage) override;

    void startVault();
    bool isVaulting() const { return m_isVaulting; }
    bool hasVaulted() const { return m_hasVaulted; }

    Rectangle getJumpTriggerBounds() const {
        return Rectangle{ m_x - 70.0f, m_y + 30.0f, 90.0f, 90.0f };
    }
    Rectangle getBodyBounds() const {
        return Rectangle{ m_x + 20.0f, m_y + 30.0f, 50.0f, 90.0f };
    }

private:
    bool m_hasVaulted = false;
    bool m_isVaulting = false;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
    std::vector<FallingPart> m_fallingParts;
};
