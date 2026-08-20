#pragma once
#include "Plant.h"
#include <algorithm>

class Gravebuster : public Plant {
private:
    bool m_hasLanded = false;
    float m_eatTimer = 0.0f;
    float m_eatDuration = 4.5f;
    float m_chompSoundTimer = 0.0f;
    bool m_finished = false;

public:
    Gravebuster(Resources& res, int x, int y);
    ~Gravebuster() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    float getEatProgress() const { return std::clamp(m_eatTimer / m_eatDuration, 0.0f, 1.0f); }
    bool isFinished() const { return m_finished; }
};
