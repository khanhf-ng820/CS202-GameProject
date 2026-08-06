#pragma once
#include "raylib.h"
#include "resources.h"
#include "Plant.h"
#include "Wallnut.h"
#include "Zombie.h"
#include "ZombieNormal.h"
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

struct BowlingCard {
    float x;
    std::string plantType;
};

struct BowlingNut {
    float x;
    float y;
    float vx;
    float vy;
    float rotationAngle;
    float rotationSpeed;
    const Zombie* lastHitZombie = nullptr;
    float hitCooldown = 0.0f;
    bool isGiant = false;
    bool isExplode = false;
    std::vector<const Zombie*> hitZombies;
};

class BowlingLevel {
public:
    BowlingLevel(Resources& res, RenderTexture2D targetScreen);
    ~BowlingLevel() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    float m_animTimer = 0.0f;
    int m_currentFrame = 0;

    std::vector<BowlingCard> m_cards;
    float m_cardSpawnTimer = 0.0f;

    // Active rolling bowling nuts
    std::vector<BowlingNut> m_bowlingNuts;

    // Active zombies
    std::vector<std::unique_ptr<Zombie>> m_zombies;
    std::vector<std::pair<const Zombie*, float>> m_hitDebugTimers;
    bool m_showDebug = true;

    // Level progression & game over state
    bool m_levelWon = false;
    bool m_levelLost = false;

    // Grid state for placed plants
    std::unique_ptr<Plant> m_grid[5][9];

    // Cursor card pickup state
    bool m_isHoldingCard = false;
    std::string m_heldPlantType = "";

    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
};
