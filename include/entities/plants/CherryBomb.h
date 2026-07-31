#pragma once
#include "Plant.h"

class CherryBomb : public Plant {
private:
    bool did_explode = false;
    bool is_exploding_effect = false;
    float explosion_timer = 0.0f;
    bool m_hasDealtDamage = false;

public:
    CherryBomb(Resources& res, int x, int y);
    ~CherryBomb() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isExplodingEffect() const { return is_exploding_effect; }
    bool hasDealtDamage() const { return m_hasDealtDamage; }
    void markDamageDealt() { m_hasDealtDamage = true; }
};
