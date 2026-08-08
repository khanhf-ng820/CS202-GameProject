#pragma once
#include "Plant.h"

class Caltrop : public Plant {
private:
    bool m_isAttacking = false;

public:
    Caltrop(Resources& res, int x, int y);
    ~Caltrop() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    // Called by Level1 collision system when a zombie is on top
    void setAttacking(bool attacking) { m_isAttacking = attacking; }
    bool isAttacking() const { return m_isAttacking; }
};
