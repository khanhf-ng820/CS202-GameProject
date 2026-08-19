#pragma once
#include "raylib.h"
#include "resources.h"
#include "Zombie.h"
#include "ZombieNormal.h"
#include "Reanimation.h"
#include "Vase.h"
#include <vector>
#include <memory>

struct VaseShard {
    float x;
    float y;
    float vx;
    float vy;
    float rotation;
    float rotSpeed;
    float groundY;
    float lifetime;
    float maxLifetime;
    int frameCol;
    int frameRow;
};

class VasebreakerLevel {
public:
    VasebreakerLevel(Resources& res, RenderTexture2D targetScreen);
    ~VasebreakerLevel() = default;

    void run();
    void update(float dt);
    void draw();

private:
    Resources& res;
    RenderTexture2D targetScreen;

    // Mallet cursor animation state
    Reanimation m_malletAnim;
    bool m_isSwinging = false;

    // 5x9 Vase Grid
    std::unique_ptr<Vase> m_vases[5][9];

    // Shard particles from destroyed vases
    std::vector<VaseShard> m_shards;

    // Pending strike target to synchronize mallet impact and prevent double-click destruction
    int m_pendingVaseRow = -1;
    int m_pendingVaseCol = -1;
    float m_pendingVaseTimer = 0.0f;

    // Active zombies on the lawn
    std::vector<std::unique_ptr<Zombie>> m_zombies;

    // Game state
    bool m_levelWon = false;
    bool m_levelLost = false;

    // Helper functions
    bool getGridCell(Vector2 mousePos, int& outRow, int& outCol) const;
    void spawnVases();
    void breakVase(int row, int col);
};
