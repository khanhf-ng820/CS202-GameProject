#pragma once
#include "Plant.h"

class SpikeRock : public Plant {
private:
    bool m_isAttacking = false;

public:
    SpikeRock(Resources& res, int x, int y);
    ~SpikeRock() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    void setAttacking(bool attacking) { m_isAttacking = attacking; }
    bool isAttacking() const { return m_isAttacking; }
};
