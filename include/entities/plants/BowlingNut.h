#pragma once
#include "raylib.h"
#include "resources.h"
#include "AudioManager.h"
#include "Zombie.h"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>

class BowlingNut {
protected:
    float m_x;
    float m_y;
    float m_vx;
    float m_vy;
    float m_rotationAngle;
    float m_rotationSpeed;
    bool m_hasExploded{false};

public:
    BowlingNut(float x, float y, float vx = 300.0f, float vy = 0.0f, float rotSpeed = 360.0f);
    virtual ~BowlingNut() = default;

    virtual void update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) = 0;
    virtual void draw(Resources& res, bool showDebug) const = 0;

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    bool isOffScreen() const { return m_x > 850.0f; }
    virtual bool isExploded() const { return m_hasExploded; }

    static std::unique_ptr<BowlingNut> Create(const std::string& plantType, float x, float y);
};

class NormalBowlingNut : public BowlingNut {
private:
    const Zombie* m_lastHitZombie = nullptr;
    float m_hitCooldown = 0.0f;

public:
    NormalBowlingNut(float x, float y);
    void update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) override;
    void draw(Resources& res, bool showDebug) const override;
};

class GiantBowlingNut : public BowlingNut {
private:
    std::vector<const Zombie*> m_hitZombies;

public:
    GiantBowlingNut(float x, float y);
    void update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) override;
    void draw(Resources& res, bool showDebug) const override;
};

class ExplodeBowlingNut : public BowlingNut {
private:
    std::vector<const Zombie*> m_hitZombies;
    bool m_isExplodingEffect{false};
    float m_explosionTimer{0.0f};

public:
    ExplodeBowlingNut(float x, float y);
    void update(float dt, std::vector<std::unique_ptr<Zombie>>& zombies, std::vector<std::pair<const Zombie*, float>>& hitDebugTimers, Resources& res) override;
    void draw(Resources& res, bool showDebug) const override;
};
