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

private:
    bool m_hasVaulted = false;
    bool m_isVaulting = false;
    bool m_hasSpawnedDeathParts = false;
    bool m_hasLostArm = false;
    std::vector<FallingPart> m_fallingParts;
};
