#pragma once
#include "Plant.h"

class Zombie;

enum class ChomperState {
    IDLE,
    BITING,
    CHEWING,
    SWALLOWING
};

class Chomper : public Plant {
private:
    ChomperState m_state = ChomperState::IDLE;
    float m_stateTimer = 0.0f;
    Zombie* m_targetZombie = nullptr;
    static constexpr float CHEW_DURATION = 30.0f; // 30 seconds chewing/digestion

public:
    Chomper(Resources& res, int x, int y);
    ~Chomper() override;

    void update(float deltaTime, std::vector<Projectile>& outProjectiles, std::vector<SunItem>& outSuns) override;
    void draw() override;

    bool isIdle() const { return m_state == ChomperState::IDLE; }
    bool isChewing() const { return m_state == ChomperState::CHEWING; }
    void devourZombie(Zombie* z);
};
